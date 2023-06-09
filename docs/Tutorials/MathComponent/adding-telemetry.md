# Adding Telemetry

## In this Section

In this section of the tutorial, you will add a telemetry channel
to report the number of math operations the `MathReceiver`
has performed. 

Before reading these steps, do your best to look at the existing 
files in this tutorial and impletement a telemetry channel 
on your own. 

1. Add a telemetry channel to `MathReceiver.fpp`: 

```fpp
# In: MathReceiver.fpp
@ Number of math operations 
    telemetry NUMBER_OF_OPS: U32 
```
**Explanation:** Here you defined a telemetry channel 
which you arbitrarily named `NUMBER_OF_OPS` which 
carries a 32 bit unsigned integer. 

2. Add a member variable to `MathReceiver.hpp`:

```cpp
// In: MathReceiver.hpp
// Under: PRIVATE
U32 numMathOps; 
```

3. Update the constructor so that it initializes `numMathOps` to zero:

```cpp
// In: MathReceiver.cpp 
// Under: Construction, Initialization, and Destruction 
MathReceiver ::
    MathReceiver(
        const char *const compName
    ) : MathReceiverComponentBase(compName),
        numMathOps(0) 
  {

  }
```

4. Increment numMathOps: 

```cpp
// In: MathReceiver.cpp 
// Within mathOpIn_handler
numMathOps++;  
```

5. Build using `fprime-util build` in `MathReceiver`. 

6. Emit telemtry: 
```cpp
// In: MathReceiver.cpp 
// Within: mathOpIn_handler
// After: numMathOps++
this->tlmWrite_NUMBER_OF_OPS(numMathOps); 
```
> It is important that you build before doing step 5 
because this->tlmWrite_NUMBER_OF_OPS is a function 
that is auto generated during the build. @TODO (Is this fact?)

7. Add the packets to `DeploymentPackets.xml` in `Deployment/Top`: 

```xml
 <packet name="MathReceiver" id="22" level="3">
        <channel name = "mathReceiver.OPERATION"/>
        <channel name = "mathReceiver.FACTOR"/>
    
        <channel name = "mathReceiver.NUMBER_OF_OPS"/>   
    </packet>
```

8. Build and test:

```shell 
fprime-util build -j4 
fprime-gds 
```

Write some unit tests to prove that this channel is working. 

## Summary 

In this section you defined a telemtry channel and impletemented 
a new variable, that will be sent through the channel.
