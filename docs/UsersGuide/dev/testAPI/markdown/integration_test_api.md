# Integration Test API

gds_test_api.py:

This file contains basic asserts that can support integration tests on an FPrime
deployment. This API uses the standard pipeline to get access to commands, events,
telemetry and dictionaries.


* **author**

    koran



#### class fprime_gds.common.testing_fw.api.IntegrationTestAPI(pipeline, logpath=None, fsw_order=True)
Bases: `object`

A value used to begin searches after the current contents in a history and only search future
items


#### __init__(pipeline, logpath=None, fsw_order=True)
Initializes API: constructs and registers test histories.
Args:

> pipeline: a pipeline object providing access to basic GDS functionality
> logpath: an optional output destination for the api test log
> fsw_order: a flag to determine whether the API histories will maintain FSW time order.


#### teardown()
To be called once at the end of the API’s use. Closes the test log and clears histories.


#### start_test_case(case_name, case_id)
To be called at the start of a test case. This function inserts a log message to denote a
new test case is beginning, records the latest time stamp in case the user clears the
aggregate histories, and then clears the API’s histories.

Args:

    case_name: the name of the test case (str)
    case_id: a short identifier to denote the test case (str or number)


#### log(msg, color=None)
User-accessible function to log user messages to the test log.
Args:

> msg: a user-provided message to add to the test log. (str)
> color: a string containing a color hex code “######” (str)


#### get_latest_time()
Finds the latest flight software time received by either history.

Returns:

    a flight software timestamp (TimeType)


#### test_assert(value, msg='', expect=False)
this assert gives the user the ability to add formatted assert messages to the test log and
raise an assertion.
Args:

> value: a boolean value that determines if the assert is successful.
> msg: a string describing what is checked by the assert.
> expect: when True, the call will behave as an expect, and will skip the assert (boolean)

Return:

    True if the assert was successful, False otherwise


#### predicate_assert(predicate, value, msg='', expect=False)
API assert gives the user the ability to add formatted assert messages to the test log and
raise an assertion.
Args:

> value: the value to be evaluated by the predicate. (object)
> predicate: an instance of predicate that will decided if the test passes (predicate)
> msg: a string describing what is checked by the assert. (str)
> expect: when True, the call will behave as an expect, and will skip the assert (boolean)

Return:

    True if the assert was successful, False otherwise


#### clear_histories(time_stamp=None)
Clears the IntegrationTestAPI’s histories. Because the command history is not correlated to
a flight software timestamp, it will be cleared entirely. This function can be used to set
up test cases so that the IntegrationTestAPI’s histories only contain objects received
during that test.
Note: this will not clear user-created sub-histories nor the aggregate histories (histories
owned by the GDS)

Args:

    time_stamp: If specified, histories are only cleared before the timestamp.


#### set_event_log_filter(event=None, args=None, severity=None, time_pred=None)
Constructs an event predicate that is then used to filter which events are interlaced in the
test logs. This method replaces the current filter. Calling this method with no arguments
will effectively reset the filter.

Args:

    event: an optional mnemonic (str), id (int), or predicate to specify the event type
    args: an optional list of arguments (list of values, predicates, or None to ignore)
    severity: an EventSeverity enum or a predicate to specify the event severity
    time_pred: an optional predicate to specify the flight software timestamp


#### get_command_test_history()
Accessor for IntegrationTestAPI’s command history
Returns:

> a history of CmdData objects


#### get_telemetry_test_history()
Accessor for IntegrationTestAPI’s telemetry history
Returns:

> a history of ChData objects


#### get_event_test_history()
Accessor for IntegrationTestAPI’s event history
Returns:

> a history of EventData objects


#### get_telemetry_subhistory(telemetry_filter=None, fsw_order=True)
Returns a new instance of TestHistory that will be updated with new telemetry updates as
they come in. Specifying a filter will only enqueue updates that satisfy the filter in this
new sub-history. The returned history can be substituted into the await and assert methods
of this API.

Args:

    telemetry_filter: an optional predicate used to filter a subhistory.
    fsw_order: a flag to determine whether this subhistory will maintain FSW time order.

Returns:

    an instance of TestHistory


#### remove_telemetry_subhistory(subhist)
De-registers the subhistory from the GDS. Once called, the given subhistory will stop
receiving telemetry updates.

Args:

    subhist: a TestHistory instance that is subscribed to event messages

Returns:

    True if the subhistory was removed, False otherwise


