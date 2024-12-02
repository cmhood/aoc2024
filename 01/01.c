#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAX_ROW_COUNT (4000000)

static void get_input(int, char **);
static void parse_input(void);
static void sort(ptrdiff_t, int *);
static void selection_sort(ptrdiff_t, int *);

static ptrdiff_t input_length = 0;
static char const *input = NULL;

static ptrdiff_t row_count = 0;
static int rows[2][MAX_ROW_COUNT] = {0};

int
main(int argc, char **argv)
{
	get_input(argc, argv);
	parse_input();

	sort(row_count, rows[0]);
	sort(row_count, rows[1]);

#ifdef SILVER
	long sum = 0;
	for (ptrdiff_t i = 0; i < row_count; ++i) {
		sum += abs(rows[0][i] - rows[1][i]);
	}
	printf("%ld\n", sum);
#endif

#ifdef GOLD
	long sum = 0;
	for (ptrdiff_t i = 0, j = 0; i < row_count; ++i) {
		int n = rows[0][i];
		while (j < row_count && rows[1][j] < n) {
			++j;
		}
		ptrdiff_t start = j;
		while (j < row_count && rows[1][j] == n) {
			++j;
		}
		sum += n * (j - start);
	}
	printf("%ld\n", sum);
#endif
}

static void
get_input(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "expected 1 argument\n");
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	input_length = lseek(fd, 0, SEEK_END);
	if (input_length < 0) {
		perror("lseek");
		exit(EXIT_FAILURE);
	}

	input = mmap(NULL, input_length, PROT_READ, MAP_PRIVATE, fd, 0);
	if (input == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
}

static void
parse_input(void)
{
	for (ptrdiff_t i = 0; i < input_length;) {
		for (int col = 0; col < 2; ++col) {
			int n = 0;
			for (;;) {
				if (i >= input_length) {
					goto fail;
				}
				char c = input[i];
				if (!('0' <= c && c <= '9')) {
					break;
				}
				n = 10 * n + (c - '0');
				++i;
			}
			rows[col][row_count] = n;
			i += (col == 0 ? 3 : 1);
		}
		if (++row_count > MAX_ROW_COUNT) {
			goto fail;
		}
	}
	return;
fail:
	fprintf(stderr, "parse error\n");
	exit(EXIT_FAILURE);
}

static void
sort(ptrdiff_t length, int *arr)
{
	if (length <= 8) {
		selection_sort(length, arr);
		return;
	}

	ptrdiff_t lo = 0;
	ptrdiff_t hi = length;
	int pivot = arr[0];
	int val = arr[1];
	while (lo + 1 < hi) {
		if (val <= pivot) {
			arr[lo] = val;
			val = arr[lo + 2];
			++lo;
		} else {
			--hi;
			int tmp = arr[hi];
			arr[hi] = val;
			val = tmp;
		}
	}
	arr[lo] = pivot;

	sort(lo, arr);
	sort(length - hi, &arr[hi]);
}

static void
selection_sort(ptrdiff_t length, int *arr)
{
	for (ptrdiff_t i = 0; i < length; ++i) {
		ptrdiff_t min = i;
		for (ptrdiff_t j = i + 1; j < length; ++j) {
			if (arr[j] < arr[min]) {
				min = j;
			}
		}
		int tmp = arr[i];
		arr[i] = arr[min];
		arr[min] = tmp;
	}
}
