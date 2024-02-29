# DpTool

This directory contains the component `DpTool`.  
The purpose is to generate Data Product binary files when the unit test is run.  
These binary files are used by the fprime-gds test for the `data_product_writer.py`  
In the fprime-gds repo copy the binary files to `test/fprime_gds/executables` and run `test_data_product_writer.py` which will generate the data in JSON format.

Assumptions:  
The F prime JSON dictionary has not yet been generated at the time of the tool development.  The Data product records and types have been hand generated into a JSON file called `dictionary.json`.  
In addition, the Data product header spec is captured in a JSON file called `dpspec.json`
