module HiNamespace {
    @ Hello World Tutorial Component
    active component HiComponent {

        # One async command/port is required for active components
        # This should be overridden by the developers with a useful command/port
        @ TODO

@ Command to issue greeting with maximum length of 20 characters
async command SAY_HI(
    greeting: string size 20 @< Greeting to repeat in the SayHiEvent event
)

@ Greeting event with maximum greeting length of 20 characters
event SayHiEvent(
    greeting: string size 20 @< Greeting supplied from the SAY_HI command
) severity activity high format "I say: {}"

@ A count of the number of greetings issued
telemetry GreetingCount: U32

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################

        # @ Example async command
        # async command COMMAND_NAME(param_name: U32)

        # @ Example telemetry counter
        # telemetry ExampleCounter: U64

        # @ Example event
        # event ExampleStateEvent(example_state: Fw.On) severity activity high id 0 format "State set to {}"

        # @ Example port: receiving calls from the rate group
        # sync input port run: Svc.Sched

        # @ Example parameter
        # param PARAMETER_NAME: U32

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Enables command handling
        import Fw.Command

        @ Enables event handling
        import Fw.Event

        @ Enables telemetry channels handling
        import Fw.Channel

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut

    }
}