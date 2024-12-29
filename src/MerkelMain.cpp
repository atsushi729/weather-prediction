#include "MerkelMain.h"

#include <iostream>
#include <exception>
#include <iomanip>
#include <limits>
#include <cmath>       // std::floor
#include <algorithm>   // std::min / std::max
#include <sstream>     // std::ostringstream

// CSVReader, CandlestickCalculator, Candlestick
#include "CSVReader.h"
#include "CandlestickCalculator.h"
#include "Candlestick.h"

// ─────────────────────────────────────────────
// 列幅・縦軸幅等の設定
// ─────────────────────────────────────────────
/** 
 * @brief キャンドル1本あたりの横幅 (ASCIIアートのカラム幅)  
 *        例: 4 なら1本を4文字ぶんで描画 
 */
static const int COLUMN_WIDTH = 5;

/**
 * @brief 縦軸に表示するラベル部分の幅  
 *        例: 8 なら「   23.0」など右寄せするのにちょうどよい
 */
static const int Y_LABEL_WIDTH = 8;  

/**
 * @brief 固定幅で文字列を返すユーティリティ関数
 * @param[in] str   変換対象の文字列
 * @param[in] width 固定幅
 * @return 幅を超える場合は切り取り、未満ならスペースで埋める
 */
static std::string fixedWidth(const std::string& str, int width)
{
    if ((int)str.size() >= width) {
        // 指定幅より大きければ切り取る
        return str.substr(0, width);
    }
    // 小さい場合はスペースでパディング
    return str + std::string(width - str.size(), ' ');
}

/**
 * @brief コンストラクタ: CSVファイルを読み込む
 */
MerkelMain::MerkelMain()
{
    std::string filename = "../weather_data.csv"; // 必要に応じてパスを変更
    csvData = CSVReader::readCSV(filename);
    if (csvData.empty()) {
        std::cerr << "Error: Failed to read CSV data from " << filename << std::endl;
        // 必要に応じてエラー処理
    }
}

/**
 * @brief メインループ
 */
void MerkelMain::init()
{
    while (true)
    {
        printMenu();
        int input = getUserOption();
        if (input == -1) {
            std::cout << "Error reading input. Exiting." << std::endl;
            break;
        }
        if (input == 0) {
            std::cout << "Exiting application." << std::endl;
            break;
        }
        processUserOption(input);
    }
}

/**
 * @brief メニュー表示
 */
void MerkelMain::printMenu()
{
    std::cout << "1: Print help\n";
    std::cout << "2: Compute Candlestick Data\n";
    std::cout << "3: Plot Candlestick Data (Compute behind the scenes)\n";
    std::cout << "0: Exit\n";
    std::cout << "==============\n";
}

/**
 * @brief ヘルプ表示
 */
void MerkelMain::printHelp()
{
    std::cout << "Help - Your aim is to compute candlestick data from a fixed weather CSV file.\n";
    std::cout << "Select option 2 to compute candlestick data.\n";
    std::cout << "Select option 3 to compute + immediately plot candlestick data.\n";
}

/**
 * @brief ユーザー入力を取得し int に変換
 */
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

/**
 * @brief メニュー選択に応じた処理
 */
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
            // 「2: Compute」 -> 計算+一覧表示
            computeAndDisplayCandlestickData();
            break;
        case 3:
            // 「3: Plot」 -> 裏で計算しつつ即プロット
            computeCandlestickAndPlot();
            break;
        default:
            std::cout << "Invalid choice. Choose a valid option." << std::endl;
            break;
    }
}

//=============================================================================
// (1) case 2: ローソク足の計算 + 一覧表示
//=============================================================================
void MerkelMain::computeAndDisplayCandlestickData()
{
    if (csvData.empty()) {
        std::cerr << "Error: No CSV data available to compute candlestick data." << std::endl;
        return;
    }

    std::string countryCode;
    std::cout << "Enter country code (e.g., GB): ";
    std::getline(std::cin, countryCode);

    if (countryCode.empty()) {
        std::cerr << "Error: Country code cannot be empty." << std::endl;
        return;
    }

    // ローソク足データを計算
    std::vector<Candlestick> candles =
        CandlestickCalculator::computeCandlestickData(csvData, countryCode);

    if (candles.empty()) {
        std::cerr << "No candlestick data computed. "
                  << "Check if the country code is correct and data is available.\n";
        return;
    }

    // 計算結果をクラスメンバに格納
    lastComputedCandles = candles;

    // 以下、計算結果のテーブル表示
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
        count++;
        if (count >= 40) break; // 40個で打ち切り
    }
    if (count < 40) {
        std::cout << "Warning: Only " << count << " years of data available for country code "
                  << countryCode << "." << std::endl;
    }
}

