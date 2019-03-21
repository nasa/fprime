'''
@brief Enumeration for event severities

@date Created July 19, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

from enum import Enum

EventSeverity = Enum('Severity', 'COMMAND ACTIVITY_LO ACTIVITY_HI WARNING_LO WARNING_HI DIAGNOSTIC FATAL')
