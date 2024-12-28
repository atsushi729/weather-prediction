#include "CandlestickCalculator.h"
#include <unordered_map>
#include <limits>
#include <sstream>
#include <algorithm>
#include <iostream>

struct TemperatureData {
    double sum;    // 年間の気温の合計
    int count;     // 気温データの数
    double high;   // 年間の最高気温
    double low;    // 年間の最低気温

    TemperatureData()
        : sum(0.0), count(0),
          high(std::numeric_limits<double>::lowest()),
          low(std::numeric_limits<double>::max()) {}
};

std::vector<Candlestick> CandlestickCalculator::computeCandlestickData(const std::vector<std::vector<std::string>>& csvData, const std::string& countryCode) {
    std::vector<Candlestick> candlesticks;

    if (csvData.empty()) {
        std::cerr << "Error: CSV data is empty." << std::endl;
        return candlesticks;
    }

    // ヘッダーから国コードの列インデックスを特定
    const std::vector<std::string>& header = csvData[0];
    std::string targetColumn = countryCode + "_temperature";
    int targetIndex = -1;
    for (size_t i = 0; i < header.size(); ++i) {
        if (header[i] == targetColumn) {
            targetIndex = static_cast<int>(i);
            break;
        }
    }

    if (targetIndex == -1) {
        std::cerr << "Error: Country code " << countryCode << " not found in headers." << std::endl;
        return candlesticks;
    }

    // 年ごとの気温データを格納するマップ
    std::unordered_map<int, TemperatureData> yearlyData;

    // データ行を処理
    for (size_t i = 1; i < csvData.size(); ++i) { // ヘッダーをスキップ
        const std::vector<std::string>& row = csvData[i];
        if (static_cast<int>(row.size()) <= targetIndex) {
            continue; // 指定された列が存在しない場合
        }

        // タイムスタンプから年を抽出
        std::string timestamp = row[0];
        if (timestamp.size() < 4) {
            continue; // 無効なタイムスタンプ
        }
        int year = 0;
        try {
            year = std::stoi(timestamp.substr(0, 4));
        } catch (...) {
            continue; // 無効な年
        }

        // 気温データを取得
        double temperature = 0.0;
        try {
            temperature = std::stod(row[targetIndex]);
        } catch (...) {
            continue; // 無効な気温データ
        }

        // 年ごとのデータを更新
        TemperatureData& data = yearlyData[year];
        data.sum += temperature;
        data.count += 1;
        if (temperature > data.high) {
            data.high = temperature;
        }
        if (temperature < data.low) {
            data.low = temperature;
        }
    }

    // 年をソートするために年のリストを作成
    std::vector<int> years;
    for (const auto& pair : yearlyData) {
        years.push_back(pair.first);
    }
    std::sort(years.begin(), years.end());

    double previousAverage = 0.0;
    bool hasPrevious = false;

    for (size_t i = 0; i < years.size(); ++i) {
        int year = years[i];
        TemperatureData& data = yearlyData[year];
        if (data.count == 0) {
            continue; // データがない年はスキップ
        }

        double average = data.sum / data.count;
        double open = hasPrevious ? previousAverage : average; // 最初の年は自身の平均を使用
        double close = average;
        double high = data.high;
        double low = data.low;

        // 年を "YYYY" 形式で設定
        std::string yearStr = std::to_string(year);

        Candlestick candle(yearStr, open, high, low, close);
        candlesticks.push_back(candle);

        previousAverage = average;
        hasPrevious = true;
    }

    return candlesticks;
}
