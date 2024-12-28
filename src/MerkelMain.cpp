#include "MerkelMain.h"
#include <iostream>
#include <exception>
#include "CSVReader.h"
#include "CandlestickCalculator.h"
#include <iomanip>

MerkelMain::MerkelMain()
{
    // 固定されたCSVファイル名を設定
    std::string filename = "../weather_data.csv"; // 必要に応じてパスを変更

    // CSVファイルを読み込む
    csvData = CSVReader::readCSV(filename);
    if (csvData.empty()) {
        std::cerr << "Error: Failed to read CSV data from " << filename << std::endl;
        // 必要に応じてエラー処理（例: プログラム終了）
    }

    // 初期時刻の設定（必要に応じて変更）
    currentTime = "1980-01-01T00:00:00Z";
}

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

void MerkelMain::printMenu()
{
    // メニューの表示
    std::cout << "1: Print help " << std::endl;
    std::cout << "2: Compute Candlestick Data" << std::endl;
    std::cout << "0: Exit " << std::endl;

    std::cout << "============== " << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - Your aim is to compute candlestick data from a fixed weather CSV file." << std::endl;
    std::cout << "Select option 2 to compute candlestick data." << std::endl;
}

void MerkelMain::computeAndDisplayCandlestickData()
{
    if (csvData.empty()) {
        std::cerr << "Error: No CSV data available to compute candlestick data." << std::endl;
        return;
    }

    std::string countryCode;

    std::cout << "Enter country code (e.g., GB): ";
    std::getline(std::cin, countryCode);

    // 入力が空でないことを確認
    if (countryCode.empty()) {
        std::cerr << "Error: Country code cannot be empty." << std::endl;
        return;
    }

    // ローソク足データを計算
    std::vector<Candlestick> candles = CandlestickCalculator::computeCandlestickData(csvData, countryCode);
    if (candles.empty()) {
        std::cerr << "No candlestick data computed. Check if the country code is correct and data is available." << std::endl;
        return;
    }

    std::cout << "Candle data : " << countryCode << std::endl;

    // 40年分のデータを表示
    std::cout << "Date\tOpen\tHigh\tLow\tClose" << std::endl;
    int count = 0;
    for (const auto& candle : candles) {
        std::string year = candle.date.substr(0, 4);
        std::cout << year << "\t"
                  << std::fixed << std::setprecision(3) << candle.open << "\t"
                  << candle.high << "\t"
                  << candle.low << "\t"
                  << candle.close << std::endl;
        count++;
        if (count >= 40) break; // 40年分のデータのみ表示
    }

    // もし40年に満たない場合の警告
    if (count < 40) {
        std::cout << "Warning: Only " << count << " years of data available for country code " << countryCode << "." << std::endl;
    }
}

int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in option number (0 to exit): ";
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }
    catch(const std::exception& e)
    {
        userOption = -1; // エラーを示す
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == -1) // 読み取りエラー
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
            computeAndDisplayCandlestickData();
            break;
        default:
            std::cout << "Invalid choice. Choose a valid option." << std::endl;
            break;
    }
}
