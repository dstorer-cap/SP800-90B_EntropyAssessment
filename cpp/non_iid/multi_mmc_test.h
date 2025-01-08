#pragma once

#define D_MMC 16
#define MAX_ENTRIES 100000

static double binaryMultiMMCPredictionEstimate(const uint8_t *S, long L, const int verbose, const char *label);

// Section 6.3.9 - MultiMMC Prediction Estimate
/* This implementation of the MultiMMC test is a based on NIST's really cleaver implementation,
 * which interleaves the predictions and updates. This makes optimization much easier.
 * It's opaque why it works correctly (in particular, the first few symbols are added in a different
 * order than in the reference implementation), but once the initialization is performed, the rest of the
 * operations are done in the correct order.
 * The general observations that explains why this approach works are that
 * 1) each prediction that could succeed (i.e., ignoring some of the early predictions that must fail due
 *    to lack of strings of the queried length) must occur only after all the correct (x,y) tuples for that
 *    length have been processed. One is free to reorder otherwise.
 * 2) If there is a distinct string of length n, then this induces corresponding unique strings of all
 *    lengths greater than n. We track all string lengths independently (thus conceptually, we
 *    could run out of a short-string length prior to a long string length, thus erroneously not add
 *    some long string to the dictionary after no longer looking for a string to the dictionary when
 *    we should have), this can't happen in practice because we add strings from shortest to longest.
 */
double multi_mmc_test(uint8_t *data, long len, int alph_size, const int verbose, const char *label);
