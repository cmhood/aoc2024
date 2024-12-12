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
#include <functional>

class Region : public std::set<ptrdiff_t> {
public:
	Region(ptrdiff_t);
	int64_t area() const;
	int64_t perimeter() const;
	int64_t sides() const;
private:
	int64_t contains(ptrdiff_t) const;
	int64_t get_corners(ptrdiff_t) const;
	ptrdiff_t m_width;
};

class Map {
public:
	Map(std::string_view);
	std::vector<Region> const &regions();
private:
	Region get_region(ptrdiff_t);
	std::vector<Region> m_regions;
	std::string_view m_grid;
	ptrdiff_t m_width;
};

static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	Map map(input);

	int64_t total_price = 0;
	size_t i = 0;
	for (Region const &r : map.regions()) {
#ifdef SILVER
		total_price += r.area() * r.perimeter();
#else
		total_price += r.area() * r.sides();
#endif
	}
	printf("%" PRIi64 "\n", total_price);
}

Region::Region(ptrdiff_t w)
: m_width{w}
{
}

int64_t
Region::area() const
{
	return size();
}

int64_t
Region::perimeter() const
{
	int64_t perim = 0;
	for (ptrdiff_t p : *this) {
		perim += !contains(p - m_width) + !contains(p - 1) +
		    !contains(p + 1) + !contains(p + m_width);
	}
	return perim;
}

int64_t
Region::sides() const
{
	int64_t corners = 0;
	for (ptrdiff_t p : *this) {
		corners += get_corners(p);
	}
	assert(corners % 2 == 0);
	return corners;
}

int64_t
Region::get_corners(ptrdiff_t p) const
{
	assert(contains(p));
	int64_t res = 0;

	// Convex corners
	res += !contains(p - 1) && !contains(p - m_width);
	res += !contains(p + 1) && !contains(p - m_width);
	res += !contains(p - 1) && !contains(p + m_width);
	res += !contains(p + 1) && !contains(p + m_width);

	// Concave corners
	res += contains(p - 1) && contains(p - m_width) &&
	    !contains(p - m_width - 1);
	res += contains(p + 1) && contains(p - m_width) &&
	    !contains(p - m_width + 1);
	res += contains(p - 1) && contains(p + m_width) &&
	    !contains(p + m_width - 1);
	res += contains(p + 1) && contains(p + m_width) &&
	    !contains(p + m_width + 1);

	assert(res <= 4);

	return res;
}

int64_t
Region::contains(ptrdiff_t p) const
{
	return find(p) != end();
}

Map::Map(std::string_view sv)
:m_grid{sv}
{
	size_t newline = m_grid.find('\n');
	if (newline == m_grid.npos) {
		fprintf(stderr, "invalid input\n");
		exit(EXIT_FAILURE);
	}
	m_width = newline + 1;

	std::set<ptrdiff_t> visited;
	for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(m_grid.size()); ++i) {
		if (m_grid[i] == '\n' || visited.find(i) != visited.end()) {
			continue;
		}
		Region r = get_region(i);
		visited.insert(r.begin(), r.end());
		m_regions.push_back(std::move(r));
	}
}

Region
Map::get_region(ptrdiff_t origin)
{
	Region region(m_width);

	auto get = [&](ptrdiff_t p) {
		if (!(0 <= p && p < static_cast<ptrdiff_t>(m_grid.size()))) {
			return '\n';
		}
		return m_grid[p];
	};

	char color = get(origin);
	std::function<void(ptrdiff_t)> expand = [&](ptrdiff_t p) {
		if (get(p) != color || region.find(p) != region.end()) {
			return;
		}

		region.insert(p);

		std::array<ptrdiff_t, 4> directions =
		    {-m_width, 1, m_width, -1};
		for (ptrdiff_t d : directions) {
			expand(p + d);
		}
	};

	expand(origin);

	return region;
}

std::vector<Region> const &
Map::regions()
{
	return m_regions;
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

