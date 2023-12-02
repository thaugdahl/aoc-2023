#include <algorithm>
#include <cctype>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <fstream>
#include <filesystem>
#include <utility>
#include <array>
#include <sstream>
#include <string_view>


#define FIRST_FOUND_MASK 0x1
#define SECOND_FOUND_MASK 0x2

#define FIRST_FOUND(flags) (flags & FIRST_FOUND_MASK)
#define SECOND_FOUND(flags) (flags & SECOND_FOUND_MASK)


#define LOGGER_ENABLED true

const std::string get_input_filename(int argc, char **argv)
{
    if ( argc < 2 ) {
        return "input.txt";
    } else {
        return std::string(argv[1]);
    }
}

std::pair<uint8_t, uint8_t> get_digits_on_line(const std::string_view &line)
{

    uint8_t first = 0, second = 0;

    uint8_t flags = 0;


    for ( const char &c : line )
    {
        bool is_digit = std::isdigit(c);

        if ( ! is_digit ) continue;

        if ( ! (FIRST_FOUND_MASK & flags) ) {
            first = static_cast<uint8_t>(c - '0');
            flags |= FIRST_FOUND_MASK;
        } else {
            second = static_cast<uint8_t>(c - '0');
            flags |= SECOND_FOUND_MASK;
        }
    }

    if ( ! (SECOND_FOUND_MASK & flags ) ) {
        second = first;
    }


    return {first,second};
}


static constexpr std::array<std::pair<std::string_view, uint8_t>, 9> DIGITS = {
    std::make_pair("one", 1),
	std::make_pair("two", 2),
	std::make_pair("three", 3),
	std::make_pair("four", 4),
	std::make_pair("five", 5),
	std::make_pair("six", 6),
	std::make_pair("seven", 7),
	std::make_pair("eight", 8),
	std::make_pair("nine", 9),
};


bool is_partial_match(const std::string_view &view)
{

    bool partial_exists = false;

    if ( view.empty() ) return false;

    for ( auto ref : DIGITS )
    {
       partial_exists = view == ref.first.substr(0, view.length());
       if ( partial_exists ) return true;
    }

    return partial_exists;
}


std::optional<int> getDigit(const std::string_view &candidate)
{
    for ( auto &x : DIGITS )
    {
        if ( candidate.length() != x.first.length() ) continue;

        if ( candidate == x.first ) return std::optional<int>{x.second};
    }

    return std::optional<int>{};
}

enum CandidateState {
    INVALID,
    PARTIAL,
    DIGIT
};

CandidateState evaluate_candidate(const std::string_view &candidate)
{
    if ( std::isdigit(candidate[0]) ) {
        return CandidateState::DIGIT;
    }

    if ( ! is_partial_match ( candidate ) ) {
        return CandidateState::INVALID;
    } else  {
        return CandidateState::PARTIAL;
    }

}


struct LineScanner {
    std::size_t begin;
    std::size_t end;
};


std::optional<int> scan_invalid(std::string_view &line, LineScanner &scanner)
{
    scanner.begin++;
    scanner.end = scanner.begin + 1;

    return std::optional<int>{};
}

std::optional<int> scan_partial(std::string_view &line, LineScanner &scanner)
{
    scanner.end++;

    auto digit = getDigit(line);

    if ( digit.has_value() )
        scanner.begin++;

    return digit;

}

std::optional<int> scan_digit(std::string_view &line, LineScanner &scanner)
{
    int digit = line[0] - '0';
    scanner.begin++;
    scanner.end = scanner.begin + 1;

    return std::optional<int>{digit};
}


std::pair<std::optional<int>, std::string_view> scan_next(std::string_view line, LineScanner &scanner)
{
    auto result = std::optional<int>{};

    std::string_view candidate{line};

    candidate = line.substr(scanner.begin, scanner.end - scanner.begin);


    auto state = evaluate_candidate(candidate);

    if ( state == INVALID ) {
        scan_invalid(candidate, scanner);
        return { std::optional<int>{}, line.substr(scanner.begin) };
    } else if ( state == DIGIT ) {
        auto potential = scan_digit(candidate, scanner);
        if ( potential.has_value() ) return {std::optional<int>{potential.value()}, line.substr(scanner.begin)};
    } else {
        auto potential = scan_partial(candidate, scanner);
        if ( potential.has_value() ) return {std::optional<int>{potential.value()}, line.substr(scanner.begin)};
    }

    return {result, candidate};
}

std::pair<uint8_t, uint8_t> get_digits_on_line_part_two(const std::string_view &line)
{
    std::string_view working_copy{line};

    int first = -1, second = -1;

    LineScanner scanner{0, 1};

    while ( scanner.begin <= scanner.end && scanner.begin < line.length() ) {
        auto res = scan_next(line, scanner);

        if ( scanner.end > line.length() ) {
            scanner.begin++;
        }


        if ( res.first.has_value() ) {
            int digit = res.first.value();

            if ( first < 0 ) first = digit;
            else { second = digit; }

        }
    }


    if ( second < 0 ) {
        second = first;
    }

    return {first,second};
}

int main(int argc, char **argv)
{
    const std::filesystem::path inputFile = get_input_filename(argc, argv);

    if ( ! std::filesystem::exists(inputFile) ) {
        std::cerr << "Input file does not exist\n";
        exit(EXIT_FAILURE);
    };

    std::ifstream file{inputFile};

    int sum = 0;

    std::string current_line{};

    auto start_time = std::chrono::high_resolution_clock::now();

    while ( std::getline(file, current_line) ) {
        auto [first_digit, second_digit] = get_digits_on_line_part_two(current_line);

        std::cout << current_line << ": " << static_cast<int>(first_digit) << ", " << static_cast<int>(second_digit) << "\n";

        sum += first_digit * 10 + second_digit;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    file.close();

    std::cout << "Final sum: " << sum << "\n";
    std::cout << "Runtime: " << duration_microseconds / 1e6 << " seconds ( " << duration_microseconds << " microseconds )\n";
}

