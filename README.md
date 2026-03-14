# LEDShirt
Arduino-based LED matrix for a T-Shirt

Requires [ArxTypeTraits](https://github.com/hideakitai/ArxTypeTraits) and [FastLED](https://github.com/FastLED/FastLED) libraries. Partly vibecoded via DeepSeek.

# Some neuroslop for your enjoyment (aka an actual documentation)
⚠️ **DISCLAIMER: I copy-pasted this mostly for fun with minimal edits. Though it has some useful info. In particular don't take the power consumption section for granted, do your own measurements.**

## Hardware Requirements
- Arduino Nano
- 8x8 WS2812B RGB LED matrix (64 LEDs)
- 5V power supply (minimum 5A recommended)
- 1000μF capacitor (optional but recommended)
- 220-470Ω resistor for data line
- Connecting wires

## Wiring Diagram

### Connection Schema
```
Arduino Nano → WS2812B Matrix
------------------------------
Pin D2      → DIN (Data In)
5V          → 5V / VCC
GND         → GND
```

### Detailed Pin Connections

```
        ┌───────────────────────────────────────────┐
        │ USB                                       │
        │  ┌──┐                                     │
        │0 │  │ ├─── D0                             │
        │  └──┘                                     │
~  D1 ──┤1          D13 SCK~   ┌─┐                  │
~  D2 ──┤2══════════════════════════► DIN (Matrix)  │
   D3 ──┤3          D12 MISO   │ │ RESET            │
   D4 ──┤4          D11 MOSI~  └─┘                  │
   D5 ──┤5          D10 SS~                         │
~  D6 ──┤6          D9 ~                            │
   D7 ──┤7          D8                              │
        │                                           │
   VIN──┤8          GND                             │
   GND──┤9══════════► GND (Matrix)                  │
   RST──┤10         RST                             │
        │                                           │
   A0 ──┤11         A5 SCL                          │
   A1 ──┤12         A4 SDA                          │
   A2 ──┤13         A3                              │
   A3 ──┤14         A2                              │
   A4 ──┤15         A1                              │
   A5 ──┤16         A0                              │
        │                                           │
  5V────┤17══════════► 5V (Matrix)                  │
  GND───┤18         D14 A6                          │
  GND───┤19         D15 A7                          │
        └───────────────────────────────────────────┘
```

## Assembly Instructions

### 1. Prepare the Components
- Gather all required components
- Ensure the power supply can deliver sufficient current (≈60mA per LED × 64 = ~3.8A total, plus headroom)
- Have a soldering iron and wire ready if needed

### 2. Power Supply Setup
⚠️ **IMPORTANT: Never power the LED matrix directly from Arduino's 5V pin!** The matrix draws too much current and can damage the board.

**Option A: External Power Supply (Recommended)**
- Connect 5V power supply positive to matrix VCC
- Connect power supply GND to Arduino GND (common ground)
- Place 1000μF capacitor between 5V and GND near the matrix

**Option B: USB Power (for small tests only)**
- Only use for low brightness or small number of LEDs
- Maximum current through Arduino 5V pin is ~500mA

### 3. Data Line Connection
- Connect Arduino Pin D2 to matrix DIN
- Add 220-470Ω resistor in series on data line (protects first LED from voltage spikes)
- Keep data line as short as possible

### 4. Ground Connection
- Connect Arduino GND to matrix GND
- Ensure power supply GND is also connected (common ground)

## Software Requirements

### Libraries Needed
Install via Arduino Library Manager
- **ArxTypeTraits***
  - Search for "ArxTypeTraits" by hideakitai
- **FastLED**
  - Search for "FastLED" by Daniel Garcia

## Power Consumption

| Component | Current per LED | Total (64 LEDs) |
|-----------|----------------|-----------------|
| LEDs at full white | ~60mA | ~3.8A |
| LEDs at 50% brightness | ~30mA | ~1.9A |
| Arduino Nano | ~40mA | ~40mA |
| **Recommended PSU** | - | **5A minimum** |

## Troubleshooting

### LEDs not lighting up
1. Check power supply connection
2. Verify common ground between Arduino and matrix
3. Check data pin number in code
4. Add resistor on data line if missing
5. Test with single LED first

### Flickering or wrong colors
1. Add capacitor near matrix power input
2. Shorten data line
3. Reduce brightness
4. Check power supply voltage (should be stable 5V)

### Only first LED works
1. Check soldering on matrix
2. Verify data line continuity
3. Ensure proper library configuration

### Matrix gets hot
1. Reduce brightness
2. Improve cooling
3. Check for short circuits
4. Verify power supply voltage is exactly 5V

## Safety Notes
⚠️ Always connect power supply before uploading code
⚠️ Never connect/disconnect matrix while powered
⚠️ Use proper wire gauge for high current
⚠️ Keep away from water/moisture
⚠️ Add fuse (5A) on power line for protection