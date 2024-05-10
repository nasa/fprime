# Predicates

The Test API uses predicates to specify data objects and filter data objects.

predicates.py:

This file contains basic predicates as well as event and telemetry predicates used by the
gds_test_api.py. The predicates are organized by type and can be used to search histories.


* **author**

    koran



#### fprime_gds.common.testing_fw.predicates.is_predicate(pred)
a helper function to determine if an object can be used as a predicate.


* **Returns**

    a boolean value of whether the function is a predicate instance or has
    both __str__ and __call__ methods.



#### fprime_gds.common.testing_fw.predicates.get_descriptive_string(value, predicate)
a helper function that formats a predicate and argument in a nice human-readable format

Args:

    value: the argument of the predicate
    predicate: a predicate function

|Quick Links|
|:----------|
|[Integration Test API User Guide](../user_guide.md)|
|[GDS Overview](https://github.com/fprime-community/fprime-gds)|
|[Integration Test API](integration_test_api.md)|
|[Histories](histories.md)|
|[Predicates](predicates.md)|
|[Test Logger](test_logger.md)|
|[Standard Pipeline](standard_pipeline.md)|
|[TimeType Serializable](time_type.md)|
