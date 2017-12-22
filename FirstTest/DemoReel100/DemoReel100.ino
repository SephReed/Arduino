// #include <Button.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DATA_PIN    7
#define BUTTON_PIN  6
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB



/*******************************
*   POTENTIOMETER SETTINGS
*******************************/
#define POT_JITTER_LIMIT 15    
#define POT_AVERAGING 8
// #define POT_MAX 750
#define POT_MAX 1023
const int OBVIOUS_USER_MOVEMENT = 25;  //max jitter seen is 17



/*******************************
*       AESTHETICS
*******************************/
// #define BRIGHTNESS_MAX     96
#define BRIGHTNESS_MAX     60
uint8_t SATURATION_MAX = 240;
const uint8_t BASELINE_BRIGHTNESS = 75;
#define FRAMES_PER_SECOND  120
int PALETTE_STEP_EVERY_N_MS = 200;
int HOLD_PATTERN_FOR_AT_LEAST_N_FRAMES = 300;
int GAWKING_FRAMES_LIMIT = 500;



/*******************************
*       LED ARRANGEMENT
*******************************/
const uint8_t NUM_SPOKES = 16;
const uint8_t NUM_LAYERS = 4;
//NUM_LEDS can not be greater than 256 without modifying much of the code
const uint8_t NUM_LEDS = NUM_SPOKES * NUM_LAYERS;
CRGB LEDS_OUT[NUM_LEDS];



/*******************************
*       USEFUL CONSTANTS
*******************************/
const uint8_t FULL_BYTE = 255;
const uint8_t ONE_THIRD_BYTE = FULL_BYTE/3;
const uint8_t TWO_THIRDS_BYTE = 2*ONE_THIRD_BYTE;
const uint8_t HALF_BYTE = FULL_BYTE/2;



/*******************************
*       INPUT PINS
*******************************/
//Brightness Pin
const int brightnessInPin = A0;
int brightnessPinAvg = 0;
  //
const uint8_t mBrightnessMin = 5;
const uint8_t mBrightnessMax = 50;
uint8_t mBrightness = mBrightnessMax;


//Root Hue Pin
const int rootHueInPin = A3;
int rootHuePinAvg = 0;
  //
const uint8_t mRootHueMin = 0;
const uint8_t mRootHueMax = 255;
uint8_t mRootHue = 0;
bool mRootHueDirty = false;


//Root Hue Pin
const int secondTonesInPin = A0;
int secondTonesPinAvg = 0;
  //
const uint8_t mSecondToneMin = 0;
const uint8_t mSecondToneMax = ONE_THIRD_BYTE;
uint8_t mSecondTone = 0;
bool mSecondToneDirty = false;

const int modInPin1 = A1;
int modPin1Last = 0;

const int modInPin2 = A2;
int modPin2Last = 0;












/******************************************************
*                                                     *
*     FUNCTION FORWARD DECLARATIONS AND VARIABLES                 
*                                                     *
******************************************************/

/*******************************
*       COLORS
*******************************/
CRGB mixColors(CRGB col1, CRGB col2, uint8_t mix);
uint8_t getBrightness(CRGB color);
CRGB HSV_to_RGB(uint8_t hue, uint8_t saturation, uint8_t value);


/*******************************
*       PALETTE
*******************************/
const uint8_t PALETTE_SIZE = 128;
CRGB mainTones [3];
CRGB mainPalette [PALETTE_SIZE];
bool mainPaletteDirty [PALETTE_SIZE];

//Forward Declarations
void markPaletteDirty();
CRGB getPaletteColorNum(uint8_t colorNum);



/*******************************
*       PATTERN
*******************************/
//ALL PATTERNS SHARED VARIABLES
uint8_t gMainPaletteOffset = 0; // rotating "base color" used by many of the patterns

//FOrward Declarations
uint8_t polarToIndex(uint8_t spoke, uint8_t layer);
void fadeOutToDarkness(CRGB* leds, uint8_t ledCount, uint8_t minBrightness, uint8_t fadeAmount);



/*******************************
*       PATTERN TYPES
*******************************/
//ALL PATTERNS FORWARD DECLARATIONS
void warpSpeed(int runTime, CRGB* leds);
void orbits(int runTime, CRGB* leds);



// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])(int runTime, CRGB* leds, bool reversed);
// SimplePatternList gPatterns = { oneColor };
// SimplePatternList gPatterns = { pinWheel };
// SimplePatternList gPatterns = { orbits };
// SimplePatternList gPatterns = { warpSpeed, orbits };
SimplePatternList gPatterns = { sprinkler, orbits, warpSpeed };
// SimplePatternList gPatterns = { zigZag, vortex, pinWheel };
// SimplePatternList gPatterns = { vortex };
// SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
const uint8_t NUM_PATTERNS = ARRAY_SIZE(gPatterns);
const bool SINGLE_PATTERN_TEST_MODE = true;
const uint8_t STARTING_PATTERN_INDEX = 2;




/*******************************
*       HELPER FNS
*******************************/
int8_t randomIndexNotFoundInCollection(uint8_t* collection, uint8_t collectionSize, uint8_t numChoices);









/******************************************************
*                                                     *
*              MAIN PATTERN FUNCTIONS                  
*                                                     *
******************************************************/




/*************************
*   SPRINKLER
**************************/


int8_t dropletLayer [NUM_SPOKES];
uint8_t dropletColorOffset [NUM_SPOKES];
uint8_t sprinklerCurrentSpoke = 0;
int sprinklerLastDropletTime = 0;
int sprinklerNextDropletMoveTime = 0;

uint8_t sprinklerMinPause = 1;
uint8_t sprinklerMaxPause = 15;

void sprinkler(int runTime, CRGB* leds, bool reversed) {
  if(runTime == -1) {
    for(uint8_t s = 0; s < NUM_SPOKES; s++) {
      dropletLayer[s] = -1;
    }
    sprinklerLastDropletTime = 0;
    sprinklerNextDropletMoveTime = 0;
  }


  uint8_t speed = sprinklerMaxPause - map(analogRead(modInPin2), 0, 1023, 0, sprinklerMaxPause-sprinklerMinPause);
  fadeOutToDarkness(leds, NUM_LEDS, BASELINE_BRIGHTNESS, 16 - speed);

  if(runTime-sprinklerLastDropletTime > speed) {
    sprinklerLastDropletTime = runTime;  
    if(reversed == false)
      dropletLayer[sprinklerCurrentSpoke] = 0;
    else
      dropletLayer[sprinklerCurrentSpoke] = NUM_LAYERS - 1;

    sprinklerCurrentSpoke++;
    sprinklerCurrentSpoke %= NUM_SPOKES;
  }


  bool moveDroplets = sprinklerNextDropletMoveTime <= runTime;
  if(moveDroplets) {
    sprinklerNextDropletMoveTime = runTime + 8;
  }

  uint8_t colorOffsetPerSpoke = map(analogRead(modInPin1), 0, 1023, 0, FULL_BYTE/NUM_SPOKES);
  for(uint8_t s = 0; s < NUM_SPOKES; s++) {
    if(dropletLayer[s] != -1) {

      uint8_t colorNum = gMainPaletteOffset;
      colorNum += colorOffsetPerSpoke * s;

      uint8_t targetIndex = polarToIndex(s, dropletLayer[s]);
      leds[targetIndex] = getPaletteColorNum(colorNum);

      if(moveDroplets) {
        if(reversed == false) {
          dropletLayer[s]++;

          if(dropletLayer[s] >= NUM_LAYERS) {
            dropletLayer[s] = -1;
          }
        }
        else {
          dropletLayer[s]--;
        }
      }
    }
  }
}










/*************************
*   ORBITS
**************************/


const uint8_t orbitMaxComets = 10;
const uint8_t orbitMinComets = 4;
int orbitCometNextStepTimes [orbitMaxComets];
uint8_t orbitCometLayers [orbitMaxComets];
uint8_t orbitCometSpokes [orbitMaxComets];
uint8_t orbitCometSpeeds [orbitMaxComets];
bool orbitCometDirs [orbitMaxComets];


