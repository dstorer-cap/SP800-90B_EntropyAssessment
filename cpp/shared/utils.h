//Version of the tool
#define VERSION "1.1.7"

#ifndef powl
#define powl pow
#endif

#ifndef logl
#define logl log
#endif

#ifndef log2l
#define log2l log2
#endif

#ifndef log1pl
#define log1pl log1p
#endif

#ifndef expl
#define expl exp
#endif

#pragma once
#include <stdlib.h>
#include <math.h>
// #include <iostream>		// std::cout
#include <string>		// std::string
#include <map>			// std::map
#include <set>			// std::set
#include <string.h>		// strlen
#include <iomanip>		// setw / setfill
#include <stdio.h>
#include <cstdlib>
#include <vector>		// std::vector
#include <time.h>		// time
#include <algorithm>	// std::sort
#include <cmath>		// pow, log2
#include <array>		// std::array
// #include <omp.h>		// openmp 4.0 with gcc 4.9
#include <bitset>
#include <mutex>		// std::mutex
#include <assert.h>
#include <cfloat>


#define SWAP(x, y) do { int s = x; x = y; y = s; } while(0)
#define INOPENINTERVAL(x, a, b) (((a)>(b))?(((x)>(b))&&((x)<(a))):(((x)>(a))&&((x)<(b))))
#define INCLOSEDINTERVAL(x, a, b) (((a)>(b))?(((x)>=(b))&&((x)<=(a))):(((x)>=(a))&&((x)<=(b))))

#define MIN_SIZE 1000000
#define PERMS 10000

//This is the smallest practical value (one can't do better with the double type)
#define RELEPSILON DBL_EPSILON
//This is clearly overkill, but it's difficult to do better without a view into the monotonic function
#define ABSEPSILON DBL_MIN
#define DBL_INFINITY __builtin_inf ()
#define ITERMAX 1076
#define ZALPHA 2.5758293035489008
#define ZALPHA_L 2.575829303548900384158L

//Make uint128_t a supported type (standard as of C23)
#ifdef __SIZEOF_INT128__
typedef unsigned __int128 uint128_t;
typedef unsigned __int128 uint_least128_t;
# define UINT128_MAX         ((uint128_t)-1)
# define UINT128_WIDTH       128
# define UINT_LEAST128_WIDTH 128
# define UINT_LEAST128_MAX   UINT128_MAX
# define UINT128_C(N)        ((uint_least128_t)+N ## WBU)
#endif

typedef struct data_t data_t;

struct data_t{
	int word_size; 		// bits per symbol
	int alph_size; 		// symbol alphabet size
	uint8_t maxsymbol; 	// the largest symbol present in the raw data stream
	uint8_t *rawsymbols; 	// raw data words
	uint8_t *symbols; 		// data words
	uint8_t *bsymbols; 	// data words as binary string
	long len; 		// number of words in data
	long blen; 		// number of bits in data
};



using namespace std;

//This generally performs a check for relative closeness, but (if that check would be nonsense)
//it can check for an absolute separation, using either the distance between the numbers, or
//the number of ULPs that separate the two numbers.
//See the following for details and discussion of this approach:
//https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
//https://floating-point-gui.de/errors/comparison/
//https://www.boost.org/doc/libs/1_62_0/libs/test/doc/html/boost_test/testing_tools/extended_comparison/floating_point/floating_points_comparison_theory.html
//Knuth AoCP vol II (section 4.2.2)
//Tested using modified test cases from https://floating-point-gui.de/errors/NearlyEqualsTest.java
bool relEpsilonEqual(double A, double B, double maxAbsFactor, double maxRelFactor, uint32_t maxULP);


/* This is xoshiro256** 1.0*/
/*This implementation is derived from David Blackman and Sebastiano Vigna, which they placed into
the public domain. See http://xoshiro.di.unimi.it/xoshiro256starstar.c
*/
static inline uint64_t rotl(const uint64_t x, int k);

static inline uint64_t xoshiro256starstar(uint64_t *xoshiro256starstarState);

/* This is the jump function for the generator. It is equivalent
   to 2^128 calls to xoshiro256starstar(); it can be used to generate 2^128
   non-overlapping subsequences for parallel computations. */
void xoshiro_jump(unsigned int jump_count, uint64_t *xoshiro256starstarState);

//This seeds using an external source
//We use /dev/urandom here. 
//We could alternately use the RdRand (or some other OS or HW source of pseudo-random numbers)
void seed(uint64_t *xoshiro256starstarState);

/*Return an integer in the range [0, high], without modular bias*/
/*This is a slight modification of Lemire's approach (as we want [0,s] rather than [0,s)*/
/*See "Fast Random Integer Generation in an Interval" by Lemire (2018) (https://arxiv.org/abs/1805.10941) */
 /* The relevant text explaining the central factor underlying this opaque approach is:
  * "Given an integer x ∈ [0, 2^L), we have that (x × s) ÷ 2^L ∈ [0, s). By multiplying by s, we take
  * integer values in the range [0, 2^L) and map them to multiples of s in [0, s × 2^L). By dividing by 2^L,
  * we map all multiples of s in [0, 2^L) to 0, all multiples of s in [2^L, 2 × 2^L) to one, and so forth. The
  * (i + 1)th interval is [i × 2^L, (i + 1) × 2^L). By Lemma 2.1, there are exactly floor(2^L/s) multiples of s in
  * intervals [i × 2^L + (2^L mod s), (i + 1) × 2^L) since s divides the size of the interval (2^L − (2^L mod s)).
  * Thus if we reject the multiples of s that appear in [i × 2^L, i × 2^L + (2^L mod s)), we get that all
  * intervals have exactly floor(2^L/s) multiples of s."
  *
  * This approach allows us to avoid _any_ modular reductions with high probability, and at worst case one
  * reduction. It's an opaque approach, but lovely.
  */
