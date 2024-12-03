#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <regex>
#include <string_view>
#include <charconv>
#include <array>

static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);

#ifdef SILVER
	std::regex re("mul\\((\\d{1,10}),(\\d{1,10})\\)");
#else
	std::regex re("mul\\((\\d{1,10}),(\\d{1,10})\\)|do\\(\\)|don't\\(\\)");
#endif

	std::regex_iterator<std::string_view::iterator>
	    i(input.begin(), input.end(), re);
	std::regex_iterator<std::string_view::iterator> end;
	int64_t sum = 0;

#ifdef SILVER
	for (; i != end; ++i) {
		std::match_results<std::string_view::iterator> match = *i;

		std::array<int32_t, 2> n;
		for (int j = 0; j < 2; ++j) {
			std::sub_match<std::string_view::iterator> sm =
			    match[j + 1];
		        std::from_chars(&*sm.first, &*sm.second, n[j]);
		}
		sum += n[0] * n[1];
	}
#else
	bool dont = false;
	for (; i != end; ++i) {
		std::match_results<std::string_view::iterator> match = *i;

		char const *s = &*(match[0].first);
		if (s[2] != 'l') {
			dont = s[2] == 'n';
			continue;
		}
		if (dont) {
			continue;
		}

		std::array<int32_t, 2> n;
		for (int j = 0; j < 2; ++j) {
			std::sub_match<std::string_view::iterator> sm =
			    match[j + 1];
		        std::from_chars(&*sm.first, &*sm.second, n[j]);
		}
		sum += n[0] * n[1];
	}
#endif

	printf("%" PRIu64 "\n", sum);
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