void orbits(int runTime, CRGB* leds, bool reversed) {

  //initialize comet positions, directions, and speeds
  if(runTime == -1) {
    for(uint8_t c = 0; c < orbitMaxComets; c++) {
      orbitCometSpokes[c] = 0;
      orbitCometSpeeds[c] = random8(12)+3;
      orbitCometDirs[c] = random8(2) % 2 == 0;
      if(reversed)
        orbitCometDirs[c] = !orbitCometDirs[c];

      orbitCometLayers[c] = c % NUM_LAYERS;
      orbitCometNextStepTimes[c] = runTime + orbitCometSpeeds[c];
    }
  }

  

  uint8_t numComets = map(analogRead(modInPin2), 0, 1023, 0, orbitMaxComets-orbitMinComets) + orbitMinComets;
  uint8_t colorOffsetPerLayer = map(analogRead(modInPin1), 0, 1023, 0, FULL_BYTE/NUM_LAYERS);

  fadeOutToDarkness(leds, NUM_LEDS, BASELINE_BRIGHTNESS, numComets-3);

  for(uint8_t c = 0; c < numComets; c++) {

    //update the orbit polar positions
    if(runTime >= orbitCometNextStepTimes[c]) {
      if(orbitCometDirs[c]) {
        orbitCometSpokes[c]++;
      }
      else {
        orbitCometSpokes[c]--;
        orbitCometSpokes[c] += NUM_SPOKES;
      }
      orbitCometSpokes[c] %= NUM_SPOKES;
      orbitCometNextStepTimes[c] = runTime + orbitCometSpeeds[c];
    }


    //draw the comet, each layer has a base color, each comet is offset from that color
    uint8_t colorNum = gMainPaletteOffset;
    colorNum += colorOffsetPerLayer * orbitCometLayers[c];
    colorNum += (colorOffsetPerLayer/3) * (c/4);

    uint8_t targetIndex = polarToIndex(orbitCometSpokes[c], orbitCometLayers[c]);
    leds[targetIndex] = getPaletteColorNum(colorNum);
  }  
}









/*************************
*   WARP SPEED
**************************/

const uint8_t warpMaxComets = NUM_SPOKES;
const uint8_t warpMinComets = 7;
int warpTimesNext [warpMaxComets];
int warpCometMoveTimeNext = 0;
int8_t warpCometSpokes [warpMaxComets];
uint8_t warpCometLayers [warpMaxComets];
uint8_t warpCometColorOffsets [warpMaxComets];

void warpEraseComet(int runTime, uint8_t cometNum) {
  warpTimesNext[cometNum] = random8(25) + runTime;
  warpCometSpokes[cometNum] = -1;
  warpCometLayers[cometNum] = 0;
}

void warpSpeed(int runTime, CRGB* leds, bool reversed) {

  if(runTime == -1) {
    for(uint8_t c = 0; c < warpMaxComets; c++) {
      warpEraseComet(runTime, c);
    }
    warpCometMoveTimeNext = 0;
  }

  fadeOutToDarkness(leds, NUM_LEDS, BASELINE_BRIGHTNESS, 12);  

  uint8_t colorRange = map(analogRead(modInPin1), 0, 1023, 0, FULL_BYTE);
  uint8_t numComets = map(analogRead(modInPin2), 0, 1023, 0, warpMaxComets-warpMinComets) + warpMinComets;
  uint8_t halfRange = colorRange/2;

  bool moveComets = runTime >= warpCometMoveTimeNext;
  if(moveComets)
    warpCometMoveTimeNext = runTime + 5;

  for(uint8_t c = 0; c < warpMaxComets; c++) {

    if(c < numComets) {

      //choose next comet placement
      if(warpTimesNext[c] < runTime && warpTimesNext[c] != -1) {
        warpTimesNext[c] = -1;

        if(reversed == false)
          warpCometLayers[c] = 0;
        else
          warpCometLayers[c] = NUM_LAYERS - 1;

        warpCometColorOffsets[c] = random8(colorRange) - halfRange;

        int randSpoke = -1;
        for(uint8_t r = 0; r < 3 && (randSpoke == -1 || warpCometLayers[randSpoke] != -1); r++) {
          randSpoke = random16(NUM_SPOKES);
        }
        warpCometSpokes[c] = randSpoke;
      }

      if(warpCometSpokes[c] != -1) {
        uint8_t colorNum = gMainPaletteOffset + warpCometColorOffsets[c];
        uint8_t targetIndex = polarToIndex(warpCometSpokes[c], warpCometLayers[c]);
        leds[targetIndex] = getPaletteColorNum(colorNum);

        if(moveComets) {
          if(reversed == false) {
            warpCometLayers[c]++;
            if(warpCometLayers[c] >= NUM_LAYERS) {
              warpEraseComet(runTime, c);
            }
          }
          else {
            if(warpCometLayers[c] == 0)
              warpEraseComet(runTime, c);
            else
              warpCometLayers[c]--;
          }
        }
      }
    }
    else if(warpCometSpokes[c] != -1) {
      warpEraseComet(runTime, c);
    }
  }
}
















/******************************************************
*                                                     *
*                PATTERN HELPER FUNCTIONS          
*                                                     *
******************************************************/

