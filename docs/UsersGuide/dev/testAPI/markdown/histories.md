# Histories

Presently there are several implementations of history within the GDS. They are all documented here.

| Classes|
| :----|
| [History](#-History-(superclass))|
| [ChronologicalHistory](#Chronological-History)|
| [TestHistory](#-Test-History)|
| [RamHistory](#-Ram-History)

# History (superclass)

[history.py](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/history/history.py)

An ordered history that defines what interfaces a history should have within the GDS


**author**

    koran

#### class fprime_gds.common.history.history.History()
Bases: `object`

An ordered history to support the GDS. Histories are intended to be registered with decoders in
order to handle incoming objects and store them for retrieval. The default behavior of a
history is to maintain objects in the order they were enqueued. However, should a sub-history
want to maintain a different order, this should be made clear to the user and still support the
calls in this History class.


#### __init__()
Constructor used to set-up  a history.


#### data_callback(data_object)
Data callback to push an object on the history.

Args:

    data_object: object to store


#### retrieve(start=None)
Retrieve objects from this history. If a starting point is specified, will return a
sub-list of all objects beginning at starting point in the order to the latest object.

Args:

    start: a position in the history’s order. Start should always be able to be specified

        by an index (int).

Returns:

    an ordered list of objects


#### retrieve_new()
Retrieves an ordered list of objects that have been enqueued since the last call to
retrieve or retrieve_new.

Returns:

    an ordered list of objects


#### clear(start=None)
Clears objects from history. A clear that specifies a starting point will clear the history
such that start becomes the earliest (with respect to the history’s order) element in the
history after objects are removed.

Args:

    start: a position in the history’s order. Start should always be able to be specified

        by an index (int).

#### size()

Accessor for the number of objects in the history

# Chronological History

ChronologicalHistory is the primary history of the Test API.

[chrono.py](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/history/chrono.py)

A chronologically-ordered history that relies on predicates to provide filtering, searching, and
retrieval operations. This history will re-order itself based on FSW time.


* **author**

    koran



#### class fprime_gds.common.history.chrono.ChronologicalHistory(filter_pred=None)
Bases: `fprime_gds.common.history.history.History`

A chronological history to support the GDS test api. This history adds support for specifying
start with predicates and python’s bracket notation.


#### __init__(filter_pred=None)
Constructor used to set-up history. If the history is given a filter, it will ignore (drop)
objects that don’t satisfy the filter predicate.

Args:

    filter_pred: an optional predicate to filter incoming data_objects


#### data_callback(data_object)
Data callback to push an object on the history. This callback will only add data_objects
that satisfy the filter predicate.

Args:

    data_object: object to store


#### retrieve(start=None)
Retrieve objects from this history. If a starting point is specified, will return a
sub-list of all objects beginning at start to the latest object.
Note: if no item satisfies the start predicate or the index is greater than the length of
the history, an empty list will be returned.

Args:

    start: optional first object to retrieve. can either be an index (int) or a predicate.

Returns:

    a list of objects in chronological order


#### retrieve_new(repeats=False)
Retrieves a chronological order of objects that haven’t been accessed through retrieve or
retrieve_new before.

Returns:

    a list of objects in chronological order


#### clear(start=None)
Clears objects from history. A clear that specifies a starting point will clear the history
such that start becomes the earliest element in the history after objects are removed. If
the start is specified as a predicate, start will be the earliest object to satisfy the
predicate.
Note: if no item satisfies the start predicate or the index is greater than the length of
the history, all items will be cleared.

Args:

    start: start: an optional indicator for the first item to remove. Can be a predicate, a

        TimeType or an index in the ordering


#### size()
Accessor for the number of objects in the history
Returns:

> the number of objects (int)


#### __len__()
Accessor for the number of objects in the history
Returns:

> the number of objects


#### __getitem__(index)
__get_item__ is a special method in python that allows using brackets.
Example: item = history[2] # this would return the second item in the history.

Args:

    index: the index of the array to return.

Returns:

    the item at the index specified.


#### _ChronologicalHistory__clear_list(start, ordered)
finds the index that start specifies
Args:

> start: an optional indicator for the first item to remove. Can be a predicate, a

>     TimeType or an index in the ordering

> ordered: the list to clear

Returns:

    the index in the given list that start refers to


#### _ChronologicalHistory__get_index(start, ordered)
finds the index that start specifies
Args:

> start: an indicator of a position in an order can be a predicate, a TimeType time

>     stamp or an index in the ordering

> ordered: the list to clear

Returns:

    the index in the given list that start refers to


#### _ChronologicalHistory__insert_chrono(data_object, ordered)
traverses the existing order (back to front) and inserts the data object in the correct
position chronologically.
Args:

> data_object: an item to insert in the history. Must have a get_time() method.
> ordered: a list to insert the item into.

Returns:

    the index that the item was inserted at (int)

# Test History

Test History is an optional history for the IntegrationTestAPI. It supports predicate searching and maintains the object order it was enqueued with.

[test.py](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/history/test.py)

A receive-ordered history that relies on predicates to provide filtering, searching, and
retrieval operations


* **author**

    koran



#### class fprime_gds.common.history.test.TestHistory(filter_pred=None)
Bases: `fprime_gds.common.history.history.History`

A receive-ordered history to support the GDS test api. This history adds support for specifying
start with predicates and python’s bracket notation.


#### __init__(filter_pred=None)
Constructor used to set-up history. If the history is given a filter, it will ignore (drop)
objects that don’t satisfy the filter predicate.

Args:

    filter_pred: an optional predicate to filter incoming data_objects


#### data_callback(data_object)
Data callback to push an object on the history. This callback will only add data_objects
that satisfy the filter predicate.

Args:

    data_object: object to store


#### retrieve(start=None)
Retrieve objects from this history. If a starting point is specified, will return a
sub-list of all objects beginning at start to the latest object.
Note: if no item satisfies the start predicate or the index is greater than the length of
the history, an empty list will be returned.

Args:

    start: optional first object to retrieve. can either be an index (int) or a predicate.

Returns:

    a list of objects in chronological order


#### retrieve_new()
Retrieves a chronological order of objects that haven’t been accessed through retrieve or
retrieve_new before.

Returns:

    a list of objects in chronological order


#### clear(start=None)
Clears objects from history. A clear that specifies a starting point will clear the history
such that start becomes the earliest element in the history after objects are removed. If
the start is specified as a predicate, start will be the earliest object to satisfy the
predicate.
Note: if no item satisfies the start predicate or the index is greater than the length of
the history, all items will be cleared.

Args:

    start: clear all objects before start. start can either be an index or a predicate.


#### size()
Accessor for the number of objects in the history
Returns:

> the number of objects (int)


#### __len__()
Accessor for the number of objects in the history
Returns:

> the number of objects


#### __getitem__(index)
__get_item__ is a special method in python that allows using brackets.
Example: item = history[2] # this would return the second item in the history.

Args:

    index: the index of the array to return.

Returns:

    the item at the index specified.


#### _TestHistory__get_index(start)
finds the index that start specifies
Args:

> start: an indicator of a position in an order can be a predicate or an index in

>     the ordering

Returns:

    the index in the given list that start refers to

# Ram History

Ram history is used by the Standard Pipeline making it the default history for several GDS Tools.

[ram.py](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/history/ram.py)

A simple implementation of a history that maintains items in RAM. This is used for simplicity, but isn’t exactly
robust nor persistent. Given that it is in the RAM, it is driven from the decoders object, which should run off the
middle-ware layer.


* **author**

    lestarch



#### class fprime_gds.common.history.ram.RamHistory()
Bases: `fprime_gds.common.history.history.History`

Chronological variant of history.  This is intended to be registered with the decoders in order
to handle incoming objects, and store them for retrieval.


#### __init__()
Constructor used to set-up in-memory store for history


#### data_callback(data_object)
Data callback to store
:param data_object: object to store


#### retrieve(start=None)
Retrieve objects from this history
:param start: return all objects newer than given start time
:return: a list of objects


#### retrieve_new()
Retrieves a chronological order of objects that haven’t been accessed through retrieve or
retrieve_new before.

Returns:

    a list of objects in chronological order


#### clear(start=None)
Clears objects from RamHistory. A clear that specifies a starting point will clear the
history such that the element at the start index becomes the earliest element in the
history after objects are removed.

Args:

    start: a position in the history’s order (int).


#### size()
Accessor for the number of objects in the history
Returns:

> the number of objects (int)

|Quick Links|
|:----------|
|[Integration Test API User Guide](../user_guide.md)|
|[GDS Overview](../../../../../README.md)|
|[Integration Test API](integration_test_api.md)|
|[Histories](histories.md)|
|[Predicates](predicates.md)|
|[Test Logger](test_logger.md)|
|[Standard Pipeline](standard_pipeline.md)|
|[TimeType Serializable](time_type.md)|
