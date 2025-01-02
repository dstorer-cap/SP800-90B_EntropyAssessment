
#include "shared/utils.h"
#include "shared/most_common.h"
#include "shared/lrs_test.h"
#include "shared/TestRunUtils.h"
#include "non_iid/collision_test.h"
#include "non_iid/lz78y_test.h"
#include "non_iid/multi_mmc_test.h"
#include "non_iid/lag_test.h"
#include "non_iid/multi_mcw_test.h"
#include "non_iid/compression_test.h"
#include "non_iid/markov_test.h"    

extern "C"{
void run_test(data_t* data_c){   
    bool initial_entropy, all_bits;
    int verbose = 1; //verbose 0 is for JSON output, 1 is the normal mode, 2 is the NIST tool verbose mode, and 3 is for extra verbose output
    bool quietMode = false;
    char *file_path;
    double H_original, H_bitstring, ret_min_entropy;
    int opt;
    data_t data = *data_c;
    double bin_t_tuple_res = -1.0, bin_lrs_res = -1.0;
    double t_tuple_res = -1.0, lrs_res = -1.0;
    unsigned long long inint;
    char *nextOption;

    initial_entropy = true;
    all_bits = true;

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
        SAalgs(data.bsymbols, data.blen, 2, bin_t_tuple_res, bin_lrs_res, verbose, "Bitstring");
        if (bin_t_tuple_res >= 0.0) {
            if (verbose == 2) printf("\tT-Tuple Test Estimate (bit string) = %f / 1 bit(s)\n", bin_t_tuple_res);
            H_bitstring = min(bin_t_tuple_res, H_bitstring);
        }
    }

    if (initial_entropy) {
        SAalgs(data.symbols, data.len, data.alph_size, t_tuple_res, lrs_res, verbose, "Literal");
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
}