uint8_t polarToIndex(uint8_t spoke, uint8_t layer) {
  
  uint8_t out = spoke * 4;

  bool isEven = (spoke%2 == 0);
  if(isEven)
    out += layer;
  else
    out += (3-layer);

  return out;
}



void fadeOutToDarkness(CRGB* leds, uint8_t ledCount, uint8_t minBrightness, uint8_t fadeAmount) {
  for (uint8_t i = 0; i < ledCount; i++) {
    if(getBrightness(leds[i]) > minBrightness)
      leds[i].fadeToBlackBy(fadeAmount);
  }
}












/******************************************************
*                                                     *
*                PALETTE FUNCTIONS          
*                                                     *
******************************************************/

void markPaletteDirty() {

  mainTones[0] = HSV_to_RGB(mRootHue, SATURATION_MAX, FULL_BYTE);
  mainTones[1] = HSV_to_RGB(mRootHue + mSecondTone, SATURATION_MAX, FULL_BYTE);
  mainTones[2] = HSV_to_RGB(mRootHue - mSecondTone, SATURATION_MAX, FULL_BYTE);

  for(uint8_t i = 0; i < PALETTE_SIZE; i++) {
    mainPaletteDirty[i] = true;
  }
}

const uint8_t oneThirdPaletteSize = PALETTE_SIZE/3;
CRGB getPaletteColorNum(uint8_t colorNum) {
  if(colorNum > PALETTE_SIZE)
    colorNum %= PALETTE_SIZE;
  else if(colorNum < 0)
    colorNum += PALETTE_SIZE;

  //if color num is dirty for current palette, calculate it;
  if(mainPaletteDirty[colorNum]) {
    uint8_t mixAmount = ((colorNum%oneThirdPaletteSize)*FULL_BYTE)/oneThirdPaletteSize;

    if(colorNum < oneThirdPaletteSize) {
      mainPalette[colorNum] = mixColors(mainTones[0], mainTones[1], mixAmount);
    }
    else if (colorNum < 2*oneThirdPaletteSize) {
      mainPalette[colorNum] = mixColors(mainTones[1], mainTones[2], mixAmount);
    }
    else if (colorNum < 3*oneThirdPaletteSize) {
      mainPalette[colorNum] = mixColors(mainTones[2], mainTones[0], mixAmount);
    }
    else {
      mainPalette[colorNum] = mainTones[0];
    }

    mainPaletteDirty[colorNum] = false;
  }
  
  //always return a color
  return mainPalette[colorNum];
}











/******************************************************
*                                                     *
*                COLOR FUNCTIONS          
*                                                     *
******************************************************/

CRGB mixColors(CRGB col1, CRGB col2, uint8_t mix) {
  int r = ((col2.r*mix) + ((FULL_BYTE-mix)*col1.r))/FULL_BYTE;
  int g = ((col2.g*mix) + ((FULL_BYTE-mix)*col1.g))/FULL_BYTE;
  int b = ((col2.b*mix) + ((FULL_BYTE-mix)*col1.b))/FULL_BYTE;
  return CRGB(r, g, b);
}

uint8_t getBrightness(CRGB color) {
  return max(color.red, max(color.green, color.blue));
}

//more expensive than CHSV, but emphasizes Cyan/Yellow/Magenta instead of Red/Green/Blue
//prettier
CRGB HSV_to_RGB(uint8_t hue, uint8_t saturation, uint8_t value) {

  // return CHSV(hue, saturation, value);

  //special case, no color, or pitch black
  if(saturation == 0 || value == 0) {
    return CRGB(value, value, value);
  }
  
  // double oneThird = fullByte/3;
  // double twoThirds = oneThird * 2;
  const double oneThird = ONE_THIRD_BYTE;
  const double twoThirds = TWO_THIRDS_BYTE;

  double rgbPos [3];
  rgbPos[0] = fmod((hue+oneThird), FULL_BYTE)/twoThirds;  //at 0, red pos = 0.5 or peak of sine wave
  rgbPos[1] = hue/twoThirds;  //at .3333, green pos = 0.5 or peak of sine wave
  rgbPos[2] = (hue-oneThird)/twoThirds;  //at .6666, blue pos = 0.5 or peak of sine wave


  uint8_t rgb[3];
  for(uint8_t i = 0; i < 3; i++) {
    double pos = rgbPos[i];
    if(pos > 0 && pos < 1)
      rgb[i] = sin(pos*M_PI) * value;
    else
      rgb[i] = 0;
  }


  if(saturation < FULL_BYTE) {
    uint8_t max = max(max(rgb[0], rgb[1]), rgb[2]);

    for(uint8_t i = 0; i < 3; i++) {
      rgb[i] += ((max-rgb[i]) * (FULL_BYTE-saturation))/FULL_BYTE;
    }
  }
  

  return CRGB(rgb[0], rgb[1], rgb[2]);
}


















