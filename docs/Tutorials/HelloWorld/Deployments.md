# Getting Started: Integration and Testing With F´ Deployments

This section will walk new users through creating a new F´ [deployment](./Tutorial.md#deployment). This deployment will
build a [topology](./Tutorial.md#topology) containing the standard F´ stack of components and a single `HelloWorld`
component instance. The `HelloWorld` was created  in the [last section](./HelloWorld.md). The tutorial will close by
testing the deployment and `HelloWorld` component through the `fprime-gds`.

### Prerequisites:
- [Getting Started: F´ Hello World Component](./HelloWorld.md)

### Tutorial Steps:
- [Creating A New Deployment](#creating-a-new-deployment)
- [Adding The Hello World Component](#adding-the-hello-world-component)
- [Testing With `fprime-gds`](#testing-with-fprime-gds)
- [Conclusion](#conclusion)

## Creating A New Deployment

F´ deployments represent one flight software executable. All the components we develop for F´ run within a deployment.
The deployment created here will contain the standard [command and data handling]() (C&DH) stack. This stack enables
ground control and data collection of the deployment.

To create a deployment, run the following commands:
```bash
cd MyProject
fprime-util new --deployment
```
This command will ask for some input. Respond with the following answers:
1. Deployment Name: MyDeployment
2. Include C&DH stack: yes

> For any other questions, select the default response.

At this point, the `MyDeployment` has been created, but our `HelloWorld` component has not been added.

## Adding The Hello World Component

In this section the `HelloWorld` component will be added to the `MyDeployment` deployment. This can be done by adding
the component to the topology defined in `MyDeployment/Top`. 

Topologies instantiate all the components in a running system and link them together. For some port types, like the
commanding, event, and telemetry ports used by `HelloWorld`, the connections are made automatically. In addition, the
topology specifies how to construct the component instance. This is also done automatically unless the component has
specific configuration.

In order to add a component to the topology, it must be added to the topology model. An instance definition and an
instance initializer must both be added.

To add an instance definition, add `instance helloWorld` to the instance definition list in the `topology Ref` section
of `MyDeployment/Top/topology.fpp`. This is shown below.

```
...
    topology Ref {
        # ----------------------------------------------------------------------
        # Instances used in the topology
        # ----------------------------------------------------------------------
        
        instance ...
        instance ...
        instance helloWorld
```
> Be careful to not remove any other instances from the list.

`helloWorld` is the name of the component instance. Like variable names, component instance names should be descriptive
and are typically named in camel or snake case.

Finally, an instance initializer must be added to topology instances defined in `MyDeploymment/Top/instances.fpp` file.
Since the `HelloWorld` component is an `active` component it should be added to the active components section and should
define a priority and queue depth options.  This is shown below.

```
...
  # ----------------------------------------------------------------------
  # Active component instances
  # ----------------------------------------------------------------------
  instance ...
    ...
    ...
    ...
    
  instance ...
  
  instance helloWorld: MyComponents.HelloWorld base id 0x0F00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 50
```
> The user must ensure that the base id (0x0F00) does not conflict with any other base ids in the topology. 0x0F00
> should be safe for deployments created with `fprime-util new --deployment`.

> Make sure to use the same instance name (i.e. helloWorld) as defined in the instance definition just added to
> `topology.fpp`.

Since this component has no custom ports nor does it require special configuration, our addition to the topology is
completed. The deployment can now be built using the following command:

```
cd MyDeployment
fprime-util build 
```
Resolve any errors that occur before continuing to the testing section.

## Testing With `fprime-gds`

It is now time to test the `HelloWorld` component by running the deployment created in this section. This can be
accomplished by running the `fprime-gds` command in the deployment, verifying connection, sending the new SEND_HELLO
command and verifying that the `Hello` event and `GreetingCount` channel appears.

To start the deployment with default settings, run:
```bash
fprime-gds
```

The F´ GDS control page should open up in your web browser. If it does not open up, navigate to `http://127.0.0.1:5000`.

Once the F´ GDS page is visible, look for a green circle icon in the upper right corner. This shows that the flight
software deployment has connected to the GDS system. If a red X appears instead, navigate to the Logs tab and look for
errors in the various logs.

Now that communication is verified, navigate to the "Commanding" tab and select `helloWorld.SAY_HELLO` from the
dropdown list. Type a greeting into the argument input box and click the button "Send Command". If the argument has
validated successfully the command will send. Resolve all errors and ensure the command has sent.

> Notice commands are instance specific. Had several HelloWorld component instances been used, there would be multiple
> `SAY_HELLO` listings, one for each component instance.

Now that the command has sent, navigate to the "Events" tab. Ensure that the event list contains the Hello event with
the text entered when sending the command.

Lastly, navigate to the "Channels" tab. Look for "helloWorld.GreetingCount" in the channel list. Ensure it has recorded
the number of times a `helloWorld.SAY_HELLO` was sent.

Congratulations, you have now set up a project, component, and deployment in F´.

## Conclusion

This concludes both the adding deployment section of the Getting Started tutorial and the tutorial itself. The user has
been able to perform the following actions:

1. Create a new blank F´ projects
2. Create a new F´ components
3. Create a new F´ deployments and add components it

To explore components more in-depth and see how components communicate with one another, see the
[Math Component Tutorial](../MathComponent/Tutorial.md).

**Next:** [Math Component Tutorial](../MathComponent/Tutorial.md)

