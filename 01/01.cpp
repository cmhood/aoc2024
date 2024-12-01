#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#define COUNT 1000

static int cmp(void const *, void const *);

int
main()
{
	int32_t l[COUNT];
	int32_t r[COUNT];

	for (int i = 0; i < COUNT; ++i) {
		if (scanf("%" PRIi32 "   %" PRIi32 "\n", &l[i], &r[i]) < 2) {
			exit(EXIT_FAILURE);
		}
	}

	qsort(l, COUNT, sizeof(int32_t), cmp);
	qsort(r, COUNT, sizeof(int32_t), cmp);

	int32_t sum = 0;
	for (int i = 0; i < COUNT; ++i) {
		sum += abs(l[i] - r[i]);
	}
	printf("%" PRIi32 "\n", sum);

	sum = 0;
	for (int i = 0; i < COUNT; ++i) {
		int32_t n = l[i];
		int count = 0;
		for (int j = 0; j < COUNT; ++j) {
			if (n == r[j]) {
				++count;
			}
		}
		sum += n * count;
	}
	printf("%" PRIi32 "\n", sum);
}

static int
cmp(void const *a, void const *b)
{
	return *(int32_t *)a - *(int32_t *)b;
}
