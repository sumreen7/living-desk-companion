# Wiring

Do this only when both boards are on the desk. Unplug USB before any wire goes on.

The program expects the pins below. They were chosen because an ESP32-S3 with 8 MB of octal PSRAM leaves them free: they are not the chip's memory pins, USB pins, or boot pins.

## Stop if a label does not match

On the ESP32 header, `10`, `IO10`, and `GPIO10` are the same pin. The same rule applies to every number below.

Stop and send a photo if:

- a number in this table is not printed on the ESP32
- a display name in this table is not printed on the display
- you see a pin you are tempted to use "instead"

Do not connect the pin labelled `5V`, `VIN`, or `BAT` to the display.

## Power, the safe first try

| Display pin | Connect to | Volts |
| --- | --- | --- |
| `VCC` | ESP32 pin labelled `3V3` or `3.3V` | 3.3 V |
| `GND` | ESP32 pin labelled `GND` | ground |
| `LED` | the same 3.3 V point as `VCC` | 3.3 V, backlight |

`LED` is the backlight. It can want more current than a data pin should give, so it goes to the 3.3 V power pin, not to a GPIO. The screen stays lit. Dimming can be added later if your board has a proper backlight switch.

Some of these red boards only start when `VCC` is 5 V, because they have a small regulator. 3.3 V is the first try because it cannot push 5 V back into the ESP32. If the screen stays dark or white, unplug USB and send a photo of the back of the display. Do not move `VCC` to 5 V on your own.

## Signal wires

| Display name | Also printed as | ESP32 pin | What it does |
| --- | --- | --- | --- |
| `CS` | | GPIO 10 | tells the screen to listen |
| `RESET` | `RST` | GPIO 14 | restarts the screen |
| `DC` | `RS` | GPIO 9 | command versus pixel data |
| `SDI` | `MOSI` | GPIO 11 | data into the screen |
| `SCK` | `CLK` | GPIO 12 | clock |
| `SDO` | `MISO` | GPIO 13 | data back from the screen |
| `T_DIN` | | GPIO 11 | touch data, same column as `SDI` |
| `T_CLK` | | GPIO 12 | touch clock, same column as `SCK` |
| `T_DO` | | GPIO 13 | touch data back, same column as `SDO` |
| `T_CS` | | GPIO 4 | tells the touch chip to listen |
| `T_IRQ` | | GPIO 5 | the touch chip says a finger is down |

Leave the SD card slot empty. Do not wire `SD_CS` unless a later milestone says to.

## How to use the breadboard

Use the large 830-point breadboard. Use **female-to-male** wires only.

- The female end grips a metal pin on the ESP32 or the display.
- The male end goes into a breadboard hole.

The breadboard's **short columns of 5 holes** (marked a–e, next to a row number) are connected to each other. The long red and blue stripes along the side are power rails. They run the length of the board. Some boards break that stripe in the middle. Keep every power wire on the same half, or bridge the gap with one extra wire.

```text
ESP32 3V3  ---- red wire ----  red rail  ---- red wire ---- display VCC
                              red rail  ---- red wire ---- display LED
ESP32 GND  ---- black wire -- blue rail ---- black wire -- display GND

GPIO 10 ---- column 1 ---- CS
GPIO 14 ---- column 2 ---- RESET
GPIO  9 ---- column 3 ---- DC
GPIO 11 ---- column 4 ---- SDI
             column 4 ---- T_DIN
GPIO 12 ---- column 5 ---- SCK
             column 5 ---- T_CLK
GPIO 13 ---- column 6 ---- SDO
             column 6 ---- T_DO
GPIO  4 ---- column 7 ---- T_CS
GPIO  5 ---- column 8 ---- T_IRQ
```

Each "column" is one group of 5 connected holes. The three shared signals (11, 12, and 13) have two display wires in the same column as the ESP32 wire. That is how one ESP32 pin can talk to both the screen and the touch chip.

Colour does not change the electricity. Use red only for 3.3 V and black only for ground so a mistake is easy to see.

## Order

1. Unplug USB.
2. Read every pin name out loud against this page.
3. Connect ground (black) first.
4. Connect the signal wires.
5. Connect 3.3 V and `LED` last.
6. Look once more.
7. Plug in USB.
8. From the project folder, run `pio run -t upload` and then `pio device monitor`.

You should see four full-screen colours — red, green, blue, white — and then the sky. The monitor prints the colour name as each one appears.

## If the picture is wrong

Leave the wires where they are. Change one line in `include/config.h`, upload again, and look.

| What you see | Change |
| --- | --- |
| Colours look like a photo negative | `#define LPC_INVERT_COLORS 1` |
| Red and blue are swapped | `#define LPC_BGR_PANEL 1` |
| The picture is upside down | `#define LPC_ROTATION 2` |
| A tap is mirrored | In `include/pins.h`, swap `TOUCH_X_MIN` with `TOUCH_X_MAX`, or the Y pair |

The touch chip on the photo labels is expected to be XPT2046-compatible. If taps do nothing, the keys below still move the pages. Send a photo of any tiny chip text on the back of the display.

While the monitor is open:

| Key | Action |
| --- | --- |
| `d` | next page |
| `a` | previous page |
| space | play or pause the record |
| `m` | next little message |
| `1`–`6` | mood |
| `r` | ask for weather again |
