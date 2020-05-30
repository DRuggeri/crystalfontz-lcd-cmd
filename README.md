# CrystalFontz LCD tool
lcd-cmd - a tool to work with Crystalfontz CFA-533 and CFA-566 LCD screens

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

### Building
Clone this repo and run `make`. A C compiler is needed on your system.
