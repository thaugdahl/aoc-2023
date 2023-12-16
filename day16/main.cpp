#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <unordered_map>
#include <list>

#include "Clock.hpp"


#define TEST_TRUE(val) assert((val) == true)

const std::string get_input_filename(int argc, char **argv)
{
    if ( argc < 2 ) {
        return "input.txt";
    } else {
        return std::string(argv[1]);
    }
}

using TileMapData = std::vector<std::string>;


struct Coordinate
{
    int x = 0;
    int y = 0;

    Coordinate operator+(const Coordinate &rhs)
    {
        return Coordinate{x + rhs.x, y + rhs.y};
    }


    Coordinate operator-(const Coordinate &rhs)
    {
        return Coordinate{x - rhs.x, y - rhs.y};
    }

    bool operator==(const Coordinate &rhs)
    {
        return x == rhs.x && y == rhs.y;
    }
};

struct TileMap
{
    std::size_t width;
    std::size_t height;
    TileMapData data;
    std::vector<std::vector<bool>> energized;
    std::vector<std::vector<std::pair<bool, bool>>> visited;

    void reset() {
        for ( std::size_t y = 0; y < height; y++ )
        {
            for ( std::size_t x = 0; x < width; x++ )
            {
                energized[y][x] = false;
            }
        }
    }

    void visit(int x, int y, int dx, int dy)
    {

        if ( ! is_inside(x,y) ) return;

        if ( std::abs(dx) > 0 ) {
            visited[y][x].first = true;
        }

        if ( std::abs(dy) > 0 ) {
            visited[y][x].second = true;
        }
    }

    bool visited_at(int x, int y, int dx, int dy) const
    {
        if ( ! is_inside(x,y) ) return true;

        if ( std::abs(dx) > 0 )
            return visited[y][x].first;

        if ( std::abs(dy) > 0 )
            return visited[y][x].second;

        return true;
    }

    void energize(int x, int y)
    {
        if ( ! is_inside(x,y) ) return;
        energized[y][x] = true;
    }

    std::optional<char> tile_at(int x, int y) const
    {
        if ( ! is_inside(x,y) )
            return std::optional<char>{};

        return std::optional<char>{data[y][x]};
    }

    bool is_inside(int x, int y) const
    {
        return ! (  x < 0 || x >= width || y < 0 || y >= height );
    }

    bool energized_equality(const TileMap &other)
    {

        if ( other.height != height || other.width != width ) return false;

        for ( std::size_t y = 0; y < height; y++ )
        {
            for ( std::size_t x = 0; x < width; x++ )
            {
                if ( energized[y][x] != other.energized[y][x] ) return false;
            }
        }

        return true;
    }

    std::size_t num_energized()
    {
        std::size_t result = 0;
        for ( std::size_t y = 0; y < height; y++ )
        {
            for ( std::size_t x = 0; x < width; x++ )
            {
                if ( energized[y][x] ) result++;
            }
        }

        return result;
    }
};


struct Beam
{
    Coordinate position;
    Coordinate direction;

    bool is_horizontal()
    {
        return std::abs(direction.x) > 0;
    }

    bool is_vertical()
    {
        return std::abs(direction.y) > 0;
    }
};


TileMap read_tile_map(const std::filesystem::path &path)
{
    if ( ! std::filesystem::exists(path) ) {
        std::cerr << "Input file does not exist\n";
        exit(EXIT_FAILURE);
    };

    std::ifstream file{path};

    if ( ! file.is_open() ) {
        std::cerr << "Failed to open file!" << "\n";
    }

    std::string current_line;

    TileMapData data{};

    while ( std::getline( file, current_line ) )
    {
        data.emplace_back(std::move(current_line));
    }

    const std::size_t width = data[0].length();
    const std::size_t height = data.size();

    TileMap result{};

    result.data = std::move(data);
    result.width = width;
    result.height = height;
    result.energized.resize(height);
    result.visited.resize(height);

    for ( auto &e : result.energized )
    {
        e.resize(width, false);
    }

    for ( auto &e : result.visited )
    {
        e.resize(width, std::make_pair(false, false));
    }

    return result;
}

