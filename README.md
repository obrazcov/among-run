# Among Run (LCD Arduino Edition)
![IMG_4040](https://github.com/user-attachments/assets/e866743a-37fd-447f-a734-b4cf1a3fdab3)

An action-packed, side-scrolling runner game for Arduino and 16x2 LCD displays. Dodge rocks, climb platforms, and transform into a scary monster to survive in this high-speed "Among Us" inspired adventure.

## 🎮 Features

* **Triple-Layer Physics:** Navigate through the Ground (Row 1), Platforms (Row 0), and the Secret Sky.
* **Monster Mode:** Collect potions to transform into a scary monster. In this mode, you are invincible and "eat" obstacles for points.
* **Dynamic Scaling:** The game starts at a steady pace and speeds up every minute to increase the challenge.
* **Safety Spawn System:** Intelligent obstacle generation ensures the game is always beatable by preventing "impossible" wall formations.
* **Deep Statistics:** After a game over, the system infinitely cycles through your survival time, jumps made, items collected, and rocks destroyed.

## 🛠️ Hardware Requirements

* **Arduino Uno** (or compatible board)
* **16x2 LCD Display** (HD44780 compatible)
* **Push Button** (Momentary)
* **Piezo Buzzer**
* **Resistors:** 220Ω (LCD Backlight), 1kΩ (Contrast or Potentiometer)
* **Jumper Wires & Breadboard**

## 🔌 Wiring Scheme

| Component | Arduino Pin |
| --- | --- |
| LCD RS | Pin 12 |
| LCD Enable | Pin 11 |
| LCD D4-D7 | Pins 5, 4, 3, 2 |
| Push Button | Pin 7 (Internal Pull-up) |
| Piezo Buzzer | Pin 8 |

*Note: Ensure LCD VSS, RW, and K are grounded. Connect A (Anode) to 5V via 220Ω resistor.*

## 🕹️ How to Play
https://github.com/user-attachments/assets/9a977201-2005-4a39-978b-a5489322ee13


1. **Start:** Press the button on the tutorial screen.
2. **Move:** Your character runs automatically.
3. **Jump:** Press the button to jump up one level.
* **Level 0 → 1:** Jump to stand on platforms.
* **Level 1 → 2:** Jump into the "Secret Sky" to avoid high-level rocks.


4. **Gravity:** You will automatically fall back down after **1.2 seconds** unless you land on a platform.
5. **Power-ups:**
* ❤️ **Heart:** Restores 1 health point.
* 🧪 **Potion:** Become a Monster! Smashed rocks are added to your stats.



## 📁 Installation

1. Download the `amongrun.ino` file.
2. Open the file in the **Arduino IDE**.
3. Install the `LiquidCrystal` library (built-in).
4. Select your board and port, then click **Upload**.

## 📈 Statistics Page

When the game ends, the screen will cycle every 2 seconds:

* **Page 1:** Game Over message and Total Time Survived.
* **Page 2:** Detailed breakdown (Hearts, Potions, Jumps, and Rocks Eaten).
* **Restart:** Press the button at any time during the stats loop to return to the start menu.