// void oneColor() {
//   if(mRootHueDirty || mSecondToneDirty) {
//     uint8_t saturation = map(analogRead(saturationInPin), 0, 1023, 0, 255);

//     CRGB palette [3];
//     palette[0] = HSV_to_RGB(mRootHue, saturation, 255);
//     if(mSecondTone > 3) {
//       palette[1] = HSV_to_RGB(mRootHue + mSecondTone, saturation, 255);
//       palette[2] = HSV_to_RGB(mRootHue - mSecondTone, saturation, 255);
//     }
//     else {
//       palette[1] = palette[0];
//       palette[2] = palette[0];
//     }


//     for( int i = 0; i < NUM_LEDS; i++) { //9948
//       leds[i] = palette[i%3];
//       // leds[i] = CHSV(mRootHue, saturation, 255);
//     }

//     // Serial.print("huepinValue");
//     // Serial.println(pinValue);

//     // Serial.print("huepinValueAvg");
//     // Serial.println(rootHuePinAvg);

//     // Serial.print("hue");
//     // Serial.println(mRootHue);

//     mRootHueDirty = false;
//     mSecondToneDirty = false;
//   }  
// }











// bool patternProgress(int stepsPerInc, int &incrementMe, bool usesPalette) {
//   bool needsRedraw = pattCount1 == -1;

//   if(usesPalette) {
//     if(mSecondToneDirty || mRootHueDirty || pattCount1 == -1) {
//       // updateMainPalette();
//       needsRedraw = true;
//     }
//   }

//   pattCount1++;

//   if(pattCount1 > stepsPerInc) {
//     needsRedraw = true;
//     incrementMe++;
//     pattCount1 = 0;
//   }

//   return needsRedraw;
// }







// int gradientOffset = 0;
// int pinSpokeOffset = 0;
// int pinSpokeOffsetCount = 0;

// void pinWheel() {
//   bool needsRedraw = true;
//   // patternProgress(3, gradientOffset, true);

//   pinSpokeOffsetCount++;
//   if(pinSpokeOffsetCount > 6) {
//     pinSpokeOffsetCount = 0;
//     pinSpokeOffset = (pinSpokeOffset + 1) % NUM_SPOKES;
//     needsRedraw = true;
//   }

//   if(needsRedraw) {
//     int oneThird = FULL_BYTE/3;
//     uint8_t offsetPerLayer = map(analogRead(saturationInPin), 0, 1023, 0, 8);

//     for(int s = 0; s < NUM_SPOKES; s++) {
//       for(int l = 0; l < NUM_LAYERS; l++) {
//         uint8_t colorNum = ((s%3)*oneThird);

//         uint8_t spoke = (s + pinSpokeOffset)%NUM_SPOKES;
//         // int spoke = (spiralSpokeOffset+s+(l/2))%NUM_SPOKES;
//         int targetIndex = polarToIndex(spoke, l);
//         // leds[targetIndex] = mainPalette[colorNum];
//       }
//     }
//   }
// }







// int pattVar1 = -1;
// void vortex() {
//   bool needsRedraw = true;
//   // patternProgress(3, pattVar1, true);

//   if(needsRedraw) {
//     uint8_t offsetPerLayer = map(analogRead(saturationInPin), 0, 1023, 0, FULL_BYTE/NUM_LAYERS);

//     for(int l = 0; l < NUM_LAYERS; l++) {
//       for(int s = 0; s < NUM_SPOKES; s++) {
//         int colorNum = (pattVar1 + (l*NUM_SPOKES) + s);
//         colorNum += offsetPerLayer * l;
//         colorNum %= 256;

//         int targetIndex = polarToIndex(s, l);
//         // leds[targetIndex] = mainPalette[colorNum];
//         // leds[targetIndex] = CHSV(colorNum, 255, 255);
//       }
//     }
//   }
// }















// void zigZag() {
//   bool needsRedraw = false;

//   if(mSecondToneDirty || mRootHueDirty || pattCount1 == -1) {
//     // updateMainPalette();
//     needsRedraw = true;
//   }

