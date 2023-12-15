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

static constexpr std::size_t HASHMAP_SIZE = 256;
using HashMapBucket = std::list<std::pair<std::string, int>>;

// Delimiter is SBO, pass as value
std::vector<std::string_view> split_all(const std::string &input_string, const std::string delimiter)
{
    // If found at index i, next string begins at i+delim_width
    auto delim_width = delimiter.length();
    std::vector<std::string_view> string_views{};
    std::string_view current_sv = input_string;


    while ( ! current_sv.empty() )
    {
        std::size_t delim_index = current_sv.find_first_of(delimiter);

        if ( delim_index == std::string_view::npos )
        {
            string_views.emplace_back(current_sv);
            break;
        }

        string_views.emplace_back(current_sv.substr(0, delim_index));

        current_sv = current_sv.substr(delim_index + delim_width);
    }


    return string_views;
}

std::size_t hash(const std::string_view &step)
{
    std::size_t current_value = 0;

    for ( auto c : step )
    {
        current_value += static_cast<std::size_t>(c);
        current_value *= 17;
        current_value %= 256;
    }

    return current_value;
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

    if ( ! std::getline(file, current_line ) )
    {
        throw "Couldn't read the input";
    }

    auto init_sequence = split_all(current_line, ",");


    std::array<HashMapBucket, HASHMAP_SIZE> HASHMAP;

    std::size_t sum = 0;
    for ( auto &s : init_sequence )
    {

        // Get the label
        auto label_end = std::find_if(s.begin(), s.end(), [] (auto c) {
                return ! std::isalpha(c);
        });

        if ( label_end == s.end() ) {
            std::cout << "No label found\n";
        }


        auto label_end_idx = std::distance(s.begin(), label_end);

        if ( label_end_idx + 1 > s.length() ) throw "The instruction is invalid";



        auto label = s.substr(0, label_end_idx) ;

        char instruction = s[label_end_idx];

        std::string_view rest = s.substr(label_end_idx+1);

        std::size_t box = hash(label);

        if ( instruction == '=' )
        {
            auto lens_strength = std::stoi(std::string{rest});

            auto &list = HASHMAP[box];

            // Find it
            auto existing_entry = std::find_if(list.begin(), list.end(),
                    [&label] (const auto &p) {
                        return p.first == label;
                    });

            if ( existing_entry != list.end() )
            {
                (*existing_entry).second = lens_strength;
            } else {
                // Emplace it if not found
                list.emplace_back(std::make_pair(std::string{label}, lens_strength));
            }

        } else if (
            instruction == '-'
        ) {
            HASHMAP[box].remove_if([&label] (auto &p) {
                    return p.first == label;
            });
        } else {
            std::cout << "Unknown instruction\n";
        }

        std::size_t hash_result = hash(s);
        sum += hash_result;
    }


    std::size_t focusing_power = 0;

    for ( std::size_t box = 0; box < 256; box++ )
    {

        auto &list = HASHMAP[box];

        auto list_iter = list.begin();
        auto end = list.end();

        while ( list_iter != end )
        {
            auto slot = std::distance(list.begin(), list_iter);

            focusing_power += (box+1) * (slot+1) * (*list_iter).second;

            list_iter++;
        }


    }




    std::cout << std::format("Result: {}\n", sum);
    std::cout << std::format("Focusing Power: {}\n", focusing_power);

    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

