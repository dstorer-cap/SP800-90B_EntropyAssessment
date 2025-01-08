#pragma once

#define D_LAG 128U
#define LAGMASK (D_LAG-1)

/*Convention:
 * if start==end, the buffer is empty. Start and end values are not ANDed, so range from 0...255
 * This extended range allows for use of all the entries of the buffer
 * See https://www.snellman.net/blog/archive/2016-12-13-ring-buffers/
 * The actual data locations range 0...127
 * start&LAGMASK is the index of the oldest data
 * end&LAGMASK is the index where the *next* data goes.
 */

struct lagBuf {
	uint8_t start;
	uint8_t end;
	long buf[D_LAG];
};

/* Lag prediction estimate (6.3.8)
 * This is a somewhat counter-intuitive approach to this test; the original idea for this approach is due
 * to David Oksner. The straight forward way is simply to check j symbols back for each case (where j runs
 * 1 to 128). It ends up that this is bizarrely slow.
 * This approach is to keep a list of offsets where we encountered each symbol (in a ring buffer,
 * which can store at most D (128) prior elements.
 * For this, one needs only check and update the current symbol's ring buffer, and we only need to spend
 * time looking at values that correspond to counters that must be updated.
 */
double lag_test(uint8_t *S, long L, int k, const int verbose, const char *label);
