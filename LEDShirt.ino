#include <ArxTypeTraits.h>
#include <FastLED.h>

#define DATA_PIN 2
#define WIDTH 8
#define HEIGHT 8
#define NUM_LEDS WIDTH * HEIGHT

#define MSEC_PER_FRAME 17

#include "anim2.h"

CRGB leds[NUM_LEDS];
CRGB screen1[NUM_LEDS];
CRGB screen2[NUM_LEDS];
#include "object_anims.h"

void setup() {
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(15);
  Serial.begin(115200);
}

ImageAnimation  imageAnim{1, 3};
RainAnimation   rainAnim{8, 3};
TextAnimation   textAnim{4};
WaveAnimation   waveAnim{30, 10};

const Animation* ANIMATIONS[] = {
  &imageAnim
  , &textAnim 
  , &imageAnim
  , &rainAnim
};

#define NUM_ANIMS sizeof(ANIMATIONS) / sizeof(Animation*)
static_assert(NUM_ANIMS <= 16, "Too many animations");

class AnimationScheduler {
private:
  enum State { PLAYING, FADING } m_state = PLAYING;
  
  CrossfadeAnimation m_crossfade{60};
  
  uint8_t m_currentPhase = 0;
  FrameCounter m_phaseFrame = 0;
  FrameCounter m_currentFrame1 = std::numeric_limits<FrameCounter>::max();
  FrameCounter m_currentFrame2 = std::numeric_limits<FrameCounter>::max();
  FrameCounter m_currentFrame3 = std::numeric_limits<FrameCounter>::max();
  FrameCounter m_fadeFrame = 0;
  const uint8_t m_crossfadeSec;
  const FrameCounter m_crossfadeFrames;
  static constexpr uint8_t FPS = 1000 / MSEC_PER_FRAME;
  
public:
  AnimationScheduler(uint8_t crossfadeSec) 
    : m_crossfadeSec(crossfadeSec),
      m_crossfadeFrames(crossfadeSec * FPS) {}
  
  void prepareNextFrame() {
    if (NUM_ANIMS == 0) return;
    
    uint8_t nextPhase = m_currentPhase + 1;
    nextPhase %= NUM_ANIMS;
    int currentDuration = ANIMATIONS[m_currentPhase]->m_duration * FPS;

    const auto updateAnim = [](FrameCounter frameNum, FrameCounter &frameCounter, const Animation *anim, CRGB screen[NUM_LEDS])
    {
      const FrameCounter nextFrame = frameNum * anim->m_fps / (1000.f / MSEC_PER_FRAME);
      if (nextFrame != frameCounter) {
        frameCounter = nextFrame;
        anim->prepareNextFrame(screen, frameCounter);
      }
    };
    
    switch (m_state) {
      case PLAYING: {
        updateAnim(m_phaseFrame,  m_currentFrame1, ANIMATIONS[m_currentPhase], leds);
        
        if (m_phaseFrame >= currentDuration - 1) {
          if (m_crossfadeFrames > 0 && NUM_ANIMS > 1) {
            m_state = FADING;
            m_fadeFrame = 0;
            m_currentFrame2 = std::numeric_limits<FrameCounter>::max();;
            m_currentFrame3 = std::numeric_limits<FrameCounter>::max();;
            for (int i = 0; i < NUM_LEDS; i++) screen1[i] = leds[i];
          } else {
            m_currentPhase = nextPhase;
            m_phaseFrame = 0;
            break;
          }
        }
        m_phaseFrame++;
        break;
      }
      
      case FADING: {
        updateAnim(m_phaseFrame + m_fadeFrame,  m_currentFrame1, ANIMATIONS[m_currentPhase], screen1);
        updateAnim(m_fadeFrame,                 m_currentFrame2, ANIMATIONS[nextPhase], screen2);
        updateAnim(m_fadeFrame,                 m_currentFrame3, &m_crossfade, leds);
         
        m_fadeFrame++;
        if (m_fadeFrame >= m_crossfadeFrames) {
          m_state = PLAYING;
          m_currentPhase = nextPhase;
          m_currentFrame1 = m_currentFrame2;
          m_phaseFrame = 0;
        }
        break;
      }
    }
  }
};

AnimationScheduler animScheduler{2};

void loop() {
  animScheduler.prepareNextFrame();
  FastLED.show();
  delay(MSEC_PER_FRAME);
}
