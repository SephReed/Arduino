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
#define FRAMES_PER_SECOND  120

const uint8_t FULL_BYTE = 255;
const uint8_t SATURATION = 150;
const uint8_t numColorPoints = 5;
const uint8_t chunkSizes = NUM_LEDS / numColorPoints;
CRGB colors[numColorPoints];
CRGB nextColors[numColorPoints];
uint8_t colorTransitionStep[numColorPoints];

uint8_t colorPositions[numColorPoints];
uint8_t nextColorPoisitions[numColorPoints];

int colorMovementSpeed[numColorPoints];
int8_t colorMoveStepCount[numColorPoints];

bool ledsIsDirty = true;

void setup() {
  delay(3000); // 3 second delay for recovery

  for (uint8_t i = 0; i < numColorPoints; i++) {
    colorMovementSpeed[i] = -1;
		colors[i] = CHSV(100, SATURATION, 255);
		colorPositions[i] = (NUM_LEDS / numColorPoints) * i;
  }

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_2,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  rerandomizeColors();
  modColors();
  regenGradients();

  FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

// any color points which have a movement speed of -1 (unset)
// rerandomize the next color, the next placement
void rerandomizeColors() {
  for (uint8_t i = 0; i < numColorPoints; i++) {
    if (colorMovementSpeed[i] == -1) {
      nextColors[i] = CHSV(80 + random8(40), SATURATION, 255);
      nextColorPoisitions[i] = (chunkSizes * i) + random8((2/3) * chunkSizes) - (chunkSizes / 3);
      colorMovementSpeed[i] = random8(300) + 100;
      colorMoveStepCount[i] = 0;
      colorTransitionStep[i] = 0;
    }
  }
}

void modColors() {
  for (uint8_t i = 0; i < numColorPoints; i++) {
    colorMoveStepCount[i]++;
    if (colorMoveStepCount[i] >= colorMovementSpeed[i]) {
      ledsIsDirty = true;
      if (colorPositions[i] > nextColorPoisitions[i]) {
        colorMoveStepCount[i]--;
      } else if if (colorPositions[i] < nextColorPoisitions[i]) {
        colorMoveStepCount[i]++;
      }

      if ((colorPositions[i] + colorMoveStepCount[i]) == nextColorPoisitions[i]) {
        colorMovementSpeed[i] = -1;
				colorPositions[i] = nextColorPoisitions[i];
      }
    }
  }
}

void regenGradients() {
  if (ledsIsDirty) {
		uint8_t currentPos = 0;
		uint8_t lastColorPos = 0;
		CRGB lastColor = 0x000000;
    for (uint8_t colorNum = 0; colorNum < numColorPoints; colorNum++) {
			const double fadeAmount = colorMoveStepCount[colorNum] / (colorPositions[colorNum] - nextColorPoisitions[colorNum]);
			const uint8_t mix = Math.floor(255.0 * fadeAmount);
			const CRGB color = mixColors(colors[colorNum], nexColors[colorNum], mix);
			const nextColorPos = colorPositions[colorNum] + colorMoveStepCount[colorNum];

			for (; currentPos < nextColorPos; currentPos++) {

			}
		}
  }
}

CRGB mixColors(CRGB col1, CRGB col2, uint8_t mix) {
  int r = ((col2.r*mix) + ((FULL_BYTE-mix)*col1.r))/FULL_BYTE;
  int g = ((col2.g*mix) + ((FULL_BYTE-mix)*col1.g))/FULL_BYTE;
  int b = ((col2.b*mix) + ((FULL_BYTE-mix)*col1.b))/FULL_BYTE;
  return CRGB(r, g, b);
}





// uint8_t colorsSorted[numColorPoints];
    // uint8_t nextInsert = 0;
    // uint8_t lastPosition = 0;
    // int8_t currentSmallestIndex = -1;
    // uint8_t currentSmallestPosition = colorPositions[0];
    // for (uint8_t s = 0; i < numColorPoints; i++) {
    //   for (uint8_t i = 0; i < numColorPoints; i++) {
    //     const uint8_t focusedPosition = colorPositions[i];
    //     if ((focusedPosition > lastPosition && focusedPosition < currentSmallestPosition)
    //     || (currentSmallestIndex == -1)) {
    //       currentSmallestIndex = i;
    //       currentSmallestPosition = focusedPosition;
    //     }
    //   }
    //   colorsSorted[nextInsert] = currentSmallestIndex;
    //   lastPosition = currentSmallestPosition;
    //   currentSmallestIndex = -1;
    // }
    // TODO: make sure that organized colors have a spcetrum.
