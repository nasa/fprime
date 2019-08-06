# GDS Integration Test API User Guide
The GDS Integration Test API is a GDS Tool that provides useful functions and asserts for creating integration-level tests on an FPrime deployment. This document hopes to give an overview of the main features associated with the Test API and demonstrates common use patterns and highlight some anti-patterns. See [this link](TODO) for the IntegrationTestAPI's sphinx-generated documentation.

## Quick Start
To work with the Integration Test API, the user must first create an instance of the StandardPipeline and then instantiate the API. This is boiler plate code that should be [moved inside the TestAPI](#moving-standardpipeline-to-api-constructor). The following code snippet accomplishes directing the GDS to a deployment dictionary, connecting to a running deployment, and finally instantiating the test API. This snippet <strong>DOES NOT</strong> run the GDS TCP Server or run an FPrime deployment. An example script to run the Ref App deployment without a GDS Tool can be found [here](../../../Ref/scripts/run_ref_for_int_test.sh).

~~~~{.python}
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.testing_fw import predicates   # Recommended, but not required

# instantiate the GDS and connect to the Deployment
pipeline = StandardPipeline()
dict_path = "/path/to/AppDictionary.xml"
pipeline.setup(ConfigManager(), dict_path)
pipeline.connect("127.0.0.1", 50000)

# instantiate Test API
log_path = "/path/to/api/output/directory"
api = IntegrationTestAPI(pipeline, log_path)

# user can now use the Test API
api.assert_telemetry("SOME_CHANNEL_MNEMONIC")
~~~~

### How to use the API with a test framework

To use the test API with a testing framework like unittest or pytest there are four methods that should be called.
<ol>
<li>First, the test framework should call a <strong>one-time</strong> setup method to instantiate the API and connect to the deployment.</li>
<li>Second, the framework should call a setup method for each test case to call the API's start_test_case method that clears histories, and logs messages to denote test-case boundaries.</li>
<li>Third, the framework should call any number of associated test cases.</li>
<li>Finally, the test framework should call a <strong>one-time</strong> teardown method to save the API at.</li>
</ol>

Below is an example of these steps using unittest. For an example of this using pytest, see the Ref App [integration tests](../../../Ref/test/int/ref_integration_test.py).

~~~~{.python}
import unittest
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.testing_fw.api import IntegrationTestAPI

class SomeTestCases(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # this is used to generate a case_id
        cls.case_list = []

        # instantiate the GDS and connect to the deployment
        cls.pipeline = StandardPipeline()
        dict_path = "/path/to/AppDictionary.xml"
        cls.pipeline.setup(ConfigManager(), dict_path)
        cls.pipeline.connect("127.0.0.1", 50000)

        # instantiate Test API
        log_path = "/path/to/api/output/directory"
        cls.api = IntegrationTestAPI(cls.pipeline, log_path)

    def setUp(self):
        count = len(self.case_list)
        self.api.start_test_case(self._testMethodName, count)
        self.case_list.append(1)

    @classmethod
    def tearDownClass(cls):
        cls.pipeline.disconnect()
        cls.api.teardown()

    def test_case_one(self):
        self.api.test_assert(True, "The user has successfully set up the API.")

    def test_case_two(self):
        self.api.test_assert(False, "The user has successfully failed a test case.")


# used when this unit test were to be run as a python module
if __name__ == "__main__":
    unittest.main()
~~~~

## Integration Test API Organization

### Integration Test API Outline

The actual Test API is a very long class that has helpful doc-strings, but these don't convey its organization while skimming. Below is a table of how the API is organized with a brief summary of each section:

| Test API Section| Section Description| Methods|
| :----| :----| :----|
| API Functions| These functions give access to helpful API features.| start_test_case, log, get_latest_time, test_assert, predicate_assert, clear_histories, set_event_log_filter|
| History Functions| These functions give the user access to test histories and the ability to create sub-histories.| get_command_test_history, get_telemetry_test_history, get_event_test_history, get_telemetry_subhistory, remove_telemetry_subhistory, get_event_subhistory, remove_event_subhistory|
| Command Functions| These functions provide the ability to send commands and search for events/telemetry.| translate_command_name, send_command, send_and_await_telemetry, send_and_await_event|
| Command Asserts| These functions send commands then perform search and asserts on the histories| send_and_assert_telemetry, send_and_assert_event|
| Telemetry Functions| These functions help specify and search for telemetry updates.| translate_telemetry_name, get_telemetry_pred, await_telemetry, await_telemetry_sequence, await_telemetry_count|
| Telemetry Asserts| These functions search and assert for telemetry updates.| assert_telemetry, assert_telemetry_sequence, assert_telemetry_count|
| Event Functions| These functions help specify and search for event messages.| translate_event_name, get_event_pred, await_event, await_event_sequence, await_event_count|
| Event Asserts| These functions search and assert for event messages.| assert_event, assert_event_sequence, assert_event_count|
| History Searches| These functions implement the various searches in the API. They aren't meant for the user, but are mentioned to highlight where searches are actually performed.| __search_test_history, find_history_item, find_history_sequence, find_history_count|

For detailed descriptions of the API's methods see the IntegrationTestAPI's sphinx documentation [here](TODO). One thing to note about the API's implementation is that the API uses layering so that all searches can be defined by common arguments and share similar behaviors. A diagram of this layering is provided below.

![Diagram of GDS Search Layering](assets/APISearchLayering.png)

This table outlines the additional functionality provided by each layer in the search hierarchy.

|Layer Name| Diagram Row| Delegated Functionality|
| :---| ---:| :---|
| Assert Layer| 1| This layer adds an assert to the end of the search to check if the search completed successfully.|
| Await Layer| 2| This layer differentiates whether the particular search is acting on the event or telemetry history.|
| Search Type Layer| 3| This layer determines what kind of search is being conducted. The API provides 3 types: item, count, and sequence.|
| Search Helper| 4| The search helper provides the logic, logging, search scoping. and sub-history functionality for all three searches.|

### Integration Test Classes

The API uses several classes to support its features. They were organized within the already-present GDS class folder structure. A component view of the Integration Test API and its relationship to the Integration Tests and the GDS is shown in the diagram below. For simplicity, the predicates library has been left out, but it can be used by Integration tests and is used by the Test API and Test History layers.
![Component View of the Test Framework](assets/TestFwComponentView.png)

## Important API Features

### Specifying Search Scope (start, timeout)
All searches in the Integration Test API rely on two common variables to define the scope of what is searched: start and timeout. Start is also used to specify the earliest item to remain when histories are only partially cleared.

The start argument specifies the starting point in an existing history. Start can be either an index in the history's ordering, a predicate, or a TimeType timestamp. Because the Test API's histories support re-ordering, the TimeType timestamp is the most reliable marker for start. A predicate can also be used to specify a start. For example, if the assert is only to begin after a certain EVR was received then an event_predicate instance could be used to find the first element to search. If start is not specified, see the particular API function to learn the default behavior.

The timeout argument specifies how long (in seconds) a search should await until the search criteria is met. Searches that await a yet-to-be-received item can only specify how long to await via timeout.

![Search Scope diagram](assets/APISearchScope.png)

#### Default Search Scope for await and assert calls
Because all search calls in the API can modify their search scope in the same way. The API doesn't use

### Substituting a history (history)

### Data object specifiers

### Types of searches (item, count, sequence)

### API Test Log

### Assert Helpers

### Predicates

A user of the Integration Test API should be familiar with the [predicates library](../../src/fprime_gds/common/testing_fw/predicates.py) used by the API. The API uses Duck Typing to determine what can and cannot be used as a predicate; therefore, user of the API can very easily create their own predicates. Below is a table of how predicates are organized with a brief summary of each section:

| Predicate Section| Section Description| Functions/predicates|
| :----| :----| :----|
| Base class/helpers| This section contains the parent class for predicates and helpers to carry out duck-typing and string formatting.| class predicate, is_predicate(), get_descriptive_string()|
| Comparison Predicates| These predicates carry out basic rich-comparisons (<, =, >, !=).| less_than, greater_than, equal_to, not_equal_to, less_than_or_equal_to, greater_than_or_equal_to, within_range|
| Set Predicates| These predicates evaluate whether predicates belong to a set of objects.| is_a_member_of, is_not_a_member_of|
| Logic Predicates| These predicates can be used to combine/manipulate other predicates with basic boolean logic.| always_true, invert (not), satisfies_all (and), satisfies_any (or)|
| Test API Predicates | These predicates operate specifically on the fields on the ChData and EventData objects. They are used by the API to specify event and telemetry messages. | args_predicate, event_predicate, telemetry_predicate|


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

## Known bugs


## Idiosyncrasies
In this document, idiosyncrasies refer to needed-improvements and future features that should/could be in the Test API. The API in its present state is functional, but these were identified as nice-to-haves or potential issues to be revised later.

### Timeout implementation

### Better History Markers (future)

### Implementing ERT ordering in Chronological History and in the GDS (future)

### Adding CSV Logger to Test Log

### Color-coding interlaced Events in the API Log

### Moving StandardPipeline to API constructor
Presently, a user of the Integration Test API needs to instantiate the GDS manually before instantiating the API. This code should really be moved to inside the API. To do this, the IntegrationTestAPI's [constructor](https://github.jpl.nasa.gov/FPRIME/fprime-sw/blob/d0309a9e265b8650ca6be03b9132dfdc682e0622/Gds/src/fprime_gds/common/testing_fw/api.py#L27) should be modified to include the pipeline instantiation and the API's [teardown](https://github.jpl.nasa.gov/FPRIME/fprime-sw/blob/d0309a9e265b8650ca6be03b9132dfdc682e0622/Gds/src/fprime_gds/common/testing_fw/api.py#L64) method should be modified to disconnect from the FPrime deployment.

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
    # add these lines
    self.pipeline = StandardPipeline()
    self.pipeline.setup(ConfigManager(), dict_path)
    self.pipeline.connect(address, port)

    # ... the rest of IntegrationTestAPI.__init__

# ~ Integration Test API constructor on ~line 73 of api.py
def teardown(self):
    """
    To be called once at the end of the API's use. Disconnects from the deployment,
    closes the test log, and clears histories.
    """
    # add this line
    self.pipeline.disconnect()
    # ... the rest of IntegrationTestAPI.teardown
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

### Better test markers using decorators

### FPrime CI/CD Test Runner

## API Unit Tests

## Reference Application Integration Tests
