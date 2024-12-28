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

    void printMenu();
    void printHelp();
    void printMarketStats();
    void enterAsk();
    void enterBid();
    void printWallet();
    void gotoNextTimeframe();

    int getUserOption();
    void processUserOption(int userOption);

    // 新規追加: ローソク足データの計算と表示
    void computeAndDisplayCandlestickData();
};

#endif // MERKELMAIN_H
