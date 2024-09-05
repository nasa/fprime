# Uploading Binary File to Hardware

## Teensy

The Teensyduino application should have appeared after running `fprime-util build` and the hex file should already be loaded. If you do not see the hex file, choose the hex file to load into Teensyduino located in `./build-artifacts/<teensy build name>/LedBlinker/bin/`. Manually press the reset button on the Teensy to upload the program.

Note: Change `<teensy build name>` to your build (i.e. `teensy41`, `teensy32`, etc.).

## Adafruit Feather M0

Double press on the reset button on the Feather to set it to programming mode. Then run the following commands below.

```sh
# Linux
~/.arduino15/packages/adafruit/tools/bossac/1.8.0-48-gb176eee/bossac -i -d --port=ttyACM0 -U -i --offset=0x2000 -w -v ./build-artifacts/featherM0/LedBlinker/bin/LedBlinker.bin -R

# MacOS
~/Library/Arduino15/packages/adafruit/tools/bossac/1.8.0-48-gb176eee/bossac -i -d --port=tty.usbmodem12345 -U -i --offset=0x2000 -w -v ./build-artifacts/featherM0/LedBlinker/bin/LedBlinker.bin -R
```

Note:
  - If you have more than one device connected, or if you are using a different OS, `ttyACM0` may differ for your system.
      - On MacOS, it may look like `tty.usbmodem12345`
      - To view your list of devices, run: `ls /dev/tty*`

## ESP32 Dev Module

```sh
chmod 0777 /dev/ttyACM0

# Linux
python3 "~/.arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool.py" --chip esp32 --port "/dev/ttyUSB0" --baud 921600  --before default_reset --after hard_reset write_flash -e -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 "build-fprime-automatic-esp32/arduino-cli-sketch/tmp8lcl76bw.ino.bootloader.bin" 0x8000 "build-fprime-automatic-esp32/arduino-cli-sketch/tmp8lcl76bw.ino.partitions.bin" 0xe000 "~/.arduino15/packages/esp32/hardware/esp32/2.0.9/tools/partitions/boot_app0.bin" 0x10000 "build-artifacts/esp32/LedBlinker/bin/LedBlinker.bin"

# MacOS
python3 "~/Library/Arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool.py" --chip esp32 --port "/dev/ttyUSB0" --baud 921600  --before default_reset --after hard_reset write_flash -e -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 "build-fprime-automatic-esp32/arduino-cli-sketch/tmp8lcl76bw.ino.bootloader.bin" 0x8000 "build-fprime-automatic-esp32/arduino-cli-sketch/tmp8lcl76bw.ino.partitions.bin" 0xe000 "~/Library/Arduino15/packages/esp32/hardware/esp32/2.0.9/tools/partitions/boot_app0.bin" 0x10000 "build-artifacts/esp32/LedBlinker/bin/LedBlinker.bin"
```

Note: Change `tmp8lcl76bw` to the actual temporary file name located in `build-fprime-automatic-esp32/arduino-cli-sketch`

## Raspberry Pi Pico W (RP2040)

#### Method 1

Hold down the BOOTSEL button on your RPi Pico W as you plug in the micro-USB cable. A media device should mount to your system as `RPI-RP2`. Copy and paste `build-artifacts/rpipicow/LedBlinker/bin/LedBlinker.uf2` into the `RPI-RP2` media. The RPi Pico W should automatically unmout and run your binary.


#### Method 2

Follow this if your RPi Pico W does not mount as a media device.

```sh
chmod 0777 /dev/ttyACM0

# Linux
python3 -I ~/.arduino15/packages/rp2040/hardware/rp2040/3.3.0/tools/uf2conv.py --serial /dev/ttyACM0 --family RP2040 --deploy build-artifacts/rpipicow/LedBlinker/bin/LedBlinker.uf2

# MacOS
python3 -I ~/Library/Arduino15/packages/rp2040/hardware/rp2040/3.3.0/tools/uf2conv.py --serial /dev/ttyACM0 --family RP2040 --deploy build-artifacts/rpipicow/LedBlinker/bin/LedBlinker.uf2
```