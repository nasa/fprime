#!/usr/bin/env python3
'''
Soak Test Monitor - Simplified monitoring for F' soak testing
Checks for health warnings, buffer allocation trends, and system resource trends
'''

import os
import re
import json
import argparse
from datetime import datetime, timedelta
from pathlib import Path
from typing import Dict, Tuple, Any
from fprime_gds.executables.cli import ParserBase, StandardPipelineParser
from fprime_gds.common.handlers import DataHandler
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager


class SoakMonitor:
    def __init__(self, state_file="soak_monitor_state.json"):
        self.state_file = state_file
        self.state = self.load_state()
        self.current_run_data = {
            'timestamp': datetime.now().isoformat(),
            'health_issues': [],
            'buffer_stats': {},
            'system_resources': {},
            'alerts': []
        }
        
    def load_state(self):
        """Load previous monitoring state"""
        if os.path.exists(self.state_file):
            with open(self.state_file, 'r') as f:
                return json.load(f)
        return {
            'last_run_timestamp': None,
            'last_processed_times': {},
            'buffer_history': [],
            'resource_history': [],
            'health_history': []
        }
    
    def save_state(self):
        """Save current monitoring state"""
        with open(self.state_file, 'w') as f:
            json.dump(self.state, f, indent=2)
    
    def get_unix_time_from_raw_time(self, raw_time):
        """Extract unix timestamp from raw_time format (d(d)-timestamp:microsec)"""
        match = re.search(r'\d+\(0\)-(\d+):\d+', raw_time)
        if match:
            return int(match.group(1))
        return None


class HealthMonitor(DataHandler):
    """Monitor for health warnings and fatals"""
    def __init__(self, monitor):
        self.monitor = monitor
        self.last_processed_time = monitor.state.get('last_processed_times', {}).get('events', 0)
        
    def data_callback(self, data, sender=None):
        """Process event data for health issues"""
        item = data.get_str(verbose=True, csv=True).split(',')
        if len(item) < 6:
            return
            
        raw_time = item[1]
        event_name = item[4]
        severity = item[5]
        description = item[6] if len(item) > 6 else ""
        
        # Get timestamp
        timestamp = self.monitor.get_unix_time_from_raw_time(raw_time)
        if not timestamp or timestamp <= self.last_processed_time:
            return
            
        # Check for health issues
        if 'FATAL' in severity or 'WARNING' in severity or 'HLTH_' in event_name:
            issue = {
                'timestamp': raw_time,
                'event_name': event_name,
                'severity': severity,
                'description': description
            }
            self.monitor.current_run_data['health_issues'].append(issue)
            
            # Add to alerts if critical
            if 'FATAL' in severity:
                self.monitor.current_run_data['alerts'].append(f"FATAL: {event_name} - {description}")
            elif 'WARNING' in severity:
                self.monitor.current_run_data['alerts'].append(f"WARNING: {event_name} - {description}")


class ResourceMonitor(DataHandler):
    """Monitor for buffer and system resource telemetry"""
    def __init__(self, monitor):
        self.monitor = monitor
        self.last_processed_time = monitor.state.get('last_processed_times', {}).get('telemetry', 0)
        self.latest_values = {}
        
    def data_callback(self, data, sender=None):
        """Process telemetry data for resources and buffers"""
        item = data.get_str(verbose=True, csv=True).split(',')
        if len(item) < 5:
            return
            
        raw_time = item[0]
        ch_name = item[3]
        ch_val = item[4]
        
        # Get timestamp
        timestamp = self.monitor.get_unix_time_from_raw_time(raw_time)
        if not timestamp or timestamp <= self.last_processed_time:
            return
            
        # Track buffer manager stats
        if 'BufferManager' in ch_name or 'bufferManager' in ch_name:
            if ch_name not in self.monitor.current_run_data['buffer_stats']:
                self.monitor.current_run_data['buffer_stats'][ch_name] = []
            
            # Parse numeric value
            try:
                value = int(ch_val.split()[0]) if ch_val.split() else 0
                self.monitor.current_run_data['buffer_stats'][ch_name].append({
                    'timestamp': raw_time,
                    'value': value
                })
                self.latest_values[ch_name] = value
            except (ValueError, IndexError):
                pass
                
        # Track system resources
        elif 'systemResources' in ch_name:
            if ch_name not in self.monitor.current_run_data['system_resources']:
                self.monitor.current_run_data['system_resources'][ch_name] = []
                
            # Parse numeric value
            try:
                value_str = ch_val.split()[0]
                value = float(value_str.replace(',', ''))
                self.monitor.current_run_data['system_resources'][ch_name].append({
                    'timestamp': raw_time,
                    'value': value
                })
                self.latest_values[ch_name] = value
            except (ValueError, IndexError):
                pass


