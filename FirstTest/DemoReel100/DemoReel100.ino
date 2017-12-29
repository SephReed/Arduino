// #include <Button.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//#define MOUTH_DATA_PIN      9
//#define SCLERA_DATA_PIN     8
#define PUPIL_DATA_PIN      7
#define BUTTON_PIN          6
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
#define OVERALL_BRIGHTNESS  60
#define SATURATION_MAX      250
#define BASELINE_BRIGHTNESS 90
#define FRAMES_PER_SECOND  120
#define PALETTE_STEP_EVERY_N_MS 200
#define HOLD_PATTERN_FOR_AT_LEAST_N_FRAMES 600
#define GAWKING_FRAMES_LIMIT 500



/*******************************
*       LED ARRANGEMENT
*******************************/
#define NUM_MOUTH_LEDS 50
#define NUM_SCLERA_LEDS 16


#define NUM_SPOKES 16
#define NUM_LAYERS 4
//NUM_PUPIL_LEDS can not be greater than 256 without modifying much of the code
const uint8_t NUM_PUPIL_LEDS = NUM_SPOKES * NUM_LAYERS;
CRGB PUPIL_LEDS_OUT[NUM_PUPIL_LEDS];



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
const uint8_t mSecondToneMin = 20;
const uint8_t mSecondToneMax = ONE_THIRD_BYTE;
uint8_t mSecondTone = mSecondToneMin;
bool mSecondToneDirty = false;

const int modInPin1 = A1;
int modPin1Avg = 0;
int gModPin1 = 0;

const int modInPin2 = A2;
int modPin2Avg = 0;
int gModPin2 = 0;




/*******************************
*       INPUT BUTTONS
*******************************/
bool mButtonHeld = false;







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
const uint8_t PALETTE_SIZE = 96;
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
void warpSpeed(int runTime, CRGB* leds, bool reversed);
void orbits(int runTime, CRGB* leds, bool reversed);
void sprinkler(int runTime, CRGB* leds, bool reversed);
void zigZag(int runTime, CRGB* leds, bool reversed);
void gradientRotate(int runTime, CRGB* leds, bool reversed);
void randomConfetti(int runTime, CRGB* leds, bool reversed);
void flappy(int runTime, CRGB* leds, bool reversed);



// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])(int runTime, CRGB* leds, bool reversed);
SimplePatternList gPatterns = { gradientRotate, randomConfetti, flappy, zigZag, sprinkler, orbits, warpSpeed };

const uint8_t NUM_PATTERNS = ARRAY_SIZE(gPatterns);
const bool SINGLE_PATTERN_TEST_MODE = false;
const uint8_t STARTING_PATTERN_INDEX = 0;




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
*   RANDOM
**************************/
void randomConfetti(int runTime, CRGB* leds, bool reversed) {
  const uint8_t minChance = 50;
  const uint8_t maxChance = 150;
  uint8_t chance = map(gModPin2, 0, 1023, 0, maxChance-minChance) + minChance;
  uint8_t colorSpread = map(gModPin1, 0, 1023, 0, PALETTE_SIZE/2);

  if(reversed == true)
    fadeOutToDarkness(leds, NUM_PUPIL_LEDS, BASELINE_BRIGHTNESS, (chance/20));

  bool dudFound = false;
  for(int c = 0; dudFound == false && c < 30; c++) {
    uint8_t randNum = random8(FULL_BYTE);
    if(randNum < chance) {
      CRGB color;
      if(reversed == true || randNum < (4*chance)/5) {
        uint8_t colorOffset = random8(colorSpread);
        int colorNum = gMainPaletteOffset + colorOffset - (colorSpread/2);  
        color = getPaletteColorNum(colorNum);
      }
      else {
        color = CRGB(0,0,0);
      }

      uint8_t targetIndex = random8(NUM_PUPIL_LEDS);
      leds[targetIndex] = color;
    }
    else {
      dudFound = true;
    }
  }
}