void print_energized(TileMap &tiles)
{
    for ( auto &row : tiles.energized )
    {
        for ( auto el : row )
        {

            std::cout << (el ? '#' : '.');
        }

        std::cout << "\n";
    }
}

void simulate(TileMap &tiles, Beam start_beam)
{
    std::vector<Beam> beams{};
    beams.emplace_back(start_beam);

    TileMap checkpoint = tiles;
    std::size_t iteration = 1;

    tiles.energize(start_beam.position.x, start_beam.position.y);

    while ( beams.size() > 0 )
    {
        iteration++;

        if ( iteration % 3 == 0 )
        {
            if ( checkpoint.energized_equality(tiles) )
            {
                break;
            }


            checkpoint = tiles;
        }


        std::size_t num_beams = beams.size();

        std::vector<Beam> new_beams{};

        for ( auto &beam : beams )
        {

            auto [x,y] = beam.position;
            auto [dx, dy] = beam.direction;

            tiles.energize(x, y);
            tiles.visit(x,y,dx,dy);

            auto tile_at_pos = tiles.tile_at(x, y);

            if ( ! tile_at_pos.has_value() )
            { continue; }

            char val = tile_at_pos.value();

            if ( val == '/' )
            {
                beam.direction = {-dy, -dx};
            } if ( val == '\\')
            {
                beam.direction = {dy, dx};
            }

            if ( val == '|' && beam.is_horizontal() )
            {
                Beam new_beam;
                new_beam.position = beam.position;
                new_beam.direction = {0, 1};
                new_beams.emplace_back(new_beam);

                beam.direction = {0, -1};
            }

            if ( val == '-' && beam.is_vertical() )
            {
                Beam new_beam;
                new_beam.position = beam.position;
                new_beam.direction = {1, 0};
                new_beams.emplace_back(new_beam);
                beam.direction = {-1,0};
            }

            beam.position = beam.position + beam.direction;

        }

        for ( auto &nb : new_beams )
        {
            beams.emplace_back(nb);
        }

        // Cleanup
        auto end_it = std::remove_if(beams.begin(), beams.end(), [&tiles] (const Beam &beam) {
            return ! tiles.is_inside(beam.position.x, beam.position.y);
        });

        beams.erase(end_it, beams.end());
    }
}

std::size_t simulate_if_not_visited(TileMap &tiles, Beam start_beam)
{
    auto [x, y] = start_beam.position;
    auto [dx, dy] = start_beam.direction;

    if ( tiles.visited_at(x,y,dx,dy) ) { return 0; }

    tiles.reset();
    simulate(tiles, start_beam);
    return tiles.num_energized();
}


std::optional<Beam> get_next_config(const TileMap &tiles)
{
        for ( int y = 0; y < tiles.height; y++ )
        {
            for ( int x = 0; x < tiles.width; x++ )
            {
                if ( ! tiles.visited_at(x, y, 1, 0 ) ) {
                    return std::optional<Beam>{Beam{x-1,y,1,0}};
                } else if ( ! tiles.visited_at(x,y,0,1) ) {
                    return std::optional<Beam>{Beam{x,y-1,0,1}};
                }
            }
        }

        return std::optional<Beam>{};
}



int main(int argc, char **argv)
{
    const std::filesystem::path input_file = get_input_filename(argc, argv);

    MicrosecondClock clock;

    clock.start();

    TileMap tiles = read_tile_map(input_file);

    std::size_t best_config = 0;


    for ( int y = 0; y < tiles.height; y++ )
    {
        auto starting_beam = Beam{0, y, 1, 0};
        best_config = std::max(simulate_if_not_visited(tiles, starting_beam), best_config);

        starting_beam = Beam{static_cast<int>(tiles.width), y, -1, 0};
        best_config = std::max(simulate_if_not_visited(tiles, starting_beam), best_config);
    }

    for ( int x = 0; x < tiles.width; x++ )
    {

        auto starting_beam = Beam{x, 0, 0, 1};
        best_config = std::max(simulate_if_not_visited(tiles, starting_beam), best_config);

        starting_beam = Beam{x, static_cast<int>(tiles.height), 0, 1};
        best_config = std::max(simulate_if_not_visited(tiles, starting_beam), best_config);

    }


    tiles.reset();


    std::cout << best_config << "\n";
    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

