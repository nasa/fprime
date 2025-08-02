#!/usr/bin/env python3
'''
Soak Test Monitor - Simplified monitoring for F' soak testing
Analyzes ComLogger .com files for health warnings and resource issues
'''

import os
import re
import sys
import json
from datetime import datetime
from pathlib import Path
from typing import Dict, Tuple, Any
from fprime_gds.executables.cli import ParserBase, StandardPipelineParser
from fprime_gds.common.handlers import DataHandler


class SoakAnalysisResults:
    """Container for soak test analysis results"""
    def __init__(self):
        self.health_issues = []
        self.buffer_metrics = {}
        self.system_resources = {}
        self.alerts = []
        self.timestamp = datetime.now().isoformat()
    
    def add_alert(self, alert_message):
        """Add an alert to the results"""
        self.alerts.append(alert_message)
        
    def has_critical_issues(self):
        """Check if there are any critical issues that should fail the test"""
        return bool(self.alerts) or any('FATAL' in issue.get('severity', '') for issue in self.health_issues)
    
    def analyze_trends(self):
        """Analyze trends over time within the ComLogger data"""
        print("\n📊 ANALYZING TRENDS ACROSS MULTIPLE RUNS:")
        
        # Analyze buffer trends
        for metric_name, readings in self.buffer_metrics.items():
            if len(readings) > 10:  # Need sufficient data points
                self._print_data_range(metric_name, readings, "Buffer")
                trend_alert = self._detect_upward_trend(metric_name, readings, "Buffer")
                if trend_alert:
                    self.add_alert(trend_alert)
        
        # Analyze resource trends  
        for metric_name, readings in self.system_resources.items():
            if len(readings) > 10:  # Need sufficient data points
                self._print_data_range(metric_name, readings, "Resource")
                trend_alert = self._detect_upward_trend(metric_name, readings, "Resource")
                if trend_alert:
                    self.add_alert(trend_alert)
    
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
            return None
            
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
                return f"{metric_type} trending up: {metric_name} increased {growth_rate:.1f}% over session"
            elif metric_type == "Resource" and growth_rate > 25:  # 25% resource growth  
                return f"{metric_type} trending up: {metric_name} increased {growth_rate:.1f}% over session"
                
        return None





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
            
            # DEBUG: Print all events to see what we're getting
            print(f"DEBUG EVENT: {event_name} | {severity} | {description}")
            
            # Check for health issues
            if 'FATAL' in severity or 'WARNING' in severity or 'HLTH_' in event_name:
                issue = {
                    'timestamp': timestamp,
                    'event_name': event_name,
                    'severity': severity,
                    'description': description
                }
                self.results.health_issues.append(issue)
                print(f"CAPTURED ISSUE: {severity} - {event_name}")
                
                # Add to alerts if critical
                if 'FATAL' in severity:
                    self.results.add_alert(f"FATAL: {event_name} - {description}")
                elif 'WARNING' in severity:
                    self.results.add_alert(f"WARNING: {event_name} - {description}")
                    
        except Exception as e:
            # Don't silently ignore - log the error for debugging
            print(f"ERROR parsing event: {e}")
            import traceback
            traceback.print_exc()


class ChannelCollector(DataHandler):
    """Channel consumer that inherits from DataHandler - similar to old log_processor pattern"""
    def __init__(self, results):
        self.results = results
        
    def data_callback(self, channel_data, sender=None):
        """Handle decoded channel data"""
        try:
            # Extract channel information
            ch_name = channel_data.template.name if hasattr(channel_data, 'template') else str(channel_data)
            ch_val = channel_data.val if hasattr(channel_data, 'val') else 0
            timestamp = str(channel_data.time) if hasattr(channel_data, 'time') else ""
            
            # Track buffer manager stats
            if 'BufferManager' in ch_name or 'bufferManager' in ch_name:
                if ch_name not in self.results.buffer_metrics:
                    self.results.buffer_metrics[ch_name] = []
                
                try:
                    value = int(ch_val) if isinstance(ch_val, (int, float)) else int(str(ch_val).split()[0])
                    self.results.buffer_metrics[ch_name].append({
                        'timestamp': timestamp,
                        'value': value
                    })
                    
                    # Check for concerning buffer levels
                    if value == 0:
                        self.results.add_alert(f"Buffer exhaustion detected: {ch_name} = 0")
                        
                except (ValueError, IndexError, TypeError):
                    pass
                    
            # Track system resources
            elif 'systemResources' in ch_name:
                if ch_name not in self.results.system_resources:
                    self.results.system_resources[ch_name] = []
                    
                try:
                    value = float(ch_val) if isinstance(ch_val, (int, float)) else float(str(ch_val).replace(',', ''))
                    self.results.system_resources[ch_name].append({
                        'timestamp': timestamp,
                        'value': value
                    })
                    
                    # Check for concerning resource levels
                    if 'cpu' in ch_name.lower() and value > 90.0:
                        self.results.add_alert(f"High CPU usage detected: {ch_name} = {value}%")
                    elif 'memory' in ch_name.lower() and value > 90.0:
                        self.results.add_alert(f"High memory usage detected: {ch_name} = {value}%")
                        
                except (ValueError, TypeError):
                    pass
                    
        except Exception as e:
            # Silently ignore parsing errors to be robust
            pass


