#include <FastLED.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DATA_PIN    2
#define DATA_PIN_2    4
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    100
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  60

const uint8_t SATURATION = 150;
CHSV colors[NUM_LEDS];

void setup() {
  delay(3000); // 3 second delay for recovery

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
		colors[i] = CHSV(100, SATURATION, 255);
	}

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_2,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  rerandomizeColors();
  FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND);
}


void rerandomizeColors() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
		if (random8(100) < 5) {
			if (random8(100) < 50) {
				colors[i].hue = Math.min(120, colors[i].hue + 1);
			} else {
				colors[i].hue = Math.max(80, colors[i].hue - 1);
			}

			if (random8(100) < 33) {
				colors[i].value = Math.min(255, colors[i].value + 1);
			} else {
				colors[i].value = Math.max(1, colors[i].value - 1);
			}

			leds[i] = colors[i];
		}
	}
}
