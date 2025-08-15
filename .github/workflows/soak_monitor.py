#!/usr/bin/env python3
'''
Soak Test Monitor - Simplified monitoring for F' soak testing
Analyzes ComLogger .com files for health warnings and resource issues
'''

import os
import re
import json
import sys
from datetime import datetime
from pathlib import Path
from typing import Dict, Tuple, Any
from fprime_gds.executables.cli import ParserBase, StandardPipelineParser
from fprime_gds.common.handlers import DataHandler
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager

class SoakAnalysisResults:
    """Container for soak test analysis results"""
    def __init__(self):
        self.buffer_metrics = {}
        self.system_resources = {}
        self.alerts = []
        self.timestamp = datetime.now().isoformat()
    
    def add_alert(self, severity, message, **details):
        """Add an alert with severity level
        
        Args:
            severity: 'FATAL' or 'WARNING'
            message: Alert message
            **details: Additional context (timestamp, channel_name, value, etc.)
        """
        alert = {
            'severity': severity,
            'message': message,
            'timestamp': details.get('timestamp', ''),
            **details
        }
        self.alerts.append(alert)
        

    
    def analyze_trends(self):
        """Analyze trends over time within the ComLogger data"""
        # Analyze buffer trends
        for metric_name, readings in self.buffer_metrics.items():
            if len(readings) > 10:  # Need sufficient data points
                self._print_data_range(metric_name, readings, "Buffer")
                self._detect_upward_trend(metric_name, readings, "Buffer")
        
        # Analyze resource trends  
        for metric_name, readings in self.system_resources.items():
            if len(readings) > 10:  # Need sufficient data points
                self._print_data_range(metric_name, readings, "Resource")
                self._detect_upward_trend(metric_name, readings, "Resource")
    
    def _print_data_range(self, metric_name, readings, metric_type):
        """Print the time range of data for this metric"""
        if not readings:
            return
        sorted_readings = sorted(readings, key=lambda x: x['timestamp'])
        earliest = sorted_readings[0]['timestamp']
        latest = sorted_readings[-1]['timestamp']
        print(f"  {metric_type} {metric_name}: {len(readings)} readings from {earliest} to {latest}")
    
    def _detect_upward_trend(self, metric_name, readings, metric_type):
        """Detect if a metric is trending upward over time"""
        if len(readings) < 10:
            return
            
        # Sort by timestamp to ensure chronological order
        sorted_readings = sorted(readings, key=lambda x: x['timestamp'])
        
        # Compare first 20% vs last 20% of data
        early_count = max(2, len(sorted_readings) // 5)
        late_count = max(2, len(sorted_readings) // 5)
        
        early_values = [r['value'] for r in sorted_readings[:early_count]]
        late_values = [r['value'] for r in sorted_readings[-late_count:]]
        
        early_avg = sum(early_values) / len(early_values)
        late_avg = sum(late_values) / len(late_values)
        
        # Check for significant upward trend
        if early_avg > 0 and late_avg > early_avg:
            growth_rate = ((late_avg - early_avg) / early_avg) * 100
            
            # Alert thresholds
            if metric_type == "Buffer" and growth_rate > 15:  # 15% buffer growth
                self.add_alert('WARNING', f"{metric_type} trending up: {metric_name} increased {growth_rate:.1f}% over session")
            elif metric_type == "Resource" and growth_rate > 25:  # 25% resource growth  
                self.add_alert('WARNING', f"{metric_type} trending up: {metric_name} increased {growth_rate:.1f}% over session")





class EventCollector(DataHandler):
    """Event consumer that inherits from DataHandler"""
    def __init__(self, results):
        self.results = results
        
    def data_callback(self, event_data, sender=None):
        """Handle decoded event data"""
        try:
            # Extract event information
            event_name = event_data.template.name if hasattr(event_data, 'template') else str(event_data)
            severity = event_data.template.severity if hasattr(event_data, 'template') else "UNKNOWN"
            description = str(event_data.args) if hasattr(event_data, 'args') else ""
            timestamp = str(event_data.time) if hasattr(event_data, 'time') else ""
            
            # Check for health issues and add alerts
            # Map events with 'Fatal' in the message to FATAL severity, all others to WARNING
            if str(severity) == "EventSeverity.FATAL" or str(severity) == "EventSeverity.WARNING_HI":
                message = f"{event_name} - {description}"
                # Check if the message contains "Fatal" for FATAL severity
                if "Fatal" in message or "FATAL" in message or str(severity) == "EventSeverity.FATAL":
                    alert_severity = 'FATAL'
                else:
                    alert_severity = 'WARNING'
                    
                self.results.add_alert(alert_severity, message, 
                                     timestamp=timestamp, event_name=event_name)
                    
        except Exception as e:
            # Silently ignore parsing errors to be robust
            pass


class ChannelCollector(DataHandler):
    """Channel consumer that inherits from DataHandler"""
    def __init__(self, results):
        self.results = results
        
    def data_callback(self, channel_data, sender=None):
        """Handle decoded channel data"""
        try:
            # Extract channel information
            ch_name = channel_data.template.name if hasattr(channel_data, 'template') else str(channel_data)
            ch_val = channel_data.val if hasattr(channel_data, 'val') else 0
            timestamp = str(channel_data.time) if hasattr(channel_data, 'time') else ""
            
            # Process channel data and check for issues
            channel_info = {
                'timestamp': timestamp,
                'channel_name': ch_name,
                'value': ch_val
            }
            
            # Handle buffer metrics
            if 'BufferManager' in ch_name:
                if ch_name not in self.results.buffer_metrics:
                    self.results.buffer_metrics[ch_name] = []
                
                value = int(ch_val)  
                self.results.buffer_metrics[ch_name].append({
                    'timestamp': timestamp,
                    'value': value
                })
                
                # Check for buffer issues
                if value == 0:
                    self.results.add_alert('WARNING', f"Buffer exhaustion detected: {ch_name} = 0", 
                                         timestamp=timestamp, channel_name=ch_name, value=value)
            
            # Handle system resources
            elif 'systemResources' in ch_name:
                if ch_name not in self.results.system_resources:
                    self.results.system_resources[ch_name] = []
                
                value = float(ch_val) if isinstance(ch_val, (int, float)) else float(str(ch_val).replace(',', ''))
                self.results.system_resources[ch_name].append({
                    'timestamp': timestamp,
                    'value': value
                })
                
                # Check for resource issues
                if 'cpu' in ch_name.lower() and value > 90.0:
                    self.results.add_alert('WARNING', f"High CPU usage detected: {ch_name} = {value}%", 
                                         timestamp=timestamp, channel_name=ch_name, value=value)
                elif 'memory' in ch_name.lower() and value > 90.0:
                    self.results.add_alert('WARNING', f"High memory usage detected: {ch_name} = {value}%", 
                                         timestamp=timestamp, channel_name=ch_name, value=value)
                    
        except Exception as e:
            # Silently ignore parsing errors to be robust
            pass


def process_logs(pipeline, logs_path, results):
    """Process log files for monitoring data"""
    
    log_files = list(logs_path.glob('**/*.com'))
        
    print(f"Processing {len(log_files)} ComLogger .com files...")
    
    if not log_files:
        print("No ComLogger .com files found!")
        return
    
    # Use the modern pipeline from StandardPipelineParser 
    # But create old-style DataHandler consumers
    event_consumer = EventCollector(results)
    channel_consumer = ChannelCollector(results)
    
    # Register consumers using the standard coders approach
    pipeline.coders.register_event_consumer(event_consumer)
    pipeline.coders.register_channel_consumer(channel_consumer)
    
    # Process each ComLogger file by reading binary data
    for log_file in log_files:
        try:
            with open(log_file, 'rb') as file_handle:
                data = file_handle.read()
                if len(data) > 0:
                    # Send binary data to distributor
                    pipeline.distributor.on_recv(data)
                    
        except Exception as e:
            print(f"Error processing {log_file}: {e}")

class SoakMonitorArgumentParser(ParserBase):
    """ Parser for F' Soak Monitor additional arguments
    
    This class provides functionality to parse command line arguments for soak monitoring
    functionality including ComLogger files location.

    Parsers should:
    1. Set a DESCRIPTION for string used in help text,
    2. Implement get_arguments() to return a dictionary of argument definitions (argparse parameter format)
    3. Implement handle_arguments() to handle the arguments as parsed
    """

    DESCRIPTION = "F' Soak Test Monitor - Analyzes ComLogger files for health, buffer, and resource issues"

    def get_arguments(self) -> Dict[Tuple[str, ...], Dict[str, Any]]:
        """Arguments for soak monitoring"""
        return {
            ("--com-logs",): {
                "action": "store",
                "required": True,
                "type": Path,
                "help": "Path to directory containing ComLogger .com files to analyze.",
            }
        }

    def handle_arguments(self, args, **kwargs):
        """Handle arguments as parsed"""
        # Validate logs directory exists
        if not args.com_logs.exists():
            raise ValueError(f"ComLogger logs directory must exist: {args.com_logs}")
            
        if not args.com_logs.is_dir():
            raise ValueError(f"ComLogger logs path must be a directory: {args.com_logs}")
        return args


def pipeline_factory(args_ns, config) -> StandardPipeline:
        """A factory of the standard pipeline given the handled arguments"""
        pipeline_arguments = {
            "config": config,
            "dictionary": args_ns.dictionary,
            "file_store": args_ns.files_storage_directory,
            "packet_spec": args_ns.packet_spec,
            "packet_set_name": args_ns.packet_set_name,
            "logging_prefix": args_ns.logs,
        }
        pipeline = StandardPipeline()
        pipeline.transport_implementation = args_ns.connection_transport
        try:
            pipeline.setup(**pipeline_arguments)
            #Call disconnect to turn off the receiving thread, we are feeding the data manually here
            pipeline.disconnect()
        except Exception:
            # In all error cases, pipeline should be shutdown before continuing with exception handling
            try:
                pipeline.disconnect()
            finally:
                raise
        return pipeline


def main():
    args, _ = ParserBase.parse_args([StandardPipelineParser, SoakMonitorArgumentParser])
    config = ConfigManager()
    config.set('framing', 'use_key', 'False')
    config.set('types', 'msg_len', 'U16')
    pipeline = pipeline_factory(args, config)
    
    # Initialize results container
    results = SoakAnalysisResults()
    
    print("="*50)
    print("F' SOAK TEST MONITOR")
    print("="*50)
    
    # Process logs
    process_logs(pipeline, args.com_logs, results)
    
    # Analyze trends within the ComLogger data
    results.analyze_trends()
    return_code = 0 
    # Print results
    print("\nMONITORING RESULTS:")
    print("-"*50)
    print(f"Buffer Metrics Tracked: {len(results.buffer_metrics)}")
    print(f"System Resources Tracked: {len(results.system_resources)}")
    print(f"Alerts Generated: {len(results.alerts)}")
    
    # Show data ranges for trend analysis
    total_readings = sum(len(readings) for readings in results.buffer_metrics.values())
    total_readings += sum(len(readings) for readings in results.system_resources.values())
    if total_readings > 0:
        print(f"Total Data Points Analyzed: {total_readings}")
    
    # Set return code to 1 if there are any fatal alerts
    if any(alert['severity'] == 'FATAL' for alert in results.alerts):
        return_code = 1
    
    if results.alerts:
        print("\nALERTS:")
        for alert in results.alerts:
            severity = alert['severity']
            message = alert['message']
            print(f"{severity}: {message}")
    
    # Show buffer status summary
    if results.buffer_metrics:
        print("\nBUFFER STATUS:")
        for metric_name, readings in results.buffer_metrics.items():
            if readings:
                latest_value = readings[-1]['value']
                print(f"{metric_name}: {latest_value}")
    
    # Show resource status summary  
    if results.system_resources:
        print("\nSYSTEM RESOURCES:")
        for metric_name, readings in results.system_resources.items():
            if readings:
                latest_value = readings[-1]['value']
                print(f" {metric_name}: {latest_value}")
    
    print("="*50)
    
    print("MONITORING COMPLETED SUCCESSFULLY")
    sys.exit(return_code)

if __name__ == '__main__':
    main() 