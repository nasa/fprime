# Develop Deployment

## Creating a Deployment

The deployment is the portion of F' that will actual run on the spacecraft. In this part of the tutorial, you will createa deployment and begin to integrate the deployment with the other work you have completed. 

Use the following command to create the deployment: 

```shell 
# In: MathProject 
fprime-util new --deployment
```

When creating the deplyoment you will be asked two questions, answer them as follows: 

```shell 
[INFO] Cookiecutter: using builtin template for new deployment
deployment_name [MyDeployment]: Deployment
path_to_fprime [./fprime]: ./fprime
```

Add project.cmake to the CMakeLists.txt that is in /Deployment: 

```cmake 
# In: Deployment/CMakeLists.txt
# Under: Components and Topology 
include("${FPRIME_PROJECT_ROOT}/project.cmake")
```

Test the build to make sure everything is okay:

```shell
# In: /Deployment
fprime-util generate 
fprime-util build
```


Create instances of the components you have created. Think of this step like creating an instance of a class. 



```fpp 
# In: Deployment/Top/topology.fpp 
# Under: Instances used in the topology
instance mathSender
instance mathReceiver 

```

    Remember that instances start with lower case. This will help you distinguish between an instance and a definition. 

Add the fresh defined instances to instances.fpp 

```fpp 
# In: Deployment/Top/instances.fpp 
# Under: Active component instances 
instance mathSender: MathModule.MathSender base id 0xE00 \
  queue size Default.QUEUE_SIZE \
  stack size Default.STACK_SIZE \
  priority 100

# Under: Queued component instances 
instance mathReceiver: MathModule.MathReceiver base id 0x2700 \
  queue size Default.QUEUE_SIZE
```

Add packets for MathSender and MathReceiver in DeploymentPackets.xml

```xml 
<!-- In: Top/DeploymentPackets.xml -->
<!-- Above: Ignored packets -->
<packet name="MathSender" id="21" level="3">
    <channel name = "mathSender.VAL1"/>
    <channel name = "mathSender.OP"/>
    <channel name = "mathSender.VAL2"/>
    <channel name = "mathSender.RESULT"/>
  </packet>
  <packet name="MathReceiver" id="22" level="3">
    <channel name = "mathReceiver.OPERATION"/>
    <channel name = "mathReceiver.FACTOR"/>
  </packet>
```

Check to make sure all of the ports have been connected: 

```shell 
# In: Deployment/Top
fprime-util fpp-check -u unconnected.txt
cat unconnected.txt 
```

At this point in time, several mathSender and mathReceiver functions (such as mathOpIn or schedIn)should still be not connected. Hence, they should appear on this list. 

Now, go into topology.fpp, connect mathReceiver.schedIn to rate group one using the code below: 

```fpp 
# In: Top/topology.fpp 
# Under: connections Deplyoment {
rateGroup1.RateGroupMemberOut[3] -> mathReceiver.schedIn
```

    Note: [3] is the next available index in rate group one

Verify that you succesfully took a port of the unconnected. 

Add the connections between the mathSender and mathReceiver

```fpp 
# In: Top/topology.fpp 
# Under: connections Deplyoment {
mathSender.mathOpOut -> mathReceiver.mathOpIn
mathReceiver.mathResultOut -> mathSender.mathResultIn
```

Verify that none of the math ports are unconnected 

Go into DeploymentTopology.cpp and uncomment <loadParameters();> near line 130. Doing so will prevent an error in operation of F' GDS.  

Now it is time to build the entire project. Navigate to /MathProject and build. 

```shell 
# In: /MathProject 
fprime-util build -j4 
```

@TODO: Michael, what's the difference between building in MathProject verus Deployment? Why doesn't building in MathProject build Deployment at the same time? 

    Additionally, you need to build in the Deployment directory. 

    ```shell 
    # In: /MathProject/Deployment
    fprime-util build -j4 
    ```

Run the math components 

```shell 
# In: Deployment
fprime-gds 
```


Under Commanding there is a dropdown menu called "mnemonic". Click Mnemonic and find mathSender.DO_MATH. When you select DO_MATH, three new option should appear. In put 7 into val1, put 6 into val2, and put MUL into op. Press send command. Nothing exciting will happen, navivate to events (top left) and find the results of your command. You should see The Ultimate Answer to Life, the Universe, and Everything. 