#### get_event_subhistory(event_filter=None, fsw_order=True)
Returns a new instance of TestHistory that will be updated with new events as they come in.
Specifying a filter will only enqueue events that satisfy the filter in this new
sub-history. The returned history can be substituted into the await and assert methods of
this API.

Args:

    event_filter: an optional predicate to filter a subhistory.
    fsw_order: a flag to determine whether this subhistory will maintain FSW time order.

Returns:

    an instance of TestHistory


#### remove_event_subhistory(subhist)
De-registers the subhistory from the GDS. Once called, the given subhistory will stop
receiving event messages.

Args:

    subhist: a TestHistory instance that is subscribed to event messages

Returns:

    True if the subhistory was removed, False otherwise


#### translate_command_name(command)
This function will translate the given mnemonic into an ID as defined by the flight
software dictionary. This call will raise an error if the command given is not in the
dictionary.

Args:

    command: Either the command id (int) or the command mnemonic (str)

Returns:

    The command ID (int)


#### send_command(command, args=[])
Sends the specified command.
Args:

> command: the mnemonic (str) or ID (int) of the command to send
> args: a list of command arguments.


#### send_and_await_telemetry(command, args=[], channels=[], timeout=5)
Sends the specified command and awaits the specified channel update or sequence of
updates. See await_telemetry and await_telemetry_sequence for full details.
Note: If awaiting a sequence avoid specifying timestamps.

Args:

    command: the mnemonic (str) or ID (int) of the command to send
    args: a list of command arguments.
    channels: a single or a sequence of channel specs (event_predicates, mnemonics, or IDs)
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    The channel update or updates found by the search


#### send_and_await_event(command, args=[], events=[], timeout=5)
Sends the specified command and awaits the specified event message or sequence of
messages. See await_event and await event sequence for full details.
Note: If awaiting a sequence avoid specifying timestamps.

Args:

    command: the mnemonic (str) or ID (int) of the command to send
    args: a list of command arguments.
    events: a single or a sequence of event specifiers (event_predicates, mnemonics, or IDs)
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    The event or events found by the search


#### send_and_assert_telemetry(command, args=[], channels=[], timeout=5)
Sends the specified command and asserts on the specified channel update or sequence of
updates. See await_telemetry and await_telemetry_sequence for full details.
Note: If awaiting a sequence avoid specifying timestamps.

Args:

    command: the mnemonic (str) or ID (int) of the command to send
    args: a list of command arguments.
    channels: a single or a sequence of channel specs (event_predicates, mnemonics, or IDs)
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    The channel update or updates found by the search


#### send_and_assert_event(command, args=[], events=[], timeout=5)
Sends the specified command and asserts on the specified event message or sequence of
messages. See assert_event and assert event sequence for full details.

Args:

    command: the mnemonic (str) or ID (int) of the command to send
    args: a list of command arguments.
    events: a single or a sequence of event specifiers (event_predicates, mnemonics, or IDs)
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    The event or events found by the search


#### translate_telemetry_name(channel)
This function will translate the given mnemonic into an ID as defined by the flight
software dictionary. This call will raise an error if the channel given is not in the
dictionary.

Args:

    channel: a channel mnemonic (str) or id (int)

Returns:

    the channel ID (int)


#### get_telemetry_pred(channel=None, value=None, time_pred=None)
This function will translate the channel ID, and construct a telemetry_predicate object. It
is used as a helper by the IntegrationTestAPI, but could also be helpful to a user of the
test API. If  channel is already an instance of telemetry_predicate, it will be returned
immediately. The provided implementation of telemetry_predicate evaluates true if and only
if all specified constraints are satisfied. If a specific constraint isn’t specified, then
it will not effect the outcome; this means all arguments are optional. If no constraints
are specified, the predicate will always return true.

Args:

    channel: an optional mnemonic (str), id (int), or predicate to specify the channel type
    value: an optional value (object/number) or predicate to specify the value field
    time_pred: an optional predicate to specify the flight software timestamp

Returns:

    an instance of telemetry_predicate


#### await_telemetry(channel, value=None, time_pred=None, history=None, start='NOW', timeout=5)
A search for a single telemetry update received. By default, the call will only await
until a correct update is found. The user can specify that await also searches the current
history by specifying a value for start. On timeout, the search will return None.

Args:

    channel: a channel specifier (mnemonic, id, or predicate)
    value: optional value (object/number) or predicate to specify the value field
    time_pred: an optional predicate to specify the flight software timestamp
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    the ChData object found during the search, otherwise, None


