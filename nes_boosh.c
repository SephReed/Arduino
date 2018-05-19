

const uint8_t BTN_A = 1;
const uint8_t BTN_B = 2;
const uint8_t BTN_START = 3;
const uint8_t BTN_SELECT = 4;
const uint8_t BTN_RIGHT = 5;
const uint8_t BTN_DOWN = 6;
const uint8_t BTN_UP = 7;
const uint8_t BTN_LEFT = 8;

const uint8_t BOOSH_LEFT = 1;
const uint8_t BOOSH_RIGHT = 2;
const uint8_t BOOSH_BIG = 3;

const int MS_SHORT = 100;
const int MS_MED = 200;
const int MS_EXTRA_MED = 400;
const int MS_LONG = 600;
const int MS_VERY_LONG = 1000;

const uint8_t NUM_PATTERNS = 20;

uint8_t* buttonCombos[NUM_PATTERNS];
uint8_t  buttonComboLengths[NUM_PATTERNS];
uint16_t* patterns[NUM_PATTERNS];
uint8_t  patternLengths[NUM_PATTERNS];

// pattern array goes { boosh num, ms hold open, ms till next, boosh num, ms hold open, ms till next, ...}

// desc: all booshs for 1 sec
buttonCombos[0] = {BTN_START, BTN_SELECT, BTN_UP};
buttonComboLengths[0] = 3;
patterns[0] = {BOOSH_BIG, MS_VERY_LONG, 0, BOOSH_LEFT, MS_VERY_LONG, 0, BOOSH_RIGHT, MS_VERY_LONG, 0};
patternLengths[0] = 9;


// desc: big boosh left
buttonCombos[1] = {BTN_A, BTN_UP};
buttonComboLengths[1] = 2;
patterns[1] = {BOOSH_LEFT, MS_LONG, 0};
patternLengths[1] = 3;


// desc: tiny boosh left
buttonCombos[2] = {BTN_A, BTN_DOWN};
buttonComboLengths[2] = 2;
patterns[2] = {BOOSH_LEFT, MS_SHORT, 0};
patternLengths[2] = 3;


// desc: normal boosh right, then big left
buttonCombos[3] = {BTN_A, BTN_LEFT};
buttonComboLengths[3] = 2;
patterns[3] = {BOOSH_RIGHT, MS_MED, MS_MED, BOOSH_LEFT, MS_LONG, 0};
patternLengths[3] = 6;


// desc: normal big left, then little right
buttonCombos[4] = {BTN_A, BTN_RIGHT};
buttonComboLengths[4] = 2;
patterns[4] = {BOOSH_LEFT, MS_LONG, MS_EXTRA_MED, BOOSH_RIGHT, MS_MED, 0};
patternLengths[4] = 6;


// desc: big boosh right
buttonCombos[5] = {BTN_B, BTN_UP};
buttonComboLengths[5] = 2;
patterns[5] = {BOOSH_RIGHT, MS_LONG, 0};
patternLengths[5] = 3;


// desc: tiny boosh right
buttonCombos[6] = {BTN_B, BTN_DOWN};
buttonComboLengths[6] = 2;
patterns[6] = {BOOSH_RIGHT, MS_SHORT, 0};
patternLengths[6] = 3;


// desc: normal boosh left, then big right
buttonCombos[7] = {BTN_B, BTN_RIGHT};
buttonComboLengths[7] = 2;
patterns[7] = {BOOSH_LEFT, MS_MED, MS_MED, BOOSH_RIGHT, MS_LONG, 0};
patternLengths[7] = 6;


// desc: big boosh right, then normal left
buttonCombos[8] = {BTN_B, BTN_LEFT};
buttonComboLengths[8] = 2;
patterns[8] = {BOOSH_RIGHT, MS_LONG, MS_EXTRA_MED, BOOSH_LEFT, MS_MED, 0};
patternLengths[8] = 6;


// desc: normal boosh left, right, left, right, left, right
buttonCombos[9] = {BTN_A, BTN_B};
buttonComboLengths[9] = 2;
patterns[9] = {BOOSH_LEFT, MS_MED, MS_MED, BOOSH_RIGHT, MS_MED, MS_MED, BOOSH_LEFT, MS_MED, MS_MED, BOOSH_RIGHT, MS_MED, MS_MED, BOOSH_LEFT, MS_MED, MS_MED, BOOSH_RIGHT, MS_MED, MS_MED};
patternLengths[9] = 18;


// desc: extra big boosh center
buttonCombos[10] = {BTN_START, BTN_UP};
buttonComboLengths[10] = 2;
patterns[10] = {BOOSH_BIG, MS_LONG, 0};
patternLengths[10] = 3;


// desc: short big boosh center
buttonCombos[11] = {BTN_START, BTN_DOWN};
buttonComboLengths[11] = 2;
patterns[11] = {BOOSH_BIG, MS_SHORT, 0};
patternLengths[11] = 3;


// desc: all booshs long
buttonCombos[12] = {BTN_SELECT, BTN_UP};
buttonComboLengths[12] = 2;
patterns[12] = {BOOSH_BIG, MS_LONG, 0, BOOSH_LEFT, MS_LONG, 0, BOOSH_RIGHT, MS_LONG, 0};
patternLengths[12] = 9;


// desc: all booshs short
buttonCombos[13] = {BTN_SELECT, BTN_DOWN};
buttonComboLengths[13] = 2;
patterns[13] = {BOOSH_BIG, MS_SHORT, 0, BOOSH_LEFT, MS_SHORT, 0, BOOSH_RIGHT, MS_SHORT, 0};
patternLengths[13] = 9;


// desc: all booshs left to right
buttonCombos[14] = {BTN_SELECT, BTN_RIGHT};
buttonComboLengths[14] = 2;
patterns[14] = {BOOSH_BIG, MS_MED, 0, BOOSH_LEFT, MS_MED, MS_MED, BOOSH_RIGHT, MS_MED, 0};
patternLengths[14] = 9;


// desc: all booshs right to left
buttonCombos[15] = {BTN_SELECT, BTN_LEFT};
buttonComboLengths[15] = 2;
patterns[15] = {BOOSH_BIG, MS_MED, 0, BOOSH_RIGHT, MS_MED, MS_MED, BOOSH_LEFT, MS_MED, 0};
patternLengths[15] = 9;


// desc: normal boosh left
buttonCombos[16] = {BTN_A};
buttonComboLengths[16] = 1;
patterns[16] = {BOOSH_LEFT, MS_MED, 0};
patternLengths[16] = 3;


// desc: normal boosh right
buttonCombos[17] = {BTN_B};
buttonComboLengths[17] = 1;
patterns[17] = {BOOSH_RIGHT, MS_MED, 0};
patternLengths[17] = 3;


// desc: normal big boosh center
buttonCombos[18] = {BTN_START};
buttonComboLengths[18] = 1;
patterns[18] = {BOOSH_BIG, MS_MED, 0};
patternLengths[18] = 3;


// desc: all booshs
buttonCombos[19] = {BTN_SELECT};
buttonComboLengths[19] = 1;
patterns[19] = {BOOSH_BIG, MS_MED, 0, BOOSH_LEFT, MS_MED, 0, BOOSH_RIGHT, MS_MED, 0};
patternLengths[19] = 9;
