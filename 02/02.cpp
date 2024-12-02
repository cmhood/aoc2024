#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAX_REPORT_COUNT 500000
#define MAX_REPORT_LEVEL_COUNT 1000

static void get_input(int, char **);
static void parse_input(void);

struct Report {
	int level_count;
	int32_t levels[MAX_REPORT_LEVEL_COUNT];
};

static ptrdiff_t input_length = 0;
static char const *input = NULL;

static int report_count = 0;
static Report reports[MAX_REPORT_COUNT] = {0};

int
main(int argc, char **argv)
{
	get_input(argc, argv);
	parse_input();

#ifdef SILVER
	int safe_report_count = 0;
	for (int i = 0; i < report_count; ++i) {
		Report const *r = &reports[i];
		bool inc = r->levels[0] < r->levels[1];
		for (int j = 0; j < r->level_count - 1; ++j) {
			int32_t a = r->levels[j];
			int32_t b = r->levels[j + 1];
			int32_t d = abs(a - b);
			if (!(1 <= d && d <= 3) || (inc ^ (a < b))) {
				goto next_report;
			}
		}
		++safe_report_count;
next_report:
		;
	}
	printf("%d\n", safe_report_count);
#endif

#ifdef GOLD
	int safe_report_count = 0;
	for (int i = 0; i < report_count; ++i) {
		Report const *r = &reports[i];
		int damp_state = 0;
		int damp = INT_MAX;

		bool inc = (r->levels[0] < r->levels[1]) + (r->levels[1] <
		    r->levels[2]) + (r->levels[2] < r->levels[3]) > 1;
		for (int j = 0; j < r->level_count - 2;) {
			int32_t a = r->levels[j + (damp <= j)];
			int32_t b = r->levels[j + 1 + (damp <= j + 1)];
			int32_t d = abs(a - b);
			if (!(1 <= d && d <= 3) || (inc ^ (a < b))) {
				switch (damp_state++) {
				case 0:
					damp = j;
					break;
				case 1:
					++damp;
					break;
				default:
					goto next_report;
				}
				j = damp - 1;
				if (j < 0) {
					j = 0;
				}
				continue;
			}
			++j;
		}
		++safe_report_count;
next_report:
		;
	}
	printf("%d\n", safe_report_count);
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

	input = (char const *)mmap(NULL, input_length, PROT_READ, MAP_PRIVATE,
	    fd, 0);
	if (input == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
}

static void
parse_input(void)
{
	for (ptrdiff_t i = 0; i < input_length;) {
		for (int j = 0;; ++j) {
			if (j >= MAX_REPORT_LEVEL_COUNT) {
				goto fail;
			}
			int32_t n = 0;
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

			reports[report_count].levels[j] = n;

			if (input[i++] == '\n') {
				int count = j + 1;
				if (count < 4) {
					goto fail;
				}
				reports[report_count].level_count = count;
				break;
			}
		}
		if (++report_count > MAX_REPORT_COUNT) {
			goto fail;
		}
	}
	return;
fail:
	fprintf(stderr, "parse error\n");
	exit(EXIT_FAILURE);
}
