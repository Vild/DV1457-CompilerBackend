#include <stdio.h>
#include <stdint.h>
#include <math.h>

extern uint64_t fact(uint64_t factor);
extern uint64_t gdc(uint64_t a, uint64_t b);
extern uint64_t lntwo(uint64_t power);

uint64_t factorial(uint64_t n) {
  uint64_t result = 1;

  for (uint64_t c = 1; c <= n; c++)
    result = result * c;

  return result;
}

int main() {
	size_t a = 0xDEAD;
	size_t b = 0xBEEF;
	size_t c = 0xDEAD;
	size_t d = 0xC0DE;
	for (uint64_t i = 0; i < 12; i++)
		if (fact(i) != factorial(i)) {
			printf("FAILURE: fact(%ul) != factorial(%ul) => %ul != %ul\n", i, i, fact(i), factorial(i));
			return 1;
		}

	for (uint64_t i = 1; i < 512; i++)
		if (lntwo(i) != (uint64_t)log2(i)) {
			printf("FAILURE: lntwo(%ul) != log2(%ul) => %ul != %ul\n", i, i, lntwo(i), (uint64_t)log2(i));
			return 2;
		}

	if (a != 0xDEAD || b != 0xBEEF || c != 0xDEAD || d != 0xC0DE) {
		printf("FAILURE: 0x%X != 0xDEAD || 0x%X != 0xBEEF || 0x%X != 0xDEAD || 0x%X != 0xC0DE\n", a, b, c, d);
		return 3;
	}

	//printf("Code works!\n");
	return 0;
}
