#include <math.h>

using FrameCounter = uint16_t;
// Преобразование координат (серпантин)
int XY(uint8_t x, uint8_t y) {
  return (y % 2 == 0) ? (y * WIDTH + x) : (y * WIDTH + (WIDTH - 1 - x));
}

class Animation
{
protected:
  friend class AnimationScheduler;
  const float m_fps;

  virtual void render(CRGB screen[NUM_LEDS], FrameCounter frameNum) = 0;

public:
  Animation(float fps = 1.0f)
  : m_fps{fps}
  {}

  void prepareNextFrame(CRGB screen[NUM_LEDS], FrameCounter frameNum) {
    render(screen, frameNum);
  }
};

class ImageAnimation : public Animation
{
private:
  void render(CRGB screen[NUM_LEDS], FrameCounter frameNum) override {
    const uint8_t currentFrame = frameNum % FRAMES_NUM;
    const fl::u8 *bytes = reinterpret_cast<const fl::u8*>(pgm_read_ptr(&FRAMES[currentFrame]));
    for (uint8_t y = 0; y < HEIGHT; y++) {
      for (uint8_t x = 0; x < WIDTH; x++) {
        CRGB color;
        const size_t offset = (y * HEIGHT + x) * sizeof(CRGB);
        color.r = pgm_read_byte(&bytes[offset]);
        color.g = pgm_read_byte(&bytes[offset] + 1);
        color.b = pgm_read_byte(&bytes[offset] + 2);
        screen[XY(x, y)] = color;
      }
    }
  }

public:
  ImageAnimation(float fps = 1.0f)
  : Animation(fps)
  {}
};

class RainAnimation : public Animation
{
private:
  uint8_t RAIN[WIDTH] = {6,3,0,1,5,7,2,4};

  void render(CRGB screen[NUM_LEDS], FrameCounter frameNum) override {
    // Обработка верхней строки (y=0)
    const FrameCounter currentFrame = frameNum;
    for (int x = 0; x < WIDTH; x++) {
      uint8_t brightness = (RAIN[x] - currentFrame);
      brightness %= HEIGHT;
      
      if (brightness == 0) {
        screen[x] = CRGB::Black;
        RAIN[x] = random(0, WIDTH);
      } else {
        screen[XY(x, 0)] = 0x202020 * brightness;
      }
    }
    
    // Сдвиг вниз (снизу вверх)
    for (int y = HEIGHT - 1; y > 0; y--) {
      for (int x = 0; x < WIDTH; x++) {
        screen[XY(x, y)] = screen[XY(x, y - 1)];
      }
    }
  }

public:
  RainAnimation(float fps = 1.0f)
  : Animation(fps)
  {}
};

class CrossfadeAnimation : public Animation
{
  static constexpr bool m_isReversed = false;
private:
  void render(CRGB screen[NUM_LEDS], FrameCounter frameNum) override {
    const FrameCounter currentFrame = frameNum % 150;
    const uint8_t progress = (currentFrame / 15) % WIDTH;
    for (uint8_t y = 0; y < HEIGHT; y++) {
      for (uint8_t x = 0; x < WIDTH; x++) {
        const size_t offset = y * WIDTH + x;
        const uint8_t t = progress;
        if ((y % 2) ^ m_isReversed) {
          screen[offset] = (x < t) ? screen2[WIDTH - t + offset] : screen1[offset - t];
        } else {
          screen[offset] = (x < WIDTH - t) ? screen1[t + offset] : screen2[offset - (WIDTH - t)];
        }
      }
    }
  }

public:
  CrossfadeAnimation(float fps = 1.0f)
  : Animation(fps)
  {}
};

