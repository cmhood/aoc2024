#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string_view>
#include <array>
#include <set>
#include <vector>

class Map {
public:
	Map(std::string_view);
	std::set<ptrdiff_t> get_trail_score(ptrdiff_t) const;
	int32_t get_trail_rating(ptrdiff_t) const;
	bool in_bounds(ptrdiff_t) const;
private:
	std::string_view m_grid;
	size_t m_width;
};

static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	Map map(input);

	int32_t sum = 0;
	for (size_t i = 0; i < input.size(); ++i) {
		if (input[i] == '0') {
#ifdef SILVER
			sum += map.get_trail_score(i).size();
#else
			sum += map.get_trail_rating(i);
#endif
		}
	}
	printf("%d\n", sum);
}

Map::Map(std::string_view grid)
: m_grid{grid}
{
	size_t newline = m_grid.find('\n');
	if (newline == m_grid.npos) {
		fprintf(stderr, "invalid input\n");
		exit(EXIT_FAILURE);
	}
	m_width = newline + 1;
}

std::set<ptrdiff_t>
Map::get_trail_score(ptrdiff_t p) const
{
	if (m_grid[p] == '9') {
		return {p};
	}

	ptrdiff_t w = m_width;
	std::array<ptrdiff_t, 4> directions = {-w, -1, 1, w};

	std::set<ptrdiff_t> set;
	for (ptrdiff_t d : directions) {
		ptrdiff_t i = p + d;
		if (in_bounds(i) && m_grid[i] == m_grid[p] + 1) {
			std::set<ptrdiff_t> s = get_trail_score(i);
			set.insert(s.begin(), s.end());
		}
	}
	for (ptrdiff_t i : set) {
		assert(m_grid[i] == '9');
	}
	return set;
}

int32_t
Map::get_trail_rating(ptrdiff_t p) const
{
	if (m_grid[p] == '9') {
		return 1;
	}

	ptrdiff_t w = m_width;
	std::array<ptrdiff_t, 4> directions = {-w, -1, 1, w};

	int32_t sum = 0;
	for (ptrdiff_t d : directions) {
		ptrdiff_t i = p + d;
		if (in_bounds(i) && m_grid[i] == m_grid[p] + 1) {
			sum += get_trail_rating(i);
		}
	}
	return sum;
}

bool
Map::in_bounds(ptrdiff_t d) const
{
	return 0 <= d && d < static_cast<ptrdiff_t>(m_grid.size());
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
