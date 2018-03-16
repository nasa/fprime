## Preparations:

### Board Preparation

1. From RPI configuration menu:

 * Enable SPI
 * Disable UART
 
 ![`Raspberry Pi Config` Diagram](img/pi_cfg.png "Pi Config")
 
2. In order to see results via loopback, connect:

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

7. Run some commands:




Some tips:
 * If your app crashes and the app window disappears, standard output is in `RPI/RPI.log`.
 * You can see the telemetry and events in `RPI/logs/<date>/channel/Channel.log` and `RPI/logs/<date>/event/Event.log` respectively.
 * Sometimes you see the error:
 ```
 EXCEPTION: Could not connect to socket at host addr localhost, port 50000
 ```
 This means that the GUI was restarted before the socket was released by Linux. Wait a minute or so and try again.
 