def analyze_trends(current_data, history, metric_name):
    """Analyze if metrics are trending upward"""
    alerts = []
    
    if len(history) < 2:
        return alerts
        
    # Get latest values from current run
    latest_values = {}
    for metric, readings in current_data.items():
        if readings:
            latest_values[metric] = readings[-1]['value']
    
    # Compare with previous runs
    if len(history) >= 3:  # Need at least 3 data points for trend
        for metric, current_value in latest_values.items():
            if metric in history[-1] and metric in history[-2]:
                prev_value = history[-1].get(metric, 0)
                prev_prev_value = history[-2].get(metric, 0)
                
                # Check for consistent upward trend
                if current_value > prev_value > prev_prev_value:
                    increase_rate = ((current_value - prev_prev_value) / prev_prev_value * 100) if prev_prev_value > 0 else 0
                    if increase_rate > 10:  # More than 10% increase over 3 runs
                        alerts.append(f"{metric_name} trending up: {metric} increased {increase_rate:.1f}% over last 3 runs")
    
    return alerts


def process_logs(pipeline, dictionary_path, logs_path, monitor):
    """Process log files for monitoring data"""
    # Setup GDS pipeline
    pipeline.config.set('framing', 'use_key', 'False')
    pipeline.config.set('types', 'msg_len', 'U16')
    
    # Register monitors
    health_monitor = HealthMonitor(monitor)
    resource_monitor = ResourceMonitor(monitor)
    
    pipeline.coders.register_event_consumer(health_monitor)
    pipeline.coders.register_channel_consumer(resource_monitor)
    
    # Find and process log files
    log_files = []
    logs_dir = Path(logs_path)
    
    if logs_dir.is_dir():
        # Look for .com files recursively
        log_files = list(logs_dir.glob('**/*.com'))
        
    elif logs_dir.is_file() and logs_dir.suffix == '.com':
        log_files = [logs_dir]
    
    print(f"Processing {len(log_files)} ComLogger .com files...")
    
    for log_file in log_files:
        try:
            with open(log_file, 'rb') as f:
                data = f.read()
                pipeline.distributor.on_recv(data)
        except Exception as e:
            print(f"Error processing {log_file}: {e}")
    
    # Update last processed timestamps
    current_time = int(datetime.now().timestamp())
    monitor.state['last_processed_times']['events'] = current_time
    monitor.state['last_processed_times']['telemetry'] = current_time
    
    # Cleanup
    if hasattr(pipeline.files, 'uplinker') and pipeline.files.uplinker:
        pipeline.files.uplinker.exit()


