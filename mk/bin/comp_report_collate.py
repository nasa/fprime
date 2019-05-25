import sys
import os

# walk through input file list and add up component inteface types

total_input_ports = 0
total_output_ports = 0
total_commands = 0
total_channels = 0
total_events = 0
total_parameters = 0

opcodes = ""
telemIDs = ""
eventIDs = ""
paramIDs = ""

report = """
Component Interface Report:
--------- --------- ------
Input Ports: %d
Output Ports: %d
Commands: %d
Channels: %d
Events: %d
Parameters: %d

Use the below to tune the lookup algorithms.

Command Opcodes: %s
Telemetry IDs: %s
Event IDs: %s
Parameter IDs: %s

Stats:

Component,InputPorts,OutputPorts,Commands,Channels,Events,Parameters

"""


for file in sys.argv[1:]:
    print("Parsing file %s"%file)
    input_ports = 0
    output_ports = 0
    commands = 0
    channels = 0
    events = 0
    parameters = 0
    for line in open(file).readlines():
        
        if line.count("Input Ports:"):
            input_ports = int(line.split("Input Ports:")[1])
            total_input_ports += input_ports 
        if line.count("Output Ports:"):
            output_ports = int(line.split("Output Ports:")[1])
            total_output_ports += output_ports 
        if line.count("Commands:"):
            commands = int(line.split("Commands:")[1])
            total_commands += commands 
        if line.count("Channels:"):
            channels = int(line.split("Channels:")[1])
            total_channels += channels
        if line.count("Events:"):
            events = int(line.split("Events:")[1])
            total_events += events 
        if line.count("Parameters:"):
            parameters = int(line.split("Parameters:")[1])
            total_parameters += parameters 
            
        if line.count("OpCodes:"):
            opcodes += line.split("OpCodes:")[1].strip()+","
        if line.count("ChanIds:"):
            telemIDs = telemIDs + line.split("ChanIds:")[1].strip()+","
        if line.count("EventIds:"):
            eventIDs = eventIDs + line.split("EventIds:")[1].strip()+","
        if line.count("ParamIds:"):
            paramIDs = paramIDs + line.split("ParamIds:")[1].strip()+","

    report += "%s,%d,%d,%d,%d,%d,%d\n" %(file.replace("ComponentReport.txt","").replace(os.environ["BUILD_ROOT"],""),
                   input_ports,output_ports,commands,channels,events,parameters) 
        
open("ComponentReport.txt","w").write(report%
          (total_input_ports,total_output_ports,total_commands,total_channels,total_events,total_parameters,
           opcodes[:-1],telemIDs[:-1],eventIDs,paramIDs))