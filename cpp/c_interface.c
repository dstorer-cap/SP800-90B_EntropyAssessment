#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <microkit.h>
#include <stdio_microkit.h>
#include "non_iid_test.h"
#include <sys/time.h> // For struct timeval
#include <errno.h>
#include <libvmm/virq.h>
#include <libvmm/util/util.h>
#include <libvmm/virtio/virtio.h>
#include <libvmm/arch/aarch64/linux.h>
#include <libvmm/arch/aarch64/fault.h>
#include <sddf/serial/queue.h>

static inline double min(double a, double b) {
    return (a < b) ? a : b;
}

// Define __errno() for compatibility with existing headers
int *__errno() {
    return &errno;
}

int read(int file, char *ptr, int len) { errno = ENOSYS; return -1; }
int write(int file, char *ptr, int len) { errno = ENOSYS; return -1; }
int lseek(int file, int offset, int whence) { errno = ENOSYS; return -1; }
int fstat(int file, struct stat *st) { errno = ENOSYS; return -1; }
int close(int file) { errno = ENOSYS; return -1; }
int isatty(int file) { return 0; }

struct _reent *_impure_ptr;

void _exit(int status) {
    while (1); // Infinite loop to stop execution
}

int gettimeofday(struct timeval *tv, void *tz) {
    if (tv) {
        tv->tv_sec = 0;
        tv->tv_usec = 0;
    }
    return 0; // Return success
}

int open(const char *pathname, int flags) {
    errno = ENOSYS; // Function not implemented
    return -1;
}

// Set to memory mapped buffer entropy is written into
// uintptr_t entropy_buffer;

bool read_file_subset(data_t* data);

void init(void){
}

void notified(microkit_channel ch){
   printf("Health check long notified\n");
   switch (ch) {
		case 5:
        	init_post();
   }
}

void init_post(void){
    data_t data;
    data.word_size = 0;

    if (!read_file_subset(&data)) {
        printf("Error reading file.\n");
    }
	
    run_tests(&data);
		
    free_data(&data);

    return 0;
}

