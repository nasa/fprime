# GDS Integration Test API User Guide
The GDS Integration Test API is a GDS Tool that provides useful functions and asserts for creating integration-level tests on an FPrime deployment. This document hopes to give a high-level overview of the main features associated with the Test API and demonstrate common use patterns and highlight some anti-patterns.

## Quick Start
Presently, instantiating and using the API is not ideal. To work with the Integration Test API, the user must first create an instance of the StandardPipeline and then instantiate the API. This is boiler plate code that should be [moved inside the TestAPI](#moving-standardpipeline-to-api-constructor). The following code snippet accomplishes directing the GDS to a deployment dictionary, connecting to a running deployment, and finally instantiating the test API. This snippet DOES NOT run the GDS TCP Server or run an FPrime deployment. An example script to run the Ref App deployment without a GDS Tool can be found [here](../../../Ref/scripts/run_ref_for_int_test.sh).

~~~~{.python}
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.testing_fw import predicates   # Recommended, but not required

# instantiate the GDS and connect to the Deployment
pipeline = StandardPipeline()
config = ConfigManager()
dict_path = "/path/to/AppDictionary.xml"
pipeline.setup(config, dict_path)
pipeline.connect("127.0.0.1", 50000)

# instantiate Test API
log_path = "/path/to/api/output/directory"
api = IntegrationTestAPI(pipeline, log_path)

# user can now use the Test API
api.assert_telemetry("SOME_CHANNEL_MNEMONIC")
~~~~

### How to use the API with a test framework
To use the test API with a testing framework like pyunit or pytest

## Organization

### API Outline

## API Features the user should be aware of

### Specifying Search Scope (start, timeout)

### Substituting a history (history)

### Data object specifiers

### Types of searches (item, count, sequence)

### API Test Log

### Assert Helpers

## Usage Patterns

### Using history markers

### Using send and assert

### Using predicates effectively

### Using sub-histories

### Search returns

### Using TimeTypes

## Anti-patterns

### Asserting None

### Specifying sequence timeStamps

### Search order

## API Usage Requirements

## Idiosyncrasies
In this document, idiosyncrasies refer to improvements, issues, and future features that should be in the Test API. The API in its present state is functional, but these were identified as nice-to-haves or potential issues to be revised later.

### Timeout implementation

### Better History Markers (future)

### Implementing ERT ordering in Chronological History (future)

### Adding CSV Logger to Test Log (potential issue)

### Color-coding interlaced Events in the API Log

### Moving StandardPipeline to API constructor
Presently, a user of the Integration Test API needs to instantiate the GDS manually before instantiating the API. This code should really be moved to inside the API. To do this, the IntegrationTestAPI's [constructor](https://github.jpl.nasa.gov/FPRIME/fprime-sw/blob/d0309a9e265b8650ca6be03b9132dfdc682e0622/Gds/src/fprime_gds/common/testing_fw/api.py#L27) should be modified to include the pipeline instantiation.

#### Modification to the Integration Test API

~~~~{.python}
# import statements on ~line 17 of api.py with other GDS includes
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager

# ~ Integration Test API constructor on ~line 29 of api.py
def __init__(self, dict_path, address, port, log_prefix=None, fsw_order=True):
    """
    Initializes API: constructs and registers test histories.
    Args:
        dict_path: dictionary path (str). Used to setup loading of dictionaries.
        address: address of middleware (str)
        port: port of middleware (int)
        log_prefix: an optional output destination for the api test log
        fsw_order: a flag to determine whether the API histories will maintain FSW time order.
    """
    self.pipeline = StandardPipeline()
    self.pipeline.setup(ConfigManager(), dict_path)
    self.pipeline.connect(address, port)

    # ... rest of IntegrationTestAPI.__init__
~~~~

#### Cleaned up version of how instantiating could look

~~~~{.python}
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.testing_fw import predicates   # Recommended, but not required

# instantiate Test API
dict_path = "/path/to/AppDictionary.xml"
server_ip = "127.0.0.1"
port = 50000
log_prefix = "/path/to/api/output/directory"
api = IntegrationTestAPI( dict_path, server_ip, port, log_prefix)

# user can now use the Test API
api.assert_telemetry("SOME_CHANNEL_MNEMONIC")
~~~~

### Using GDS Prefix to output the test Logs (future)

## API Unit Tests

## Reference Application Integration Tests
