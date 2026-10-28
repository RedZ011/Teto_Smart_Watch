🕐 Teto Watch

A DIY smartwatch built from scratch on an ESP32-C3, featuring a Kasane Teto-themed OLED display, an animated background character, and Bluetooth media controls for your phone.

![Teto Watch](<img width="1718" height="1722" alt="VideoCapture_20261029-010051" src="https://github.com/user-attachments/assets/29f5aac5-dd8b-4d66-ade7-12c1b0687aae" />
)

✨ Features

- 🕓 Live clock with date and AM/PM, with a blinking colon
- 🎵 Bluetooth media control — acts as a BLE remote for whatever music app is playing on your phone (Play/Pause, Next, Back)
- 🔘 Smart single-button input:
  - Hold ~1 second → switch between the clock screen and the music player screen
  - Hold 4 seconds → power the screen off (Teto waves "Bye!!") or back on ("Hi!!")
  - Short press while the screen is off → wake the screen
  - Multi-tap (1/2/3 taps) → Play/Pause, Next, Back inside the music player
- 👋 Bye / Hi animation: Teto waves goodbye before the screen turns off and says hi when it wakes
- 🔋 Screen-off power saving: the clock and date keep running in the background while the display, Bluetooth and CPU speed are scaled down
- 🔵 Bluetooth connection icon shown on-screen when paired
- 🖼️ Custom pixel-art animations, hand-converted to C byte arrays

📸 Photos

| Clock screen | Music player screen |
|---|---|
| ![Clock screen](<img width="1718" height="1722" alt="VideoCapture_20261029-010051" src="https://github.com/user-attachments/assets/eb883939-6319-42aa-af7c-3b6d53a019df" />
) | ![Music screen](<img width="475" height="452" alt="Screenshot_20261029_005421_ASD Player" src="https://github.com/user-attachments/assets/b00d8f34-0f8a-4249-8f35-b3905b3e72dc" />
) |

| Bye animation | Wiring |
|---|---|
| ![Bye animation](<img width="502" height="487" alt="Screenshot_20261029_011452_Video Player" src="https://github.com/user-attachments/assets/fa65ba71-1471-4a75-bd8a-6a97bbc0d571" />
) |  |

🧰 Hardware

| Component | Notes |
|---|---|
| ESP32-C3 Super Mini | Main microcontroller — BLE only (no Classic Bluetooth) |
| SSD1306 OLED (I2C, 128x64) | Main display, address 0x3C |
| 1x push button | Handles all input — short press, long press, and multi-tap |
| LiPo battery + TP4056 charger | Optional, for portable use (planned) |

🔌 Wiring

| OLED Pin | ESP32-C3 GPIO |
|---|---|
| GND | GND |
| VCC | 3.3V |
| SDA | 8 |
| SCL | 9 |

| Other | GPIO |
|---|---|
| Button | 2 (INPUT_PULLUP, other leg to GND) |

⚠️ GPIO pin numbers can vary by board/seller — always double check your specific Super Mini's silkscreen before wiring. GPIO8 and GPIO9 are boot-related pins on the C3, so if the display acts up at power-up, move I2C to other pins and change `Wire.begin(SDA, SCL)` in main.cpp.

⚙️ How It Works

🖥️ The display
The OLED has no real "layers" — the screen buffer is cleared, everything (clock text, date, Teto sprite, icons) is drawn fresh into that single buffer in order, and then pushed to the screen in one display.display() call. This avoids flicker and keeps every element in sync.

