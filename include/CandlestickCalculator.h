#ifndef CANDLESTICKCALCULATOR_H
#define CANDLESTICKCALCULATOR_H

#include "Candlestick.h"
#include <vector>
#include <string>

class CandlestickCalculator {
public:
    // CSVデータと国コードを受け取り、ローソク足データのベクターを返す
    static std::vector<Candlestick> computeCandlestickData(const std::vector<std::vector<std::string>>& csvData, const std::string& countryCode);
};

#endif // CANDLESTICKCALCULATOR_H