/*************************
*   FLAPPY
**************************/
const uint8_t flappyBirdsMax = 8;
const uint8_t flappyBirdsMin = 1;
int flappyBirdsNextStepTime = 0;
uint8_t flappyBirdsRotation = 0;
uint8_t flappyBirdsRing = 0;
bool flappyBirdsRising = true;

void flappy(int runTime, CRGB* leds, bool reversed) {
  if(runTime == -1) {
    flappyBirdsRotation = 0;
    flappyBirdsNextStepTime = 0;
  }


  uint8_t numBirds = map(gModPin2, 0, 1023, 0, flappyBirdsMax-flappyBirdsMin) + flappyBirdsMin;
  uint8_t colorOffsetPerBird = map(gModPin1, 0, 1023, 0, PALETTE_SIZE/flappyBirdsMax);

  fadeOutToDarkness(leds, NUM_PUPIL_LEDS, BASELINE_BRIGHTNESS, 5 + (numBirds * 3));

  for(uint8_t b = 0; b < flappyBirdsMax; b++) {
    uint8_t spoke;
    if(b == 1)
      spoke = 4;
    else if(b == 2)
      spoke = 2;
    else
      spoke = b*2;

    if(b < numBirds) {
      uint8_t colorNum = gMainPaletteOffset + (b * colorOffsetPerBird);

      uint8_t targetIndex = polarToIndex(spoke + flappyBirdsRotation, flappyBirdsRing);
      leds[targetIndex] = getPaletteColorNum(colorNum);  
    }
  }

  bool stepBirdsForward = (runTime >= flappyBirdsNextStepTime);
  if(stepBirdsForward) {
    if(reversed == false)
      flappyBirdsRotation++;
    else
      flappyBirdsRotation--;

    flappyBirdsRotation %= NUM_SPOKES;
    flappyBirdsNextStepTime = runTime + 6;

     if(flappyBirdsRising) {
        if(flappyBirdsRing == 3) {
          flappyBirdsRising = false; 
        }
        else
          flappyBirdsRing++;
      }
      else {
        if(flappyBirdsRing == 0) {
          flappyBirdsRising = true;
        }
        else
          flappyBirdsRing--;
      }
  }
}









// /*************************
// *   SMILEY
// **************************/
// // const int pinIds[] = {rootHueInPin, secondTonesInPin, modInPin1, modInPin2};
// const uint8_t smileyEyesOpenCoordinates[] = { 
//   1,0,  1,1,    2,0,  2,1,  
//   6,0,  6,1,    5,0,  5,1
// };
// const uint8_t smileyEyesOpenCoordinatesCount = ARRAY_SIZE(smileyEyesOpenCoordinates);


// const uint8_t smileyEyesBlinkCoordinates[] = { 
//   1,1,  2,0,  
//   6,1,  5,0, 
// };
// const uint8_t smileyEyesBlinkCoordinatesCount = ARRAY_SIZE(smileyEyesBlinkCoordinates);


// const uint8_t smileyMouthCoordinates[] = {  
//    10,1, 11,1, 12,1, 13,1, 
// };
// const uint8_t smileyMouthCoordinatesCount = ARRAY_SIZE(smileyMouthCoordinates);

// const uint8_t smileyMouthClosedCoordinates[] = {  
//   9,1, 14,1
// };
// const uint8_t smileyMouthClosedCoordinatesCount = ARRAY_SIZE(smileyMouthClosedCoordinates);

// const uint8_t smileyMouthOpenCoordinates[] = {  
//    11,0, 12,0, 
// };
// const uint8_t smileyMouthOpenCoordinatesCount = ARRAY_SIZE(smileyMouthOpenCoordinates);


// int smileyNextBlinkTime = 0;
// int8_t smileyBlinkTimer = -1;
// int smileyNextBlurb = 0;
// int8_t smileyBlurbTimer = -1;
// uint8_t smileyLongPauseRolls = 0;


