# Milestones

A milestone is finished only when the real hardware has shown it. The software for the first nine milestones is written so the board can be programmed the day it arrives.

| # | Goal | Status |
| --- | --- | --- |
| 0 | Read the real pin labels and compare them with `docs/wiring.md` | Waiting on the packages |
| 1 | Serial monitor shows the chip, 16 MB flash, and 8 MB PSRAM | Code ready, not uploaded |
| 2 | Red, green, blue, white, then a stable picture | Code ready |
| 3 | Tap and swipe, or the keyboard fallback if the touch chip differs | Code ready |
| 4 | Four pages and the dots at the bottom | Code ready |
| 5 | Clock set from the internet, then kept while powered | Code ready |
| 6 | Sky follows sunrise and sunset, with a clock-hour fallback | Code ready |
| 7 | Open-Meteo weather, cached, sky still moves offline | Code ready |
| 8 | Spinning record with a generated label | Code ready. Real album art is later |
| 9 | Messages and mood | Code ready |
| 10 | Smoother motion, brightness, reconnect behaviour | Not started |
| 11 | A compact solder-free assembly | Not started |
| 12 | Handmade case and stand | Not started |

## Still true

- No weather account and no API key.
- No Spotify in this version. The record is a sample.
- Sunrise and sunset come from Open-Meteo for the city in `secrets.h`.
- After a full power loss the clock waits for Wi-Fi, because there is no backup battery.
- The backlight is tied to 3.3 V, so the program cannot dim it yet.
- Wiring stays at 3.3 V until the display's own printing has been checked.
