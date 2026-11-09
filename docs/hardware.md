# Hardware inventory

Nothing has been inspected in person yet. Advertised details are marked as such. A blank in the "Confirmed" column means we do not know it yet.

## Ordered

| Item | Advertised | Confirmed on the real part |
| --- | --- | --- |
| ESP32-S3 N16R8 development board, 2-pack | Dual-core ESP32-S3, 16 MB flash, 8 MB PSRAM, Wi-Fi, Bluetooth 5, USB-C, pre-soldered headers, two USB-C ports | Not yet |
| EC Buying 2.4-inch TFT, 2-pack | 240×320, SPI, ILI9341, resistive touch, red PCB, header pins, SD slot, touch pen | Not yet |
| HUAREW breadboard and jumper kit | 830-point and 400-point breadboards; male-to-male, male-to-female, and female-to-female jumpers | Not yet |

Use one ESP32 and one display. The second of each stays sealed as a spare.

## Already owned

| Item | Use |
| --- | --- |
| MacBook | Write, compile, and upload the program |
| USB-C cable | Program the board, then power it |
| USB power adapter | Run the finished gadget without the Mac |

## Not purchased, and not needed yet

Permanent circuit board, enclosure, stand, sensors, battery, buttons, speaker, extra display, soldering iron.

The one tool that is worth buying before the screen is powered, only if the voltage marking on the display is unclear, is a cheap multimeter. It measures volts. Milestone 1 does not need one.

## How the three parts will work together

The ESP32 is the computer. The screen is a window it draws on. The touch layer is a separate sensor sitting on top of that window.

They talk over **SPI**, a short shared set of wires:

- a clock wire (the heartbeat of the transfer)
- a data-out wire, usually labelled MOSI or SDI
- a data-back wire, usually labelled MISO or SDO
- a chip-select wire for each device, so the screen and the touch chip do not listen at the same time

The display controller (advertised as ILI9341) stores the pixels. The ESP32 sends it colours and positions.

The touch controller sits on the same kind of SPI bus, with its own chip-select pin (`T_CS`). `T_IRQ` is an optional "a finger is down" signal. The customer photos show `T_IRQ`, `T_DO`, `T_DIN`, `T_CS`, and `T_CLK`, which is the usual set for this style of module. The chip itself has not been seen, so the controller name stays unconfirmed. Many of these red boards use an XPT2046-compatible chip. We will read the marking before any touch code is written.

Resistive touch means the screen senses pressure, from a finger or the included pen. A swipe is a press that moves. It can work. It is less sensitive than a phone screen, so Milestone 3 will measure real swipes before we depend on them. If swipes are unreliable, the fallback is large tap zones on the left and right edges. Touch is not dropped quietly.

Power:

- The ESP32's GPIO pins use **3.3 volts**. A 5-volt signal into a GPIO can damage the chip.
- The ESP32 board also has a 5-volt pin that comes from USB, and a 3.3-volt pin made by a small regulator on the board.
- The usual red 2.4-inch module is advertised as accepting 3.3 V or 5 V on `VCC`. Its backlight pin (`LED`) is often specified for **3.3 V**. Putting 5 V on that backlight pin can burn the backlight on some versions of this module.
- Until the real board is seen, the safe plan is: power and backlight from the ESP32's **3V3** pin, ground to **GND**, and every signal wire between a GPIO and a display pin. No 5 V on a signal pin. No 5 V on `LED` unless that specific board's printing says it is safe.

The SD card slot on the back shares the SPI wires. Leave it empty. An inserted card with a floating chip-select can interrupt the screen.

## Pins that are probably already busy

These are properties of the ESP32-S3 chip and of an 8 MB octal-PSRAM module. They are risks to confirm against your board's silkscreen, not a wiring plan.

| Pins | Why they are a poor choice for the screen |
| --- | --- |
| GPIO 26–32 | Used inside the module for flash memory |
| GPIO 33–37 | Used inside an 8 MB octal PSRAM module. Headers may still show 35, 36, and 37 |
| GPIO 19 and 20 | The native USB data pins. Using them disconnects that USB port |
| GPIO 43 and 44 | The serial pins behind the UART USB port |
| GPIO 0, 3, 45, 46 | Read once at startup to choose boot mode. A display wire on these can stop uploads |
| GPIO 22–25 | These numbers do not exist on the ESP32-S3 |

