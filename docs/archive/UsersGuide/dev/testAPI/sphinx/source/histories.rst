Histories
=========
Presently there are several implementations of history within the GDS. They are all documented here.


\| Classes\|
\| \:\-`-`\|
\| [History](#-History-(superclass))\|
\| [ChronologicalHistory](#Chronological-History)\|
\| [TestHistory](#-Test-History)\|
\| [RamHistory](#-Ram-History)\|


History (superclass)
====================

.. automodule:: fprime_gds.common.history.history
   :members:
   :special-members:
   :no-undoc-members:
   :member-order: bysource
   :show-inheritance: True

Chronological History
=====================
ChronologicalHistory is the primary history of the Test API.

.. automodule:: fprime_gds.common.history.chrono
   :members:
   :special-members:
   :private-members:
   :no-undoc-members:
   :member-order: bysource
   :show-inheritance: True

Test History
============
Test History is an optional history for the IntegrationTestAPI. It supports predicate searching and maintains the object order it was enqueued with.

.. automodule:: fprime_gds.common.history.test
   :members:
   :special-members:
   :private-members:
   :no-undoc-members:
   :member-order: bysource
   :show-inheritance: True

Ram History
===========
Ram history is used by the Standard Pipeline making it the default history for several GDS Tools.

.. automodule:: fprime_gds.common.history.ram
   :members:
   :special-members:
   :no-undoc-members:
   :member-order: bysource
   :show-inheritance: True

\|Quick Links\|
\| \:\-`-`\|
\|[Integration Test API User Guide](../user_guide.md)\|
\|[GDS Overview](../../../README.md)\|
\|[Integration Test API](integration_test_api.md)\|
\|[Histories](histories.md)\|
\|[Predicates](predicates.md)\|
\|[Test Logger](test_logger.md)\|
\|[Standard Pipeline](standard_pipeline.md)\|
\|[TimeType Serializable](time_type.md)\|