⏱️ Timing
The main loop is built on non-blocking millis() timers. Every recurring action (clock tick, Teto's animation frame, button hold detection) just checks "has enough time passed since I last did this?" every loop cycle, so the clock, animation and button reading run independently.

The one exception is the 3-second Bye/Hi animation, which currently blocks the loop while it plays. The clock catches up on the missed seconds afterwards, so the time stays correct. Turning the animation into a non-blocking state machine is on the roadmap.

🔘 Button logic
A single button drives everything, using three techniques:

- Press-start timestamps — the time is saved at the moment the button goes down, and compared against millis() while it is still held
- Hold detection with a flag — a 4-second hold fires once per press (so the screen toggles one time instead of every loop), and the release afterwards is ignored so it doesn't also count as a short press
- Multi-tap counting — counts short presses within a rolling time window to distinguish 1 tap vs 2 vs 3, triggering Play/Next/Back once the window expires

How to actually use it:

- 🔲 Hold the button for 4 seconds on any screen → Teto says "Bye!!" and the screen turns off. Press again (or hold 4 seconds) → Teto says "Hi!!" and the screen turns back on.
- ⏳ Hold the button for about 1 second and release → switches between the clock screen and the Music Player screen.
- 🎧 Once in the Music Player, control it just like tapping the button on your earbuds/handsfree:
  - 1 tap → Play / Pause
  - 2 taps → Next
  - 3 taps → Back

🔋 Screen-off mode
The watch does not use deep sleep, so it never reboots and the clock keeps counting. When the screen turns off:

1. The Bye animation plays, then the OLED is switched off with the SSD1306_DISPLAYOFF command.
2. All drawing stops, and the main loop only runs the clock and button logic.
3. Bluetooth is disconnected and advertising is stopped (NimBLE).
4. The CPU drops from 160 MHz to 80 MHz.

Waking the watch reverses these steps. The time and date are never paused.

📶 Pairing & on-screen status icons

- To pair, look for a device named "Teto Watch" in your phone's Bluetooth settings.
- Once connected successfully, a small Bluetooth icon appears on the display.
- While music is playing, a music note icon appears on the display too.

🎵 Bluetooth media control
The ESP32-C3 advertises itself as a BLE HID keyboard, sending standard media key signals (Play, Pause, Next Track, Previous Track) that any phone recognizes system-wide — no companion app needed. The watch doesn't play audio itself; it's purely a remote control for whatever's already playing on your phone.

🧩 Code structure
The project is split across multiple files instead of one giant sketch:

- main.cpp — clock, date, button handling, power modes, top-level screen logic
- MusicPlayer.h / MusicPlayer.cpp — music menu drawing, button handling, BLE media key calls
- Teto.h / Bye.h / Bluetooth.h — pixel-art frames and icons stored as byte arrays in flash (PROGMEM)

Shared objects and variables (like the display and BLE keyboard instance) are declared extern in headers so all files can safely access the same single instance.

🚀 Setup

1. Install PlatformIO (VS Code extension)
2. Clone this repo
3. Open the folder in VS Code
4. Build and upload to your ESP32-C3 Super Mini
5. (Only once WiFi/NTP time sync is enabled) add your WiFi credentials in main.cpp, and keep them out of version control

Libraries (platformio.ini): Adafruit SSD1306, Adafruit GFX, h2zero/NimBLE-Arduino@1.4.1 and a NimBLE-based BLE Keyboard library.

🗺️ Roadmap

- [ ] Sync the time over WiFi (NTP) or add an RTC module, instead of the hard-coded start time
- [ ] Non-blocking Bye/Hi animation (state machine)
- [ ] Battery and charging module, plus a battery level icon
- [ ] Light sleep with button wake for even lower power use
- [ ] 3D-printed case and strap

🧠 Known Quirks

- BLE HID bonding can be finicky on some Android phones/versions — if pairing fails, try:
  - Clearing the Bluetooth cache on your phone
  - Using a NimBLE-based BLE Keyboard fork instead of the default Bluedroid one
- Turning Bluetooth off and on again depends on the BLE Keyboard library. If the watch stays visible while the screen is off, the library is restarting advertising when the phone disconnects.
- The ESP32-C3 is BLE only — it cannot receive/play actual audio (no Classic Bluetooth/A2DP), which is why the watch controls playback rather than outputting sound itself

🙏 Credits

- Kasane Teto — fan character, used here purely as a personal fan project
- Built with Adafruit_SSD1306 and a NimBLE-based BLE Keyboard library

📄 License

This project's code (firmware/logic) is licensed under the MIT License — see the LICENSE file for details.

⚠️ Note: This license covers the original code only. The Teto artwork/sprites used on the display are fan-created content based on Kasane Teto and are not covered by this license — they belong to their respective creators/rights holders and are used here purely as a personal, non-commercial fan project.
