# Troubleshooting

Work on one symptom at a time. Copy the exact message from the terminal into the chat.

## Milestone 1: the uploader cannot find a port

1. Try the other USB-C socket on the ESP32. Many of these boards have two, and only one of them speaks to the uploader for a given cable.
2. Try another USB-C cable. Some cables only carry power and have no data wires. A cable that charges a phone may still be charge-only.
3. In the project folder, list what the Mac can see:

```bash
pio device list
```

A usable port looks like `/dev/cu.usbmodem...` or `/dev/cu.wchusbserial...` or `/dev/cu.usbserial...`.

4. If the list is empty with both sockets and a known data cable, the Mac may need a driver for a WCH CH340/CH343 or Silicon Labs CP2102 chip. Tell me the board's USB-port labels and a photo before installing a random driver.

## Milestone 1: upload starts, then fails

1. Hold the button marked **BOOT** (sometimes **0** or **IO0**).
2. While holding it, press and release the button marked **RST** or **RESET**.
3. Release BOOT.
4. Run `pio run -t upload` again.

That sequence puts the chip in the mode where it accepts a new program.

## Milestone 1: upload succeeds, but the monitor is blank

1. Confirm the monitor speed is 115200. `pio device monitor` uses the speed in `platformio.ini`.
2. Press the ESP32 reset button after the monitor window is open. The identity text is printed at startup, then a short status line every few seconds.
3. If you uploaded through one USB socket, try opening the monitor on the other socket. This program prints on both the native USB serial port and the UART serial port, but a single cable can only hear the socket it is plugged into.
4. Close anything else that might be holding the port, including another monitor window or the Arduino IDE serial plotter.

## Milestone 1: the chip resets over and over

The memory setting may not match the module. Do not try to fix this by editing random values. Copy about 20 lines of the repeating message and send them. The likely change is `qio_opi` to the setting your particular board uses.

## Milestone 1: flash or PSRAM check fails

Stop. Send the identity text. A wrong memory setting can make later graphics code fail in confusing ways, so we correct it before the screen is involved.

## Screen and touch

There is no screen procedure yet. If the display is connected before a diagram in `docs/wiring.md` says to connect it, unplug USB power first and leave it unplugged until we review the wiring.
