// #include <microkit.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "non_iid_test.h"

// Set to memory mapped buffer entropy is written into
// uintptr_t entropy_buffer;

bool read_file_subset(data_t* data);

int main(void){
    int verbose = 1; //verbose 0 is for JSON output, 1 is the normal mode, 2 is the NIST tool verbose mode, and 3 is for extra verbose output
    bool quietMode = false;
    double H_original, H_bitstring;
    data_t data;

    data.word_size = 0;

    // If quiet mode is enabled, force minimum verbose
    if (quietMode) {
        verbose = 0;
    }

    if (!read_file_subset(&data)) {
        printf("Error reading file.\n");
    }

    H_original = data.word_size;
    H_bitstring = 1.0;

    if ((verbose == 1) || (verbose == 2)) {
        printf("\nRunning non-IID tests...\n\n");
        printf("Running Most Common Value Estimate...\n");
    }

    run_test(&data);

    free_data(&data);
    return 0;
}

// Read in binary file to test
bool read_file_subset(data_t *dp) {

	int mask, j, max_symbols;
	long i;
	long fileLen;

	// Set length of buffer 
	dp->len = 20000;

	dp->symbols = (uint8_t*)malloc(sizeof(uint8_t)*dp->len);
	dp->rawsymbols = (uint8_t*)malloc(sizeof(uint8_t)*dp->len);
	if((dp->symbols == NULL) || (dp->rawsymbols == NULL)){
    printf("Error: failure to initialize memory for symbols\n");
		if(dp->symbols != NULL) {
			free(dp->symbols);
			dp->symbols = NULL;
		}
		if(dp->rawsymbols != NULL) {
			free(dp->rawsymbols);
			dp->rawsymbols = NULL;
		}
		return false;
	} 

	uint8_t* entropy_buffer = (uint8_t*)malloc(dp->len);
    memset(entropy_buffer, 0, dp->len);

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Fill the buffer with random data
    for (size_t i = 0; i < dp->len; i++) {
        entropy_buffer[i] = (uint8_t)(rand() & 0xFF); // Generate a random byte
    }
	memcpy(dp->symbols, entropy_buffer, dp->len);

	//Do we need to establish the word size?
	if(dp->word_size == 0) {
		uint8_t datamask = 0;
		uint8_t curbit = 0x80;

		for(i = 0; i < dp->len; i++) {
			datamask = datamask | dp->symbols[i];
		}

		for(i=8; (i>0) && ((datamask & curbit) == 0); i--) {
			curbit = curbit >> 1;
		}

		dp->word_size = i;
	} else {
		uint8_t datamask = 0;
		uint8_t curbit = 0x80;

		for(i = 0; i < dp->len; i++) {
			datamask = datamask | dp->symbols[i];
		}

		for(i=8; (i>0) && ((datamask & curbit) == 0); i--) {
			curbit = curbit >> 1;
		}

		if( i < dp->word_size ) {
			printf("Warning: Symbols appear to be narrower than described.\n");
		} else if( i > dp->word_size ) {
			printf("Incorrect bit width specification: Data (%ld) does not fit within described bit width: %d.\n",i,dp->word_size); 
                        free(dp->symbols);
			dp->symbols = NULL;
			free(dp->rawsymbols);
			dp->rawsymbols = NULL;
			return false;
		}
	}

	memcpy(dp->rawsymbols, dp->symbols, sizeof(uint8_t)* dp->len);
	dp->maxsymbol = 0;

	max_symbols = 1 << dp->word_size;
	int symbol_map_down_table[max_symbols];

	// create symbols (samples) and check if they need to be mapped down
	dp->alph_size = 0;
	memset(symbol_map_down_table, 0, max_symbols*sizeof(int));
	mask = max_symbols-1;
	for(i = 0; i < dp->len; i++){ 
		dp->symbols[i] &= mask;
		if(dp->symbols[i] > dp->maxsymbol) dp->maxsymbol = dp->symbols[i];
		if(symbol_map_down_table[dp->symbols[i]] == 0) symbol_map_down_table[dp->symbols[i]] = 1;
	}

	for(i = 0; i < max_symbols; i++){
		if(symbol_map_down_table[i] != 0) symbol_map_down_table[i] = (uint8_t)dp->alph_size++;
	}

	// create bsymbols (bitstring) using the non-mapped data
	dp->blen = dp->len * dp->word_size;
	if(dp->word_size == 1) dp->bsymbols = dp->symbols;
	else{
		dp->bsymbols = (uint8_t*)malloc(dp->blen);
		if(dp->bsymbols == NULL){
			printf("Error: failure to initialize memory for bsymbols\n");
			free(dp->symbols);
			dp->symbols = NULL;
			free(dp->rawsymbols);
			dp->rawsymbols = NULL;

			return false;
		}

		for(i = 0; i < dp->len; i++){
			for(j = 0; j < dp->word_size; j++){
				dp->bsymbols[i*dp->word_size+j] = (dp->symbols[i] >> (dp->word_size-1-j)) & 0x1;
			}
		}
	}

	// map down symbols if less than 2^bits_per_word unique symbols
	if(dp->alph_size < dp->maxsymbol + 1){
		for(i = 0; i < dp->len; i++) dp->symbols[i] = (uint8_t)symbol_map_down_table[dp->symbols[i]];
	} 

	return true;
}

void free_data(data_t *dp){
	if(dp->symbols != NULL) free(dp->symbols);
	if(dp->rawsymbols != NULL) free(dp->rawsymbols);
	if((dp->word_size > 1) && (dp->bsymbols != NULL)) free(dp->bsymbols);
} 