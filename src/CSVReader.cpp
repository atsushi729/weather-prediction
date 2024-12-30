#include "CSVReader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Function to read CSV data from a file
std::vector<std::vector<std::string>> CSVReader::readCSV(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream infile(filename); // Open the CSV file
    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return data;
    }

    std::string line;
    // Read the file line by line
    while (std::getline(infile, line)) {
        data.emplace_back(tokenise(line, ',')); // Split each line by comma and add to data
    }

    infile.close(); // Close the file
    return data; // Return the parsed CSV data
}

// Function to split a string by a given delimiter
std::vector<std::string> CSVReader::tokenise(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str); // Create a string stream from the input string
    std::string token;
    // Extract tokens separated by the delimiter
    while (std::getline(ss, token, delimiter)) {
        tokens.emplace_back(token); // Add each token to the tokens vector
    }
    return tokens; // Return the vector of tokens
}