// void smileyDrawCoordinates(uint8_t* coords, uint8_t count, CRGB* leds) {
//   for(uint8_t c = 0; c < count; c += 2) {
//     uint8_t spoke = coords[c];
//     uint8_t ring = coords[c+1];
//     uint8_t targetIndex = polarToIndex(spoke, ring);
//     leds[targetIndex] = getPaletteColorNum(gMainPaletteOffset);
//   }
// }

// void smiley(int runTime, CRGB* leds, bool reversed) {
//   if(runTime == -1) {
//     smileyNextBlinkTime = 0;
//     smileyBlinkTimer = -1;
//     smileyNextBlurb = 0;
//     smileyBlurbTimer = -1;
//     smileyLongPauseRolls = 8;
//   }

//   if(runTime >= smileyNextBlinkTime) {
//     smileyNextBlinkTime = runTime + 250 + random8(250);
//     smileyBlinkTimer = 0;
//   }
//   if(runTime >= smileyNextBlurb) {
//     int timeout = random8(20);
//     if(timeout == 19) {
//       smileyLongPauseRolls++;
//       if(smileyLongPauseRolls > 5) {
//         timeout = 800;  
//         smileyLongPauseRolls = 0;
//       }
//     }
//     smileyNextBlurb = runTime + 5 + timeout;
//     smileyBlurbTimer = 0;
//   }

//   fadeOutToDarkness(leds, NUM_PUPIL_LEDS, 0, 50);


//   smileyDrawCoordinates(smileyEyesBlinkCoordinates, smileyEyesBlinkCoordinatesCount, leds);
//   if((smileyBlinkTimer == -1) == (reversed == false)) {
//     smileyDrawCoordinates(smileyEyesOpenCoordinates, smileyEyesOpenCoordinatesCount, leds);
//   }

//   smileyDrawCoordinates(smileyMouthCoordinates, smileyMouthCoordinatesCount, leds);
//   if((smileyBlurbTimer != -1) == (reversed == false)) {
//     smileyDrawCoordinates(smileyMouthOpenCoordinates, smileyMouthOpenCoordinatesCount, leds);
//   }
//   else {
//     smileyDrawCoordinates(smileyMouthClosedCoordinates, smileyMouthClosedCoordinatesCount, leds);
//   }


//   if(smileyBlinkTimer != -1) {
//     smileyBlinkTimer++;
//     if(smileyBlinkTimer > 20) {
//       smileyBlinkTimer = -1;
//     }
//   }

//   if(smileyBlurbTimer != -1) {
//     smileyBlurbTimer++;
//     if(smileyBlurbTimer > 5) {
//       smileyBlurbTimer = -1;
//     }
//   }
// }







/*************************
*   GRADIENT ROTATE
**************************/
const uint8_t gradientStepMaxPause = 15;
const uint8_t gradientStepMinPause = 1;

uint8_t gradientRotateOffset;
int lastGradientStepTime;

void gradientRotate(int runTime, CRGB* leds, bool reversed) {
  if(runTime == -1) {
    gradientRotateOffset = 0;
    lastGradientStepTime = 0;
  }

  uint8_t currentPause = gradientStepMaxPause - map(gModPin2, 0, 1023, 0, gradientStepMaxPause - gradientStepMinPause);
  if(runTime - lastGradientStepTime >= currentPause) {
    lastGradientStepTime = runTime;
    if(reversed == false)
      gradientRotateOffset++;
    else
      gradientRotateOffset--;

    gradientRotateOffset %= PALETTE_SIZE;

    uint8_t stepSize = map(gModPin1, 0, 1023, 0, 12);
    for(uint8_t l = 0; l < NUM_PUPIL_LEDS; l++) {
      uint8_t colorNum = gMainPaletteOffset + gradientRotateOffset + l;
      if(stepSize != 0) {
        colorNum = (colorNum * stepSize)/2;
      }

      leds[l] = getPaletteColorNum(colorNum);
    }
  }


  
  // leds[gMainPaletteOffset%NUM_PUPIL_LEDS] = CRGB(250, 250, 250);
}





