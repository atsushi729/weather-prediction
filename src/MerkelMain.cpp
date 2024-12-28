#include "MerkelMain.h"
#include <iostream>
#include <exception>
#include "CSVReader.h"
#include "CandlestickCalculator.h"
#include <iomanip>

MerkelMain::MerkelMain()
{
    // 初期化処理があればここに記述
}

void MerkelMain::init()
{
    currentTime = "1980-01-01T00:00:00Z"; // 初期時刻を設定（必要に応じて変更）

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

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - Your aim is to compute candlestick data from weather CSV files." << std::endl;
    std::cout << "Select option 2 to compute candlestick data." << std::endl;
}

void MerkelMain::computeAndDisplayCandlestickData()
{
    std::string filename;
    std::string countryCode;

    std::cout << "Enter CSV filename (e.g., weather_data.csv): ";
    std::getline(std::cin, filename);

    std::cout << "Enter country code (e.g., GB): ";
    std::getline(std::cin, countryCode);

    // CSVファイルを読み込む
    std::vector<std::vector<std::string>> csvData = CSVReader::readCSV(filename);
    if (csvData.empty()) {
        std::cerr << "Failed to read CSV data." << std::endl;
        return;
    }

    // ローソク足データを計算
    std::vector<Candlestick> candles = CandlestickCalculator::computeCandlestickData(csvData, countryCode);
    if (candles.empty()) {
        std::cerr << "No candlestick data computed." << std::endl;
        return;
    }

    // 結果を表示
    std::cout << "Date\t\tOpen\tHigh\tLow\tClose" << std::endl;
    for (const auto& candle : candles) {
        std::cout << candle.date << "\t"
                  << std::fixed << std::setprecision(3) << candle.open << "\t"
                  << candle.high << "\t"
                  << candle.low << "\t"
                  << candle.close << std::endl;
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