class SoakMonitorArgumentParser(ParserBase):
    """ Parser for F' Soak Monitor additional arguments
    
    This class provides functionality to parse command line arguments for soak monitoring
    functionality including ComLogger files location, state file, and report output.

    Parsers should:
    1. Set a DESCRIPTION for string used in help text,
    2. Implement get_arguments() to return a dictionary of argument definitions (argparse parameter format)
    3. Implement handle_arguments() to handle the arguments as parsed
    """

    DESCRIPTION = "F' Soak Test Monitor - Analyzes ComLogger files for health, buffer, and resource trends"

    def get_arguments(self) -> Dict[Tuple[str, ...], Dict[str, Any]]:
        """Arguments for soak monitoring"""
        return {
            ("--logs",): {
                "action": "store",
                "required": True,
                "type": Path,
                "help": "Path to directory containing ComLogger .com files to analyze.",
            },
            ("--state-file",): {
                "action": "store",
                "default": "soak_monitor_state.json",
                "type": Path,
                "help": "Path to monitoring state file for tracking trends over time.",
            },
            ("--report-file",): {
                "action": "store",
                "default": None,
                "required": False,
                "type": Path,
                "help": "Path to save JSON monitoring report file (optional).",
            }
        }

    def handle_arguments(self, args, **kwargs):
        """Handle arguments as parsed"""
        # Validate logs directory exists
        if args.logs is None or not args.logs.exists():
            raise ValueError(f"ComLogger logs directory must exist: {args.logs}")
        
        if not args.logs.is_dir():
            raise ValueError(f"ComLogger logs path must be a directory: {args.logs}")
        
        # Ensure state file directory exists
        args.state_file.parent.mkdir(parents=True, exist_ok=True)
        
        # Ensure report file directory exists if specified
        if args.report_file:
            if args.report_file.suffix != ".json":
                raise ValueError("Report file must have .json extension")
            args.report_file.parent.mkdir(parents=True, exist_ok=True)
        
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
    
    # Initialize monitor
    monitor = SoakMonitor(args.state_file)
    
    print("="*50)
    print("F' SOAK TEST MONITOR")
    print("="*50)
    print("Analyzes ComLogger .com files (binary F' packets)")
    print("Note: This does NOT analyze GDS text logs")
    print("-"*50)
    print(f"Dictionary: {args.dictionary}")
    print(f"ComLogger files directory: {args.logs}")
    print(f"State file: {args.state_file}")
    print(f"Last run: {monitor.state.get('last_run_timestamp', 'Never')}")
    print("-"*50)
    
    # Process logs
    process_logs(pipeline, args.dictionary, args.logs, monitor)
    
    # Analyze trends
    buffer_alerts = analyze_trends(
        monitor.current_run_data['buffer_stats'],
        monitor.state['buffer_history'],
        "Buffer allocation"
    )
    
    resource_alerts = analyze_trends(
        monitor.current_run_data['system_resources'],
        monitor.state['resource_history'],
        "System resource"
    )
    
    monitor.current_run_data['alerts'].extend(buffer_alerts)
    monitor.current_run_data['alerts'].extend(resource_alerts)
    
    # Update history
    if monitor.current_run_data['buffer_stats']:
        buffer_summary = {k: v[-1]['value'] if v else 0 for k, v in monitor.current_run_data['buffer_stats'].items()}
        monitor.state['buffer_history'].append(buffer_summary)
        
    if monitor.current_run_data['system_resources']:
        resource_summary = {k: v[-1]['value'] if v else 0 for k, v in monitor.current_run_data['system_resources'].items()}
        monitor.state['resource_history'].append(resource_summary)
    
    if monitor.current_run_data['health_issues']:
        monitor.state['health_history'].extend(monitor.current_run_data['health_issues'])
    
    # Keep only last 10 runs in history
    monitor.state['buffer_history'] = monitor.state['buffer_history'][-10:]
    monitor.state['resource_history'] = monitor.state['resource_history'][-10:]
    monitor.state['health_history'] = monitor.state['health_history'][-100:]  # Keep more health history
    
    monitor.state['last_run_timestamp'] = monitor.current_run_data['timestamp']
    
    # Print results
    print("\nMONITORING RESULTS:")
    print("-"*50)
    print(f"Health Issues Found: {len(monitor.current_run_data['health_issues'])}")
    print(f"Buffer Metrics Tracked: {len(monitor.current_run_data['buffer_stats'])}")
    print(f"System Resources Tracked: {len(monitor.current_run_data['system_resources'])}")
    print(f"Alerts Generated: {len(monitor.current_run_data['alerts'])}")
    
    if monitor.current_run_data['alerts']:
        print("\nALERTS:")
        for alert in monitor.current_run_data['alerts']:
            print(f"  ⚠️  {alert}")
    
    if monitor.current_run_data['health_issues']:
        print("\nHEALTH ISSUES:")
        for issue in monitor.current_run_data['health_issues']:
            print(f"  🔥 {issue['severity']}: {issue['event_name']}")
    
    # Save state and report
    monitor.save_state()
    
    if args.report_file:
        with open(args.report_file, 'w') as f:
            json.dump(monitor.current_run_data, f, indent=2)
        print(f"\nReport saved to: {args.report_file}")
    
    print("="*50)
    
    # Exit with error code if there are alerts
    if monitor.current_run_data['alerts'] or any('FATAL' in issue['severity'] for issue in monitor.current_run_data['health_issues']):
        print("⚠️  EXITING WITH ERROR DUE TO ALERTS/FATALS")
        exit(1)
    
    print("MONITORING COMPLETED SUCCESSFULLY")


if __name__ == '__main__':
    main() 