/*************************
*   ZIGZAG
**************************/
const uint8_t zigAntMinPause = 2;
const uint8_t zigAntMaxPause = NUM_PUPIL_LEDS/2;
const uint8_t maxZigAnts = (NUM_PUPIL_LEDS/zigAntMinPause) + 2;
const uint8_t moveForwardPause = 6;

int lastZigAntTime = 0;
int nextZigAntStepForwardTime = 0;
uint8_t zigAntCount = 0;
uint8_t firstZigAntIndex = 0;
int8_t zigAntPosition [maxZigAnts];
// uint8_t zigAntColorOffset [maxZigAnts];

void zigZag(int runTime, CRGB* leds, bool reversed) {
  if(runTime == -1){
    for(uint8_t a = 0; a < maxZigAnts; a++) {
      zigAntPosition[a] = -1;
    }

    firstZigAntIndex = 0;
    zigAntCount = 0;
    lastZigAntTime = 0;
    nextZigAntStepForwardTime = 0;
  }


  uint8_t currentZigAntPause = zigAntMaxPause - map(gModPin2, 0, 1023, 0, zigAntMaxPause - zigAntMinPause);
  // fadeOutToDarkness(leds, NUM_PUPIL_LEDS, BASELINE_BRIGHTNESS, 20 - (currentZigAntPause/4));
  fadeOutToDarkness(leds, NUM_PUPIL_LEDS, BASELINE_BRIGHTNESS, 30);

  int nextZigAntTime = lastZigAntTime + (currentZigAntPause * moveForwardPause);
  if(nextZigAntTime <= runTime) {
    zigAntPosition[firstZigAntIndex + zigAntCount] = 0;
    zigAntCount++;
    lastZigAntTime = runTime;
  }


  uint8_t colorOffsetPerAntIndex = map(gModPin1, 0, 1023, 0, 12);
  bool doStepForward = nextZigAntStepForwardTime <= runTime;
  uint8_t lostAntsCount = 0;

  for(uint8_t a = 0; a < zigAntCount; a++) {
    uint8_t index = (firstZigAntIndex + a) % maxZigAnts;

    uint8_t colorNum = gMainPaletteOffset;
    colorNum += colorOffsetPerAntIndex * index;

    uint8_t targetIndex = zigAntPosition[index];
    leds[targetIndex] = getPaletteColorNum(colorNum);

    if(doStepForward) {
      zigAntPosition[index]++;

      if(zigAntPosition[index] >= NUM_PUPIL_LEDS) {
        zigAntPosition[index] = -1;
        lostAntsCount++;
      }
    }
  }

  if(doStepForward) {
    nextZigAntStepForwardTime = runTime + moveForwardPause;

    firstZigAntIndex += lostAntsCount;
    firstZigAntIndex %= maxZigAnts;
    zigAntCount -= lostAntsCount;
  }
  

  // leds[colorOffsetPerAntIndex] = CRGB(255, 0, 0);
}






/*************************
*   SPRINKLER
**************************/


int8_t dropletLayer [NUM_SPOKES];
uint8_t dropletColorOffset [NUM_SPOKES];
uint8_t sprinklerCurrentSpoke = 0;
int sprinklerLastDropletTime = 0;
int sprinklerNextDropletMoveTime = 0;

const uint8_t sprinklerMinPause = 1;
const uint8_t sprinklerMaxPause = 15;

