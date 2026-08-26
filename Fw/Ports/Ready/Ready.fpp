#####
# Ready:
#
# A port without arguments for coordinating startup.
#####

module Fw {
    @ As an input: Request that a component start operating or indicate that
    @              the prerequisites for the component are satisfied.
    @ As an output: Indicate that a component is now operating normally and is
    @               ready to service requests.
    @
    @ Each Ready port shall be invoked once.
    @  - Invoking a Ready port more than once is a coding defect worthy of an
    @    assertion.
    @  - Failing to invoke a Ready port is a coding defect that may result in a
    @    component remaining inoperative or not becoming fully operative.
    @  - It may be a coding defect worthy of an assertion, depending on the
    @    specification for each component, to attempt to operate a component
    @    that is not ready: that is, a component that has not received all
    @    Ready inputs or has not yet invoked all Ready outputs.
    port Ready()
}