//   pattCount1++;

//   if(pattCount1 > 3) {
//     needsRedraw = true;
//     pattVar1++;
//     pattVar1 %= 256;
//     pattCount1 = 0;
//   }

  
//   if(needsRedraw) {
//     for(int i = 0; i < NUM_LEDS; i++) {
//       int colorNum = (pattVar1 + i)%256;
//       // leds[i] = mainPalette[colorNum];
//     }
//   }
// }









// void rainbow() 
// {
//   // FastLED's built-in rainbow generator
//   fill_rainbow( leds, NUM_LEDS, gHue, 7);
// }

// void rainbowWithGlitter() 
// {
//   // built-in FastLED rainbow, plus some random sparkly glitter
//   rainbow();
//   addGlitter(80);
// }

// void addGlitter( fract8 chanceOfGlitter) 
// {
//   if( random8() < chanceOfGlitter) {
//     leds[ random16(NUM_LEDS) ] += CRGB::White;
//   }
// }

// void confetti() 
// {
//   // random colored speckles that blink in and fade smoothly
//   fadeToBlackBy( leds, NUM_LEDS, 10);
//   int pos = random16(NUM_LEDS);
//   leds[pos] += CHSV( gHue + random8(64), 200, 255);
// }

// void sinelon()
// {
//   // a colored dot sweeping back and forth, with fading trails
//   fadeToBlackBy( leds, NUM_LEDS, 20);
//   int pos = beatsin16( 13, 0, NUM_LEDS-1 );
//   leds[pos] += CHSV( gHue, 255, 192);
// }

// void bpm()
// {
//   // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
//   uint8_t BeatsPerMinute = 62;
//   CRGBPalette16 palette = PartyColors_p;
//   uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
//   for( int i = 0; i < NUM_LEDS; i++) { //9948
//     leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
//   }
// }

// void juggle() {
//   // eight colored dots, weaving in and out of sync with each other
//   fadeToBlackBy( leds, NUM_LEDS, 20);
//   byte dothue = 0;
//   for( int i = 0; i < 8; i++) {
//     leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
//     dothue += 32;
//   }
// }








/******************************************************
*                                                     *
*               MAIN LOOP AND SETUP         
*                                                     *
******************************************************/


/*******************************
*       PATTERN LAYERS
*******************************/
const uint8_t NUM_PATTERN_LAYERS = 2;
bool mPatternLayerReversed [NUM_PATTERN_LAYERS];
int mPatternLayerRunTimes [NUM_PATTERN_LAYERS];
uint8_t mPatternLayerFnIndex [NUM_PATTERN_LAYERS];
CRGB mPatternLayerLeds [NUM_LEDS * NUM_PATTERN_LAYERS];



/*******************************
*       SETUP
*******************************/
void setup() {
  delay(3000); // 3 second delay for recovery

  for(uint8_t p = 0; p < NUM_PATTERN_LAYERS; p++) {
    mPatternLayerRunTimes[p] = -1;
    if(p == 0) 
      mPatternLayerFnIndex[p] = STARTING_PATTERN_INDEX;
    else
      mPatternLayerFnIndex[p] = p % NUM_PATTERNS;
    // CRGB* ptr = PATTERN_LEDS + NUM_LEDS;
    mPatternLayerReversed[p] = false;
  }

  for(uint8_t l = 0; l < NUM_LEDS * NUM_PATTERN_LAYERS; l++) {
    mPatternLayerLeds[l] = CRGB(0,0,0);
  }

  
  
  markPaletteDirty();

  // button1.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(LEDS_OUT, NUM_LEDS).setDither(BRIGHTNESS_MAX<255);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(mBrightness);
  Serial.begin(9600);
}







/*******************************
*       MAIN LOOP
*******************************/
//mixing
int mPatternTransitionTimeTotal = -1;
int mPatternTransitionTimePassed = 0;
uint8_t mCurrentLayer = 0;
uint8_t mNextLayer = 1;

int mBeingPlayedWith = false;
int mTimeWithoutPlay = 0;
int mPatternFramesHeld = 0;

