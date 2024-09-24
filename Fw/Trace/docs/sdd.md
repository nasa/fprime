\page FwTracePort Fw::Trace Port
# Fw::Trace Port

## 1. Introduction
The `Fw::Trace` port is used to pass serialized trace log values with time tags to record diagnostic data that indicates the order of execution in software. 

## 2. Requirements
Trace Requirements in FPrime:
1. F’Prime shall provide a feature called Trace to log diagnostic information on various points of software execution. 
	- It is a way to create time correlation events as a reaction for resetting software. 
2. Trace shall provide a port to log the following:
	- Event ID : A unique identifier recording a “Trace Event”
	- Time Tag : Captures a high precision timestamp
	- Client Data : A small sample of user arguments recorded into trace buffer
3. Trace shall create trace points and log events in software automatically for thread switching(?) and  ...TBD, ask Tim
4. Trace shall provide a port for other components or user defined points to log events during software execution.
5. Trace shall record log events in an in-memory logger with data products to dump the buffers
6. Trace shall store data on a per thread basis so there is only one write to the buffer, preventing a need for mutual exclusion.
7. Trace shall maintain a double buffer, one for logs collected in the previous run before the latest reset and the other for current autopsy events. This way during init a data product can be generated for the trace events from prior run while actively collecting current trace logs. 
8. Trace logs shall be stored in recoverable memory so they can be preserved over resets
9. Trace shall write the trace data (stored in buffers) to data products upon boot initialization, a reset and by ground command
10. Trace shall provide a ground command to dump trace logs upon request. 
11. Trace shall provide a command to enable/disable Trace logging as it can consume significant code and processing 
12. Trace entries shall be put in a dictionary so ground systems can automatically decode trace data

