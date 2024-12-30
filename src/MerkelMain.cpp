#include "MerkelMain.h"

#include <iostream>
#include <exception>
#include <iomanip>
#include <limits>
#include <cmath>       // std::floor
#include <algorithm>   // std::min / std::max
#include <sstream>     // std::ostringstream
#include <map>

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
    std::cout << "4: Show Yearly Temperature Histogram\n";
    std::cout << "5: Predict Future Temperature (Linear Regression)\n";
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
            // (1) ヘルプ表示
            printHelp();
            break;
        case 2:
            // (2) ローソク足の計算 + 一覧表示
            computeAndDisplayCandlestickData();
            break;
        case 3:
            // (3) ローソク足の計算 ＋ 即時テキスト描画
            computeCandlestickAndPlot();
            break;
        case 4:
            // (4) 縦型のヒストグラム表示
            showYearlyHistogram();
            break;
        case 5:
            // (5) 将来の気温予測 (単回帰分析)
            predictFutureTemperature();
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


int MerkelMain::getDataTypeFromUser()
{
    // データタイプ選択
    std::cout << "1: Average Temperature\n"
              << "2: Max Temperature\n"
              << "3: Min Temperature\n"
              << ">> ";
    std::string line;
    std::getline(std::cin, line);
    int dataType = 0;
    try {
        dataType = std::stoi(line);
    } catch(...) {
        dataType = 1; // デフォルト
    }
    if (dataType < 1 || dataType > 3) dataType = 1;
    return dataType;
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

// ─────────────────────────────────────────────
// (4) 国コード + データタイプ を指定して、年ごとに集計した気温を縦型のヒストグラムで表示
// ─────────────────────────────────────────────
void MerkelMain::showYearlyHistogram()
{
    // (1) 国コード取得
    std::string countryCode = getCountryCodeFromUser();
    if (countryCode.empty()) {
        return; // 入力エラー時は中断
    }

    // (2) データタイプ選択
    int dataType = getDataTypeFromUser(); 
    // 1=平均, 2=最高, 3=最低

    // (3) CSV データの存在確認
    if (csvData.empty()) {
        std::cout << "CSVデータが空です。\n";
        return;
    }

    // ヘッダー行を取得
    const std::vector<std::string>& header = csvData[0];
    std::string targetColumn = countryCode + "_temperature";
    int targetIndex = -1;

    // ヘッダーから対象の列を探す
    for (size_t i = 0; i < header.size(); ++i) {
        if (header[i] == targetColumn) {
            targetIndex = static_cast<int>(i);
            break;
        }
    }

    if (targetIndex == -1) {
        std::cout << "指定された国コード「" << countryCode << "」に対応する列が見つかりません。\n";
        std::cout << "利用可能な国コードは以下の通りです:\n";
        for (size_t i = 1; i < header.size(); ++i) { // 0はタイムスタンプ
            // "AT_temperature" から "AT" を抽出
            size_t pos = header[i].find("_temperature");
            if (pos != std::string::npos) {
                std::string code = header[i].substr(0, pos);
                std::cout << "- " << code << "\n";
            }
        }
        return;
    }

    // 年ごとの気温データを格納するマップ
    std::map<int, std::vector<double>> yearToTemps;

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
        yearToTemps[year].push_back(temperature);
    }

    // (4) 年ごとの値を「平均 or 最高 or 最低」に集約
    //     これにより年 -> 1つの値 になる
    std::vector<std::pair<int, double>> yearlyData; // (year, temperature)

    for (const auto& kv : yearToTemps)
    {
        // kv.first = year, kv.second = vector<double>
        int year = kv.first;
        const std::vector<double>& temps = kv.second;

        if (temps.empty()) continue;

        // 集計
        double finalVal = 0.0;

        if (dataType == 1) {
            // 平均: sum / count
            double sum = 0.0;
            for (double t : temps) sum += t;
            finalVal = sum / temps.size();
        }
        else if (dataType == 2) {
            // 最高
            finalVal = *std::max_element(temps.begin(), temps.end());
        }
        else {
            // 最低
            finalVal = *std::min_element(temps.begin(), temps.end());
        }

        yearlyData.emplace_back(year, finalVal);
    }

    if (yearlyData.empty()) {
        std::cout << "\n指定された国コード「" << countryCode << "」に対応するデータが存在しません。\n";
        return;
    }

    // 年で昇順ソート
    std::sort(yearlyData.begin(), yearlyData.end(),
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) -> bool {
            return a.first < b.first;
        }
    );

    // (5) ヒストグラム用のスケーリング準備
    //     縦軸の最大値を探し、テキストベースで縦棒を描画する
    double maxVal = std::numeric_limits<double>::lowest();
    double minVal = std::numeric_limits<double>::max();
    for (const auto& p : yearlyData) {
        double val = p.second;
        if (val > maxVal) maxVal = val;
        if (val < minVal) minVal = val;
    }
    // 0 から始めたい場合は minVal = 0.0 としても良い

    double range = maxVal - minVal;
    if (range <= 0.0) {
        std::cout << "\nすべてのデータポイントが同じ値です。縦型ヒストグラムを描画できません。\n";
        return;
    }

    // 縦方向の高さ (適当に 20 行とする)
    const int chartHeight = 20;
    // スケーリング
    double scale = static_cast<double>(chartHeight) / range;

    // (6) 上から下に向かって一行ずつ描画
    std::cout << "\n===== Yearly "
              << ((dataType == 1) ? "Average" : (dataType == 2) ? "Max" : "Min")
              << " Temp for " << countryCode << " =====\n\n";

    // 縦軸ラベルの幅を確保 (例: 6文字)
    const int labelWidth = 6;

    // row: chartHeight ~ 0
    for (int row = chartHeight; row >= 0; --row)
    {
        // 縦軸ラベルを計算
        double currentVal = minVal + (range * row / chartHeight);
        // ラベルを固定幅で整形 (小数点以下1桁)
        std::ostringstream labelStream;
        labelStream << std::fixed << std::setprecision(1) << currentVal;
        std::string label = labelStream.str();

        // ラベルを右寄せで表示
        std::cout << std::setw(labelWidth) << label << " | ";

        // 各年分ループ
        for (size_t i = 0; i < yearlyData.size(); ++i)
        {
            double val = yearlyData[i].second;
            // スケール上での高さ
            int barHeight = static_cast<int>((val - minVal) * scale);

            // 現在の row がバー以上なら "#"、そうでなければ " "
            // ただしバーの下側は row == 0 が基底線なので
            if (row <= barHeight - 1) {
                // '#' 描画
                std::cout << "  #  ";
            } else {
                std::cout << "     ";
            }
        }
        std::cout << "\n";
    }

    // (7) X軸（年）ラベルを出力
    //     1行か2行にまとめて、年の下に印字する
    //     年が多いと崩れるので、行数が多い場合は工夫が必要です
    //     簡易実装として、1行だけに yearInt を書く

    // X軸のラベル前にスペースを追加 (ラベル幅 + ' | ')
    std::cout << std::setw(labelWidth + 2) << " " << " ";

    for (size_t i = 0; i < yearlyData.size(); ++i)
    {
        int yearInt = yearlyData[i].first;
        // 5文字幅くらい確保
        std::ostringstream oss;
        oss << std::setw(5) << yearInt;
        std::cout << oss.str();
    }
    std::cout << "\n";
}