uint64_t randomRange64(uint64_t s, uint64_t *xoshiro256starstarState);

/*
 * This function produces a double that is uniformly distributed in the interval [0, 1).
 * Note that 2^53 is the largest integer that can be represented in a 64 bit IEEE 754 double, such that all 
 * smaller positive integers can also be represented. Shifting the initial random 64-bit value right by 11 
 * bits makes the result only in the lower 53 bits, so the resulting integer is in the range [0, 2^53 - 1].
 * 1.1102230246251565e-16 (0x1.0p-53) is 2^(-53). Multiplying by this value just effects the exponent of the 
 * resulting double, not the significand. We get a double uniformly distributed in the range [0, 1).  
 * The delta between adjacent values is 2^(-53).
 */
double randomUnit(uint64_t *xoshiro256starstarState);

// Quick sum array  // TODO
long int sum(const uint8_t arr[], const int sample_size);

// Quick sum std::array // TODO
template<size_t LENGTH>
int sum(const array<int, LENGTH> &arr);

// Quick sum vector
template<typename T>
T sum(const vector<T> &v);

// Calculate baseline statistics
// Finds mean, median, and whether or not the data is binary
void calc_stats(const data_t *dp, double &rawmean, double &median);


// Map initialization for integers
void map_init(map<uint8_t, int> &m);

// Map initialization for doubles
void map_init(map<uint8_t, double> &m);

// Map initialization for pair<uint8_t, uint8_t> to int
void map_init(map<pair<uint8_t, uint8_t>, int> &m);

// Calculates proportions of each value as an index
void calc_proportions(const uint8_t data[], vector<double> &p, const int sample_size);

// Calculates proportions of each value as an index
void calc_counts(const uint8_t data[], vector<int> &c, const int sample_size);

// Determines the standard deviation of a dataset
double std_dev(const vector<int> x, const double x_mean);

// Quick formula for n choose 2 (which can be simplified to [n^2 - n] / 2)
long int n_choose_2(const long int n);

vector<uint8_t> substr(const uint8_t text[], const int pos, const int len, const int sample_size);

// Fast substring with no bounds checking
array<uint8_t, 16> fast_substr(const uint8_t text[], const int pos, const int len);

template<typename T>
T max_vector(const vector<T> &vals);

template<typename T>
T max_arr(const T* vals, const unsigned int k);

double divide(const int a, const int b);

double prediction_estimate_function(long double p, long r, long N);

double calc_p_local(long max_run_len, long N, double ldomain);

double predictionEstimate(long C, long N, long max_run_len, long k, const char *testname, const int verbose, const char *label);
//The idea here is that we've given an array of pointers (binaryDict). 
//We are trying to produce the address of the length-2 array associated with the length-d prefix "b".
// array The dth index is d-1, so we first find the start of the address space (binaryDict[(d)-1])
//We take the least significant d bits from "b": this is the expression "(b) & ((1U << (d)) - 1)"
//We then multiply this by 2 (as each pattern is associated with a length-2 array) by left shifting by 1.
#define BINARYDICTLOC(d, b) (binaryDict[(d)-1] + (((b) & ((1U << (d)) - 1))<<1))

uint32_t compressedBitSymbols(const uint8_t *S, long length);

// static void printVersion(string name) {
//     cout << name << " " << VERSION << "\n\n";
//     cout << "Disclaimer: ";
//     cout << "NIST-developed software is provided by NIST as a public service. You may use, copy, and distribute copies of the software in any medium, provided that you keep intact this entire notice. You may improve, modify, and create derivative works of the software or any portion of the software, and you may copy and distribute such modifications or works. Modified works should carry a notice stating that you changed the software and should note the date and nature of any such change. Please explicitly acknowledge the National Institute of Standards and Technology as the source of the software.";
//     cout << "\n\n";
//     cout << "NIST-developed software is expressly provided \"AS IS.\" NIST MAKES NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT, OR ARISING BY OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT, AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.";
//     cout << "\n\n";
//     cout << "You are solely responsible for determining the appropriateness of using and distributing the software and you assume all risks associated with its use, including but not limited to the risks and costs of program errors, compliance with applicable laws, damage to or loss of data, programs or equipment, and the unavailability or interruption of operation. This software is not intended to be used in any situation where a failure could cause risk of injury or damage to property. The software developed by NIST employees is not subject to copyright protection within the United States.";
//     cout << "\n\n";
// }

static string recreateCommandLine(int argc, char* argv[]);

class PostfixDictionary {
	map<uint8_t, long> postfixes;
	long curBest;
	uint8_t curPrediction;
public:
	PostfixDictionary() { curBest = 0; curPrediction = 0;}
	uint8_t predict(long &count) {assert(curBest > 0); count = curBest; return curPrediction;}
	bool incrementPostfix(uint8_t in, bool makeNew) {
		map<uint8_t, long>::iterator curp = postfixes.find(in);
		long curCount;
		bool newEntry=false;

		if(curp != postfixes.end()) {
			//The entry is already there. We always increment in this case.
			curCount = ++(curp->second);
		} else if(makeNew) {
			//The entry is not here, but we are allowed to create a new entry
			newEntry = true;
			curCount = postfixes[in] = 1;
		} else {
			//The entry is not here, we are not allowed to create a new entry
			return false;
		}

		//Only instances where curCount is set and an increment was performed get here
		if((curCount > curBest) || ((curCount == curBest) && (in > curPrediction))) { 
			curPrediction = in; 
			curBest = curCount; 
		} 

		return newEntry;
	}
};