void sprinkler(int runTime, CRGB* leds, bool reversed) {
  if(runTime == -1) {
    for(uint8_t s = 0; s < NUM_SPOKES; s++) {
      dropletLayer[s] = -1;
    }
    sprinklerLastDropletTime = 0;
    sprinklerNextDropletMoveTime = 0;
  }


  uint8_t speed = sprinklerMaxPause - map(gModPin2, 0, 1023, 0, sprinklerMaxPause-sprinklerMinPause);
  fadeOutToDarkness(leds, NUM_PUPIL_LEDS, BASELINE_BRIGHTNESS, 16 - speed);

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

  uint8_t colorOffsetPerSpoke = map(gModPin1, 0, 1023, 0, PALETTE_SIZE/NUM_SPOKES);
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


const uint8_t orbitMaxComets = 8;
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

  

  uint8_t numComets = map(gModPin2, 0, 1023, 0, orbitMaxComets-orbitMinComets) + orbitMinComets;
  uint8_t colorOffsetPerLayer = map(gModPin1, 0, 1023, 0, PALETTE_SIZE/NUM_LAYERS);

  fadeOutToDarkness(leds, NUM_PUPIL_LEDS, BASELINE_BRIGHTNESS, numComets-3);

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

const uint8_t warpMaxComets = NUM_SPOKES-2;
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

  fadeOutToDarkness(leds, NUM_PUPIL_LEDS, BASELINE_BRIGHTNESS, 12);  

  uint8_t colorRange = map(gModPin1, 0, 1023, 0, PALETTE_SIZE);
  uint8_t numComets = map(gModPin2, 0, 1023, 0, warpMaxComets-warpMinComets) + warpMinComets;
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
  spoke %= NUM_SPOKES;
  layer %= NUM_LAYERS;

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



//void showMouthColor(uint8_t scale, CRGB color, ) {
//  // guard against showing too rapidly
//  while(m_nMinMicros && ((micros()-lastshow) < m_nMinMicros));
//  lastshow = micros();
//
//  // If we have a function for computing power, use it!
//  if(m_pPowerFunc) {
//    scale = (*m_pPowerFunc)(scale, m_nPowerData);
//  }
//
//  CLEDController *pCur = CLEDController::head();
//  while(pCur) {
//    uint8_t d = pCur->getDither();
//    if(m_nFPS < 100) { pCur->setDither(0); }
//    pCur->showLeds(scale);
//    pCur->setDither(d);
//    pCur = pCur->next();
//  }
//  countFPS();
//}









/******************************************************
*                                                     *
*                PALETTE FUNCTIONS          
*                                                     *
******************************************************/

CRGB mouthColor;
CRGB scleraColor;

void markPaletteDirty() {
    //
  mainTones[0] = HSV_to_RGB(mRootHue, SATURATION_MAX, FULL_BYTE);
  mainTones[1] = HSV_to_RGB(mRootHue + mSecondTone, SATURATION_MAX, FULL_BYTE);
  mainTones[2] = HSV_to_RGB(mRootHue - mSecondTone, SATURATION_MAX, FULL_BYTE);

  CRGB mouthColor = mixColors(mainTones[0], mixColors(mainTones[1], mainTones[2], FULL_BYTE/2), FULL_BYTE/3);

  const uint8_t dR = 3;  //desaturate ratio
  uint8_t red = (mouthColor.r/dR) + ((dR-1)*FULL_BYTE)/dR;
  uint8_t green = (mouthColor.g/dR) + ((dR-1)*FULL_BYTE)/dR;
  uint8_t blue = (mouthColor.b/dR) + ((dR-1)*FULL_BYTE)/dR;
  CRGB scleraColor = CRGB(red, green, blue);

  for(uint8_t i = 0; i < PALETTE_SIZE; i++) {
    mainPaletteDirty[i] = true;
  }
}

const uint8_t oneThirdPaletteSize = PALETTE_SIZE/3;  //42
CRGB getPaletteColorNum(uint8_t colorNum) {
  if(colorNum >= PALETTE_SIZE) {
    colorNum %= PALETTE_SIZE;
  }

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

    // if(colorNum >= 56 && colorNum <= 64) {
    //   // Serial.print("#");
    //   // Serial.print(colorNum);
    //   // Serial.print(", Mix ");
    //   Serial.println(mixAmount);
    // }

    // mainPalette[colorNum] = CRGB(255, 0, mixAmount);
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
    uint8_t maxValue = max(max(rgb[0], rgb[1]), rgb[2]);

    for(uint8_t i = 0; i < 3; i++) {
      rgb[i] += ((maxValue-rgb[i]) * (FULL_BYTE-saturation))/FULL_BYTE;
    }
  }
  

  return CRGB(rgb[0], rgb[1], rgb[2]);
}




















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
CRGB mPatternLayerLeds [NUM_PUPIL_LEDS * NUM_PATTERN_LAYERS];




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
    // CRGB* ptr = PATTERN_LEDS + NUM_PUPIL_LEDS;
    mPatternLayerReversed[p] = false;
  }

  for(uint8_t l = 0; l < NUM_PUPIL_LEDS * NUM_PATTERN_LAYERS; l++) {
    mPatternLayerLeds[l] = CRGB(0,0,0);
  }

  
  for(uint8_t p = 0; p < PALETTE_SIZE; p++) {
    mainPalette[p] = CRGB(1,1,5);
  }
  markPaletteDirty();

  // button1.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,PUPIL_DATA_PIN,COLOR_ORDER>(PUPIL_LEDS_OUT, NUM_PUPIL_LEDS).setDither(true);
