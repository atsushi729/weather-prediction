#ifndef CSVREADER_H
#define CSVREADER_H

#include <vector>
#include <string>

class CSVReader {
public:
    // CSVファイルを読み込み、行ごとのセルを格納した2次元ベクターを返す
    static std::vector<std::vector<std::string>> readCSV(const std::string& filename);

    // 文字列を指定したデリミタで分割する
    static std::vector<std::string> tokenise(const std::string& str, char delimiter);
};

#endif // CSVREADER_H
