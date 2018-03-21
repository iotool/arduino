# Reset-Button-Counter

Use Arduino builtin reset button like a push button without EEPROM. The counter value stored at SRAM.

[![Arduino Uno reset button as input like a push button](https://img.youtube.com/vi/5k2Rmplv25g/hqdefault.jpg)](http://www.youtube.com/watch?v=5k2Rmplv25g "Arduino Uno reset button as input like a push button")

## hardware

* Arduino Uno R3
* builtin reset button
* builtin led

## toggle mode by reset

### mode 1

* poweron
* led blinks 1x for 3 seconds = mode 1
* led on for 2 seconds
* start mode 1

### mode 2

* poweron
* led blinks 1x for 3 seconds = mode 1
* click reset button
* led blinks 2x for 3 seconds = mode 2
* led on for 2 seconds
* start mode 2

### mode 3

* poweron
* led blinks 1x for 3 seconds = mode 1
* click reset button
* led blinks 2x for 3 seconds = mode 2
* click reset button
* led blinks 3x for 3 seconds = mode 3
* led on for 2 seconds
* start mode 3

## push button by reset

### button

* poweron
* led blinks / toogle mode
* led on for 2 seconds
* start mode
* click reset button
* led blinks for mode
* restart mode with push button
* led blinks fast for 1 second
