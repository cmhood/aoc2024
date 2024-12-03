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
#include <algorithm>

static std::string_view get_input(int, char **);
static char const *parse(char const *, char const *, int32_t *, size_t);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);

	std::vector<int32_t> left;
	std::vector<int32_t> right;

	for (char const *p = input.data(); p < &input[input.size()];) {
		int32_t l, r;
		p = parse(p, &input[input.size()], &l, 3);
		p = parse(p, &input[input.size()], &r, 1);
		left.push_back(l);
		right.push_back(r);
	}

	std::sort(left.begin(), left.end());
	std::sort(right.begin(), right.end());

#ifdef SILVER
	int64_t sum = 0;
	for (size_t i = 0; i < left.size(); ++i) {
		sum += abs(left[i] - right[i]);
	}
	printf("%" PRIu64 "\n", sum);
#else
	int64_t sum = 0;
	size_t j = 0;
	for (size_t i = 0; i < left.size(); ++i) {
		int32_t l = left[i];
		while (j < right.size() && right[j] < l) {
			++j;
		}
		size_t start = j;
		while (j < right.size() && right[j] == l) {
			++j;
		}
		sum += l * (j - start);
	}
	printf("%" PRIu64 "\n", sum);
#endif
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
parse(char const *ptr, char const *end, int32_t *n, size_t padding)
{
	std::from_chars_result result = std::from_chars(ptr, end, *n);
	if (result.ec != std::errc() || result.ptr + padding > end) {
		fprintf(stderr, "parse error\n");
		exit(EXIT_FAILURE);
	}
	return result.ptr + padding;
}
