#include <cmath>
#include <format>
#include <iterator>
#include <numeric>
#include <ostream>
#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <ranges>
#include <sstream>
#include <ranges>

#include "Clock.hpp"


#define TEST_TRUE(val) assert((val) == true)

constexpr std::size_t MULTIPLIER_X = 1000000;
constexpr std::size_t MULTIPLIER_Y = 1000000;


const std::string get_input_filename(int argc, char **argv)
{
    if ( argc < 2 ) {
        return "input.txt";
    } else {
        return std::string(argv[1]);
    }
}


enum class SpaceEntityType
{
    EMPTY,
    GALAXY
};

struct SpaceEntity
{
    std::size_t size_x = 1;
    std::size_t size_y = 1;
    SpaceEntityType type;
};

using Galaxy = std::pair<int64_t, int64_t>;


std::vector<std::pair<Galaxy, Galaxy>> get_galaxy_pairs(const std::vector<Galaxy> &galaxies)
{
    std::vector<std::pair<Galaxy, Galaxy>> res;

    auto msiter = galaxies.begin();
    auto end = galaxies.end();

    while ( msiter != end )
    {
        auto lsiter = msiter + 1;

        while ( lsiter != end )
        {
            res.push_back(std::make_pair(*msiter, *lsiter));
            lsiter++;
        }

        msiter++;
    }

    return res;
}


int main(int argc, char **argv)
{
    const std::filesystem::path inputFile = get_input_filename(argc, argv);





    if ( ! std::filesystem::exists(inputFile) ) {
        std::cerr << "Input file does not exist\n";
        exit(EXIT_FAILURE);
    };

    std::ifstream file{inputFile};

    if ( ! file.is_open() ) {
        std::cerr << "Failed to open file!" << "\n";
    }

    std::string current_line;

    MicrosecondClock clock;

    clock.start();

    std::vector<std::vector<SpaceEntity>> rows;

    while ( getline(file, current_line) )
    {
        std::vector<SpaceEntity> row{};

        for ( auto c : current_line )
        {
            SpaceEntityType type = SpaceEntityType::EMPTY;

            if ( c == '#' ) type = SpaceEntityType::GALAXY;

            row.emplace_back(SpaceEntity{
                1, 1, type
            });
        }

        rows.push_back(std::move(row));
    }

    for ( auto &r: rows )
    {
        if ( std::all_of(r.begin(), r.end(), [] (auto g) { return g.type == SpaceEntityType::EMPTY; } ) )
        {
            std::for_each(r.begin(), r.end(), [] ( auto &e ) { e.size_y = MULTIPLIER_Y; } );
        }
    }


    // Expect all rows to be of equal length
    for ( std::size_t col = 0; col < rows[0].size(); col++ )
    {
        if ( std::all_of(rows.begin(), rows.end(), [&col] (auto &r) -> bool {
            return r[col].type == SpaceEntityType::EMPTY;
        })) {
            std::for_each(rows.begin(), rows.end(), [&col] (auto &r) {
                r[col].size_x = MULTIPLIER_X;
            });
        }
    }


    std::vector<Galaxy> galaxies{};

    std::size_t space_row = 0, space_col = 0;

    for ( std::size_t mem_row = 0; mem_row < rows.size(); mem_row++ )
    {
        space_col = 0;


        for ( std::size_t mem_col = 0; mem_col < rows[0].size(); mem_col++ )
        {
            if ( rows[mem_row][mem_col].type == SpaceEntityType::GALAXY )
            {
                galaxies.emplace_back(std::make_pair(space_col, space_row));
            }

            space_col += rows[mem_row][mem_col].size_x;
        }

        space_row += rows[mem_row][0].size_y;
    }

    std::vector<std::pair<Galaxy, Galaxy>>
        galaxy_pairs = get_galaxy_pairs(galaxies);



    std::size_t sum = 0;

    for ( auto &p : galaxy_pairs )
    {
        auto dist_x = std::abs(p.second.first - p.first.first);
        auto dist_y = std::abs(p.second.second - p.first.second);

        sum += dist_x + dist_y;
    }

    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();

    std::cout << "Manhattan distance: " << sum << "\n";
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

