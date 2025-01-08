#pragma once

// Section 6.3.3 - Markov Estimate
// data is assumed to be binary (e.g., bit string)
double markov_test(uint8_t* data, long len, const int verbose, const char *label);
