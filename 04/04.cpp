#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string_view>
#include <array>
#include <algorithm>

static std::string_view get_input(int, char **);
static size_t find_matches(std::string_view, size_t);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);

	char const *newline = static_cast<char const *>(memchr(
	    static_cast<void const *>(input.data()), '\n', input.size()));
	if (!newline) {
		fprintf(stderr, "parse error\n");
		exit(EXIT_FAILURE);
	}
	size_t width = newline - input.data() + 1;

	size_t word_count = 0;
	for (size_t i = 0; i < input.size(); ++i) {
		word_count += find_matches(input.substr(i), width);
	}
	printf("%zu\n", word_count);
}

static size_t
find_matches(std::string_view s, size_t width)
{
#ifdef SILVER
	size_t count = 0;
	std::array<size_t, 4> const spacings = {1, width - 1, width, width + 1};
	for (size_t sp : spacings) {
		std::array<size_t, 4> const pattern = {0, sp, 2 * sp, 3 * sp};

		if (s.size() <= pattern.back()) {
			break;
		}

		std::array<char, pattern.size()> text;
		std::transform(pattern.begin(), pattern.end(), text.begin(),
		    [&](size_t n) {
			return s[n];
		});

		count += std::equal(text.begin(), text.end(), "XMAS") ||
		    std::equal(text.begin(), text.end(), "SAMX");
	}
	return count;
#else
	std::array<size_t, 5> const pattern =
	    {0, 2, width + 1, 2 * width, 2 * width + 2};
	if (s.size() <= pattern.back()) {
		return 0;
	}
	std::array<char, pattern.size()> text;
	std::transform(pattern.begin(), pattern.end(), text.begin(),
	    [&](size_t n) {
		return s[n];
	});
	constexpr std::array<char const *, 4> matches =
	    {"MMASS", "SMASM", "MSAMS", "SSAMM"};
	return std::any_of(matches.begin(), matches.end(), [&](char const *m) {
		return std::equal(text.begin(), text.end(), m);
	});
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
