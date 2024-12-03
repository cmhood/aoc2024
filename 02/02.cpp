#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string_view>
#include <vector>
#include <charconv>

static std::string_view get_input(int, char **);
static char const *parse(char const *, char const *, int32_t *, bool *);
static bool is_report_safe(std::vector<int32_t> const &);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);

	int32_t count = 0;
	for (char const *p = input.data(); p < &input[input.size()];) {
		std::vector<int32_t> levels;
		for (bool eol = false; !eol;) {
			int32_t n;
			p = parse(p, &input[input.size()], &n, &eol);
			levels.push_back(n);
		}
		if (is_report_safe(levels)) {
			++count;
		}
	}
	printf("%" PRIi32 "\n", count);
}

static std::string_view
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

	off_t length = lseek(fd, 0, SEEK_END);
	if (length < 0) {
		perror("lseek");
		exit(EXIT_FAILURE);
	}
	void *data = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	return {static_cast<char const *>(data), static_cast<size_t>(length)};
}

static char const *
parse(char const *ptr, char const *end, int32_t *n, bool *eol)
{
	std::from_chars_result result = std::from_chars(ptr, end, *n);
	if (result.ec != std::errc()) {
		fprintf(stderr, "parse error\n");
		exit(EXIT_FAILURE);
	}
	char const *p = result.ptr;
	while (p < end && *p == ' ') {
		++p;
	}
	if (p < end && (*eol = *p == '\n')) {
		++p;
	}
	return p;
}

static bool
is_report_safe(std::vector<int32_t> const &levels)
{
#ifdef SILVER
	bool inc = levels[0] < levels[1];
	for (size_t i = 0; i < levels.size() - 1; ++i) {
		int32_t l = levels[i];
		int32_t r = levels[i + 1];
		int32_t d = abs(l - r);
		if (inc ^ (l < r) || !(1 <= d && d <= 3)) {
			return false;
		}
	}
	return true;
#else
	int damp_state = 0;
	size_t damp = SIZE_MAX;
	bool inc = (levels[0] < levels[1]) + (levels[1] < levels[2]) +
	    (levels[2] < levels[3]) > 1;
	for (size_t i = 0; i < levels.size() - 2;) {
		int32_t l = levels[i + (damp <= i)];
		int32_t r = levels[i + 1 + (damp <= (i + 1))];
		int32_t d = abs(l - r);
		if (!(inc ^ (l < r)) && 1 <= d && d <= 3) {
			++i;
			continue;
		}
		switch (damp_state++) {
		case 0:
			damp = i;
			break;
		case 1:
			++damp;
			break;
		default:
			return false;
		}
		i = damp > 0 ? damp - 1 : 0;
	}
	return true;
#endif
}