def process_logs(pipeline, dictionary_path, logs_path, results):
    """Process log files for monitoring data"""
    
    # Find log files
    log_files = []
    logs_dir = Path(logs_path)
    
    if logs_dir.is_dir():
        # Look for .com files recursively
        log_files = list(logs_dir.glob('**/*.com'))
        
    elif logs_dir.is_file() and logs_dir.suffix == '.com':
        log_files = [logs_dir]
    
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
    
    print("Consumers registered, processing ComLogger files...")
    
    # Process each ComLogger file by reading binary data
    for log_file in log_files:
        try:
            print(f"Processing file: {log_file} (size: {log_file.stat().st_size} bytes)")
            with open(log_file, 'rb') as file_handle:
                data = file_handle.read()
                if len(data) > 0:
                    print(f"Sending {len(data)} bytes to distributor")
                    # Send raw binary data to distributor (same as old approach)
                    pipeline.distributor.on_recv(data)
                else:
                    print(f"File {log_file} is empty")
                    
        except Exception as e:
            print(f"Error processing {log_file}: {e}")
            import traceback
            traceback.print_exc()
    
    # Let the modern pipeline handle its own cleanup
    print(f"Processed {len(results.health_issues)} health events and {len(results.buffer_metrics) + len(results.system_resources)} telemetry metrics")





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
        # Convert to Path object explicitly (even though type=Path is specified, it might still be a string)
        if hasattr(args, 'com_logs') and args.com_logs is not None:
            args.com_logs = Path(args.com_logs)
            
            # Validate logs directory exists
            if not args.com_logs.exists():
                raise ValueError(f"ComLogger logs directory must exist: {args.com_logs}")
            
            if not args.com_logs.is_dir():
                raise ValueError(f"ComLogger logs path must be a directory: {args.com_logs}")
        else:
            raise ValueError("ComLogger logs argument is required")
        
        return args


def main():
    args, _ = ParserBase.parse_args([StandardPipelineParser, SoakMonitorArgumentParser])
    pipeline = StandardPipelineParser.pipeline_factory(args)
   
    # Validate required arguments
    if not hasattr(args, 'dictionary') or not args.dictionary:
        raise ValueError("Dictionary argument is required from StandardPipelineParser")
    
    dictionary_path = Path(args.dictionary)
    if not dictionary_path.exists():
        raise ValueError(f"Dictionary file does not exist: {args.dictionary}")
    
    # Initialize results container
    results = SoakAnalysisResults()
    
    print("="*50)
    print("F' SOAK TEST MONITOR")
    print("="*50)
    print("Analyzes ComLogger .com files for health and resource issues")
    print("-"*50)
    print(f"Dictionary: {args.dictionary}")
    print(f"ComLogger files directory: {args.com_logs}")
    print(f"Analysis timestamp: {results.timestamp}")
    print("-"*50)
    
    # Process logs
    process_logs(pipeline, args.dictionary, args.com_logs, results)
    
    # Analyze trends within the ComLogger data
    results.analyze_trends()
    
    # Print results
    print("\nMONITORING RESULTS:")
    print("-"*50)
    print(f"Health Issues Found: {len(results.health_issues)}")
    print(f"Buffer Metrics Tracked: {len(results.buffer_metrics)}")
    print(f"System Resources Tracked: {len(results.system_resources)}")
    print(f"Alerts Generated: {len(results.alerts)}")
    
    # Show data ranges for trend analysis
    total_readings = sum(len(readings) for readings in results.buffer_metrics.values())
    total_readings += sum(len(readings) for readings in results.system_resources.values())
    if total_readings > 0:
        print(f"Total Data Points Analyzed: {total_readings}")
    
    if results.alerts:
        print("\nALERTS:")
        for alert in results.alerts:
            if "trending up" in alert:
                print(f"  📈 {alert}")
            else:
                print(f"  ⚠️  {alert}")
    
    if results.health_issues:
        print("\nHEALTH ISSUES:")
        for issue in results.health_issues:
            print(f"  🔥 {issue['severity']}: {issue['event_name']}")
    
    # Show buffer status summary
    if results.buffer_metrics:
        print("\nBUFFER STATUS:")
        for metric_name, readings in results.buffer_metrics.items():
            if readings:
                latest_value = readings[-1]['value']
                print(f"  📊 {metric_name}: {latest_value}")
    
    # Show resource status summary  
    if results.system_resources:
        print("\nSYSTEM RESOURCES:")
        for metric_name, readings in results.system_resources.items():
            if readings:
                latest_value = readings[-1]['value']
                print(f"  📊 {metric_name}: {latest_value}")
    
    print("="*50)
    
    # Exit with error code if there are critical issues
    if results.has_critical_issues():
        print("⚠️  EXITING WITH ERROR DUE TO CRITICAL ISSUES")
        exit(1)
    
    print("MONITORING COMPLETED SUCCESSFULLY")


if __name__ == '__main__':
    main() 