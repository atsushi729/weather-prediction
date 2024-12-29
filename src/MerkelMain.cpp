#include "MerkelMain.h"

#include <iostream>
#include <exception>
#include <iomanip>
#include <limits>
#include <cmath>       // std::floor
#include <algorithm>   // std::min / std::max
#include <sstream>     // std::ostringstream

#include "CSVReader.h"
#include "CandlestickCalculator.h"
#include "Candlestick.h"

namespace {
    /** 
     * @brief キャンドル1本あたりの横幅 (ASCIIアートのカラム幅)  
     */
    const int COLUMN_WIDTH = 5;

    /**
     * @brief 縦軸に表示するラベル部分の幅
     */
    const int Y_LABEL_WIDTH = 8;  

    /**
     * @brief 固定幅で文字列を返すユーティリティ関数
     */
    std::string fixedWidth(const std::string& str, int width)
    {
        if (static_cast<int>(str.size()) >= width) {
            return str.substr(0, width);
        }
        return str + std::string(width - str.size(), ' ');
    }
}

// ─────────────────────────────────────────────
// コンストラクタ
// ─────────────────────────────────────────────
MerkelMain::MerkelMain()
{
    std::string filename = "../weather_data.csv"; // 必要に応じてパスを変更
    csvData = CSVReader::readCSV(filename);
    if (csvData.empty()) {
        std::cerr << "Error: Failed to read CSV data from " << filename << std::endl;
        // 必要に応じてエラー処理
    }
}

// ─────────────────────────────────────────────
// メインループ
// ─────────────────────────────────────────────
void MerkelMain::init()
{
    while (true)
    {
        printMenu();
        int selectedOption = getUserOption();
        if (selectedOption == -1) {
            std::cout << "Error reading input. Exiting." << std::endl;
            break;
        }
        if (selectedOption == 0) {
            std::cout << "Exiting application." << std::endl;
            break;
        }
        processUserOption(selectedOption);
    }
}

// ─────────────────────────────────────────────
// メニュー表示
// ─────────────────────────────────────────────
void MerkelMain::printMenu()
{
    std::cout << "1: Print help\n";
    std::cout << "2: Compute Candlestick Data\n";
    std::cout << "3: Plot Candlestick Data (Compute behind the scenes)\n";
    std::cout << "0: Exit\n";
    std::cout << "==============\n";
}

// ─────────────────────────────────────────────
// ヘルプ表示
// ─────────────────────────────────────────────
void MerkelMain::printHelp()
{
    std::cout << "Help - Your aim is to compute candlestick data from a fixed weather CSV file.\n";
    std::cout << "Select option 2 to compute candlestick data.\n";
    std::cout << "Select option 3 to compute + immediately plot candlestick data.\n";
}

