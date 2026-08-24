# 🕐 Teto Watch

A DIY smartwatch built from scratch on an ESP32-C3, featuring a Kasane Teto-themed OLED display, an animated background character, and Bluetooth media controls for your phone.



---

## ✨ Features

- 🕓 **Live clock** with date and AM/PM, synced via NTP over WiFi
- 🎵 **Bluetooth media control** — acts as a BLE remote for whatever music app is playing on your phone (Play/Pause, Next, Back)
- 🔘 **Smart single-button input**:
  - Short press → toggle screen on/off
  - Long press → open the music player menu
  - Multi-tap (1/2/3 taps) → Play/Pause, Next, Back inside the menu
- 🔵 **Bluetooth connection icon** shown on-screen when paired
- 🖼️ Custom pixel-art animations, hand-converted to C byte arrays

---

## 🧰 Hardware

| Component | Notes |
|---|---|
| ESP32-C3 Super Mini | Main microcontroller — BLE only (no Classic Bluetooth) |
| SSD1306 OLED (SPI, 128x64) | Main display |
| 1x push button | Handles all input — short press, long press, and multi-tap |
| LiPo battery + TP4056 charger | Optional, for portable use |

---

## 🔌 Wiring

| OLED Pin | ESP32-C3 GPIO |
|----------|---------------|
| GND      | GND           |
| VCC      | 3.3V          |
| SCK      | 4             |
| MOSI     | 6             |
| RES      | 7             |
| DC       | 8             |
| CS       | 10            |
|----------|---------------|

| Other | GPIO |
|---|---|
| Button | 3 (INPUT_PULLUP) |

> ⚠️ GPIO pin numbers can vary by board/seller — always double check your specific Super Mini's silkscreen before wiring.

---

## ⚙️ How It Works

### 🖥️ The display
The OLED has no real "layers" — every frame, the screen buffer is cleared, everything (clock text, date, Teto sprite, icons) is drawn fresh into that single buffer in order, and then pushed to the screen in one `display.display()` call. This avoids flicker and keeps every element in sync.

### ⏱️ Timing — no `delay()`
Nothing in the main loop blocks. Every recurring action (clock tick, Teto's animation frame, button debounce, tap-window timeout) is handled with **non-blocking `millis()` timers** — each one just checks "has enough time passed since I last did this?" every loop cycle, so the clock, animation, and button reading all run independently and smoothly at the same time.

### 🔘 Button logic
A single button drives everything, using two techniques:
- **Press-duration detection** — timestamps when the button goes down and up to tell a short press from a long press
- **Multi-tap counting** — counts short presses within a rolling time window (e.g. 400ms) to distinguish 1 tap vs 2 vs 3, triggering Play/Next/Back accordingly once the window expires

**How to actually use it:**
- 🔲 **Hold and release the button for a short time** while on the clock screen → turns the display **off**. Do it again → turns it back **on**.
- ⏳ **Hold the button and release it after ~1.5 seconds** → takes you into the **Music Player** screen, where you can control your music.
- 🎧 Once in the Music Player, control it just like tapping the button on your earbuds/handsfree:
  - **1 tap** → Play / Pause
  - **2 taps** → Next
  - **3 taps** → Back
- 🔙 To return to the clock screen from the Music Player, **long press and release** the button again.

### 📶 Pairing & on-screen status icons
- To pair, look for a device named **"Teto Watch"** in your phone's Bluetooth settings.
- Once connected successfully, a small **Bluetooth icon** appears on the display.
- While music is playing, a **music note icon** appears on the display too.

### 🎵 Bluetooth media control
The ESP32-C3 advertises itself as a **BLE HID keyboard**, sending standard media key signals (`Play`, `Pause`, `Next Track`, `Previous Track`) that any phone recognizes system-wide — no companion app needed. The watch doesn't play audio itself; it's purely a remote control for whatever's already playing on your phone.

### 🧩 Code structure
The project is split across multiple files instead of one giant sketch:
- `main.cpp` — clock, date, WiFi/NTP, top-level screen logic
- `MusicPlayer.h` / `MusicPlayer.cpp` — music menu drawing, button handling, BLE media key calls

Shared objects and variables (like the display and BLE keyboard instance) are declared `extern` in headers so both files can safely access the same single instance.

---

## 🚀 Setup

1. Install [PlatformIO](https://platformio.org/) (VS Code extension)
2. Clone this repo
3. Open the folder in VS Code
4. Update your WiFi credentials in `main.cpp`
5. Build and upload to your ESP32-C3 Super Mini

---

## 🧠 Known Quirks

- BLE HID bonding can be finicky on some Android phones/versions — if pairing fails, try:
  - Clearing the Bluetooth cache on your phone
  - Using a NimBLE-based BLE Keyboard fork instead of the default Bluedroid one
- The ESP32-C3 is **BLE only** — it cannot receive/play actual audio (no Classic Bluetooth/A2DP), which is why the watch controls playback rather than outputting sound itself

---

## 🙏 Credits

- Kasane Teto — fan character, used here purely as a personal fan project
- Built with [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306) and a BLE Keyboard library

---

## 📄 License

This project's code (firmware/logic) is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

> ⚠️ **Note:** This license covers the original code only. The Teto artwork/sprites used on the display are fan-created content based on Kasane Teto and are **not** covered by this license — they belong to their respective creators/rights holders and are used here purely as a personal, non-commercial fan project.
