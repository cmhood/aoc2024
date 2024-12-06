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
#include <map>
#include <set>
#include <vector>
#include <charconv>

static size_t evaluate_sequence(std::vector<int32_t> *,
    std::map<int32_t, std::set<int32_t>> const &);
static std::string_view get_input(int, char **);
static bool parse_dependency(std::string_view, size_t *, std::pair<int, int> *);
static bool parse_sequence(std::string_view, size_t *, std::vector<int32_t> *);
static int32_t parse_int(std::string_view, size_t *);
static bool parse_char(std::string_view, size_t *, char);
static void expect(bool);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	size_t offset = 0;

	std::map<int32_t, std::set<int32_t>> dependencies;
	std::pair<int, int> dep;
	while (parse_dependency(input, &offset, &dep)) {
		dependencies[dep.second].insert(dep.first);
	}

	int32_t sum = 0;
	std::vector<int32_t> seq;
	while (parse_sequence(input, &offset, &seq)) {
		sum += evaluate_sequence(&seq, dependencies);
		seq.clear();
	}
	printf("%" PRIi32 "\n", sum);
}

static size_t
evaluate_sequence(std::vector<int32_t> *seq,
    std::map<int32_t, std::set<int32_t>> const &dependencies)
{
#ifdef SILVER
	std::set<int32_t> forbidden;
	for (size_t i = 0; i < seq->size(); ++i) {
		if (forbidden.find((*seq)[i]) != forbidden.end()) {
			return 0;
		}
		auto d = dependencies.find((*seq)[i]);
		if (d != dependencies.end()) {
			std::set<int32_t> s = d->second;
			forbidden.insert(s.begin(), s.end());
		}
	}
	return (*seq)[seq->size() / 2];
#else
	bool reordered = false;
restart:
	std::set<int32_t> forbidden;
	for (size_t i = 0; i < seq->size(); ++i) {
		if (forbidden.find((*seq)[i]) != forbidden.end()) {
			std::swap((*seq)[i], (*seq)[i - 1]);
			reordered = true;
			goto restart;
		}
		auto d = dependencies.find((*seq)[i]);
		if (d != dependencies.end()) {
			std::set<int32_t> s = d->second;
			forbidden.insert(s.begin(), s.end());
		}
	}
	return reordered ? (*seq)[seq->size() / 2] : 0;
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

static bool
parse_dependency(std::string_view input, size_t *offset, std::pair<int, int> *d)
{
	if (parse_char(input, offset, '\n')) {
		return false;
	}
	std::array<int32_t, 2> a;
	for (size_t i = 0; i < a.size(); ++i) {
		a[i] = parse_int(input, offset);
		expect(parse_char(input, offset, "|\n"[i]));
	}
	d->first = a[0];
	d->second = a[1];
	return true;
}

static bool
parse_sequence(std::string_view input, size_t *offset, std::vector<int32_t> *seq)
{
	if (*offset >= input.size()) {
		return false;
	}
	for (;;) {
		int32_t n = parse_int(input, offset);
		seq->push_back(n);
		if (parse_char(input, offset, '\n')) {
			return true;
		}
		expect(parse_char(input, offset, ','));
	}
}

static int32_t
parse_int(std::string_view input, size_t *offset)
{
	int32_t i;
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