//  FastLED.addLeds<LED_TYPE,SCLERA_DATA_PIN,COLOR_ORDER>(NULL, NUM_SCLERA_LEDS).setDither(true);
  // FastLED.addLeds<LED_TYPE,MOUTH_DATA_PIN,COLOR_ORDER>(MOUTH_LEDS_OUT, NUM_MOUTH_LEDS).setDither(true);
  

  // set master brightness control
  FastLED.setBrightness(OVERALL_BRIGHTNESS);
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

bool mBeingPlayedWith = false;
int mTimeWithoutPlay = 0;
int mPatternFramesHeld = 0;

bool nextPatternForced = false;

void loop() {
  // Call the current pattern function once, updating the 'leds' array
  // NUM_PATTERN_LAYERS
  for(uint8_t p = 0; p < NUM_PATTERN_LAYERS; p++) {
    uint8_t patternNum = mPatternLayerFnIndex[p];
    int runTime = mPatternLayerRunTimes[p];
    // if(runTime != -1)
    //   runTime += skippedFrames;

    CRGB* leds = & mPatternLayerLeds[p * NUM_PUPIL_LEDS];
    gPatterns[patternNum](runTime, leds, mPatternLayerReversed[p]);  

    mPatternLayerRunTimes[p]++;
  }
  
  if(mPatternTransitionTimeTotal == -1) {
    for(uint8_t l = 0; l < NUM_PUPIL_LEDS; l++) {
      PUPIL_LEDS_OUT[l] = mPatternLayerLeds[l+ (mCurrentLayer * NUM_PUPIL_LEDS)];
    }
    if(mPatternFramesHeld < HOLD_PATTERN_FOR_AT_LEAST_N_FRAMES) {
      mPatternFramesHeld++;
    }
  }

  //TODO: loop to support more than 2 pattern layers
  else {
    double mixRatio = (double)mPatternTransitionTimePassed / (double)mPatternTransitionTimeTotal;
    uint8_t mix = mixRatio * (int)(FULL_BYTE);
    CRGB* fadeInLayer = & mPatternLayerLeds[NUM_PUPIL_LEDS * mNextLayer];
    CRGB* fadeOutLayer = & mPatternLayerLeds[NUM_PUPIL_LEDS * mCurrentLayer];
    // uint8_t mix = 128;

    for(uint8_t l = 0; l < NUM_PUPIL_LEDS; l++) {
      PUPIL_LEDS_OUT[l] = mixColors(fadeOutLayer[l], fadeInLayer[l], mix);
    }    

    if(nextPatternForced || mBeingPlayedWith == false) {
      mPatternTransitionTimePassed++;
      if(mPatternTransitionTimePassed >= mPatternTransitionTimeTotal) {
        mPatternTransitionTimePassed = 0;
        mPatternTransitionTimeTotal = -1;
        mCurrentLayer = mNextLayer; //binary switch as long as NUM_PATTERN_LAYERS == 2
        mNextLayer = 0;
        nextPatternForced = false;
      }
    }
  }

  // showLEDS();
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
    nextPattern(random16(800) + 1200);
  }


  
  watchPots();

  watchButton();
}




