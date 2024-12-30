#ifndef CSVREADER_H
#define CSVREADER_H

#include <vector>
#include <string>

class CSVReader {
public:
    // Read CSV file and return data as a 2D vector
    static std::vector<std::vector<std::string>> readCSV(const std::string& filename);

    // Tokenise a string based on a delimiter
    static std::vector<std::string> tokenise(const std::string& str, char delimiter);
};

#endif // CSVREADER_H
