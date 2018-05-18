// SMALL_BOOSH_1  = 1
// SMALL_BOOSH_2  = 2
// BIG_BOOSH      = 3
// pattern array goes { boosh num, ms hold open, ms till next, boosh num, ms hold open, ms till next, ...}

const uint8_t NUM_PATTERNS = 20;

uint8_t* buttonCombos[NUM_PATTERNS];
uint8_t  buttonComboLengths[NUM_PATTERNS];
uint8_t* patterns[NUM_PATTERNS];
uint8_t  patternLengths[NUM_PATTERNS];

// buttons: Start, Select, Up
// desc: all booshs for 1 sec
buttonCombos[0] = {3, 4, 7};
buttonComboLengths[0] = 3;
patterns[0] = {3, 1000, 0, 1, 1000, 0, 2, 1000, 0};
patternLengths[0] = 9;


// buttons: A, Up
// desc: big boosh left
buttonCombos[1] = {1, 7};
buttonComboLengths[1] = 2;
patterns[1] = {1, 600, 0};
patternLengths[1] = 3;


// buttons: A, Down
// desc: tiny boosh left
buttonCombos[2] = {1, 6};
buttonComboLengths[2] = 2;
patterns[2] = {1, 100, 0};
patternLengths[2] = 3;


// buttons: A, Left
// desc: normal boosh right, then big left
buttonCombos[3] = {1, 8};
buttonComboLengths[3] = 2;
patterns[3] = {2, 200, 200, 1, 600, 0};
patternLengths[3] = 6;


// buttons: A, Right
// desc: normal big left, then little right
buttonCombos[4] = {1, 5};
buttonComboLengths[4] = 2;
patterns[4] = {1, 600, 400, 2, 200, 0};
patternLengths[4] = 6;


// buttons: B, Up
// desc: big boosh right
buttonCombos[5] = {2, 7};
buttonComboLengths[5] = 2;
patterns[5] = {2, 600, 0};
patternLengths[5] = 3;


// buttons: B, Down
// desc: tiny boosh right
buttonCombos[6] = {2, 6};
buttonComboLengths[6] = 2;
patterns[6] = {2, 100, 0};
patternLengths[6] = 3;


// buttons: B, Right
// desc: normal boosh left, then big right
buttonCombos[7] = {2, 5};
buttonComboLengths[7] = 2;
patterns[7] = {1, 200, 200, 2, 600, 0};
patternLengths[7] = 6;


// buttons: B, Left
// desc: big boosh right, then normal left
buttonCombos[8] = {2, 8};
buttonComboLengths[8] = 2;
patterns[8] = {2, 600, 400, 1, 200, 0};
patternLengths[8] = 6;


// buttons: A, B
// desc: normal boosh left, right, left, right, left, right
buttonCombos[9] = {1, 2};
buttonComboLengths[9] = 2;
patterns[9] = {1, 200, 200, 2, 200, 200, 1, 200, 200, 2, 200, 200, 1, 200, 200, 2, 200, 200};
patternLengths[9] = 18;


// buttons: Start, Up
// desc: extra big boosh center
buttonCombos[10] = {3, 7};
buttonComboLengths[10] = 2;
patterns[10] = {3, 600, 0};
patternLengths[10] = 3;


// buttons: Start, Down
// desc: short big boosh center
buttonCombos[11] = {3, 6};
buttonComboLengths[11] = 2;
patterns[11] = {3, 100, 0};
patternLengths[11] = 3;


// buttons: Select, Up
// desc: all booshs long
buttonCombos[12] = {4, 7};
buttonComboLengths[12] = 2;
patterns[12] = {3, 600, 0, 1, 600, 0, 2, 600, 0};
patternLengths[12] = 9;


// buttons: Select, Down
// desc: all booshs short
buttonCombos[13] = {4, 6};
buttonComboLengths[13] = 2;
patterns[13] = {3, 100, 0, 1, 100, 0, 2, 100, 0};
patternLengths[13] = 9;


// buttons: Select, Right
// desc: all booshs left to right
buttonCombos[14] = {4, 5};
buttonComboLengths[14] = 2;
patterns[14] = {3, 200, 0, 1, 200, 200, 2, 200, 0};
patternLengths[14] = 9;


// buttons: Select, Left
// desc: all booshs right to left
buttonCombos[15] = {4, 8};
buttonComboLengths[15] = 2;
patterns[15] = {3, 200, 0, 2, 200, 200, 1, 200, 0};
patternLengths[15] = 9;


// buttons: A
// desc: normal boosh left
buttonCombos[16] = {1};
buttonComboLengths[16] = 1;
patterns[16] = {1, 200, 0};
patternLengths[16] = 3;


// buttons: B
// desc: normal boosh right
buttonCombos[17] = {2};
buttonComboLengths[17] = 1;
patterns[17] = {2, 200, 0};
patternLengths[17] = 3;


// buttons: Start
// desc: normal big boosh center
buttonCombos[18] = {3};
buttonComboLengths[18] = 1;
patterns[18] = {3, 200, 0};
patternLengths[18] = 3;


// buttons: Select
// desc: all booshs
buttonCombos[19] = {4};
buttonComboLengths[19] = 1;
patterns[19] = {3, 200, 0, 1, 200, 0, 2, 200, 0};
patternLengths[19] = 9;
