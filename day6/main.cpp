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

std::vector<std::size_t> get_line(const std::string &line)
{

    std::vector<std::size_t> result;

    std::size_t colon_pos = line.find(':');

    if ( colon_pos == std::string::npos )
    {
        throw "Invalid format for seeds line";
    }


    std::string string = line.substr(colon_pos + 2);

    std::istringstream ss{string};

    std::istream_iterator<std::size_t> iter{ss};
    std::istream_iterator<std::size_t> end{};

    while ( iter != end ) {

        result.emplace_back(*iter);

        iter++;
    }

    return result;

}


struct Race {
    std::size_t race_time;
    std::size_t record_distance;
};

std::size_t get_speed(std::size_t miliseconds_held)
{
    return miliseconds_held;
}

std::size_t get_distance(std::size_t speed, std::size_t time)
{
    return speed*time;
}

std::size_t get_race_distance(std::size_t race_time, std::size_t time_held)
{
    return get_distance(time_held, race_time - time_held);
}


std::size_t min_time_to_hold(const Race race) {
    std::size_t cand = 0;
    std::size_t distance_to_beat = race.record_distance;

    std::size_t current_distance = 0;

    std::size_t jump_delta = std::floor(std::sqrt(race.race_time));

    while ( current_distance <= distance_to_beat )
    {
        cand += jump_delta;
        current_distance = get_race_distance(race.race_time, cand);
    }

    cand -= jump_delta;
    current_distance = get_race_distance(race.race_time, cand);

    while ( current_distance <= distance_to_beat )
    {
        current_distance = get_race_distance(race.race_time, cand++);
    }

    return cand - 1;

}

std::size_t get_num_ways_to_beat_race(const Race race)
{

    std::size_t result = 0;

    for ( std::size_t hold_time = 0; hold_time < race.race_time; hold_time++ )
    {
        auto distance = get_race_distance(race.race_time, hold_time);

        if ( distance > race.record_distance ) result++;
    }

    return result;
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

    std::vector<std::size_t> time = get_line(current_line);

    if ( ! std::getline(file, current_line) ) {
        throw "Failed getting a line from the file";
    }

    std::vector<std::size_t> distance = get_line(current_line);


    std::vector<Race> races{};
    for ( std::size_t i = 0; i < time.size(); i++ )
    {
        races.emplace_back(Race{time[i], distance[i]});
    }


    std::size_t result_product = 1;

    auto time_ss = std::ostringstream{};
    auto dist_ss = std::ostringstream{};
    for ( auto &race : races)
    {

        time_ss << std::to_string(race.race_time);
        dist_ss << std::to_string(race.record_distance);

        std::cout << std::format("Race t={} ms, d={} mm", race.race_time, race.record_distance) << "\n";

        auto num_ways = get_num_ways_to_beat_race(race);


        result_product *= num_ways;
    }




    Race single_race = Race {
        std::stoul(time_ss.str()),
        std::stoul(dist_ss.str())
    };


    std::cout << std::format("Race t={} ms, d={} mm", single_race.race_time, single_race.record_distance) << "\n";
    auto min = min_time_to_hold(single_race);

    std::cout << std::format("Num ways {} - {} = {}\n", single_race.race_time - min, min, single_race.race_time - min - min + 1);

    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();


    std::cout << std::format("Resulting product: {}\n", result_product);
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