An onboard LED, sometimes a tiny RGB LED, is often on GPIO 48 or GPIO 38. We will not blink it until the silkscreen or schematic says which.

Plenty of ordinary GPIO pins remain for a screen and a touch controller. We pick them after reading the board.

## What to photograph when the boxes arrive

Lay each board on a plain surface, in focus.

ESP32, both boards if they differ:

1. Front, so the USB ports and pin labels are readable.
2. Back.
3. Close-up of the metal module's printed text (the line that should contain something like `ESP32-S3` and `N16R8`).
4. Any words next to the USB sockets: USB, UART, COM, OTG.
5. The pin numbers printed beside the headers, both long edges.

Display, one module is enough if both look identical:

1. Front of the glass.
2. Back of the red PCB, so every pin name is readable.
3. Close-up of any small black chip that is not hidden under the screen. We are looking for a touch-controller marking and a voltage-regulator marking.
4. Any text that says 3.3 V, 5 V, VCC, or LED.

Also send the product page links if you have them.

## Arrival checklist

Fill this in from the photos. Leave a row blank rather than guessing.

### ESP32

- Module text:
- USB port labels:
- Onboard LED pin, if printed:
- Pins present on the headers:
- Pins we will treat as off-limits:

### Display

- Pin names, in order:
- Touch chip marking:
- Regulator chip marking:
- Voltage text printed on the PCB:
- Backlight pin name and any voltage note:
- SD card pin names:
- Resolution printed anywhere:

## Wire types, for later

No connection is authorised yet. When a diagram exists, each line will name the wire:

| Wire | When it is the right one |
| --- | --- |
| Male-to-male | Both ends go into breadboard holes |
| Female-to-male | The female end grips a pin sticking out of the display or the ESP32; the male end goes into the breadboard |
| Female-to-female | Both ends grip bare pins, with no breadboard in between |

Colour only helps you trace a wire. Electrically, colour does not matter. We will still use red only for power and black only for ground, so a mistake is easier to see.

The ESP32 headers are male pins. This style of display also usually has male pins. The usual prototype is: ESP32 pressed into the breadboard, and female-to-male jumpers from each display pin into the breadboard hole next to the matching ESP32 pin.

## Shopping gap

Nothing else is required to program the ESP32 or, if the display matches the common 3.3 V-safe module, to light the screen.

Buy later only if a milestone proves we need it:

| Possible item | Only if |
| --- | --- |
| Multimeter | The display's voltage marking cannot be read |
| Two small push buttons | The touch controller cannot be identified or swipes cannot be made reliable |
| Perfboard or a solder-free terminal board | Milestone 11, after the breadboard version works |
| Craft materials for a box | Milestone 12 |

## Services we will use later

| Need | Choice | Why |
| --- | --- | --- |
| Weather, sunrise, sunset | [Open-Meteo](https://open-meteo.com/) | Free for personal use, no account and no API key. The forecast response includes sunrise and sunset for a latitude and longitude you type into settings. |
| Clock | NTP, the internet time protocol, then the ESP32's internal clock | After a power-up with Wi-Fi, the chip keeps time until it is unplugged. There is no backup battery in this build, so each fresh plug-in needs Wi-Fi once, or the clock waits. |
| Album art, first version | A small picture stored in `assets/album_art/` | The music screen must work with no Spotify account and no network. |
| Album art, later | A small program on the Mac | See the Spotify note below. |

### Spotify

You use Spotify Free. The official Spotify Web API can report the current track and an album-art link, but since February 2026 a new developer app in Spotify's development mode requires the app owner to have **Spotify Premium**. A Free account should not be planned around that API.

The ESP32 also cannot see what an iPhone is playing unless a Spotify API says so.

A realistic later path, after the screen is already useful:

- If music is playing in the Spotify app **on the Mac**, a small local helper can ask the Mac for the track name and artwork, then hand a tiny image to the ESP32 over your home Wi-Fi. That uses the Mac, not the Spotify developer API.
- If that helper cannot see artwork, the ESP32 keeps showing the sample picture you stored on it.
- Playback from an iPhone stays out of scope unless a future API path actually works with your account.

The desk companion does not depend on any of this. Milestone 8 is a spinning disc with a picture that already lives on the chip.
