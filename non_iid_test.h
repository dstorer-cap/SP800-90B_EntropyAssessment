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
    void run_test(data_t* data);
#ifdef __cplusplus
}
#endif