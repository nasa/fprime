# LED Blinker: Hardware Requirements

Users will need at least one hardware element to follow the LED Blinker tutorial: an embedded Arduino-based microcontroller with an on-board LED. If there is no on-board LED, you will need an external LED capable of withstanding the operating voltage.

## Arduino Microcontroller Requirements

The microcontroller must have sufficient FLASH memory and SRAM to run F´. The current requirements are at least 140 KB of FLASH memory and 30 KB of RAM.

## Wiring Diagram

For this tutorial, the on-board LED pin will be used (LED_BUILTIN). For most Arduino microcontrollers, this is GPIO pin 13. For platforms that do not have an on-board LED pin readily available, another pin should be chosen, noted, and used in-place of LED_BUILTIN (13).


[:octicons-arrow-right-24: Back to LED Blinker](index.md)

[:octicons-arrow-right-24: Back to Arduino LED Blinker](../arduino-led-blinker/index.md)