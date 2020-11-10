"""
@brief Enumeration for event severities

@date Created July 19, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from enum import Enum


class EventSeverity(Enum):
    COMMAND = 1
    ACTIVITY_LO = 2
    ACTIVITY_HI = 3
    WARNING_LO = 4
    WARNING_HI = 5
    DIAGNOSTIC = 6
    FATAL = 7
