#pragma once

#include <climits>
#include <divsufsort.h>
#include <divsufsort64.h>

#define SAINDEX_MAX INT32_MAX
#define SAINDEX64_MAX INT64_MAX

//Using the Kasai (et al.) O(n) time "13n space" algorithm.
//"Linear-Time Longest-Common-Prefix Computation in Suffix Arrays and Its Applications", by Kasai, Lee, Arimura, Arikawa, and Park
//https://doi.org/10.1007/3-540-48194-X_17
//http://web.cs.iastate.edu/~cs548/references/linear_lcp.pdf
//The default implementation uses 4 byte indexes
static void sa2lcp32(const uint8_t text[], long int n, const vector<saidx_t> &sa, vector<saidx_t> &lcp);

//Using the Kasai (et al.) O(n) time "25n space" algorithm (with 64-bit indicies)
static void sa2lcp64(const uint8_t text[], long int n, const vector<saidx64_t> &sa, vector<saidx64_t> &lcp);

void calcSALCP32(const uint8_t text[], long int n, vector<saidx_t> &sa, vector<saidx_t> &lcp);

void calcSALCP64(const uint8_t text[], long int n, vector<saidx64_t> &sa, vector<saidx64_t> &lcp);
/* Based on the algorithm outlined by Aaron Kaufer
 * This is described here:
 * http://www.untruth.org/~josh/sp80090b/Kaufer%20Further%20Improvements%20for%20SP%20800-90B%20Tuple%20Counts.pdf
 */
void SAalgs32(const uint8_t text[], long int n, int k, double &t_tuple_res, double &lrs_res, const int verbose, const char *label);

void SAalgs64(const uint8_t text[], long int n, int k, double &t_tuple_res, double &lrs_res, const int verbose, const char *label);

void SAalgs(const uint8_t text[], long int n, int k, double &t_tuple_res, double &lrs_res, const int verbose, const char *label);

long int len_LRS32(const uint8_t text[], const int sample_size);
long int len_LRS64(const uint8_t text[], const int sample_size);
/*
* ---------------------------------------------
*			 HELPER FUNCTIONS
* ---------------------------------------------
*/

void calc_collision_proportion(const vector<double> &p, long double &p_col);

/*
* ---------------------------------------------
* 		  			 TEST
* ---------------------------------------------
*/

bool len_LRS_test(const uint8_t data[], const int L, const int k, const int verbose, const char *label);