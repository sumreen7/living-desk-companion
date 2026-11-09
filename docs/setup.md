# What you set up

The program is already written. It still needs four facts about your life, and then it needs the real boards.

## You do this on the Mac, before the packages arrive

1. Install the **PlatformIO IDE** extension in Cursor, or run `brew install platformio`.
2. Copy the settings template:

```bash
cd /Users/fathimasumreen/DIY/living-pixel-companion
cp include/secrets.example.h include/secrets.h
```

3. Open `include/secrets.h` and fill in the Wi-Fi name, Wi-Fi password, and city.

`include/secrets.h` is not uploaded to GitHub. Type the password into that file only. `secrets.example.h` is the published template, so leave its password line empty. Do not paste the password into the chat.

### Wi-Fi

The ESP32 only understands **2.4 GHz** Wi-Fi. A network whose name ends in `5G` or `5GHz` will not work. Use the other name. If you have one name for the whole house, it usually still includes 2.4 GHz and is fine.

The board only needs the internet. It does not need to see your phone.

### Weather

No account. No API key. No payment.

The board asks [Open-Meteo](https://open-meteo.com/) for the temperature, the kind of weather, and today's sunrise and sunset. The request includes your city's coordinates. That is the only thing sent.

If several places share the city's name, write the region as well: `London, Canada` or `Portland, Oregon`.

### Clock

This desk is set for Pittsburgh, US Eastern time, Celsius, and a 24-hour clock (`20:26`). Other time zones are listed in `include/secrets.example.h`.

There is no clock battery. Each time you unplug the gadget, the next plug-in needs Wi-Fi once. After that it keeps time until power is removed.

### Not needed

- A weather account
- A Spotify account
- A second screen, buttons, a battery, or a soldering iron
- A 3D printer

Spotify comes much later, and Spotify Free cannot use Spotify's developer tools. The music page already spins a sample record without it.

## You do this when the ESP32 arrives, before the screen is wired

The screen stays in the bag.

```bash
cd /Users/fathimasumreen/DIY/living-pixel-companion
pio run -t upload
pio device monitor
```

Success is a message that includes:

```text
CHECK: flash and PSRAM look right.
```

and a new line every few seconds. Press `Ctrl` + `C` to leave the monitor.

If that check fails, stop and send the text. Do not wire the screen yet.

## You do this when both boards are in front of you

Follow [wiring.md](wiring.md). The wires are planned, and each GPIO number has to be printed on your ESP32 before it is connected. If a label is missing or different, send a photo instead of guessing.

Then upload the same program again. You should see red, green, blue, and white, then a small sky.