// ─────────────────────────────────────────────
// 入力取得
// ─────────────────────────────────────────────
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in option number (0 to exit): ";
    std::getline(std::cin, line);
    try {
        userOption = std::stoi(line);
    }
    catch (const std::exception&) {
        userOption = -1; // エラーを示す
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

// ─────────────────────────────────────────────
// 入力値を元に処理振り分け
// ─────────────────────────────────────────────
void MerkelMain::processUserOption(int userOption)
{
    if (userOption == -1)
    {
        std::cout << "Invalid input. Please enter a number." << std::endl;
        return;
    }

    switch(userOption) {
        case 0:
            // Exit
            break;
        case 1:
            printHelp();
            break;
        case 2:
            // (1) ローソク足の計算 + 一覧表示
            computeAndDisplayCandlestickData();
            break;
        case 3:
            // (2) ローソク足の計算 ＋ 即時テキスト描画
            computeCandlestickAndPlot();
            break;
        default:
            std::cout << "Invalid choice. Choose a valid option." << std::endl;
            break;
    }
}

// ─────────────────────────────────────────────
// ユーザーから国コードを取得
// ─────────────────────────────────────────────
std::string MerkelMain::getCountryCodeFromUser()
{
    std::string countryCode;
    std::cout << "Enter country code (e.g., GB): ";
    std::getline(std::cin, countryCode);
    if (countryCode.empty()) {
        std::cerr << "Error: Country code cannot be empty." << std::endl;
    }
    return countryCode;
}

// ─────────────────────────────────────────────
// CSVデータから Candlestick を計算
// ─────────────────────────────────────────────
std::vector<Candlestick> MerkelMain::computeCandlestickDataForCountry(const std::string& countryCode)
{
    if (csvData.empty()) {
        std::cerr << "Error: No CSV data available to compute candlestick data." << std::endl;
        return {};
    }
    return CandlestickCalculator::computeCandlestickData(csvData, countryCode);
}

// ─────────────────────────────────────────────
// (1) ローソク足の計算 + 一覧表示
// ─────────────────────────────────────────────
void MerkelMain::computeAndDisplayCandlestickData()
{
    std::string countryCode = getCountryCodeFromUser();
    if (countryCode.empty()) {
        return; // 入力エラー
    }

    std::vector<Candlestick> candles = computeCandlestickDataForCountry(countryCode);
    if (candles.empty()) {
        std::cerr << "No candlestick data computed. "
                  << "Check if the country code is correct and data is available.\n";
        return;
    }

    lastComputedCandles = candles;

    std::cout << "Candle data : " << countryCode << std::endl;
    std::cout << "Date\tOpen\tHigh\tLow\tClose\n";

    int count = 0;
    for (const auto& candle : candles) {
        std::string year = candle.date.substr(0, 4);
        std::cout << year << "\t"
                  << std::fixed << std::setprecision(3) << candle.open << "\t"
                  << candle.high << "\t"
                  << candle.low << "\t"
                  << candle.close << std::endl;

        if (++count >= 40) break;
    }

    if (count < 40) {
        std::cout << "Warning: Only " << count << " years of data available for country code "
                  << countryCode << "." << std::endl;
    }
}

// ─────────────────────────────────────────────
// (2) ローソク足の計算 ＋ 即時テキスト描画
// ─────────────────────────────────────────────
void MerkelMain::computeCandlestickAndPlot()
{
    std::string countryCode = getCountryCodeFromUser();
    if (countryCode.empty()) {
        return;
    }

    std::vector<Candlestick> candles = computeCandlestickDataForCountry(countryCode);
    if (candles.empty()) {
        std::cerr << "No candlestick data computed. "
                  << "Check if the country code is correct and data is available.\n";
        return;
    }

    lastComputedCandles = candles;

    std::cout << "\n=== Text-based Candlestick Chart (up to 40 candles) ===\n";
    plotCandlestickData(lastComputedCandles, 40);
}

// ─────────────────────────────────────────────
// (3) テキストでキャンドルスティックを描画
// ─────────────────────────────────────────────
void MerkelMain::plotCandlestickData(const std::vector<Candlestick>& candles, int maxDisplayCount)
{
    if (candles.empty()) {
        std::cout << "No data to plot." << std::endl;
        return;
    }
    int displayCount = std::min(static_cast<int>(candles.size()), maxDisplayCount);

    // minLow, maxHigh を算出
    double minLow = std::numeric_limits<double>::max();
    double maxHigh = std::numeric_limits<double>::lowest();
    for (int i = 0; i < displayCount; ++i) {
        minLow = std::min(minLow, candles[i].low);
        maxHigh = std::max(maxHigh, candles[i].high);
    }

    const int chartHeight = 20;
    double range = maxHigh - minLow;
    if (range <= 0.0) {
        std::cout << "Invalid range for plotting." << std::endl;
        return;
    }
    double scale = static_cast<double>(chartHeight) / range;

    // グラフ本体 (上から下へ)
    for (int row = chartHeight; row >= 0; --row)
    {
        // 左端にY軸ラベル
        double actualValue = minLow + (row / scale);
        std::ostringstream yLabelSS;
        yLabelSS << std::fixed << std::setprecision(1) << actualValue;
        std::string yLabelStr = yLabelSS.str();
        if (static_cast<int>(yLabelStr.size()) < Y_LABEL_WIDTH) {
            yLabelStr = std::string(Y_LABEL_WIDTH - yLabelStr.size(), ' ') + yLabelStr;
        }
        std::cout << yLabelStr << " | ";

        // キャンドル描画
        for (int i = 0; i < displayCount; ++i)
        {
            double open  = candles[i].open;
            double close = candles[i].close;
            double high  = candles[i].high;
            double low   = candles[i].low;

            int scaledHigh  = static_cast<int>(std::floor((high  - minLow) * scale));
            int scaledLow   = static_cast<int>(std::floor((low   - minLow) * scale));
            int scaledOpen  = static_cast<int>(std::floor((open  - minLow) * scale));
            int scaledClose = static_cast<int>(std::floor((close - minLow) * scale));

            int boxTop    = std::max(scaledOpen, scaledClose);
            int boxBottom = std::min(scaledOpen, scaledClose);

            // ローソク足描画範囲外
            if (row > scaledHigh || row < scaledLow) {
                std::cout << std::string(COLUMN_WIDTH, ' ');
                continue;
            }

            bool isWickRange = ((row <= scaledHigh) && (row >= boxTop)) ||
                               ((row <= boxBottom) && (row >= scaledLow));
            bool isBoxRange  = (row <= boxTop && row >= boxBottom);

            if (row == scaledHigh) {
                // 上ヒゲの先端
                std::cout << "  ^  ";
            }
            else if (row == scaledLow) {
                // 下ヒゲの先端
                std::cout << "  v  ";
            }
            else if (isBoxRange) {
                // ボックス本体
                if (close >= open) { // 陽線
                    std::cout << "\033[32m";
                } else {             // 陰線
                    std::cout << "\033[31m";
                }
                std::cout << " [#] ";
                std::cout << "\033[0m"; // 色リセット
            }
            else if (isWickRange) {
                // ヒゲ部分
                std::cout << "  |  ";
            }
            else {
                std::cout << std::string(COLUMN_WIDTH, ' ');
            }
        }
        std::cout << std::endl;
    }

    // X軸ラベル (年)
    std::cout << std::string(Y_LABEL_WIDTH, ' ') << "  ";
    for (int i = 0; i < displayCount; ++i) {
        std::string yearStr = "----";
        if (candles[i].date.size() >= 4) {
            yearStr = candles[i].date.substr(0, 4);
        }
        std::cout << fixedWidth(yearStr, COLUMN_WIDTH);
    }
    std::cout << std::endl;
}
