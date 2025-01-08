#pragma once

#define NUM_WINS 4

// Section 6.3.7 - Multi Most Common in Window (MCW) Prediction Estimate
double multi_mcw_test(uint8_t *data, long len, int alph_size, const int verbose, const char *label);