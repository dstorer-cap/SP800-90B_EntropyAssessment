#pragma once

inline void kahan_add(double &sum, double &comp, double in);

//There is some cleverness associated with this calculation of G; in particular,
//one doesn't need to calculate all the terms independently (they are inter-related!)
//See UL's implementation comments here: https://bit.ly/UL90BCOM 
//Look in the section "Compression Estimate G Function Calculation"
double G(double z, int d, long num_blocks);

double com_exp(double p, unsigned int alph_size, int d, long num_blocks);

// Section 6.3.4 - Compression Estimate
// data is assumed to be binary (e.g., bit string)
double compression_test(uint8_t* data, long len, const int verbose, const char *label);