#### await_telemetry_sequence(channels, history=None, start='NOW', timeout=5)
A search for a sequence of telemetry updates. By default, the call will only await until
the sequence is completed. The user can specify that await also searches the history by
specifying a value for start. On timeout, the search will return the list of found
channel updates regardless of whether the sequence is complete.
Note: It is recommended (but not enforced) not to specify timestamps for this assert.
Note: This function will always return a list of updates. The user should check if the
sequence was completed.

Args:

    channels: an ordered list of channel specifiers (mnemonic, id, or predicate)
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    an ordered list of ChData objects that satisfies the sequence


#### await_telemetry_count(count, channels=None, history=None, start='NOW', timeout=5)
A search on the number of telemetry updates received. By default, the call will only await
until a correct count is achieved. The user can specify that await also searches the current
history by specifying a value for start. On timeout, the search will return the list of
found channel updates regardless of whether a correct count is achieved.
Note: this search will always return a list of objects. The user should check if the search
was completed.

Args:

    count: either an exact amount (int) or a predicate to specify how many objects to find
    channels: a channel specifier or list of channel specifiers (mnemonic, ID, or predicate)
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    a list of the ChData objects that were counted


#### assert_telemetry(channel, value=None, time_pred=None, history=None, start=None, timeout=0)
An assert on a single telemetry update received. If the history doesn’t have the
correct update, the call will await until a correct update is received or the
timeout, at which point it will assert failure.

Args:

    channel: a channel specifier (mnemonic, id, or predicate)
    value: optional value (object/number) or predicate to specify the value field
    time_pred: an optional predicate to specify the flight software timestamp
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    the ChData object found during the search


#### assert_telemetry_sequence(channels, history=None, start=None, timeout=0)
A search for a sing sequence of telemetry updates messages. If the history doesn’t have the
complete sequence, the call will await until the sequence is completed or the timeout, at
which point it will return the list of found channel updates.
Note: It is recommended (but not enforced) not to specify timestamps for this assert.
Note: This function will always return a list of updates the user should check if the
sequence was completed.

Args:

    channels: an ordered list of channel specifiers (mnemonic, id, or predicate)
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    an ordered list of ChData objects that satisfies the sequence


#### assert_telemetry_count(count, channels=None, history=None, start=None, timeout=0)
An assert on the number of channel updates received. If the history doesn’t have the
correct update count, the call will await until a correct count is achieved or the
timeout, at which point it will assert failure.

Args:

    count: either an exact amount (int) or a predicate to specify how many objects to find
    channels: a channel specifier or list of channel specifiers (mnemonic, ID, or predicate)
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    a list of the ChData objects that were counted


#### translate_event_name(event)
This function will translate the given mnemonic into an ID as defined by the
flight software dictionary. This call will raise an error if the event given is
not in the dictionary.

Args:

    event: an event mnemonic (str) or ID (int)

Returns:

    the event ID (int)


#### get_event_pred(event=None, args=None, severity=None, time_pred=None)
This function will translate the event ID, and construct an event_predicate object. It is
used as a helper by the IntegrationTestAPI, but could also be helpful to a user of the test
API. If event is already an instance of event_predicate, it will be returned immediately.
The provided implementation of event_predicate evaluates true if and only if all specified
constraints are satisfied. If a specific constraint isn’t specified, then it will not
effect the outcome; this means all arguments are optional. If no constraints are specified,
the predicate will always return true.

Args:

    event: mnemonic (str), id (int), or predicate to specify the event type
    args: list of arguments (list of values, predicates, or None to ignore)
    severity: an EventSeverity enum or a predicate to specify the event severity
    time_pred: predicate to specify the flight software timestamp

Returns:

    an instance of event_predicate


#### await_event(event, args=None, severity=None, time_pred=None, history=None, start='NOW', timeout=5)
A search for a single event message received. By default, the call will only await until a
correct message is found. The user can specify that await also searches the current history
by specifying a value for start. On timeout, the search will return None.

Args:

    event: an event specifier (mnemonic, id, or predicate)
    args: a list of expected arguments (list of values, predicates, or None for don’t care)
    severity: an EventSeverity enum or a predicate to specify the event severity
    time_pred: an optional predicate to specify the flight software timestamp
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    the EventData object found during the search, otherwise, None