void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  // NUM_PATTERN_LAYERS
  for(uint8_t p = 0; p < NUM_PATTERN_LAYERS; p++) {
    uint8_t patternNum = mPatternLayerFnIndex[p];
    int runTime = mPatternLayerRunTimes[p];
    CRGB* leds = & mPatternLayerLeds[p * NUM_LEDS];
    gPatterns[patternNum](runTime, leds, mPatternLayerReversed[p]);  

    mPatternLayerRunTimes[p]++;
  }
  
  if(mPatternTransitionTimeTotal == -1) {
    for(uint8_t l = 0; l < NUM_LEDS; l++) {
      LEDS_OUT[l] = mPatternLayerLeds[l+ (mCurrentLayer * NUM_LEDS)];
    }
    if(mPatternFramesHeld < HOLD_PATTERN_FOR_AT_LEAST_N_FRAMES) {
      mPatternFramesHeld++;
    }
  }

  //TODO: loop to support more than 2 pattern layers
  else {
    double mixRatio = (double)mPatternTransitionTimePassed / (double)mPatternTransitionTimeTotal;
    uint8_t mix = mixRatio * (int)(FULL_BYTE);
    CRGB* fadeInLayer = & mPatternLayerLeds[NUM_LEDS * mNextLayer];
    CRGB* fadeOutLayer = & mPatternLayerLeds[NUM_LEDS * mCurrentLayer];
    // uint8_t mix = 128;

    for(uint8_t l = 0; l < NUM_LEDS; l++) {
      LEDS_OUT[l] = mixColors(fadeOutLayer[l], fadeInLayer[l], mix);
    }    

    mPatternTransitionTimePassed++;
    if(mPatternTransitionTimePassed >= mPatternTransitionTimeTotal) {
      mPatternTransitionTimePassed = 0;
      mPatternTransitionTimeTotal = -1;
      mCurrentLayer = mNextLayer; //binary switch as long as NUM_PATTERN_LAYERS == 2
      mNextLayer = 0;
    }
  }

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( PALETTE_STEP_EVERY_N_MS ) { 
    gMainPaletteOffset++; 
    gMainPaletteOffset %= PALETTE_SIZE;
  } 
  

  bool patternHasHadTime = mPatternFramesHeld >= HOLD_PATTERN_FOR_AT_LEAST_N_FRAMES;
  bool notBeingPlayedWith = mBeingPlayedWith == false;

  if(SINGLE_PATTERN_TEST_MODE == false && patternHasHadTime && notBeingPlayedWith) {
    nextPattern(random8(256) + 900);
  }


  
  watchPots();

  watchButton();
}




/*******************************
*  MAIN LOOP HELPER FUNCTIONS
*******************************/


const int pinIds[] = {rootHueInPin, secondTonesInPin, modInPin1, modInPin2};
const uint8_t NUM_POTS = ARRAY_SIZE(pinIds);
int lastObviousTweakPoints [NUM_POTS];

void watchPots() {
  //get pin value
  // int pinValue;
  // pinValue = analogRead(brightnessInPin);
  // bool brightnessChanged = updateForPinAndTellWhenChanged(pinValue, brightnessPinAvg, mBrightness, mBrightnessMin, mBrightnessMax);

  // if(brightnessChanged) {
  //   FastLED.setBrightness(mBrightness);
  // }


  
  

  
  
  //COLOR CHANGE CHECK
  int pinValue = analogRead(rootHueInPin);
  bool baseHueDirty = updateForPinAndTellWhenChanged(pinValue, rootHuePinAvg, mRootHue, mRootHueMin, mRootHueMax, 1);

  pinValue = analogRead(secondTonesInPin);     
  bool secondToneDirty = updateForPinAndTellWhenChanged(pinValue, secondTonesPinAvg, mSecondTone, mSecondToneMin, mSecondToneMax, 2);

  if(baseHueDirty || secondToneDirty) {
    markPaletteDirty();
  }



  //USER INTERACTION CHECK
  bool tweakFound = false;
  int currentPinValues [NUM_POTS];
  for(uint8_t p = 0; p < NUM_POTS; p++) {
    currentPinValues[p] = analogRead(pinIds[p]);
    int potDiff = abs(lastObviousTweakPoints[p] - currentPinValues[p]);
    bool potTweaked = potDiff >= OBVIOUS_USER_MOVEMENT;
    tweakFound = tweakFound || potTweaked;
  }

  if(tweakFound) {
    for(uint8_t p = 0; p < NUM_POTS; p++) {
      lastObviousTweakPoints[p] = currentPinValues[p];
    }

    mBeingPlayedWith = true;
    mTimeWithoutPlay = 0;  
    Serial.print("tweak found");
  }
  else if(mBeingPlayedWith) {
    mTimeWithoutPlay++;
    if(mTimeWithoutPlay > GAWKING_FRAMES_LIMIT) {
      Serial.print("no playing");
      mBeingPlayedWith = false;  // :(
      mTimeWithoutPlay = 0;
    }
  }
}





