# GDS Integration Test API User Guide

The GDS integration test API is a GDS Tool that provides useful functions and asserts for creating integration-level tests on an F Prime deployment. This document hopes to give an overview of the main features associated with the Test API and demonstrates common use patterns and highlight some anti-patterns. See [this link](markdown/contents.md) for the IntegrationTestAPI's sphinx-generated documentation.

This integration test API was developed by Kevin Oran in the summer of 2019.

## Quick Start

***

To work with the integration test API, the user must first create an instance of the StandardPipeline and then instantiate the API. This is boiler plate code that should be [moved inside the TestAPI](#moving-standardpipeline-to-api-constructor). The following code snippet accomplishes directing the GDS to a deployment dictionary, connecting to a running deployment, and finally instantiating the test API. This snippet **DOES NOT** run the GDS TCP Server or run an F Prime deployment. An example script to run the Ref App deployment without a GDS Tool can be found [here](../../../../Ref/scripts/run_ref_for_int_test.sh).

~~~~{.python}
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.testing_fw import predicates   # Recommended, but not required

# instantiate the GDS and connect to the Deployment
pipeline = StandardPipeline()
dict_path = "/path/to/AppDictionary.xml"  # user must replace with an actual path
pipeline.setup(ConfigManager(), dict_path)
pipeline.connect("127.0.0.1", 50000)  # user must replace with actual address and port

# instantiate Test API
log_path = "/path/to/api/output/directory"  # user must replace with an actual path
api = IntegrationTestAPI(pipeline, log_path)

# user can now use the Test API
api.assert_telemetry("SOME_CHANNEL_MNEMONIC")
~~~~

### How to use the API with a test framework

To use the test API with a testing framework like unittest or pytest there are four methods that should be called.

1. First, the test framework should call a **one-time** setup method to instantiate the API and connect to the deployment.
2. Second, the framework should call a setup method for each test case to call the API's start_test_case method that clears histories, and logs messages to denote test-case boundaries.
3. Third, the framework should call any number of associated test cases.
4. Finally, the test framework should call a **one-time** teardown method to save the API at.

Below is an example of these steps using unittest. For an example of this using pytest, see the Ref App [integration tests](../../../../Ref/test/int/ref_integration_test.py).

~~~~{.python}
import unittest
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.testing_fw import predicates   # Recommended, but not required

class SomeTestCases(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # this is used to generate a case_id
        cls.case_list = []

        # instantiate the GDS and connect to the deployment
        cls.pipeline = StandardPipeline()
        dict_path = "/path/to/AppDictionary.xml"  # user must replace with an actual path
        cls.pipeline.setup(ConfigManager(), dict_path)
        cls.pipeline.connect("127.0.0.1", 50000)

        # instantiate Test API
        log_path = "/path/to/api/output/directory"  # user must replace with an actual path
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

## Usage Patterns

***

All usage patterns are written such that they would be compatible with the test framework example described above: each test case assumes that the histories were recently emptied and that the `self.api` field is a connected instance of the integration test API. For simplicity, usage examples will rely on mock flight software dictionaries that were used in the integration test API unit tests. This dictionary can be found [here](../../../../Gds/test/fprime_gds/common/testing_fw/UnitTestDictionary.xml).

If a user would like to play with the test API in a unit testing environment they could append their own test cases to the unit tests [here](../../../../Gds/test/fprime_gds/common/testing_fw/api_unit_test.py).
**NOTE**: there is no F Prime deployment in these unit tests so data objects need to be added manually.

If a user would like to experiment with integration tests on an actual F Prime application, they could modify the Ref app [integration tests](../../../../Ref/test/int/ref_integration_test.py).
**NOTE**: running integration tests requires building and running the Ref app.

### Sending Commands

The Integration Test API provides several methods for sending commands to the user. The most simple is the send_command method. **NOTE**: The command arguments [must be strings](#-GDS-arguments-should-allow-non-string-types) (str) instead of a literal.

~~~~{.python}
self.api.send_command("TEST_CMD_1") # sending a command via mnemonic
self.api.send_command(0x01)         # sending the same command via opcode

self.api.send_command("TEST_CMD_2", ["235", "43"]) # sending a command with arguments
~~~~

### Searching for Telemetry

The integration Test API provides several different [types of searches](#-types-of-searches). Using a telemetry_predicate will enable the user to better specify the fields of the ChData object to be searched for.  **NOTE**: all searches in the API will return the results of the search. This is so the user may perform additional checks on the results. Whether or not the search was successful is left to the user to check.

~~~~{.python}
# awaits a telemetry update on the Counter Channel
result = self.api.await_telemetry("Counter")

# same search, but using an id
result = self.api.await_telemetry(3)

# awaits a Counter update with a value of 8
result = self.api.await_telemetry("Counter", 8)

# searches for an existing telemetry update on the Counter Channel
result = self.api.await_telemetry("Counter", start=0, timeout=0)

# awaits for 7 telemetry updates that are not guaranteed to be in order
results = self.api.await_telemetry_count(7, "Counter")

ch_seq = []
for i in range(0,10):
    ch_seq.append(self.api.get_telemetry_pred("Counter", i))

# awaits for 10 Counter updates with the values 0 through 9 (inclusive).
# the resulting sequence must follow the history's enforced order
results = self.api.await_telemetry_sequence(ch_seq)
~~~~

### Searching for Events

The integration Test API provides several different [types of searches](#-types-of-searches). Using an event_predicate will enable the user to better specify the fields of the EventData object to be searched for.  **NOTE**: all searches in the API will return the results of the search. This is so the user may perform additional checks on the results. Whether or not the search was successful is left to the user to check.

~~~~{.python}
# awaits a "CommandReceived" event
result = self.api.await_event("CommandReceived")

# same search, but using an id
result = self.api.await_event(0x01)

# awaits a "CommandReceived" event with arguments that match
result = self.api.await_event("CommandReceived", [0x01])

# searches for an existing "CommandReceived" event
result = self.api.await_event("CommandReceived", start=0, timeout=0)

# awaits for any 7 events updates that are not guaranteed to be in order
results = self.api.await_event_count(7)

evr_seq = []
for i in range(0,10):
    evr_seq.append(self.api.get_event_pred("CommandReceived", [i]))

# awaits for 10 "CommandReceived" events with the argument values 0 through 9 (inclusive).
# the resulting sequence must follow the history's enforced order
results = self.api.await_event_sequence(evr_seq)
~~~~

### Asserting on Telemetry

The integration Test API provides several different [types of searches](#-types-of-searches) that can be followed by an assert on whether the search succeeded. Using a telemetry_predicate will enable the user to better specify the fields of the ChData object to be searched for.

**NOTE**: all successful search-then-assert calls in the API will return the results of the search. This is so the user may perform additional checks on the results. Because an assertion is raised on search failure, the user can be sure the results reflect a successful test.

~~~~{.python}
# asserts a telemetry update exists in the current history
result = self.api.assert_telemetry("Counter")

# same search, but using an id
result = self.api.assert_telemetry(3)

# asserts a "Counter" update with a value of 8 exists in the current history
result = self.api.assert_telemetry("Counter", 8)

# awaits and asserts a "Counter" update was received
result = self.api.assert_telemetry("Counter", start="END", timeout=5)

# asserts a count of exactly 7 "Counter" updates exist in the current history
results = self.api.assert_telemetry_count(7, "Counter")

ch_seq = []
for i in range(0,10):
    ch_seq.append(self.api.get_telemetry_pred("Counter", i))

# asserts the history contains a sequence of "Counter" updates with vals 0 through 9
# the resulting sequence must follow the history's enforced order
results = self.api.assert_telemetry_sequence(ch_seq)
~~~~

### Asserting on Events

The integration Test API provides several different [types of searches](#-types-of-searches) that can be followed by an assert on whether the search succeeded. Using an event_predicate will enable the user to better specify the fields of the EventData object to be searched for.

**NOTE**: all successful search-then-assert calls in the API will return the results of the search. This is so the user may perform additional checks on the results. Because an assertion is raised on search failure, the user can be sure the results reflect a successful test.

~~~~{.python}
# asserts a "CommandReceived" event is in the history
result = self.api.assert_event("CommandReceived")

# same search, but using an id
result = self.api.assert_event(0x01)

# asserts a "CommandReceived" event with arguments is in the history
result = self.api.assert_event("CommandReceived", [0x01])

# awaits and asserts on a single "CommandReceived" event
result = self.api.assert_event("CommandReceived", start="END", timeout=5)

# asserts that exactly 7 of any event are in the history
results = self.api.assert_event_count(7)

evr_seq = []
for i in range(0,10):
    evr_seq.append(self.api.get_event_pred("CommandReceived", [i]))

# asserts tha history has a sequence of 10 "CommandReceived" events with the argument vals 0 through 9 (inclusive).
# the resulting sequence must follow the history's enforced order
results = self.api.assert_event_sequence(evr_seq)
~~~~

### Sending and Searching/Asserting

The Test API provides 4 versions of send and search to enable searching for telemetry and events following a command. Internally, these calls record the current position of the history, then send a command then begin a search from the recorded position. All 4 versions support both item and sequence searches depending on whether the channels/events arguments are a list.

~~~~{.python}
# sends "TEST_CMD_1" then awaits a "CommandCounter" channel update
result = self.api.send_and_await_telemetry("TEST_CMD_1", channels="CommandCounter")

# sends "TEST_CMD_1" then awaits a sequence of "CommandCounter" then five "Counter" updates
seq = ["CommandCounter"] + ["Counter"] * 5
results = self.api.send_and_await_telemetry("TEST_CMD_1", channels=seq)

# sends "TEST_CMD_1" then awaits and asserts a "CommandCounter" channel update
result = self.api.send_and_assert_telemetry("TEST_CMD_1", channels="CommandCounter")

# sends "TEST_CMD_1" then awaits a "CommandReceived" event
result = self.api.send_and_await_event("TEST_CMD_1", events="CommandReceived")

# sends "TEST_CMD_1" then awaits a sequence of "CommandReceived" then five "SeverityDIAGNOSTIC" events
seq = ["CommandReceived"] + ["SeverityDIAGNOSTIC"] * 5
results = self.api.send_and_await_event("TEST_CMD_1", events=seq)

# sends "TEST_CMD_1" then awaits and asserts "CommandReceived" event
result = self.api.send_and_assert_event("TEST_CMD_1", events="CommandReceived")
~~~~

### Using predicates

The API uses predicates to identify valid values in searches and filter data objects into histories.
The provided [predicates](#-predicates) can be combined to make specifying an event message or channel update incredibly flexible. When using predicates, it is important to understand that a predicate is used to determine if a value belongs to a set of values that satisfies a rule. Not satisfying a rule [**DOES NOT** imply](#-Interpreting-predicates-correctly) that a value satisfies a second complementary rule.

#### Combining Predicates

One pattern is to have multiple predicate specifications and want to combine these.

~~~~{.python}
from fprime_gds.common.testing_fw import predicates

gt_pred = predicates.greater_than(8)
eq_pred = predicates.equal_to("some_string")

# satisfies any will evaluate true if any of it's predicates are valid
or_pred = predicates.satisfies_any([gt_pred, eq_pred])

or_pred(121)           # evaluates True
or_pred("some_string") # evaluates True

rng_pred = predicates.within_range(0, 100)
ne_pred = predicates.not_equal_to(50)

# a valid value must be within the range 0 to 100 and must not be 50.
and_pred = predicates.satisfies_all([rng_pred, ne_pred])

or_pred(15) # evaluates True
or_pred(50) # evaluates False
~~~~

#### Set Predicates

Another pattern is to specify a collection and check if the value is a member of that collection.

~~~~{.python}
from fprime_gds.common.testing_fw import predicates

is_in_pred = predicates.is_a_member_of(["A", 2, False])
isnt_in_pred = predicates.is_not_a_member_of(["A", 3])

is_in_pred(2)     # evaluates True
is_in_pred(False) # evaluates True

is_in_pred("A")   # evaluates True
isnt_in_pred("A") # evaluates False
~~~~

This pattern is useful for creating filters. For example, if we want to search or filter for certain event severities.

~~~~{.python}
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity

severities = []
severities.append(EventSeverity.FATAL)
severities.append(EventSeverity.WARNING_HI)
sev_pred = predicates.is_a_member_of(severities)

# event pred will now identify any event with either Fatal or HI Warning severity
event_pred = self.api.get_event_pred(severity=sev_pred)
~~~~

#### Specifying data objects

The test API has two methods to help create event and telemetry predicates: `api.get_telemetry_pred` and `api.get_event_pred`. These methods overload argument types such that fields can be specified as a value (becomes an equal_to predicate) or they can be specified by user-created predicates. To specify the type of event/telemetry, the helpers can accept both mnemonics (str) or ids (int).

~~~~{.python}
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity

# both predicates will now identify any event with a command severity
sev_pred = predicates.equal_to(EventSeverity.COMMAND)
event_pred1 = self.api.get_event_pred(severity=sev_pred)
event_pred2 = self.api.get_event_pred(severity=EventSeverity.COMMAND)

# both predicates will now identify any "CommandCounter" Update
ch_pred1 = self.api.get_telemetry_pred("CommandCounter")
ch_pred2 = self.api.get_telemetry_pred(1)
~~~~

### Using sub-histories

One pattern that the API supports is creating a sub-history of telemetry or event objects. There are several [behaviors](#-Substituting-a-history-(history-argument)) to understand with sub-histories that are outlined in the API features section. Below is an example of how to create sub-histories, search on sub-histories, and remove sub-histories. Sub-histories can be created for both telemetry and event data objects.

~~~~{.python}
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity

# Creates an event sub-history with the default object ordering (fsw_order).
fsw_subhist = self.api.get_event_subhistory()

# Creates a filtered sub-history with all events of COMMAND severity
event_filter = self.api.get_event_pred(severity=EventSeverity.COMMAND)
filt_subhist = self.api.get_event_subhistory(event_filter)

# Creates an event sub-history with ERT ordering
ert_subhist = self.api.get_event_subhistory(fsw_order=False)

# Substitutes a sub-history into an API assert
result = self.api.assert_event("SeverityCOMMAND", history=filt_subhist)

# If a sub-history hasn't been removed. It can also be awaited on.
results = self.api.await_event_count(5, history=fsw_subhist)

# De-register a sub-history from the GDS
self.api.remove_event_subhistory(ert_subhist)
~~~~

### Search returns

API calls that perform a search and do not end by raising an Assertion Error will return the results of the search. This is so that the user can find some event or channel updates then perform additional checks on the results or use the results to specify a future search.

Here is an example of awaiting a counter sequence and verifying that the sequence always ascends.

~~~~{.python}
search_seq = ["Counter"] * 5
results = self.api.await_telemetry_sequence(search_seq)

last = None
for update in results:
    if last is not None:
        assert update.get_val() > last.get_val()
    last = update
~~~~

### Assert Helpers

Another feature provided to the user is the ability to raise asserts with formatted assert messages reflected in the test logs.

~~~~{.python}
from fprime_gds.common.testing_fw import predicates

# assert on values that can be evaluated as True or False
self.api.test_assert(2 < 3, "The number two should be less than three")

# assert a predicate on a value the log message will be more descriptive.
lt_pred = predicates.less_than(3)
self.api.predicate_assert(lt_pred, 2, "The number two should be less than three")
~~~~

Assert helpers can be configured not to raise an assertion error. They will also return True if the assertion passed or False if it failed. This can be used to perform multiple checks. This behavior is referred to as expecting instead of asserting.

~~~~{.python}
# a variable to accumulate whether all checks were successful
all_passed = True
all_passed &= self.api.test_assert(1 < 3, "1 should be less than 3", expect=True)
all_passed &= self.api.test_assert(2 < 3, "2 should be less than 3", expect=True)
# this call will not raise an assert, but will return False
all_passed &= self.api.test_assert(3 < 3, "3 should not be less than 3", expect=True)

# checks that previous expectations passed.
self.api.test_assert(all_passed, "All checks should have passed, see log")
~~~~

### Using TimeTypes

The TimeType serializable stores timestamp information for both events and telemetry. As part of the development for the integration test API, the TimeType object was updated to support rich comparison and math operations. These are implemented with python special methods and are compatible with floating point numbers.

**NOTE**: Math operations will return a new TimeType object with the resulting value and the TimeType serializable does not allow negative values.

**NOTE**: Math operations between TimeType objects of different time_bases or time_context will return a TimeType with the same base and context as the left operand.

~~~~{.python}
from fprime.common.models.serialize.time_type import TimeType

t0 = TimeType() # 0.0 seconds

t1 = t0 + 1   # Assigns a TimeType with a time of 1.0 seconds
t3 = t0 + 3   # Assigns a TimeType with a time of 3.0 seconds
t2 = t3 - t1  # Assigns a TimeType with a time of 2.0 seconds
t0 = t1 - t3  # Assigns a TimeType with a time of 0.0 seconds (negatives are set to 0)
t15 = t3 / 2  # Assigns a TimeType with a time of 1.5 seconds
t6 =  t2 * t3 # Assigns a TimeType with a time of 6.0 seconds


t1 > 0   # evaluates True
t1 > t0  # evaluates True
t6 == 6  # evaluates True
t3 >= t2 # evaluates True
~~~~

Accessing TimeStamps from event and channel data types can be done with the `get_time()` getter. These comparisons can be very useful in testing whether FSW meets timing requirements.

~~~~{.python}
seq = ["Counter"] * 5
results = self.api.await_telemetry_sequence(seq)

# checks that all adjacent elements in the sequence happened within 2 seconds of each other
last = None
for result in results:
    if last is not None:
        assert result.get_time() - last.get_time() < 2
    last = result
~~~~

### Recording a point in the histories

**NOTE** There is an [issue](#-Latest-FSW-Time-Getter-is-incorrect) with how get_latest_time() is implemented. Getting history markers should be updated to be more robust.

If a user wants to record a marker, send some commands and then come back and evaluate items after that marker, then they can do the following: This all will return the latest FSW TimeStamp when the histories are ordered by FSW time.

~~~~{.python}
# if using time-ordered histories
fsw_start = self.api.get_latest_time()

# do some stuff
self.api.send_command("TEST_CMD_1")

# search
results = self.api.assert_telemetry("Counter", start=fsw_start)
~~~~

If using receive-ordered histories, this point should be marked as an index.

~~~~{.python}
# if using re-ordered histories
ro_start = self.api.get_telemetry_test_history().size()

# do some stuff
self.api.send_command("TEST_CMD_1")

# search
results = self.api.assert_telemetry("Counter", start=ro_start)
~~~~

## Anti-patterns

***

### Asserting none of a data object were received

One thing a user might want to do is assert that no instances of a certain update or message were received. This can be done using a count search for zero items on **existing history items only**.  This constraint is because the count search behaves as follows:

1. count all items in the search scope of the current history
2. await future updates until a correct count is received.

So, if count search is awaiting zero items, it will exit immediately and claim success where the user may believe it searched future objects.

~~~~{.python}
# incorrect, this will exit as the search found 0 items and was looking for 0 items
self.api.assert_telemetry_count(0, start="END", timeout=5)
~~~~

If the user wants to assert that none of a certain type of object were received in the future scope, they should wait for items to accumulate then assert on existing scope.

~~~~{.python}
import time

# correct, way to say no telemetry was received in now or in the next 5 seconds.
time.sleep(5)
self.api.assert_telemetry_count(0)
~~~~

### Specifying sequence searches with timestamps

The doc-strings in the API recommend not specifying FSW timestamps when searching for sequences. This is simply because the timestamps can change depending on when tests are run. The easiest way to verify timing is to process timestamps after a search is completed.

### No-scope search

Because searches allow the user to define the [existing and future scope to search](), it is possible to completely de-scope a search.

~~~~{.python}
# setting timeout to zero on await functions w/o a start, results in no scope
self.api.await_telemetry_count(5, timeout=0)

# setting start to END on assert functions w/o a timeout, results in no scope
self.api.assert_telemetry_count(5, start="END")

# setting start to END and timeout to zero on any search, results in no scope
self.api.assert_telemetry_count(5, start="END", timeout=0)
self.api.await_telemetry_count(5, start="END", timeout=0)
~~~~

### Interpreting predicates correctly

Predicates may compare a value to another, but their purpose isn't to compare two objects, rather to identify objects that satisfy a certain property or rule. If a user uses a greater_than predicate to see if a string is greater than a numeric value, 8, the predicate will return False. The correct interpretation is that the string is not in the set of values that are greater than 8. It is incorrect to say the string is less than 8.

~~~~{.python}
from fprime_gds.common.testing_fw import predicates

gt_pred = predicates.greater_than(8)
gt_pred(9)        # evaluates True
gt_pred(7)        # evaluates False
gt_pred("string") # evaluates False: String is not a value that is greater than 8

lte_pred = predicates.less_than_or_equal_to(8)
lte_pred(8)        # evaluates True
lte_pred(7)        # evaluates True
lte_pred("string") # evaluates False: String is not a value that is less than 8
~~~~

**Takeaway**: using invert to try to convert a greater_then predicate to a less_than_or_equal_to predicate will introduce false positives if the user isn't aware of what a predicate is describing

~~~~{.python}
from fprime_gds.common.testing_fw import predicates

gt_pred = predicates.greater_than(8)
lte_pred = predicates.less_than_or_equal_to(8)
not_lte_pred = predicates.invert(lte_pred) # inverts how a predicate evaluates.

not_lte_pred("string") # evaluates True: because "string" is not a value that is less than 8
gt_pred("string") # evaluates False: String is not a value that is greater than 8
~~~~

## API Installation Requirements

***

The following libraries were added to the [GDS pip requirements file](../../../../mk/python/pip_required_gds.txt).

| Library| Provides|
| :--| :--|
|openpyxl| ability to create formatted .xlsx files|
|sphinx| ability to generate code documentation
|sphinx-markdown-builder| ability to output code docs as markdown files|

## Integration Test API Organization

***

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

For detailed descriptions of the API's methods see the IntegrationTestAPI's sphinx documentation [here](markdown/contents.md). One thing to note about the API's implementation is that the API uses layering so that all searches can be defined by common arguments and share similar behaviors. A diagram of this layering is provided below. In the diagram, each box is an API call. The arrows show how the calls are layered.

![Diagram of GDS Search Layering](assets/APISearchLayering.png)

The table below outlines the additional functionality provided by each layer in above diagram.

|Layer Name| Diagram Row| Delegated Functionality|
| :---| ---:| :---|
| Assert Layer| 1| This layer adds an assert to the end of the search to check if the search completed successfully.|
| Await Layer| 2| This layer differentiates whether the particular search is acting on the event or telemetry history.|
| Search Type Layer| 3| This layer determines what kind of search is being conducted. The API provides 3 types: item, count, and sequence.|
| Search Helper| 4| The search helper provides the logic, logging, search scoping. and sub-history functionality for all searches.|

### Integration Test Classes

The API uses several classes to support its features. They were organized within the already-present GDS class folder structure. A component view of the integration test API and its relationship to the Integration Tests and the GDS is shown in the diagram below. For simplicity, the predicates library has been left out, but it can be used by Integration tests and is presently used by the Test API and Test History layers.
![Component View of the Test Framework](assets/TestFwComponentView.png)

## Important API Features

***

### Specifying Search Scope (start and timeout arguments)

All searches in the integration test API can be configured to search part of the existing history (Current Search Scope) and/or part of the future history (Future Search Scope). The API relies on two common variables to define the scope of what is searched: `start` and `timeout`.

The `start` argument specifies the Current Search Scope in an existing history. `start` is used to choose the earliest item that the search will evaluate in a given history's ordering. `start` can be either an index in the history's ordering, a predicate, or a TimeType timestamp. Because the Test API's histories support re-ordering, the TimeType timestamp is the most reliable marker for `start`. A predicate can also be used to specify a `start`. For example, if the assert is only to begin after a certain EVR was received then an event_predicate instance could be used to find the first element to search. If `start` is not specified, see the particular API function to learn the default behavior. For convenience, the API includes a member variable, `NOW`, that will begin the search after all existing history when `NOW` is passed as the `start` argument.

The `timeout` argument specifies the Future Search Scope (FSS) in seconds. FSS is how long a search should await until the search criteria is met. Searches that await a yet-to-be-received item can only specify how long in seconds. A `timeout` of zero seconds will skip awaiting at all.

![Search Scope diagram](assets/APISearchScope.png)

All search methods can either be configured with CSS, FSS, or both.

### Types of searches

The integration test API defines three types of searches: item, count, and sequence. Each of these searches has an [assert version and an await version](#default-search-scope-for-await-and-assert-calls) as well as a version for both telemetry and event versions. This means the API has a total of 12 search calls.

| Search Type| Description|
| :---| :---|
| item search| an item search is searching for a specific item in the history. The result will return a single data object|
| count search| a count search is searching for a number of items in history. The result is a group of items that isn't necessarily ordered according to the history. |
| sequence search| a sequence search is searching for an ordered sequence of items in the history. The result is an ordered list of items that is ordered with respect to the given history's order.|

#### Default Search Scope for await and assert calls

The integration test API provides to versions of each type of search: the await version will always return results and the assert version will search and then assert on whether the search completed successfully. For any given combination of search type (item, count, sequence) and search behavior (await or assert), the API allows the user to reconfigure the search scope with CSS, FSS, or both. Because all search calls in the API can manipulate this scope, the API doesn't provide different combinations of search scopes as different API calls. However, the API does provide default arguments for its searches and name its searches accordingly. This was done to satisfy the common request to support await functionality explicitly even though this behavior is already being provided by all search types.

| Prefix| Default Behavior|
| :---| :---|
| `await_`| By default, all search-only calls begin with `await_` and will only search for future data objects for at most 5 seconds|
| `assert_`| By default, all search and assert calls begin with `assert_` and will search all current data objects from the beginning of the history|

### Substituting a history (history argument)

Another useful feature in the integration test API is the ability to create filtered sub-histories and substitute them into any regular API call. This feature provides the user with the ability to manage their own histories. The API methods that support this are get_telemetry_subhistory, remove_telemetry_subhistory, get_event_subhistory, remove_event_subhistory. There are several behaviors to know when creating sub-histories.

- When creating a sub-history, the get_ calls allow the user to specify whether the history will be ordered by receive order or by flight software time (FSW) order. This is done with the `fsw_order` argument (ordered by FSW time is default).
- When creating a sub-history, the get_ calls allow the user to optionally specify a predicate filter to determine which items to allow into the sub-history (allows all by default). These filters should be a predicate composed of either telemetry_predicate's or event_predicate's depending on the type of sub-history.
- A new subhistory WILL be registered with the GDS to automatically receive data objects from its respective decoder (event/telemetry).
- A new subhistory will NOT be managed by the Test API. It will not be cleared nor de-registered when a test case ends.

Removing a sub-history is currently permanent as the API doesn't provide for sub-histories to be re-registered. Removing a sub-history will unsubscribe it from the GDS and it will no longer receive new data objects.

### Data object specifiers (event and channel arguments)

Throughout the API specifying an event message or channel update to search for is very flexible. This is because all search types use predicates to specify a single or multiple objects when searching. The word used to describe this overloading behavior in the API is `specifier`. The phrases to look for in doc-strings are channel specifiers and event specifiers.

Providing this flexibility in the `event` and `channel` arguments is done via the get_event_predicate and get_telemetry_predicate calls. These calls provide argument-overloading when specifying different fields of ChData or EventData objects. The way this works for most fields is that if the user specifies the field as a value, the returned predicate will check that data object's corresponding field against that value; however, if the user specifies the field as a predicate, then the user's predicate will be called on that field instead. This enables using the predicates library to do more complex checks on certain fields.

In addition to specifying values by value or predicate, the get_event_predicate and get_telemetry_predicate calls also access the deployment dictionary to allow the user to specify a data object type by mnemonic or ID.

### API Test Log

When an output location is specified, the integration test API will generate a formatted test log as a .xlsx file.
There are four columns in this file. They are summarized in the table below:

| Log Column| Description|
| :---| :---|
| Log Time| A time stamp of when the message was logged. Format "HH:mm:ss.us"|
| Case ID| An identifier for a test case. This field will help navigate when looking through long logs.|
| Sender| Describes where the message originated (GDS, Test API, API user). Note: all log messages are still made through API calls.|
| Message| A string message recording test behavior.

The following image is an excerpt from an API log generated by the Ref App integration tests.
![Test Log Example](assets/TestLogExample.png)

The following table summarizes the color meanings from API-generated messages.

| Color| Meaning|
| :---| :---|
| Red| red indicates failed asserts. If a red log message is from the Test API, then an assertion error was raised by an API call.|
| Orange| orange indicates failed expectations and warnings.|
| Yellow| yellow indicates the beginning and end of any search.|
| Green| green indicates successful asserts and expectations within API calls.|
| Blue| blue indicates interlaced EVR's.|
| Purple| purple indicates commands that were sent to flight software.|
| Gray| gray indicates the beginning of a new test case.|
| White| white or blank fill is used for diagnostic messages.|

### Predicates

The integration test API uses predicates for filtering, searching and asserting. A predicate is a callable class that evaluates if an object/value satisfies a certain property. Predicates used by the API are defined [here](../../../../Gds/src/fprime_gds/common/testing_fw/predicates.py). The API uses Duck Typing to determine what can and cannot be used as a predicate; therefore, a user of the API can very easily create their own. Below is a table of how predicates are organized with a brief summary of each section:

| Predicate Section| Section Description| Functions/predicates|
| :----| :----| :----|
| Base class/helpers| This section contains the parent class for predicates and helpers to carry out duck-typing and string formatting.| class predicate, is_predicate(), get_descriptive_string()|
| Comparison Predicates| These predicates evaluate basic comparison rules (<, =, >, !=).| less_than, greater_than, equal_to, not_equal_to, less_than_or_equal_to, greater_than_or_equal_to, within_range|
| Set Predicates| These predicates evaluate whether predicates belong to a set of objects.| is_a_member_of, is_not_a_member_of|
| Logic Predicates| These predicates can be used to combine/manipulate other predicates with basic boolean logic.| always_true, invert (not), satisfies_all (and), satisfies_any (or)|
| Test API Predicates| These predicates operate specifically on the fields on the ChData and EventData objects. They are used by the API to specify event and telemetry messages.| args_predicate, event_predicate, telemetry_predicate|

## Known bugs

***

### Latest FSW Time Getter is incorrect

Because it was demonstrated that data objects can come in a different receive order than they were created in FSW, the `get_latest_time()` method is not correct. When implemented, it was assumed that all history items would be enqueued in the same order that they were created. This is no longer a safe assumption. Effectively `get_latest_time()` is returns an approximation of the latest time.

I see two options to address this:

1. Replacing the Ram Histories in the GDS with Chronological Histories
2. Having the TestAPI be subscribed to all data objects and calculate latest time as items are enqueued.

### The openpyxl library has thrown WorkbookAlreadySaved error

While running unit tests on the API, there was an error thrown by openpyxl that caused the log to close early. The behavior wasn't able to be recreated, but the [Test Logger](../../../../Gds/src/fprime_gds/common/logger/test_logger.py) was updated to [catch the exception](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/logger/test_logger.py#L124) to prevent tests from failing due to the logger.

~~~~
___________________________________ APITestCases.test_find_history_item _________________________________

self = <WriteOnlyWorksheet "Sheet">, row = [<Cell 'Sheet'.A1>, <Cell 'Sheet'.A1>, <Cell 'Sheet'.A1>, <Cell 'Sheet'.A1>]

    def append(self, row):
        """
        :param row: iterable containing values to append
        :type row: iterable
        """

        if (not isgenerator(row) and
            not isinstance(row, (list, tuple, range))
            ):
            self._invalid_row(row)

        self._max_row += 1

        if self.writer is None:
            self.writer = self._write_header()
            next(self.writer)

        try:
>           self.writer.send(row)
E           StopIteration

/usr/lib/python3/dist-packages/openpyxl/writer/write_only.py:241: StopIteration

During handling of the above exception, another exception occurred:

self = <api_unit_test.APITestCases testMethod=test_find_history_item>

    def setUp(self):
        for t in self.threads:
            if t.isAlive():
                t.join()
        self.threads.clear()
        count = len(self.case_list)
>       self.api.start_test_case(self._testMethodName, count)

test/fprime_gds/common/testing_fw/api_unit_test.py:102:
_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
src/fprime_gds/common/testing_fw/api.py:96: in start_test_case
    self.__log(msg, TestLogger.GRAY, TestLogger.BOLD, case_id=case_id)
src/fprime_gds/common/testing_fw/api.py:1214: in __log
    self.logger.log_message(message, sender, color, style, case_id)
src/fprime_gds/common/logger/test_logger.py:121: in log_message
    self.worksheet.append(row)
/usr/lib/python3/dist-packages/openpyxl/writer/write_only.py:243: in append
    self._already_saved()
_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _

self = <WriteOnlyWorksheet "Sheet">

    def _already_saved(self):
>       raise WorkbookAlreadySaved('Workbook has already been saved and cannot be modified or saved anymore.')
E       openpyxl.utils.exceptions.WorkbookAlreadySaved: Workbook has already been saved and cannot be modified or saved anymore.

/usr/lib/python3/dist-packages/openpyxl/writer/write_only.py:247: WorkbookAlreadySaved
------------------------------------------ Captured stdout call ---------------------------------
10:46:08.703826 [Test API] [STARTING CASE] test_find_history_item
~~~~

To fully resolve this would require being able to reproduce the issue and explain why the test log failed. However, the test logger should at least have a reliable csv format that won't stop logging if the xml logger fails.

#### Adding CSV Logger to Test Logger

Recommendation for adding a csv logger to the TestLogger class:

1. Set up the csv log file in the constructor [here](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/logger/test_logger.py#L49).
2. Add a `_log_csv_row()` helper along similar lines to the `_get_ws_row()` helper [here](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/logger/test_logger.py#L159)
3. Log the start time at the top of the file like the excel output does [here](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/logger/test_logger.py#L85).
4. Log the column headers to csv like the excel does [here](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/logger/test_logger.py#L88).
5. Log messages in the lock block [here](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/logger/test_logger.py#L119).


## Idiosyncrasies

***

In this document, idiosyncrasies refer to needed-improvements and future features that should/could be in the Test API. The API in its present state is functional, but these were identified as nice-to-haves or potential issues to be revised later.

### Timeout implementation

Presently timeouts are using the signal library and throw an exception to end the search. This timeout behavior can be modified very easily by changing the [__search_test_history](https://github.com/nasa/fprime/blob/6cd4c8007a7f562d5b0b616eb494270ac5c7b95d/Gds/src/fprime_gds/common/testing_fw/api.py#L911) method. All searches use this method to accomplish scoping, logging and history substitution. Changing the timeout to something like below  would be better.

~~~~{.python}
# in IntegrationTestAPI's __search_test_history method on ~line 912 of api.py
if timeout:
    self.__log(name + " now awaiting for at most {} s.".format(timeout))
    end_time = time.time() + timeout
    while True:
        new_items = history.retrieve_new()
        for item in new_items:
            if searcher.incremental_search(item):
                return searcher.get_return_value()
        if time.time() >= end_time:
            msg = name + " timed out and ended unsuccessfully."
            self.__log(msg, TestLogger.YELLOW)
            break
        time.sleep(0.1)
else:
    self.__log(name + " ended unsuccessfully.", TestLogger.YELLOW)
return searcher.get_return_value()
~~~~

**NOTE**: The above code hasn't been tested and may have issues if the system time changes: `time.time()`.

### Implementing ERT ordering in Chronological History and in the GDS

In order to properly support ERT ordering, I recommend:

1. Add a TimeType field to the [SysData](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/data_types/sys_data.py#L19) class and add an accessor for `get_ert_time()`.
2. Have the GDS record ERT at some point.
3. Preserve the use of the `fsw_order` argument in the test API's [constructor](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/testing_fw/api.py#L29) and [sub-history](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/testing_fw/api.py#L244) functions by passing the fsw_order argument to the chronological [history constructor](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/history/chrono.py#L23).
4. Modify chronological history to choose whether to use `get_time()` or `get_ert_time()` for its ordering/returning operations:
    - [clearing history](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/history/chrono.py#L111)
    - `__insert_chrono()` [helper](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/history/chrono.py#L165)
    - `__get_index()` [helper](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/history/chrono.py#L206)

#### Better History Markers

As part of the work to add ERT and have chronological histories work for both ERT and FSW orders, histories should be updated to have a `get_current_marker()` method. This will allow the histories to specify the best way to mark a position with respect to their own implementations. For reference: ChronologicalHistories should use a TimeType, Ram and Test History should use an index.

### Color-coding interlaced Events in the API Log

One feature that wasn't completed this summer was to color-code interlaced event logs based on severity. Presently, interlacing events are implemented by making the API a consumer of the event decoder in the GDS and then filtering events. Modifying the color of these log messages can be done [here](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/testing_fw/api.py#L1258).

### Moving StandardPipeline to API constructor

Presently, a user of the integration test API needs to instantiate the GDS manually before instantiating the API. This code should really be moved to inside the API. To do this, the IntegrationTestAPI's [constructor](https://github.com/nasa/fprime/blob/d0309a9e265b8650ca6be03b9132dfdc682e0622/Gds/src/fprime_gds/common/testing_fw/api.py#L27) should be modified to include the pipeline instantiation and the API's [teardown](https://github.com/nasa/fprime/blob/d0309a9e265b8650ca6be03b9132dfdc682e0622/Gds/src/fprime_gds/common/testing_fw/api.py#L64) method should be modified to disconnect from the F Prime deployment.

#### Modification to the Integration Test API

~~~~{.python}
# import statements on ~line 17 of api.py with other GDS includes
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager

# ~ IntegrationTestAPI constructor on ~line 29 of api.py
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

# ~ IntegrationTestAPI teardown on ~line 73 of api.py
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

### Using GDS Prefix to output the test Logs

Currently the StandardPipeline (GDS Helper layer) uses a path to a directory to specify an output location for log directories. When GDS instantiation is [moved inside](#-Moving-StandardPipeline-to-API-constructor) of IntegrationTestAPI's constructor, this prefix and directory should be used to output test logs as well.

### Better test identifiers using decorators

When a new test case [is started](https://github.com/nasa/fprime/blob/717bc6fab85c53680108fc961cad6338e779816f/Gds/src/fprime_gds/common/testing_fw/api.py#L85), the API user can specify a `case_id` that will be used in the logs to identify the current test case without scrolling to the test case header. Future uses of the API should investigate using decorators to specify an ID to put in this column. Present tests just use a counter and assign a number to each test case.

### GDS command arguments should allow non-string types

Presently, the GDS doesn't accept command arguments that aren't strings. This is kind of annoying and means the test API is more flexible about

### F Prime CI/CD Test Runner

During the development of the API it became apparent that the Test Runner would have bled into the scope of other testing efforts at the time. So the test API work de-scoped developing a test runner script. However, because discussions were had on what the Test Runner should do, the requirements for the Test Runner are still captured here.

- The Test Runner shall collect artifacts to record the condition of the tests.
  - History logs
  - Copies of the FSW dictionaries
  - A copy of the FSW binary
- The Test Runner shall collect files to record the results of the tests.
  - Test logs
  - Test reports
- The Test Runner shall support specifying a deployment directory to discover and run Integration Tests.
- The Test Runner should be usable by CI/CD setup.
- The Test Runner should support different test configurations
- The Test Runner should have a Command Line Interface

## Generating Code Documentation

***

If the API is modified and a developer wants to generate new documentation, they can navigate to the sphinx directory and run the command `make markdown` to create new code docs. The files will be in `fprime-sw/Gds/docs/testAPI/sphinx/build/markdown` they should be moved to `fprime-sw/Gds/docs/testAPI/markdown` and committed there.