#### await_event_sequence(events, history=None, start='NOW', timeout=5)
A search for a sequence of event messages. By default, the call will only await until
the sequence is completed. The user can specify that await also searches the history by
specifying a value for start. On timeout, the search will return the list of found
event messages regardless of whether the sequence is complete.
Note: It is recommended (but not enforced) not to specify timestamps for this assert.
Note: This function will always return a list of events the user should check if the
sequence was completed.

Args:

    events: an ordered list of event specifiers (mnemonic, id, or predicate)
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    an ordered list of EventData objects that satisfies the sequence


#### await_event_count(count, events=None, history=None, start='NOW', timeout=5)
A search on the number of events received. By default, the call will only await until a
correct count is achieved. The user can specify that this await also searches the current
history by specifying a value for start. On timeout, the search will return the list of
found event messages regardless of whether a correct count is achieved.
Note: this search will always return a list of objects. The user should check if the search
was completed.

Args:

    count: either an exact amount (int) or a predicate to specify how many objects to find
    events: an event specifier or list of event specifiers (mnemonic, ID, or predicate)
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    a list of the EventData objects that were counted


#### assert_event(event, args=None, severity=None, time_pred=None, history=None, start=None, timeout=0)
An assert on a single event message received. If the history doesn’t have the
correct message, the call will await until a correct message is received or the
timeout, at which point it will assert failure.

Args:

    event: an event specifier (mnemonic, id, or predicate)
    args: a list of expected arguments (list of values, predicates, or None for don’t care)
    severity: an EventSeverity enum or a predicate to specify the event severity
    time_pred: an optional predicate to specify the flight software timestamp
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    the EventData object found during the search


#### assert_event_sequence(events, history=None, start=None, timeout=0)
An assert that a sequence of event messages is received. If the history doesn’t have the
complete sequence, the call will await until the sequence is completed or the timeout, at
which point it will assert failure.
Note: It is recommended (but not enforced) not to specify timestamps for this assert.

Args:

    events: an ordered list of event specifiers (mnemonic, id, or predicate)
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    an ordered list of EventData objects that satisfied the sequence


#### assert_event_count(count, events=None, history=None, start=None, timeout=0)
An assert on the number of events received. If the history doesn’t have the
correct event count, the call will await until a correct count is achieved or the
timeout, at which point it will assert failure.

Args:

    count: either an exact amount (int) or a predicate to specify how many objects to find
    events: optional event specifier or list of specifiers (mnemonic, id, or predicate)
    history: if given, a substitute history that the function will search and await
    start: an optional index or predicate to specify the earliest item to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    a list of the EventData objects that were counted


#### exception TimeoutException()
Bases: `Exception`

This exception is used by the history searches to signal the end of the timeout.


#### __weakref__()
list of weak references to the object (if defined)


#### find_history_item(search_pred, history, start=None, timeout=0)
This function can both search and await for an element in a history. The function will
return the first valid object it finds. The search will return when an object is found, or
the timeout is reached.

Args:

    search_pred: a predicate to specify a history item.
    history: the history that the function will search and await
    start: an index or predicate to specify the earliest item from the history to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    the data object found during the search, otherwise, None


#### find_history_sequence(seq_preds, history, start=None, timeout=0)
This function can both search and await for a sequence of elements in a history. The
function will return a list of the history objects to satisfy the sequence search. The
search will return when an order of data objects is found that satisfies the entire
sequence, or the timeout occurs.
Note: this search will always return a list of objects. The user should check if the search
was completed.

Args:

    seq_preds: an ordered list of predicate objects to specify a sequence
    history: the history that the function will search and await
    start: an index or predicate to specify the earliest item from the history to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    a list of data objects that satisfied the sequence


#### __weakref__()
list of weak references to the object (if defined)


#### find_history_count(count, history, search_pred=None, start=None, timeout=0)
This function first counts all valid items in the current history, then can await until a
valid number of elements is received by the history. The function will return a list of the
history objects counted by the search. The search will return when a correct count of data
objects is found, or the timeout occurs.
Note: this search will always return a list of objects. The user should check if the search
was completed.

Args:

    count: either an exact amount (int) or a predicate to specify how many objects to find
    history: the history that the function will search and await
    search_pred: a predicate to specify which items to count. If left blank, all will count
    start: an index or predicate to specify the earliest item from the history to search
    timeout: the number of seconds to wait before terminating the search (int)

Returns:

    a list of data objects that were counted during the search


#### data_callback(data_object)
Data callback used by the api to log events and detect when they are received out of order.
Args:

> data_object: object to store

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
