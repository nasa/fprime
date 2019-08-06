Histories
=========
Presently there are several implementations of history within the GDS. They are all documented here.

History superclass
==================

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