class TextAnimation : public Animation
{
  uint8_t m_letterIndex = 255;
  static inline const uint8_t m_font[] = {
    // A (0)
    0b00111100,
    0b01000010,
    0b01000010,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,

    // B (1)
    0b01111100,
    0b01000010,
    0b01000010,
    0b01111100,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01111100,

    // C (2)
    0b00111110,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b00111110,

    // D (3)
    0b01111100,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01111100,

    // E (4)
    0b01111110,
    0b01000000,
    0b01000000,
    0b01111100,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01111110,

    // F (5)
    0b01111110,
    0b01000000,
    0b01000000,
    0b01111100,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,

    // G (6)
    0b00111110,
    0b01000000,
    0b01000000,
    0b01001110,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00111110,

    // H (7)
    0b01000010,
    0b01000010,
    0b01000010,
    0b01111110,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01000010,

    // I (8)
    0b01111110,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b01111110,

    // J (9)
    0b00000110,
    0b00000010,
    0b00000010,
    0b00000010,
    0b00000010,
    0b01000010,
    0b01000010,
    0b00111100,

    // K (10)
    0b01000010,
    0b01000100,
    0b01001000,
    0b01110000,
    0b01110000,
    0b01001000,
    0b01000100,
    0b01000010,

    // L (11)
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01111110,

    // M (12)
    0b01000010,
    0b01100110,
    0b01011010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,

    // N (13)
    0b01000010,
    0b01100010,
    0b01010010,
    0b01001010,
    0b01000110,
    0b01000010,
    0b01000010,
    0b01000010,

    // O (14)
    0b00111100,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00111100,

    // P (15)
    0b01111100,
    0b01000010,
    0b01000010,
    0b01111100,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,

    // Q (16)
    0b00111100,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01001010,
    0b01000100,
    0b00111010,

    // R (17)
    0b01111100,
    0b01000010,
    0b01000010,
    0b01111100,
    0b01001000,
    0b01000100,
    0b01000010,
    0b01000010,

    // S (18)
    0b00111100,
    0b01000000,
    0b01000000,
    0b00111100,
    0b00000010,
    0b00000010,
    0b01000010,
    0b00111100,

    // T (19)
    0b01111110,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,

    // U (20)
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00111100,

    // V (21)
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00011000,

    // W (22)
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01011010,
    0b01100110,
    0b01000010,
    0b01000010,

    // X (23)
    0b01000010,
    0b00100100,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b01000010,

    // Y (24)
    0b01000010,
    0b00100100,
    0b00011000,
    0b00001000,
    0b00001000,
    0b00001000,
    0b00001000,
    0b00001000,

    // Z (25)
    0b01111110,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b01000000,
    0b01111110
  };
  static inline const char m_string[] = " I LOVE TEA ";
  static inline const uint8_t m_stringLen = sizeof(m_string) - 1;
private:
  inline uint8_t getLetterRow(uint8_t index, uint8_t row) {
    if (m_string[index] == ' ') {
      return 0;
    }

    uint8_t letter = m_string[index] - 'A';
    return m_font[letter * HEIGHT + row];
  }

  void render(CRGB screen[NUM_LEDS], FrameCounter frameNum) override {
    const FrameCounter currentFrame = frameNum % 150;
    uint8_t progress = currentFrame % (WIDTH);

    if (progress == 0) {
      m_letterIndex++;
      m_letterIndex %= m_stringLen;
    }

    uint8_t nextIndex = m_letterIndex + 1;
    nextIndex %= m_stringLen;

    for (uint8_t y = 0; y < HEIGHT; y++) {
      for (uint8_t x = 0; x < WIDTH; x++) {
        const uint8_t t = progress;
        const uint8_t curLetter = getLetterRow(m_letterIndex, y);
        const uint8_t nextLetter = getLetterRow(nextIndex, y);

        constexpr auto getPixel = [](uint8_t letter, uint8_t offset) {
          return (letter >> WIDTH - offset - 1) & 0b1 ? CRGB::White : CRGB::Black;
        };

        size_t offset = y * WIDTH;
        offset += (y % 2) ? (WIDTH - 1 - x) : x;
        screen[offset] = (x <= WIDTH - 1 - t) ? getPixel(curLetter, x + t) : getPixel(nextLetter, x - (WIDTH - t));
      }
    }
  }

public:
  TextAnimation(float fps = 1.0f)
  : Animation(fps)
  {}
};

class WaveAnimation : public Animation
{
private:
  void render(CRGB screen[NUM_LEDS], FrameCounter frameNum) override {
    const double period = M_PI * 2 / (WIDTH * 2);
    for (uint8_t y = 0; y < HEIGHT; y++) {
      for (uint8_t x = 0; x < WIDTH; x++) {
        const fl::u8 hue = ( y + sin( ( x + double(frameNum) ) * period ) ) * 32;
        screen[XY(x, y)].setHue(hue);
      }
    }
  }

public:
  WaveAnimation(float fps = 1.0f)
  : Animation(fps)
  {}
};