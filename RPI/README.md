## Preparations:

### Read up!

Read the documents in the F' repo as described in the [README](../README.md) to get an understanding of the architecture.

### Board Preparation

This demo was developed on a Raspberry Pi 2 model B.

1. From RPI configuration menu:

 * Enable SPI
 * Enable UART
 
 ![`Raspberry Pi Config`](img/pi_cfg.png "Pi Config")
 
2. Disable use of the UART for the Linux console:

[Disable UART](https://www.raspberrypi.org/documentation/configuration/uart.md)

If the UART port is not set up correctly, there will be a file open error.
 
3. In order to see results via loopback, connect:

|Pin 1|Pin 2|Signal|
|---|---|---|
|TXD|RXD|UART|
|MISO|MOSI|SPI|
|IO23|IO25|GPIO|
|IO24|IO17|GPIO|

If you wish to see the blinking LED demo, connect an LED to GPIO 22 as follows:

[Connect an LED](https://thepihut.com/blogs/raspberry-pi-tutorials/27968772-turning-on-an-led-with-your-raspberry-pis-gpio-pins)
 
### Package Installation

4. Install the packages necessary to run the demo:
```
./mk/os-pkg/ubuntu-packages.sh
sudo pip install -r  mk/python/pip_required_build.txt
sudo pip install -r  mk/python/pip_required_gui.txt
```

### Build the software

5. From the `RPI` directory in the repo, run:
```
make rebuild_rpi
```
The build should run to completion. This will take a few minutes.

6. Run the software:

```
./runBoth.sh
```

This will start the software itself in a window and start the ground system. The application window appears like this:

![`Application Window`](img/app.png "App")

The ground system window appears like this:

![`Ground System`](img/gse.png "Gse")


7. Run some commands:

Click on the `Commands` tab and drop down the `Cmds` list to see a list of available commands. There are some infrastructure commands, but for the purpose of this demo you can use the Raspberry Pi demo component commands:

|Command|Function|Notes|
|---|---|---|
|RD_SendString|Sends a string on the UART|Loops back to an event if wired. See `Log Events` tab.|
|RD_SendSpi|Sends a string on the SPI|Loops back to an event if wired. See `Log Events` tab.|
|RD_SendSpi|Sends a string on the SPI|Loops back to an event if wired. See `Log Events` tab.|
|RD_SetGpio|Sets a GPIO value|Loops back to GPIO based on the loopback table.|
|RD_GetGpio|Gets a GPIO value|Reads back from GPIO based on the loopback table. The value can be seen in an event in the `Log Events` tab|
|RD_SetLed|Turns blinking LED on or off|
|RD_SetLedDivider|Sets LED blink rate. Rate is 10Hz/argument|

8. Look at channelized telemetry and events

The telemetry in the system can be seen in the `Channel Telemetry` tab. The values update once per second. You can see various values updated by the demonstration component.

`Log Events` are asynchronous messages that indicate events have happened in the system. As you exercise the `RD`commands, you should see messages appear indicating what is happening in the system.

9. Use parameters

Parameters are values that are stored on disk for subsequent executions of the software. The demo has one parameter that is used to set the start up state of the LED to blinking or not blinking. To change the value, invoke the `RD_PRMLEDINITSTATE_PRM_SET` command with the new value. Then invoke `RD_PRMLEDINITSTATE_PRM_SAVE` to save the value to the RAM value in `Svc/PrmDb`. Sending the command `PRM_SAVE_FILE` will then save the value from RAM to disk. The next time you start the software, you can see the new value in operation.

10. Exit the software

The demo can be exited by clicking on the window exit button or by choosing `File` then `Exit.`

11. Things to look at

The demo consists of a single component along with all the infrastructure components that are part of the repository. The demo component can be found in `RPI/RpiDemo`. The topology for the demo can be found in `RPI/Top`. That directory contains the program `main()` as well as the topology XML (`RPITopologyAppAi.xml`). You can modify and reuse the demo component, or you can create a whole series of interconnected components to do a project. Developers are encouraged to fork the repo to add functionality to the basic demo to enable whatever projects they have in mind.

12. Some tips:

 * You can script a set of commands through the ground system. For an example, run from the RPI directory:
 ```
 ./run_rpi_cmds.sh scripts/tests/RPI_cmds.txt
 ```
 * If you would like to add your own components, read the User's manual on how to add new directories to the build.
 * If you have defined some component XML, you can generate an empty implementation class by typing `make impl`. 
 * If you would like to know how many lines of code you have, you can type `make sloc` in the component directory or `make RASPIAN_sloc` in the `RPI` deployment directory.
 * If your app crashes and the app window disappears, standard output is in `RPI/RPI.log`.
 * You can see the telemetry and events in `RPI/logs/<date>/channel/Channel.log` and `RPI/logs/<date>/event/Event.log` respectively.
 * Sometimes you see the error:
 ```
 EXCEPTION: Could not connect to socket at host addr localhost, port 50000
 ```
This means that the GUI was restarted before the socket was released by Linux. Wait a minute or so and try again.
 
Sometimes the GUI can be sluggish on older RPI models. In this case, connect the RPI to a network and run the GUI on another host such as an Ubuntu Linux machine configured for development as described earlier. The `runGui.sh` and `runRPI.sh` scripts should be modified with the network addresses selected for the host and Raspberry Pi.

Follow these steps to run the GUI on a host:

1) Clone the repository on the host machine
2) Follow the steps to configure the host for development.
3) Run the following make command from `RPI`: `make dictionary`. This will complete enough of the build to generate a dictionary for the GUI.
4) Run `runGui.sh` on the host.
5) Run `runRPI.sh` on the Raspberry Pi.

