"""
predicates.py:

This file contains basic predicates as well as event and telemetry predicates used by the gds_test_api.py

:author: koran
"""

##################
# Basic predicates
##################
class less_than_pred:
    """
    A predicate that evaluates if the argument is less than the given value
    """
    __init__(self, value):
        pass

class greater_than_pred:
    """
    A predicate that evaluates if the argument is greater than the given value
    """
    __init__(self, value):
        pass

class equal_to_pred:
    """
    A predicate that evaluates if the argument is equal to the given value
    """
    __init__(self, value):
        pass

class not_equal_to_pred:
    """
    A predicate that evaluates if the argument is not equal to the given value
    """
    __init__(self, value):
        pass

class greater_than_or_equals_pred:
    """
    A predicate that evaluates if the argument is greater than or equal to the given value
    """
    __init__(self, value):
        pass

class less_than_or_equals_pred:
    """
    A predicate that evaluates if the argument is greater than or equal to the given value
    """
    __init__(self, value):
        pass
        
class within_range_pred:
    """
    A predicate that evaluates if the argument is between the two values
    """
    __init__(self, lower, upper):
        pass

#########
# Set Predicates
#########
class is_a_member_of_pred:
    """
    A predicate that evaluates if the argument is equivalent to any member in the set
    """
    __init__(self, collection):
        pass

class is_not_a_member_of:
    """
    A predicate that evaluates if the argument is not equivalent to all members in the set
    """
    __init__(self, collection):
        pass

###########
# Predicates that modify and combine other basic predicates
###########
class not_pred:
    """
    A predicate that inverts a given predicate
    """
    __init__(self, pred):
        pass

class and_pred:
    """
    A predicate that evaluates if the argument satisfies all predicates in the given list
    """
    __init__(self, pred_list):
        pass

class or_pred:
    """
    A predicate that evaluates if the argument satisfies any predicates in the given list
    """
    __init__(self, pred_list):
        pass

############
# Predicates that support the test api
############
class event_predicate:
    """
    A predicate for specifying a EventData object from data_types.event_data
    This predicate can be used to search a history
    :param id_pred: If specified, the object's id field must satisfy the given predicate for the telemetry predicate to evaluate to true.
    :param id_value: If specified, the object's value_obj field must satisfy the given predicate for the telemetry predicate to evaluate to true.
    :param time_pred: If specified, the object's time field must satisfy the given predicate for the telemetry predicate to evaluate to true.
    """
    __init__(self, id_pred=None, args_pred=None, time_pred=None):
        pass

class telemetry_predicate:
    """
    A predicate for specifying a ChData object from data_types.ch_data
    This predicate can be used to search a history
    :param id_pred: If specified, the object's id field must satisfy the given predicate for the telemetry predicate to evaluate to true.
    :param id_value: If specified, the object's value_obj field must satisfy the given predicate for the telemetry predicate to evaluate to true.
    :param time_pred: If specified, the object's time field must satisfy the given predicate for the telemetry predicate to evaluate to true.
    """
    __init__(self, id_pred=None, value_pred=None, time_pred=None):
        pass


