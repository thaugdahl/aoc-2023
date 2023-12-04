#include <cmath>
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

std::vector<int> extract_numbers(std::string_view sv)
{

    std::string number_string{sv};
    std::istringstream ss{number_string};


    std::istream_iterator<int> numbers_iter{ss};
    std::istream_iterator<int> end{};

    std::vector<int> result{};

    while ( numbers_iter != end ) {

        result.emplace_back(*numbers_iter);

        ++numbers_iter;
    }

    return result;
}

int num_card_matches(std::string_view card)
{

    int res = 0;

    auto colon_idx = card.find(':');
    auto bar_idx = card.find('|');

    if ( colon_idx == std::string::npos || bar_idx == std::string::npos ) {
        return 0;
    }

    auto winning_numbers_sv = card.substr(colon_idx + 1, bar_idx);
    auto numbers_to_match_sv = card.substr(bar_idx + 1);

    auto winning_numbers = extract_numbers(winning_numbers_sv);
    auto matching_numbers = extract_numbers(numbers_to_match_sv);

    std::size_t matches_found = 0;
    for ( auto x : matching_numbers ) {
        if ( std::find(winning_numbers.begin(), winning_numbers.end(), x ) != winning_numbers.end() ) {
            matches_found++;
        }
    }

    return matches_found;

}

int score_card_part1(std::string_view card)
{

    int matches_found = num_card_matches(card);

    if ( matches_found == 0 ) return 0;
    else {
        return std::pow(2, matches_found - 1);

    }

}


int score_card_part2(std::string_view card)
{

    int res = num_card_matches(card);

    return res;
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

    int total_score = 0;


    // while ( std::getline(file, current_line) ) {
    //     int score = score_card_part1(std::string_view{current_line});
    //     total_score += score;
    // }

    // std::cout << score_card_part1("Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53") << std::endl;
    // std::cout << score_card_part1("Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19") << std::endl;
    // std::cout << score_card_part1("Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1") << std::endl;

    std::vector<std::string> cards_raw{};


    while ( std::getline(file, current_line) ) {
        cards_raw.emplace_back(std::move(current_line) );
    }

    std::size_t num_cards = cards_raw.size();

    std::vector<int> num_cards_held{};
    num_cards_held.resize(num_cards, 1);


    bool new_cards_won = true;

    int cards = 1;


    std::vector<int> scores{};
    scores.resize(num_cards);

    for ( std::size_t i = 0; i < num_cards; i++ ) {
        scores[i] = score_card_part2(cards_raw[i]);
    }

    for ( std::size_t i = 0; i < num_cards; i++ ) {

        auto score = scores[i];

        for ( int j = 1; j <= score; ++j )
        {
            num_cards_held[i+j] += num_cards_held[i];
        }


    }

    // std::cout << std::accumulate(num_cards_held.begin(), num_cards_held.end(), 0) << "\n";
    std::cout << std::accumulate(num_cards_held.begin(), num_cards_held.end(), 0) << "\n";



    std::cout << cards << std::endl;

    clock.stop();
    uint64_t duration_microseconds = clock.elapsed();

    std::cout << "Score " << total_score << std::endl;
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

