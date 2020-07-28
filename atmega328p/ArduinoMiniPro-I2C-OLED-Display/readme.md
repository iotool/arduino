# Arduino Mini Pro - I2C OLED Display

Examples:

* Arduino-I2C-Scanner-LED111Blinkcode (detect i2c address and display as led blinkcode)
* Arduino-I2C-OLED-0x96-LayoutText (different text layouts for OLED 0.96'' display)
* Arduino-SoftI2C-Scanner-LED111Blinkcode (detect i2c address at GPIO A0 and A1)

| Display | W x H | Top |
| --- | --- | --- |
| 0.96 OLED Yellow-Blue | 128 x 64 | 128 x 16 |

## Adafruit Library (SSD1306, GFX, BusIO)

Download Github Libraries:

* Adafruit_SSD1306-master.zip
* Adafruit-GFX-Library-master.zip
* Adafruit_BusIO-master.zip

## OLED 0.96'' 128x64

* Color 1: 0..15
* Color 2: 16..63

### Layout 1 - Tiny 8 Lines

* Fontsize (1) - Rows: 21 Byte (8x6 Pixel/Byte)
* Fontsize (1) - Lines Color 1: 0, 8
* Fontsize (1) - Lines Color 2: 16, 24, 32, 40, 48, 56

### Layout 2 - Small 4 Lines

* Fontsize (2) - Rows: 10 Byte (16x12 Pixel/Byte)
* Fontsize (2) - Lines Color 1: 0
* Fontsize (2) - Lines Color 2: 16, 32, 48

### Layout 3 - Medium 1+2 Lines

* Fontsize (3) - Rows: 7 Byte (24x18 Pixel/Byte)
* Fontsize (2) - Lines Color 1: 0
* Fontsize (3) - Lines Color 2: 17, 41

### Layout 4 - Large 1+1+1 Lines

* Fontsize (4) - Rows: 5 Byte (32x24 Pixel/Byte)
* Fontsize (2) - Lines Color 1: 0
* Fontsize (4) - Lines Color 2: 16
* Fontsize (2) - Lines Color 2: 49