/*******************************
*  MAIN LOOP HELPER FUNCTIONS
*******************************/


const uint8_t pinIds[] = {rootHueInPin, secondTonesInPin, modInPin1, modInPin2};
const uint8_t NUM_POTS = ARRAY_SIZE(pinIds);
int lastObviousTweakPoints [NUM_POTS];

void watchPots() {
  //MOD PIN UPDATE
  int pinValue = analogRead(modInPin1);
  updateModPinData(pinValue, modPin1Avg, gModPin1);

  pinValue = analogRead(modInPin2);
  updateModPinData(pinValue, modPin2Avg, gModPin2);
  
  
  
  //COLOR CHANGE CHECK
  pinValue = analogRead(rootHueInPin);
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






bool buttonState = false;
int8_t buttonHoldTimer = -1;
int8_t buttonReleaseTimer = -1;
const uint8_t buttonDebounceTimeout = 15;
const uint8_t buttonHoldMinTime = 35;

void watchButton() {
  bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  //if pressed now reset release timer
  //check if the hold timer has not yet started, if it hasn't this is initial button pressing
  //if it has started and it's over the min hold time, set held to true
  if(buttonPressed) {
    buttonReleaseTimer = 0;
    mTimeWithoutPlay = 0;

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
    buttonReleaseTimer++;
    mTimeWithoutPlay = 0;

    if(buttonReleaseTimer > buttonDebounceTimeout) {
      if(mButtonHeld == false) {
        nextPatternForced = true;
        nextPattern(80);
      }

      buttonState = buttonPressed;
      buttonHoldTimer = -1;
      mButtonHeld = false;
    }
  }


  //increase button timer, as long as button is held
  if(buttonState && buttonHoldTimer < buttonHoldMinTime + 1) {
    buttonHoldTimer++;
  }
}







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


    uint8_t numChoices = NUM_PATTERNS - NUM_PATTERN_LAYERS;
    uint8_t patternChoices [numChoices];
    uint8_t insertAt = 0;
    for(uint8_t p = 0; p < NUM_PATTERNS; p++) {
      bool patternInUse = false;
      for(uint8_t l = 0; patternInUse == false && l < NUM_PATTERN_LAYERS; l++) {
        patternInUse = mPatternLayerFnIndex[l] == p;
      }
      if(patternInUse == false) {
        patternChoices[insertAt] = p;
        insertAt++;
      }
    }
    mPatternLayerFnIndex[mNextLayer] = patternChoices[random8(numChoices)];
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
      pinAvg = ((POT_AVERAGING-1)*pinAvg/POT_AVERAGING) + (pinValue/POT_AVERAGING);
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


void updateModPinData(int pinValue, int &pinAvg, int &globalValue) {
  int maxMovement = max(abs(pinValue - pinAvg), abs(pinValue - globalValue));
  if(maxMovement > POT_JITTER_LIMIT) {
    pinAvg = pinValue;
    globalValue = pinAvg;
  }
  else {
    pinAvg = ((POT_AVERAGING-1)*pinAvg/POT_AVERAGING) + (pinValue/POT_AVERAGING);
  }
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


