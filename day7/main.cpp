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

#include "Clock.hpp"


#define TEST_TRUE(val) assert((val) == true)


static constexpr std::array<char, 14> CARD_ORDER{
    'J', // First element is always a joker, replace with an X if using J as jack
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'T',
    'Q',
    'K',
    'A',
    'X'
};


struct Play {
    uint8_t hand[5]{0};
    int bid{0};


    int card_score() const
    {
        uint8_t occurences[15]{0};

        uint8_t num_equal_tuples[6]{0};



        std::size_t num_pairs = 0;
        std::size_t num_triples = 0;
        std::size_t num_quadruples = 0;

        for ( auto c : hand ) {

            if ( c >= 15 ) throw "Hand is not initialized properly";

            occurences[c]++;
        }

        // Replace jokers with the most frequent card
        uint8_t num_jokers = occurences[0];
        auto max_occur = std::max_element(std::begin(occurences) + 1, std::end(occurences));
        *max_occur += num_jokers;
        occurences[0] = 0;


        for ( std::size_t i = 0; i < 15; ++i )
        {
            std::size_t num_occurences = occurences[i];
            num_equal_tuples[num_occurences]++;
        }

        if ( num_equal_tuples[5] == 1 ) return 6;
        else if ( num_equal_tuples[4] == 1 ) return 5;
        else if ( num_equal_tuples[3] == 1 && num_equal_tuples[2] == 1) return 4;
        else if ( num_equal_tuples[3] == 1 ) return 3;
        else if ( num_equal_tuples[2] == 2 ) return 2;
        else if ( num_equal_tuples[2] == 1 ) return 1;

        return 0;
    }

    int lexicographic_comparison(const Play &other) const
    {
        for ( std::size_t i = 0; i < 5; i++ )
        {
            uint8_t my_card = hand[i];
            uint8_t other_card = other.hand[i];

            if ( my_card < other_card ) return -1;
            else if ( my_card > other_card ) return 1;
        }

        return 0;
    }

    friend std::basic_ostream<char> &operator<<(std::basic_ostream<char> &stream, const Play &play );

    int compare(const Play &other) const
    {
        auto my_card_score = card_score();
        auto other_card_score = other.card_score();

        int result = 0;

        if ( my_card_score > other_card_score ) result =  1;
        else if ( my_card_score < other_card_score ) result =  -1;
        else result =  lexicographic_comparison(other);

        return result;
    }
};

std::basic_ostream<char> &operator<<(std::basic_ostream<char> &stream, const Play &play )
{
    for ( std::size_t i = 0; i < 5; i++ )
    {
        std::cout << CARD_ORDER[play.hand[i]];
    }

    stream << "\t";

    stream << play.bid;

    return stream;
}


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


uint8_t char_to_card_value(char c)
{
    auto pos = std::find(CARD_ORDER.begin(), CARD_ORDER.end(), c);

    auto val = pos - CARD_ORDER.data();

    return val;

}

Play get_line(const std::string &line)
{

    Play result{};


    std::istringstream ss{line};

    std::string hand_str{};
    std::string bid_str{};

    ss >> hand_str >> bid_str;

    for ( std::size_t i = 0; i < 5; i++ )
    {
        result.hand[i] = char_to_card_value(hand_str[i]);
    }

    result.bid = std::stoi(bid_str);

    return result;

}


using CardOrder = std::array<char, 14>;


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

    std::vector<Play> plays{};

    while ( std::getline(file, current_line) )
    {
        plays.emplace_back(get_line(current_line));
    }


    std::cout << (plays[1].compare(plays[2])) << std::endl;

    std::sort(plays.begin(), plays.end(), [] (const Play &first, const Play &second) -> bool {
            return ((-1) * first.compare(second)) > 0;
        });


    std::size_t sum = 0;

    for ( std::size_t i = 0; i < plays.size(); i++)
    {
        sum += plays[i].bid * (i+1);
        std::cout << plays[i] << ">> " << sum << "\n";
    }

    clock.stop();
    std::size_t duration_microseconds = clock.elapsed();


    std::cout << std::format("Resulting sum of bid and rank products: {}\n", sum);
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

