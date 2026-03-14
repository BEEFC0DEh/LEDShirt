

template<uint8_t FPS, typename RenderFunc>
void prepareNextFrame(RenderFunc render, CRGB screen[NUM_LEDS]) {
  static int millisDelta = 0;
  static bool firstFrame = true;
  const int FRAMETIME = 1000 / FPS;
  millisDelta += MSEC_PER_FRAME;
  if ((millisDelta < FRAMETIME) && !firstFrame) {
    return;
  }

  static uint8_t currentFrame = 0;
  millisDelta %= FRAMETIME;
  render(currentFrame, screen);
  firstFrame = false;
}

void renderAnim(uint8_t &currentFrame, CRGB screen[NUM_LEDS]) {
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
  currentFrame = ++currentFrame % FRAMES_NUM;
}

// void renderRain(uint8_t &currentFrame, CRGB screen[NUM_LEDS]) {
//   for (int y = 0; y < HEIGHT; y++) {
//     for (int x = 0; x < WIDTH; x++) {
//       CRGB color;
//       const size_t offset = y * HEIGHT + x;
//       uint8_t brightness = (pgm_read_byte(&RAIN[offset]) - m_currentFrame) % 8;
//       color = 0x202020 * brightness;
//       screen[XY(x, y)] = color;
//     }
//   }
//   currentFrame++;
// }

void renderRed(uint8_t &currentFrame, CRGB screen[NUM_LEDS]) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      // const size_t offset = y * HEIGHT + x;
      screen[XY(x, y)] = 0x170000 * (x + 1);
    }
  }
}

void renderBlue(uint8_t &currentFrame, CRGB screen[NUM_LEDS]) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      // const size_t offset = y * HEIGHT + x;
      screen[XY(x, y)] = 0x000017 * (8 - x);
    }
  }
}

void renderBlack(uint8_t &currentFrame, CRGB screen[NUM_LEDS]) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      const size_t offset = y * HEIGHT + x;
      screen[offset] = CRGB::Black;
    }
  }
}


// void crossfade(uint8_t &currentFrame, CRGB screen[NUM_LEDS]) {
//   const uint8_t progress = (currentFrame / 15) % WIDTH;
//   // Serial.print(currentFrame);
//   // Serial.print(": ");
//   // Serial.println(progress);
//   for (uint8_t y = 0; y < HEIGHT; y++) {
//     // Serial.print("[");
//     for (uint8_t x = 0; x < WIDTH; x++) {
//       const size_t offset = y * HEIGHT + x;
//       const uint8_t t = progress;
//       if (y % 2) {
//         screen[offset] = (x < WIDTH - t) ? screen1[t + offset] : screen2[offset - (WIDTH - t)];
//       } else {
//         screen[offset] = (x < t) ? screen2[WIDTH - t + offset] : screen1[offset - t];
//       }
//       // Serial.print(t);
//       // Serial.print(": ");
//       // Serial.print((x < WIDTH - t) ? offset + 1 : offset - (WIDTH - t));
//       // Serial.print(", ");
//     }
//     // Serial.println("]");
//   }
//   currentFrame = ++currentFrame % 150;
// }