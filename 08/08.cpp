#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string_view>
#include <map>
#include <vector>
#include <set>

struct Coords : public std::pair<ptrdiff_t, ptrdiff_t> {
	using std::pair<ptrdiff_t, ptrdiff_t>::pair;
	Coords operator+(Coords const &) const;
	Coords operator-(Coords const &) const;
	Coords operator*(ptrdiff_t const &) const;
};

struct Map {
	std::string_view map;
	ptrdiff_t width, height;

	Map(std::string_view);
	bool is_in_range(Coords) const;
	char get(Coords) const;
};

static void get_antinodes(std::set<Coords> *, Map const &, Coords, Coords);
static std::string_view get_input(int, char **);

int
main(int argc, char **argv)
{
	std::string_view input = get_input(argc, argv);
	Map map(input);

	std::map<char, std::vector<Coords>> station_sets;

	for (ptrdiff_t x = 0; x < map.width; ++x) {
		for (ptrdiff_t y = 0; y < map.height; ++y) {
			Coords p(x, y);
			char c = map.get(p);
			if (c == '.') {
				continue;
			}
			station_sets[c].push_back(p);
		}
	}

	std::set<Coords> antinodes;
	for (auto const &[key, set] : station_sets) {
		for (size_t i = 0; i < set.size(); ++i) {
			for (size_t j = 0; j < set.size(); ++j) {
				get_antinodes(&antinodes, map, set[i], set[j]);
			}
		}
	}
	printf("%zu\n", antinodes.size());
}

static void
get_antinodes(std::set<Coords> *antinodes, Map const &map, Coords a, Coords b)
{
	if (a == b) {
		return;
	}

#ifdef SILVER
	ptrdiff_t start = 1, end = 2;
#else
	ptrdiff_t start = 0, end = PTRDIFF_MAX;
#endif

	for (ptrdiff_t k = start; k < end; ++k) {
		Coords an = a + (a - b) * k;
		if (!map.is_in_range(an)) {
			break;
		}
		antinodes->insert(an);
	}
}

Coords
Coords::operator+(Coords const &other) const
{
	Coords res;
	res.first =  this->first  + other.first;
	res.second = this->second + other.second;
	return res;
}

Coords
Coords::operator-(Coords const &other) const
{
	Coords res;
	res.first =  this->first  - other.first;
	res.second = this->second - other.second;
	return res;
}

Coords
Coords::operator*(ptrdiff_t const &k) const
{
	Coords res;
	res.first  = this->first  * k;
	res.second = this->second * k;
	return res;
}

Map::Map(std::string_view sv)
: map{sv}
{
	char const *newline = static_cast<char const *>(memchr(
	    static_cast<void const *>(this->map.data()), '\n',
	    this->map.size()));
	if (!newline) {
		fprintf(stderr, "parse error\n");
		exit(EXIT_FAILURE);
	}
	this->width = newline - this->map.data();
	if (this->map.size() % static_cast<size_t>(width + 1) != 0) {
		fprintf(stderr, "invalid input dimensions\n");
		exit(EXIT_FAILURE);
	}
	this->height = this->map.size() / static_cast<size_t>(width + 1);
}

bool
Map::is_in_range(Coords p) const
{
	return 0 <= p.first && p.first < this->width &&
	    0 <= p.second && p.second < this->height;
}

char
Map::get(Coords p) const
{
	return this->map[p.second * (this->width + 1) + p.first];
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
