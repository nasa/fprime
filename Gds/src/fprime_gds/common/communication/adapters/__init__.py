""" Provides communication adapters for communication with the spacecraft

Adapters are used to adapt the GDS communication layer to a specific communication format (driver). This package
provides two built in adapters: UART and IP used to communicate with a serial device and using IP stack respectively.

If a user wishes to use a different communication format, subclass and implement abstract functions defined by the
BaseAdapter class found in the base module.
"""
