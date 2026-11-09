# Living Pixel Companion

A desk gadget: an ESP32-S3 and a 2.4-inch colour touchscreen, with a small animated sky, a clock, weather, a spinning record, and a mood you can tap.

The Mac is for writing and uploading. After that, the board runs from a USB charger.

## What is already written

The program in this folder draws the four pages, keeps time, and asks Open-Meteo for weather. It does not need a weather account. It does not need Spotify.

It has not been run on the real boards yet. The wiring in [docs/wiring.md](docs/wiring.md) is the plan for this kind of ESP32-S3. Match every printed label before connecting a wire.

| Milestone | Status |
| --- | --- |
| 0. Confirm the printed pin labels | Plan is written. Check it against the boards |
| 1. ESP32 talks to the Mac | Code is in the upload. Not run on hardware yet |
| 2–9. Screen, touch, sky, clock, weather, music, mood | Written. Waiting on the boards |
| 10–12. Polish, permanent build, handmade case | Not started |

Details: [docs/milestones.md](docs/milestones.md). What you personally need to fill in: [docs/setup.md](docs/setup.md).

## Before the packages arrive

```bash
cd /Users/fathimasumreen/DIY/living-pixel-companion
cp include/secrets.example.h include/secrets.h
```

Edit `include/secrets.h`: Wi-Fi name, Wi-Fi password, and city. The password stays on your Mac. That file is listed in `.gitignore`.

Defaults already in the program, change them in `secrets.h` if they are wrong:

- Timezone: US Eastern
- Temperature: Fahrenheit
- Clock: 12-hour, with AM/PM

Install PlatformIO (`brew install platformio`, or the PlatformIO extension in Cursor).

## When the ESP32 arrives

Leave the screen unplugged. Plug the board into the Mac with a USB-C cable that carries data, preferably the socket marked USB.

```bash
pio run -t upload
pio device monitor
```

A healthy board says `CHECK: flash and PSRAM look right.` Press `Ctrl` + `C` to close the monitor. If the check fails, stop and send the text.

## When the screen arrives

Follow [docs/wiring.md](docs/wiring.md), then run the same upload. The screen should show red, green, blue, and white, then the sky.

Swipe left and right to move between sky, weather, music, and mood. If the touch chip is not the expected one, the keyboard keys in the wiring notes still change pages.

## Pages

- **Sky** — colour, sun or moon, clouds, and a short message. The message changes with the time of day. Tap for the next one.
- **Weather** — temperature and condition for your city, refreshed about every 20 minutes and remembered if Wi-Fi drops. Tap to ask again.
- **Music** — a record with a made-up label. It spins until you tap. This is not Spotify yet.
- **Mood** — tap to move through studying, working, relaxing, listening, a break, and sleeping.

Weather and the network run in the background so a slow request does not freeze the sky.

## Project map

```text
include/secrets.example.h   copy to secrets.h and fill in
include/pins.h              the planned GPIO numbers
include/config.h            rotation and colour fixes
src/main.cpp                startup
src/sky_animation.cpp       the sky
src/weather.cpp             Open-Meteo
src/net.cpp                 Wi-Fi, in the background
docs/wiring.md              do not skip the label check
```
