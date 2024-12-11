#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <charconv>

class Multiset {
public:
	void insert(int64_t);
	void blink();
	int64_t size();
private:
	std::unordered_map<int64_t, int64_t> m_map;
};

static std::vector<int64_t> evaluate(int64_t);
static int64_t parse_int(std::string_view, size_t *);
static bool parse_char(std::string_view, size_t *, char);
static void expect(bool);
static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	size_t offset = 0;

	Multiset ms;
	for (;;) {
		ms.insert(parse_int(input, &offset));
		if (parse_char(input, &offset, '\n')) {
			break;
		}
		expect(parse_char(input, &offset, ' '));
	}

#ifdef SILVER
	int count = 25;
#else
	int count = 75;
#endif

	for (int i = 0; i < count; ++i) {
		ms.blink();
	}
	printf("%" PRIi64 "\n", ms.size());
}

static std::vector<int64_t>
evaluate(int64_t n)
{
	if (n == 0) {
		return {1};
	}

	double ndigits = floor(log10(static_cast<double>(n)) + 1);
	if (fmod(ndigits, 2) != 0) {
		return {2024 * n};
	}

	double div = pow(10, ndigits * 0.5);
	double l = floor(static_cast<double>(n) / div);
	double r = fmod(n, div);
	return {static_cast<int64_t>(l), static_cast<int64_t>(r)};
}

void
Multiset::blink()
{
	Multiset new_ms;
	for (auto [n, count] : m_map) {
		assert(count > 0);
		std::vector<int64_t> res = evaluate(n);
		for (int64_t r : res) {
			new_ms.m_map[r] += count;
		}
	}
	*this = new_ms;
}

void
Multiset::insert(int64_t n)
{
	++m_map[n];
}

int64_t
Multiset::size()
{
	int64_t count = 0;
	for (auto [n, c] : m_map) {
		count += c;
	}
	return count;
}

static int64_t
parse_int(std::string_view input, size_t *offset)
{
	int64_t i;
	std::from_chars_result r =
	    std::from_chars(&input[*offset], &input[input.size()], i);
	expect(r.ec == std::errc());
	*offset = r.ptr - input.data();
	return i;
}

static bool
parse_char(std::string_view input, size_t *offset, char c)
{
	expect(*offset < input.size());
	bool b = input[*offset] == c;
	*offset += b;
	return b;
}

static void
expect(bool cond)
{
	if (!cond) {
		fprintf(stderr, "parse error\n");
		exit(EXIT_FAILURE);
	}
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