// ─────────────────────────────────────────────
// (5) 将来の気温予測 (単回帰分析)
// ─────────────────────────────────────────────
void MerkelMain::predictFutureTemperature()
{
    std::string countryCode = getCountryCodeFromUser();
    if (countryCode.empty()) {
        return; // 入力エラー
    }

    // CSVデータから対象の気温データを取得
    std::vector<Candlestick> candles = computeCandlestickDataForCountry(countryCode);
    if (candles.empty()) {
        std::cerr << "No candlestick data computed. "
                  << "Check if the country code is correct and data is available.\n";
        return;
    }

    // 年と平均気温のデータポイントを抽出
    std::vector<std::pair<int, double>> dataPoints; // (年, 平均気温)
    for (const auto& candle : candles) {
        int year = 0;
        try {
            year = std::stoi(candle.date.substr(0, 4));
        } catch (...) {
            continue; // 無効な年の場合はスキップ
        }
        dataPoints.emplace_back(year, candle.close); // ここでは 'close' を平均気温として仮定
    }

    if (dataPoints.size() < 2) {
        std::cerr << "Not enough data points for regression analysis.\n";
        return;
    }

    // 単回帰分析の計算
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    int n = dataPoints.size();
    for (const auto& point : dataPoints) {
        sumX += point.first;
        sumY += point.second;
        sumXY += point.first * point.second;
        sumX2 += point.first * point.first;
    }

    // 単回帰分析の式: Y = slope * X + intercept
    double denominator = n * sumX2 - sumX * sumX;
    if (denominator == 0.0) {
        std::cerr << "Denominator is zero. Cannot perform regression.\n";
        return;
    }

    // 単回帰分析の係数を計算
    double slope = (n * sumXY - sumX * sumY) / denominator;
    // Y切片
    double intercept = (sumY * sumX2 - sumX * sumXY) / denominator;

    std::cout << "\n=== Simple Linear Regression ===\n";
    std::cout << "Equation: Y = " << slope << " * X + " << intercept << "\n";

    // 将来の予測年数をユーザーに入力してもらう
    int futureYears = 0;
    std::cout << "Enter the number of future years to predict: ";
    std::string line;
    std::getline(std::cin, line);
    try {
        futureYears = std::stoi(line);
    }
    catch (const std::exception&) {
        std::cerr << "Invalid input for future years.\n";
        return;
    }

    if (futureYears <= 0) {
        std::cerr << "Number of future years must be positive.\n";
        return;
    }

    // 最後のデータポイントの年を基準に予測
    int lastYear = dataPoints.back().first;
    std::cout << "\n=== Predicted Temperatures ===\n";
    std::cout << "Year\tPredicted Temperature\n";
    for (int i = 1; i <= futureYears; ++i) {
        int predictYear = lastYear + i;
        double predictedTemp = slope * predictYear + intercept;
        std::cout << predictYear << "\t" << std::fixed << std::setprecision(3) << predictedTemp << "\n";
    }
}