bool mButtonHeld = false;
bool buttonState = false;
int8_t buttonHoldTimer = -1;
const uint8_t buttonDebounceTimeout = 7;
const uint8_t buttonHoldMinTime = 35;

void watchButton() {
  bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  if(buttonPressed) {
    //button press
    if(buttonHoldTimer == -1) {
      buttonState = buttonPressed;
      Serial.print("button pressed");
    }

    //button held
    else if(buttonHoldTimer > buttonHoldMinTime && mButtonHeld == false) {
      mButtonHeld = true;
      Serial.print("button held");
    }
  }
  //button release (only after bounce)
  else if(buttonHoldTimer > buttonDebounceTimeout) {

    if(mButtonHeld == false) {
      nextPattern(80);
    }

    buttonState = buttonPressed;
    buttonHoldTimer = -1;
    mButtonHeld = false;
  }


  //increase button timer, as long as button is held
  if(buttonState) {
    buttonHoldTimer++;
  }
}







uint8_t nextPatternFnID = NUM_PATTERN_LAYERS;

// int pattCount1 = -1;
void nextPattern(int changeTime)
{
  Serial.print("pattern change");

  if(mPatternTransitionTimeTotal == -1) {
    mPatternTransitionTimePassed = 0;
    mPatternTransitionTimeTotal = changeTime;
    mNextLayer = (mCurrentLayer + 1) % NUM_PATTERN_LAYERS;
    mPatternLayerRunTimes[mNextLayer] = -1;
    mPatternLayerReversed[mNextLayer] = (random8(2) > 0);
    mPatternLayerFnIndex[mNextLayer] = nextPatternFnID;
    nextPatternFnID++;
    nextPatternFnID %= NUM_PATTERNS;
    // mPatternLayerFnIndex[mNextLayer] = randomIndexNotFoundInCollection(mPatternLayerFnIndex, NUM_PATTERN_LAYERS, NUM_PATTERNS);
  }
  else {
    mPatternFramesHeld = 0;
    double ratio = (double)(mPatternTransitionTimePassed)/ (double)(mPatternTransitionTimeTotal);
    mPatternTransitionTimePassed = ratio * mPatternTransitionTimePassed; 
    mPatternTransitionTimeTotal = changeTime;
  }

  
  
  
  
  // add one to the current pattern number, and wrap around at the end
  // gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}


bool updateForPinAndTellWhenChanged(int pinValue, int &pinAvg, uint8_t &outputValue, int outputMin, int outputMax, int curve) {
  if(pinValue != pinAvg) {
    bool outputDirty = false;

    //if the pot has been twisted
    if(abs(pinValue - pinAvg) > POT_JITTER_LIMIT) {
      outputDirty = true;
      outputValue = map(pinValue, 0, POT_MAX, outputMin, outputMax);

      if(curve > 1) {
        outputValue = pow(outputValue, curve)/pow(outputMax, curve-1);
      }

      pinAvg = pinValue;
    }

    //if the avg is noticably different than the current value
    else {
      pinAvg = (7*pinAvg/POT_AVERAGING) + (pinValue/POT_AVERAGING);
      uint8_t checkMe = map(pinAvg, 0, POT_MAX, outputMin, outputMax);
      if(curve > 1) {
        checkMe = pow(checkMe, curve)/pow(outputMax, curve-1);
      }

      if(abs(outputValue - checkMe) > 1) {
        outputValue = checkMe;
        outputDirty = true;
      }
    }

    return outputDirty;
  }
  return false;
}





/******************************************************
*                                                     *
*               HELPER FUNCTIONS          
*                                                     *
******************************************************/

int8_t randomIndexNotFoundInCollection(uint8_t* collection, uint8_t collectionSize, uint8_t numChoices) {
  uint8_t choosableItemCount = numChoices - collectionSize;
  if(choosableItemCount > 0) {
    uint8_t choosableItems [choosableItemCount];
    uint8_t insertionPoint = 0;
    for(uint8_t i = 0; i < choosableItemCount; i++) {
      bool notInUse = true;
      for(uint8_t c = 0; notInUse && c < collectionSize; c++) {
        notInUse = collection[c] != i;
      }

      if(notInUse) {
        choosableItems[insertionPoint] = i;
        insertionPoint++;
      }
    }

    return choosableItems[random8(choosableItemCount)];
  }

  return -1;
}


