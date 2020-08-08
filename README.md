# CrystalFontz LCD tool
lcd-cmd - a tool to work with Crystalfontz CFA-533 and CFA-566 LCD screens.

This code was built from the Crystalfontz [example code](https://github.com/crystalfontz/cfa_linux_examples)

```
Usage: lcd-cmd [options]
Options:
  -port: the TTY this talks to like "/dev/ttyS0" or "/dev/usb/ttyUSB0" (default: /dev/ttyUSB0)
  -baud: is 19200 or 115200 (default: 19200)
  -clear: clear the LCD screen. If lines to display are set, this is done before displaying them.
  -line1 "string": Set the first line of the LCD to this value. String must be 16 characters or less.
  -line2 "string": Set the second line of the LCD to this value. String must be 16 characters or less.
  -lines "line1" "line2": Set line1 and line2 of the LCD to these values. Strings must be 16 characters or less.
  -setboot: set LCD screen nv memory to the contents of the screen. Will be done after line1 and line2 are processed, if set.
```

## Building
Clone this repo and run `make`. A standard C compiler is needed on your system.

## USAGE
```
lcd-cmd - a tool to work with CrystalFontz CFA-533 and CFA-566 LCD screens

Usage: lcd-cmd [options]
Options:
  -port: the TTY this talks to like "/dev/ttyS0" or "/dev/usb/ttyUSB0" (default: /dev/ttyUSB0)
  -baud: is 19200 or 115200 (default: 19200)
  -clear: clear the LCD screen. If lines to display are set, this is done before displaying them.
  -brightness S T: Set the brigtness of the screen and touchpad (between 0 - 100)
  -contrast VALUE: Set the contrast of the screen (between 0 - 200)
  -line1 "string": Set the first line of the LCD to this value. String must be 16 characters or less.
  -line2 "string": Set the second line of the LCD to this value. String must be 16 characters or less.
  -lines "line1" "line2": Set line1 and line2 of the LCD to these values. Strings must be 16 characters or less.
  -gpioconfigure INDEX enable|disable TYPE: Configure GPIO index to enable or disable (0|1) and the configuration type. Both are required. Index and TYPEs are described below.
  -gpioset INDEX VALUE: Set GPIO index to value. Index values are described below. Value must be from 0 to 100. Note, GPIO configuration may need to be set first!
  -gpioget INDEX: Get the GPIO configuration at index. Index values are described below.
  -setboot: set LCD screen nv memory to the contents and configuration of the screen. Will be done after all other actions are processed.
  -reboot: reboot the LCD screen. Useful for testing boot state.
  -ping: ping the device for a response.

GPIO notes:
Index (NUM) and location:
  0 = J8, Pin 7
  1 = J8, Pin 6
  2 = J8, Pin 5
  3 = J8, Pin 4
  4 = J9, Pin 2

Values for GPIO set:
  Set values may be from 0 to 100 with 0 being 'off' and 100 being 'on'. 1-99 are output duty cycle percentages.

Configure TYPE may be one of the following:
  0 - 1=Resistive Pull Up,     0=Fast, Strong Drive Down
  1 - 1=Fast, Strong Drive Up, 0=Fast, Strong Drive Down
  2 - Hi-Z, use for input
  3 - 1=Resistive Pull Up,     0=Fast, Strong Drive Down
  4 - 1=Slow, Strong Drive Up, 0=Hi-Z
  5 - 1=Slow, Strong Drive Up, 0=Slow, Strong Drive Down
  7 - 1=Hi-Z,                  0=Slow, Strong Drive Down
```