void run_tests(data_t* data_c){   
    bool initial_entropy, all_bits;
    int verbose = 2; //verbose 0 is for JSON output, 1 is the normal mode, 2 is the NIST tool verbose mode, and 3 is for extra verbose output
    bool quietMode = false;
    char *file_path;
    double H_original, H_bitstring, ret_min_entropy;
    int opt;
    data_t data = *data_c;
    double bin_t_tuple_res = -1.0, bin_lrs_res = -1.0;
    double t_tuple_res = -1.0, lrs_res = -1.0;
    unsigned long long inint;
    char *nextOption;

    initial_entropy = false;
    all_bits = true;

    // The maximum min-entropy is -log2(1/2^word_size) = word_size
    // The maximum bit string min-entropy is 1.0
    H_original = data.word_size;
    H_bitstring = 1.0;

    if ((verbose == 1) || (verbose == 2)) {
        printf("\nRunning non-IID tests...\n\n");
        printf("Running Most Common Value Estimate...\n");
    }

    // Section 6.3.1 - Estimate entropy with Most Common Value
    if (((data.alph_size > 2) || !initial_entropy)) {
        
        ret_min_entropy = most_common(data.bsymbols, data.blen, 2, verbose, "Bitstring");
        
        if (verbose == 2) printf("\tMost Common Value Estimate (bit string) = %f / 1 bit(s)\n", ret_min_entropy);
        
        H_bitstring = min(ret_min_entropy, H_bitstring);
    }

    if (initial_entropy) {
        
        ret_min_entropy = most_common(data.symbols, data.len, data.alph_size, verbose, "Literal");
         
        if (verbose == 2) printf("\tMost Common Value Estimate = %f / %d bit(s)\n", ret_min_entropy, data.word_size);
        
        H_original = min(ret_min_entropy, H_original);
    }

    // Section 6.3.2 - Estimate entropy with Collision Test (for bit strings only)

    if ((verbose == 1) || (verbose == 2)) printf("\nRunning Entropic Statistic Estimates (bit strings only)...\n");

    if (((data.alph_size > 2) || !initial_entropy)) {
        ret_min_entropy = collision_test(data.bsymbols, data.blen, verbose, "Bitstring");
        if (verbose == 2) printf("\tCollision Test Estimate (bit string) = %f / 1 bit(s)\n", ret_min_entropy);
        H_bitstring = min(ret_min_entropy, H_bitstring);
    }

    if (initial_entropy && (data.alph_size == 2)) {
        ret_min_entropy = collision_test(data.symbols, data.len, verbose, "Literal");
        if (verbose == 2) printf("\tCollision Test Estimate = %f / 1 bit(s)\n", ret_min_entropy);
        H_original = min(ret_min_entropy, H_original);
    }

    // Section 6.3.3 - Estimate entropy with Markov Test (for bit strings only)

    if (((data.alph_size > 2) || !initial_entropy)) {
        ret_min_entropy = markov_test(data.bsymbols, data.blen, verbose, "Bitstring");
        if (verbose == 2) printf("\tMarkov Test Estimate (bit string) = %f / 1 bit(s)\n", ret_min_entropy);
        H_bitstring = min(ret_min_entropy, H_bitstring);
    }

    if (initial_entropy && (data.alph_size == 2)) {
        ret_min_entropy = markov_test(data.symbols, data.len, verbose, "Literal");
        if (verbose == 2) printf("\tMarkov Test Estimate = %f / 1 bit(s)\n", ret_min_entropy);
        H_original = min(ret_min_entropy, H_original);
    }

    // Section 6.3.4 - Estimate entropy with Compression Test (for bit strings only)

    if (((data.alph_size > 2) || !initial_entropy)) {
        ret_min_entropy = compression_test(data.bsymbols, data.blen, verbose, "Bitstring");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tCompression Test Estimate (bit string) = %f / 1 bit(s)\n", ret_min_entropy);
            H_bitstring = min(ret_min_entropy, H_bitstring);
        }
    }

    if (initial_entropy && (data.alph_size == 2)) {
        ret_min_entropy = compression_test(data.symbols, data.len, verbose, "Literal");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tCompression Test Estimate = %f / 1 bit(s)\n", ret_min_entropy);
            H_original = min(ret_min_entropy, H_original);
        }
    }

    // Section 6.3.5 - Estimate entropy with t-Tuple Test

    if ((verbose == 1) || (verbose == 2)) printf("\nRunning Tuple Estimates...\n");

    if (((data.alph_size > 2) || !initial_entropy)) {
        SAalgs(data.bsymbols, data.blen, 2, &bin_t_tuple_res, &bin_lrs_res, verbose, "Bitstring");
        if (bin_t_tuple_res >= 0.0) {
            if (verbose == 2) printf("\tT-Tuple Test Estimate (bit string) = %f / 1 bit(s)\n", bin_t_tuple_res);
            H_bitstring = min(bin_t_tuple_res, H_bitstring);
        }
    }

    if (initial_entropy) {
        SAalgs(data.symbols, data.len, data.alph_size, &t_tuple_res, &lrs_res, verbose, "Literal");
        if (t_tuple_res >= 0.0) {
            if (verbose == 2) printf("\tT-Tuple Test Estimate = %f / %d bit(s)\n", t_tuple_res, data.word_size);
            H_original = min(t_tuple_res, H_original);
        }
    }

    // Section 6.3.6 - Estimate entropy with LRS Test

    if ((((data.alph_size > 2) || !initial_entropy)) && (bin_lrs_res >= 0.0)) {
        if (verbose == 2) printf("\tLRS Test Estimate (bit string) = %f / 1 bit(s)\n", bin_lrs_res);
        H_bitstring = min(bin_lrs_res, H_bitstring);
    }

    if (initial_entropy && (lrs_res >= 0.0)) {
        if (verbose == 2) printf("\tLRS Test Estimate = %f / %d bit(s)\n", lrs_res, data.word_size);
        H_original = min(lrs_res, H_original);
    }

    // Section 6.3.7 - Estimate entropy with Multi Most Common in Window Test
    if ((verbose == 1) || (verbose == 2)) printf("\nRunning Predictor Estimates...\n");

    if (((data.alph_size > 2) || !initial_entropy)) {
        ret_min_entropy = multi_mcw_test(data.bsymbols, data.blen, 2, verbose, "Bitstring");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tMulti Most Common in Window (MultiMCW) Prediction Test Estimate (bit string) = %f / 1 bit(s)\n", ret_min_entropy);
            H_bitstring = min(ret_min_entropy, H_bitstring);
        }
    }

    if (initial_entropy) {
        ret_min_entropy = multi_mcw_test(data.symbols, data.len, data.alph_size, verbose, "Literal");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tMulti Most Common in Window (MultiMCW) Prediction Test Estimate = %f / %d bit(s)\n", ret_min_entropy, data.word_size);
            H_original = min(ret_min_entropy, H_original);
        }
    }

    // Section 6.3.8 - Estimate entropy with Lag Prediction Test
    if (((data.alph_size > 2) || !initial_entropy)) {
        ret_min_entropy = lag_test(data.bsymbols, data.blen, 2, verbose, "Bitstring");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tLag Prediction Test Estimate (bit string) = %f / 1 bit(s)\n", ret_min_entropy);
            H_bitstring = min(ret_min_entropy, H_bitstring);
        }
    }

    if (initial_entropy) {
        ret_min_entropy = lag_test(data.symbols, data.len, data.alph_size, verbose, "Literal");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tLag Prediction Test Estimate = %f / %d bit(s)\n", ret_min_entropy, data.word_size);
            H_original = min(ret_min_entropy, H_original);
        }
    }

    // Section 6.3.9 - Estimate entropy with Multi Markov Model with Counting Test (MultiMMC)
    if (((data.alph_size > 2) || !initial_entropy)) {
        ret_min_entropy = multi_mmc_test(data.bsymbols, data.blen, 2, verbose, "Bitstring");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tMulti Markov Model with Counting (MultiMMC) Prediction Test Estimate (bit string) = %f / 1 bit(s)\n", ret_min_entropy);
            H_bitstring = min(ret_min_entropy, H_bitstring);
        }
    }

    if (initial_entropy) {
        ret_min_entropy = multi_mmc_test(data.symbols, data.len, data.alph_size, verbose, "Literal");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tMulti Markov Model with Counting (MultiMMC) Prediction Test Estimate = %f / %d bit(s)\n", ret_min_entropy, data.word_size);
            H_original = min(ret_min_entropy, H_original);
        }
    }

    // Section 6.3.10 - Estimate entropy with LZ78Y Test
    if (((data.alph_size > 2) || !initial_entropy)) {
        ret_min_entropy = LZ78Y_test(data.bsymbols, data.blen, 2, verbose, "Bitstring");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tLZ78Y Prediction Test Estimate (bit string) = %f / 1 bit(s)\n", ret_min_entropy);
            H_bitstring = min(ret_min_entropy, H_bitstring);
        }
    }

    if (initial_entropy) {
        ret_min_entropy = LZ78Y_test(data.symbols, data.len, data.alph_size, verbose, "Literal");
        if (ret_min_entropy >= 0) {
            if (verbose == 2) printf("\tLZ78Y Prediction Test Estimate = %f / %d bit(s)\n", ret_min_entropy, data.word_size);
            H_original = min(ret_min_entropy, H_original);
        }
    }

    double h_assessed;
    h_assessed = data.word_size;

    if ((data.alph_size > 2) || !initial_entropy) {
        h_assessed = min(h_assessed, H_bitstring * data.word_size);
    }

    if (initial_entropy) {
        h_assessed = min(h_assessed, H_original);
    }

    if ((verbose == 1) || (verbose == 2)) {
        if (initial_entropy) {
            printf("\nH_original: %f\n", H_original);
            if (data.alph_size > 2) {
                printf("H_bitstring: %f\n", H_bitstring);
                printf("min(H_original, %d X H_bitstring): %f\n", data.word_size, min(H_original, data.word_size * H_bitstring));
            }
        } else {
            printf("\nh': %f\n", H_bitstring);
        }
    } else if (verbose > 2) {
        if ((data.alph_size > 2) || !initial_entropy) {
            printf("H_bitstring = %.17g\n", H_bitstring);
            printf("H_bitstring Per Symbol = %.17g\n", H_bitstring * data.word_size);
        }

        if (initial_entropy) {
            printf("H_original = %.17g\n", H_original);
        }

        printf("Assessed min entropy: %.17g\n", h_assessed);
    }
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