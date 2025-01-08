#pragma once

#define B_len 16
#define MAX_DICTIONARY_SIZE 65536

static double binaryLZ78YPredictionEstimate(const uint8_t *S, long L, const int verbose, const char *label);
// Section 6.3.10 - LZ78Y Prediction Estimate
double LZ78Y_test(uint8_t *data, long len, int alph_size, const int verbose, const char *label);