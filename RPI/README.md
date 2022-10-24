## Preparations:

### Read up!

Read the documents in the F' repo as described in the [README](../README.md) to get an understanding of the architecture.

### Board Preparation

This demo was developed on a Raspberry Pi 2 model B. The following directions are how to checkout and run the demo on the Raspberry Pi itself. There are also alternative directions for cross-compiling for the RPI. The cross-compile has been tested on Ubuntu as a host. Compiling and running on the RPI can be very slow, and the ground system has had some issues running on Raspberry Pi so this guide will walk the user through the setup to cross-compile for the raspberry pi.

**From the RPI configuration menu:**

 * Enable SPI
 * Enable UART
 
 ![`Raspberry Pi Config`](img/pi_cfg.png "Pi Config")
 
**Disable use of the UART for the Linux console:**

[Disable UART](https://www.raspberrypi.org/documentation/configuration/uart.md)

If the UART port is not set up correctly, there will be a file open error.
 
**In order to see results via loopback, connect:**


|Pin 1|Pin 2|Signal|
|---|---|---|
|TXD|RXD|UART|
|MISO|MOSI|SPI|
|IO23|IO25|GPIO|
|IO24|IO17|GPIO|

**If you wish to see the blinking LED demo, connect an LED to GPIO 21 as follows:**

[Connect an LED](https://thepihut.com/blogs/raspberry-pi-tutorials/27968772-turning-on-an-led-with-your-raspberry-pis-gpio-pins)
 
### Package Installation

**Install the packages necessary to run the demo.** 

Please see [INSTALL.md](../docs/INSTALL.md) to ensure that the F´ application has been installed and tested with the basic Ref. Additionally, the
cross-compilers need to be installed. This is done by installing the `gcc-arm-linux-gnueabihf` and `g++-arm-linux-gnueabihf` packages on the host
system. This is shown on Ubuntu below.

**Example: Installing Cross-Compilers for Raspberry PI on Ubuntu**
```
sudo apt update && sudo apt install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
```

### Build the software

**Crosscompiling using CMake:**

The following commands are described at length in the getting started [tutorial](../docs/Tutorials/GettingStarted/Tutorial.md). These commands will
go to the RPI directory and generate a build directory for the RPI example. This step generates a CMake Cache, sets the toolchain use to build the
code and does an initial scan of the source tree. Since the RPI example sets a default F´ toolchain file in its CMakeLists.txt, we do not need to 
supply one on the command line when generating the build. This only needs to be done once to prepare for the build because CMake will detect
changes to its files.

```
cd fprime/RPI
fprime-util generate
```

The next set of commands will build the RPI code and install it.  The installation step is only necessary to make it easier to locate the binary
file that the build generates. By default this binary is deep in the build tree and installing it puts the output in the RPI folder under the
bin subfolder. 

```
cd fprime/RPI
fprime-util build
```

After the build has completed, the output binary can be found in the `build-artifacts/arm-linux-gnueabihf/bin` folder.

**Run the software:**

The first step to running the software is to run the ground station. This will allow the code running on the Raspberry PI to downlink telemetry
that is then displayed in the user's browser. This next step uses port 50000 for the PI to connect to. The user should ensure that this port
is not blocked by the system firewall, and is properly forwarded to the network that the raspberry pi is connected to.  In the case of a standard
home network where the pi and the user's computer are connected to the same network, these typically do not require special setup.

```
cd fprime/RPI
fprime-gds -n --dictionary ./build-artifacts/raspberrypi/dict/RPITopologyAppDictionary.xml
```
The ground station should now appear in the user's default browser. Should the user wish to terminate the ground system, return to that terminal
and press CTRL-C to shut it down. Please allow it a few moments to finalize and exit.  The user may then kill the browser tab displaying the GUI.


In order to run the compiled software, we'll first need to copy it to the RPI. This can be done by creating a new terminal, and running the
following commands. Note: the ground station should be left running.

```
cd fprime/RPI
scp ./bin/raspberrypi/RPI pi@<pi's ip>:
ssh pi@<pi's IP>
```

Now the binary is available in home directory of the pi user on the raspberry pi and the user has logged into the pi. The user can then run the
following command to launch the embedded application:

```
./RPI -a <ground computer's IP> -p 50000
```

Switching back to the browser's GUI, the user should see channelized telemetry updating by clicking on the "Channels" tab and events by clicking
on the "Events" tab.

**Note:** the user should ensure that port 50000 is allowed through the ground computer's firewall.

**Run some commands:**

Click on the `Commanding` tab and drop down the `Mnemonic` list to see a list of available commands. There are some infrastructure commands, but for the purpose of this demo you can use the Raspberry Pi demo component commands:

|Command|Function|Notes|
|---|---|---|
|RD_SendString|Sends a string on the UART|Loops back to an event if wired. See `Log Events` tab.|
|RD_SendSpi|Sends a string on the SPI|Loops back to an event if wired. See `Log Events` tab.|
|RD_SendSpi|Sends a string on the SPI|Loops back to an event if wired. See `Log Events` tab.|
|RD_SetGpio|Sets a GPIO value|Loops back to GPIO based on the loopback table.|
|RD_GetGpio|Gets a GPIO value|Reads back from GPIO based on the loopback table. The value can be seen in an event in the `Log Events` tab|
|RD_SetLed|Turns blinking LED on or off|
|RD_SetLedDivider|Sets LED blink rate. Rate is 10Hz/argument|

Look at channelized telemetry and events.

The telemetry in the system can be seen in the `Channels` tab. The values update once per second. You can see various values updated by the demonstration component.

`Events` are asynchronous messages that indicate events have happened in the system. As you exercise the `RD`commands, you should see messages appear indicating what is happening in the system.

**Use parameters**

Parameters are values that are stored on disk for subsequent executions of the software. The demo has one parameter that is used to set the start up state of the LED to blinking or not blinking. To change the value, invoke the `RD_PRMLEDINITSTATE_PRM_SET` command with the new value. Then invoke `RD_PRMLEDINITSTATE_PRM_SAVE` to save the value to the RAM value in `Svc/PrmDb`. Sending the command `PRM_SAVE_FILE` will then save the value from RAM to disk. The next time you start the software, you can see the new value in operation.

**Exit the software**

CTRL-C in both terminals will stop the software from running.

**Things to look at**

The demo consists of a single component along with all the infrastructure components that are part of the repository. The demo component can be found in `RPI/RpiDemo`. The topology for the demo can be found in `RPI/Top`. That directory contains the program `main()` as well as the topology XML (`RPITopologyAppAi.xml`). You can modify and reuse the demo component, or you can create a whole series of interconnected components to do a project. Developers are encouraged to fork the repo to add functionality to the basic demo to enable whatever projects they have in mind.

**Some tips:**

 * If you would like to add your own components, read the User's manual on how to add new directories to the build.
 * If you have defined some component XML, you can generate an empty implementation class by typing `fprime-util impl`. 
 * You can see the telemetry and events in `RPI/logs/<date>/channel.log` and `RPI/logs/<date>/event.log` respectively.
 * Sometimes you see the error: Could not connect to socket at host addr localhost, port 50000, or address in use
This means that the GUI was restarted before the socket was released by Linux. Wait a minute or so and try again.
