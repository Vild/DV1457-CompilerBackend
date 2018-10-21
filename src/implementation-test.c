#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

extern uint64_t fact(int64_t factor);
extern uint64_t gdc(int64_t a, int64_t b);
extern uint64_t lntwo(int64_t power);

int64_t factorial(int64_t n) {
  int64_t result = 1;

  for (int64_t c = 1; c <= n; c++)
    result = result * c;

  return result;
}

int main() {
	size_t a = 0xDEAD;
	size_t b = 0xBEEF;
	size_t c = 0xDEAD;
	size_t d = 0xC0DE;
	for (int64_t i = 0; i < 12; i++)
		if (fact(i) != factorial(i)) {
			printf("FAILURE: fact(%" PRId64 ") != factorial(%" PRId64 ") => %" PRId64 " != %" PRId64 "\n", i, i, fact(i), factorial(i));
			return 1;
		}

	for (uint64_t i = 1; i < 512; i++)
		if (lntwo(i) != (uint64_t)log2(i)) {
			printf("FAILURE: lntwo(%" PRId64 ") != log2(%" PRId64 ") => %" PRId64 " != %" PRId64 "\n", i, i, lntwo(i), (uint64_t)log2(i));
			return 2;
		}

	if (a != 0xDEAD || b != 0xBEEF || c != 0xDEAD || d != 0xC0DE) {
		printf("FAILURE: 0x%" PRIX64 " != 0xDEAD || 0x%" PRIX64 " != 0xBEEF || 0x%" PRIX64 " != 0xDEAD || 0x%" PRIX64 " != 0xC0DE\n", a, b, c, d);
		return 3;
	}

	//printf("Code works!\n");
	return 0;
}
