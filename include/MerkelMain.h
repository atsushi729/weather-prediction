#ifndef MERKELMAIN_H
#define MERKELMAIN_H

#include <string>
#include <vector>
#include "Candlestick.h"

class MerkelMain {
public:
    MerkelMain();
    void init();

private:
    std::string currentTime;
    std::vector<std::vector<std::string>> csvData; // CSVデータを保持

    void printMenu();
    void printHelp();
    void computeAndDisplayCandlestickData();

    int getUserOption();
    void processUserOption(int userOption);
};

#endif // MERKELMAIN_H