//=============================================================================
// (2) case 3: ローソク足の計算（裏で実施）＋即時テキストプロット
//=============================================================================
void MerkelMain::computeCandlestickAndPlot()
{
    if (csvData.empty()) {
        std::cerr << "Error: No CSV data available to compute candlestick data." << std::endl;
        return;
    }

    std::string countryCode;
    std::cout << "Enter country code (e.g., GB): ";
    std::getline(std::cin, countryCode);

    if (countryCode.empty()) {
        std::cerr << "Error: Country code cannot be empty." << std::endl;
        return;
    }

    // Candlestickを計算
    std::vector<Candlestick> candles =
        CandlestickCalculator::computeCandlestickData(csvData, countryCode);

    if (candles.empty()) {
        std::cerr << "No candlestick data computed. "
                  << "Check if the country code is correct and data is available.\n";
        return;
    }

    // 計算結果を保持
    lastComputedCandles = candles;

    // テキストベースでのプロット
    std::cout << "\n=== Text-based Candlestick Chart (up to 40 candles) ===\n";
    plotCandlestickData(lastComputedCandles, 40);
}

//=============================================================================
// (3) 縦軸ラベル付き：テキストベースのキャンドルスティック描画
//=============================================================================
void MerkelMain::plotCandlestickData(const std::vector<Candlestick>& candles, int maxDisplayCount)
{
    if (candles.empty()) {
        std::cout << "No data to plot." << std::endl;
        return;
    }
    int displayCount = std::min((int)candles.size(), maxDisplayCount);
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
    double scale = (double)chartHeight / range;
    // グラフ本体 (上から下へ)
    for (int row = chartHeight; row >= 0; --row)
    {
        // Y軸ラベルを出力
        double actualValue = minLow + (row / scale);
        std::ostringstream yLabelSS;
        yLabelSS << std::fixed << std::setprecision(1) << actualValue;
        std::string yLabelStr = yLabelSS.str();
        if ((int)yLabelStr.size() < Y_LABEL_WIDTH) {
            yLabelStr = std::string(Y_LABEL_WIDTH - yLabelStr.size(), ' ') + yLabelStr;
        }
        std::cout << yLabelStr << " | ";
        // 各キャンドルを処理
        for (int i = 0; i < displayCount; ++i)
        {
            double open = candles[i].open;
            double close = candles[i].close;
            double high = candles[i].high;
            double low = candles[i].low;
            int scaledHigh = (int)std::floor((high - minLow) * scale);
            int scaledLow = (int)std::floor((low - minLow) * scale);
            int scaledOpen = (int)std::floor((open - minLow) * scale);
            int scaledClose = (int)std::floor((close - minLow) * scale);
            int boxTop = std::max(scaledOpen, scaledClose);
            int boxBottom = std::min(scaledOpen, scaledClose);
            if (row > scaledHigh || row < scaledLow) {
                std::cout << "     ";  // COLUMN_WIDTH = 5 のため
                continue;
            }
            bool isWickRange = (row <= scaledHigh && row >= boxTop) ||
                              (row <= boxBottom && row >= scaledLow);
            bool isBoxRange = (row <= boxTop && row >= boxBottom);
            if (row == scaledHigh) {
                std::cout << "  ^  ";
            }
            else if (row == scaledLow) {
                std::cout << "  v  ";
            }
            else if (isBoxRange) {
                // ボックス部分の色を設定
                if (close >= open) {  // 陽線
                    std::cout << "\033[32m";  // 緑色
                } else {            // 陰線
                    std::cout << "\033[31m";  // 赤色
                }
                std::cout << " [#] ";  // 中央揃えに変更
                std::cout << "\033[0m";  // 色をリセット
            }
            else if (isWickRange) {
                std::cout << "  |  ";
            }
            else {
                std::cout << "     ";
            }
        }
        std::cout << std::endl;
    }
    // X軸のラベル (年)
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