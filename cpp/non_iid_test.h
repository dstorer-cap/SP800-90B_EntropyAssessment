#pragma once

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

#ifdef __cplusplus
extern "C" {
#endif
	double most_common(uint8_t* data, const long len, const int alph_size, const int verbose, const char *label);
	double collision_test(uint8_t* data, long len, const int verbose, const char *label);
	double markov_test(uint8_t* data, long len, const int verbose, const char *label);
	double compression_test(uint8_t* data, long len, const int verbose, const char *label);
	void SAalgs(const uint8_t text[], long int n, int k, double* t_tuple_res, double* lrs_res, const int verbose, const char *label);
	double multi_mcw_test(uint8_t *data, long len, int alph_size, const int verbose, const char *label);
	double lag_test(uint8_t *S, long L, int k, const int verbose, const char *label);
	double multi_mmc_test(uint8_t *data, long len, int alph_size, const int verbose, const char *label);
	double LZ78Y_test(uint8_t *data, long len, int alph_size, const int verbose, const char *label);
#ifdef __cplusplus
}
#endif