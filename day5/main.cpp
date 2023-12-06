#include <cmath>
#include <format>
#include <iterator>
#include <numeric>
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

#include "Clock.hpp"


#define TEST_TRUE(val) assert((val) == true)

struct MappingRange
{
    std::size_t destination;
    std::size_t range_begin;
    std::size_t range_length;
};

struct Mapping
{
    std::size_t destination;
    std::size_t source;
};


const std::string get_input_filename(int argc, char **argv)
{
    if ( argc < 2 ) {
        return "input.txt";
    } else {
        return std::string(argv[1]);
    }

}

void tests()
{
}

std::vector<std::size_t> get_seeds(const std::string &seeds_line)
{

    std::vector<std::size_t> result;

    std::size_t colon_pos = seeds_line.find(':');

    if ( colon_pos == std::string::npos )
    {
        throw "Invalid format for seeds line";
    }


    std::string seeds_string = seeds_line.substr(colon_pos + 2);

    std::istringstream ss{seeds_string};

    std::istream_iterator<std::size_t> iter{ss};
    std::istream_iterator<std::size_t> end{};

    while ( iter != end ) {

        result.emplace_back(*iter);

        iter++;
    }

    return result;

}


using Mappings = std::vector<MappingRange>;
using MappingsHashMap = std::unordered_map<std::size_t, std::size_t>;


struct FullTraceConfig {
    Mappings seed_to_soil;
    Mappings soil_to_fertilizer;
    Mappings fertilizer_to_water;
    Mappings water_to_light;
    Mappings light_to_temperature;
    Mappings temperature_to_humidity;
    Mappings humidity_to_location;
};

MappingRange read_mapping_line(const std::string &line)
{

    MappingRange res;

    std::istringstream ss{line};

    ss >> res.destination >> res.range_begin >> res.range_length;

    return res;

}

bool skip_to_header(std::ifstream &file)
{
    std::string current_line;
    while ( std::getline(file, current_line) )
    {
        if ( isalpha(current_line[0]) ) break;
    }

    return true;
}

Mappings read_block(std::ifstream &file)
{
    Mappings mappings{};

    std::string current_line;

    while ( std::getline(file, current_line) )
    {

        if ( current_line.size() < 1 )
        {
            break;
        }

        auto mapping = read_mapping_line(current_line);

        mappings.emplace_back(mapping);
    }

    skip_to_header(file);


    return mappings;
}

MappingsHashMap mappings_to_hashmap(const Mappings &maps)
{
    MappingsHashMap result{};

    for ( auto &x : maps ) {
        for ( auto source_offset = 0; source_offset < x.range_length; source_offset++)
        {
            result.emplace(
                    std::make_pair(
                        x.range_begin + source_offset,
                        x.destination + source_offset));
        }
    }

    return result;
}

inline std::size_t get_or_fallthrough(const Mappings &map, std::size_t key)
{
    for ( auto mapping : map )
    {
        auto range_begin = mapping.range_begin;
        auto range_end = mapping.range_begin + mapping.range_length - 1;

        bool in_range = key >= range_begin && key <= range_end;


        if (in_range)
        {
            auto offset = key - range_begin;

            return mapping.destination + offset;
        }
    }

    return key;
}

std::size_t trace_to_location(std::size_t seed, const FullTraceConfig &config)
{
    std::size_t soil        = get_or_fallthrough(config.seed_to_soil, seed);
    std::size_t fertilizer  = get_or_fallthrough(config.soil_to_fertilizer, soil);
    std::size_t water       = get_or_fallthrough(config.fertilizer_to_water, fertilizer);
    std::size_t light       = get_or_fallthrough(config.water_to_light, water);
    std::size_t temperature = get_or_fallthrough(config.light_to_temperature, light);
    std::size_t humidity    = get_or_fallthrough(config.temperature_to_humidity, temperature);
    std::size_t location    = get_or_fallthrough(config.humidity_to_location, humidity);

    return location;
}


int main(int argc, char **argv)
{
    const std::filesystem::path inputFile = get_input_filename(argc, argv);

    // Test runner
    tests();

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



    if ( ! std::getline(file, current_line) ) {
        throw "Failed getting a line from the file";
    }

    std::vector<std::size_t> seeds = get_seeds(current_line);
    skip_to_header(file);

    if ( seeds.size() % 2 > 0 )
    {
        throw "Number of seeds not divisible by two. Ranges representation not supported";
    }


    FullTraceConfig config = FullTraceConfig{
        .seed_to_soil            = read_block(file),
        .soil_to_fertilizer      = read_block(file),
        .fertilizer_to_water     = read_block(file),
        .water_to_light          = read_block(file),
        .light_to_temperature    = read_block(file),
        .temperature_to_humidity = read_block(file),
        .humidity_to_location    = read_block(file)
    };


    std::size_t lowest_location = std::numeric_limits<std::size_t>::max();

    auto iter_fst = seeds.begin();
    auto iter_snd = seeds.begin() + 1;

    while ( iter_fst != seeds.end() && iter_snd != seeds.end() )
    {
        auto seed_current = *iter_fst;
        auto seed_end = seed_current + *iter_snd - 1;


        while ( seed_current < seed_end )
        {
            std::size_t location = trace_to_location(seed_current, config);
            if ( location < lowest_location ) lowest_location = location;

            seed_current++;
        }

        ++(++iter_fst);
        ++(++iter_snd);
    }



    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();

    std::cout << "Lowest location " << lowest_location << "\n";

    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

