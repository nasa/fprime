GSE Utilities
=============

This directory contains python utilities that can be used to test deployments. In particular 2 APIs are provided: 1) `gse_api.py` which allows a developer to send commands and receive events and telemetry from a running topology and 2) `test_api.py` which is an integration test framework built on top of `gse_api.py`.

Contents
--------

1. **history.py**: This is a generic history class which wraps a python dictionary whose keys are names and whose values are a list of any type. This class provides many assertion methods to check the size and value of elements in the history.

2. **test_history.py**: This class extends `history.py` by providing a test history interface tailored to telemetry and event checking. The history can be filled with telemetry and events and assertions can be made against the size or value of the added events or telemetry points.

3. **gse_api.py**: This API provides a developer with methods to send commands to and receive telemetry and events from a running topology over a socket interface.

4. **test_api.py**: This API extends `test_history.py` and `gse_api` to provide an integration testing framework. A developer can send commands, wait for certain conditions to be met within the telemetry and event history, and run assertions on the received telemetry and events.

Installation
------------
To use these utilities you will need a working python 2.7 installation. The recommended python environment setup is outlined [here](https://github.jpl.nasa.gov/ASTERIA/FSW/blob/master/Gse/README.md).

Using the GSE API
-----------------
To best see the methods available in the GSE API, install [pydoc](https://docs.python.org/2/library/pydoc.html) and run:

    pydoc test_api

Using the Test API
------------------
To best see the methods available in the TEST API, install [pydoc](https://docs.python.org/2/library/pydoc.html) and run:

    pydoc test_api
