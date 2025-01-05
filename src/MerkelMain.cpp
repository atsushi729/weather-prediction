#include "MerkelMain.h"

#include <iostream>
#include <exception>
#include <iomanip>
#include <limits>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <map>

#include "CSVReader.h"
#include "CandlestickCalculator.h"
#include "Candlestick.h"

namespace {
    /** 
     * @brief Width per candlestick (ASCII art column width)  
     */
    const int COLUMN_WIDTH = 5;

    /**
     * @brief Width of the label part displayed on the Y-axis
     */
    const int Y_LABEL_WIDTH = 8;  

    /**
     * @brief Utility function to return a string with fixed width
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
// Constructor
// ─────────────────────────────────────────────
MerkelMain::MerkelMain()
{
    std::string filename = "../weather_data.csv";
    csvData = CSVReader::readCSV(filename);
    if (csvData.empty()) {
        std::cerr << "Error: Failed to read CSV data from " << filename << std::endl;
    }
}

// ─────────────────────────────────────────────
// Main Loop
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
// Display Menu
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
// (Menu 1) Display Help
// ─────────────────────────────────────────────
void MerkelMain::printHelp()
{
    std::cout << "========================================\n";
    std::cout << "              Help Menu                  \n";
    std::cout << "========================================\n";
    std::cout << "This application allows you to analyze and visualize weather data using various tools.\n\n";
    
    std::cout << "Available Options:\n";
    std::cout << "1: Print Help - Display this help message.\n";
    std::cout << "2: Compute Candlestick Data - Process the weather CSV file to compute candlestick data for a specified country.\n";
    std::cout << "   - Candlestick data includes Open, High, Low, and Close temperatures for each year.\n\n";
    
    std::cout << "3: Plot Candlestick Data - Compute and immediately plot the candlestick data as a text-based chart.\n";
    std::cout << "   - This provides a visual representation of temperature trends over the years.\n\n";
    
    std::cout << "4: Show Yearly Temperature Histogram - Display a vertical histogram of yearly temperatures.\n";
    std::cout << "   - You can choose to view Average, Maximum, or Minimum temperatures.\n";
    std::cout << "   - This helps in understanding the distribution and range of temperatures over the years.\n\n";
    
    std::cout << "5: Predict Future Temperature (Linear Regression) - Perform linear regression analysis to predict future temperatures.\n";
    std::cout << "   - Based on historical data, the application will forecast temperatures for the specified number of future years.\n";
    std::cout << "   - The prediction is visualized on a text-based plot, differentiating past data from predicted values.\n\n";
    
    std::cout << "0: Exit - Close the application.\n\n";
    
    std::cout << "Instructions:\n";
    std::cout << "- To select an option, enter the corresponding number when prompted.\n";
    std::cout << "- Ensure that the weather CSV file is correctly formatted and located in the expected directory.\n";
    std::cout << "- For options requiring a country code, enter the appropriate ISO country code (e.g., GB for Great Britain).\n";
    std::cout << "- Follow on-screen prompts for additional inputs required by each option.\n\n";
    
    std::cout << "Example Usage:\n";
    std::cout << "1. To compute candlestick data, select option 2 and enter the desired country code when prompted.\n";
    std::cout << "2. To visualize the data, select option 3 to see the candlestick chart.\n";
    std::cout << "3. For a statistical overview, select option 4 to view the temperature histogram.\n";
    std::cout << "4. To forecast future temperatures, select option 5 and specify the number of years you wish to predict.\n";
    
    std::cout << "========================================\n";
}

// ─────────────────────────────────────────────
// Get User Input
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
        userOption = -1; // Indicates error
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

// ─────────────────────────────────────────────
// Process User Option Based on Input
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
            // (1) Display Help
            printHelp();
            break;
        case 2:
            // (2) Compute Candlestick Data + Display List
            computeAndDisplayCandlestickData();
            break;
        case 3:
            // (3) Compute Candlestick Data + Immediate Text Plotting
            computeCandlestickAndPlot();
            break;
        case 4:
            // (4) Display Vertical Histogram
            showYearlyHistogram();
            break;
        case 5:
            // (5) Predict Future Temperature (Linear Regression)
            predictFutureTemperature();
            break;
        default:
            std::cout << "Invalid choice. Choose a valid option." << std::endl;
            break;
    }
}

// ─────────────────────────────────────────────
// Get Country Code from User
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
// Get Data Type (Average, Max, Min) from User
// ─────────────────────────────────────────────
int MerkelMain::getDataTypeFromUser()
{
    // Select Data Type
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
        dataType = 1; // Default
    }
    if (dataType < 1 || dataType > 3) dataType = 1;
    return dataType;
}

// ─────────────────────────────────────────────
// Compute Candlestick Data from CSV for a Given Country
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
// (Menu 2) Compute and Display Candlestick Data
// ─────────────────────────────────────────────
void MerkelMain::computeAndDisplayCandlestickData()
{
    std::string countryCode = getCountryCodeFromUser();
    if (countryCode.empty()) {
        return; // Input error
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
// (Menu 2) Compute Candlestick Data + Immediate Text Plotting
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
// (Menu 3) Plot Candlestick Data as Text
// ─────────────────────────────────────────────
void MerkelMain::plotCandlestickData(const std::vector<Candlestick>& candles, int maxDisplayCount)
{
    if (candles.empty()) {
        std::cout << "No data to plot." << std::endl;
        return;
    }
    int displayCount = std::min(static_cast<int>(candles.size()), maxDisplayCount);

    // Calculate minLow and maxHigh
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

    // Main chart body (from top to bottom)
    for (int row = chartHeight; row >= 0; --row)
    {
        // Y-axis label on the left
        double actualValue = minLow + (row / scale);
        std::ostringstream yLabelSS;
        yLabelSS << std::fixed << std::setprecision(1) << actualValue;
        std::string yLabelStr = yLabelSS.str();

        // Align the label to the right
        std::cout << std::setw(6) << yLabelStr << " ┃ ";

        // Plot each candlestick
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

            // Outside candlestick drawing range
            if (row > scaledHigh || row < scaledLow) {
                std::cout << std::string(COLUMN_WIDTH, ' ');
                continue;
            }

            bool isWickRange = ((row <= scaledHigh) && (row >= boxTop)) ||
                               ((row <= boxBottom) && (row >= scaledLow));
            bool isBoxRange  = (row <= boxTop && row >= boxBottom);

            if (row == scaledHigh) {
                // Top of upper wick
                std::cout << "  ^  ";
            }
            else if (row == scaledLow) {
                // Bottom of lower wick
                std::cout << "  v  ";
            }
            else if (isBoxRange) {
                // Body of the candlestick
                if (close >= open) { // Bullish
                    std::cout << "\033[32m"; // Green color
                } else {             // Bearish
                    std::cout << "\033[31m"; // Red color
                }
                std::cout << "  █  ";
                std::cout << "\033[0m"; // Reset color
            }
            else if (isWickRange) {
                // Wick part
                std::cout << "  │  ";
            }
            else {
                std::cout << std::string(COLUMN_WIDTH, ' ');
            }
        }
        std::cout << std::endl;
    }

    // X-axis labels (years)
    std::cout << std::string(6, ' ') << "  ";
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
// (Menu 4) Display Yearly Temperature Histogram Based on Country Code and Data Type
// ─────────────────────────────────────────────
void MerkelMain::showYearlyHistogram()
{
    // (1) Get Country Code
    std::string countryCode = getCountryCodeFromUser();
    if (countryCode.empty()) {
        return; // Exit on input error
    }

    // (2) Select Data Type
    int dataType = getDataTypeFromUser(); 
    // 1=Average, 2=Max, 3=Min

    // (3) Check if CSV Data Exists
    if (csvData.empty()) {
        std::cout << "CSV data is empty.\n";
        return;
    }

    // Get header row
    const std::vector<std::string>& header = csvData[0];
    std::string targetColumn = countryCode + "_temperature";
    int targetIndex = -1;

    // Find target column in header
    for (size_t i = 0; i < header.size(); ++i) {
        if (header[i] == targetColumn) {
            targetIndex = static_cast<int>(i);
            break;
        }
    }

    if (targetIndex == -1) {
        std::cout << "The specified country code \"" << countryCode << "\" was not found in the headers.\n";
        std::cout << "Available country codes are as follows:\n";
        for (size_t i = 1; i < header.size(); ++i) { // Skip first column (timestamp)
            // Extract country code from "AT_temperature"
            size_t pos = header[i].find("_temperature");
            if (pos != std::string::npos) {
                std::string code = header[i].substr(0, pos);
                std::cout << "- " << code << "\n";
            }
        }
        return;
    }

    // Map to store yearly temperature data
    std::map<int, std::vector<double>> yearToTemps;

    // Process data rows
    for (size_t i = 1; i < csvData.size(); ++i) { // Skip header
        const std::vector<std::string>& row = csvData[i];
        if (static_cast<int>(row.size()) <= targetIndex) {
            continue; // Skip if target column does not exist
        }

        // Extract year from timestamp
        std::string timestamp = row[0];
        if (timestamp.size() < 4) {
            continue; // Invalid timestamp
        }
        int year = 0;
        try {
            year = std::stoi(timestamp.substr(0, 4));
        } catch (...) {
            continue; // Invalid year
        }

        // Get temperature data
        double temperature = 0.0;
        try {
            temperature = std::stod(row[targetIndex]);
        } catch (...) {
            continue; // Invalid temperature data
        }

        // Update yearly data
        yearToTemps[year].push_back(temperature);
    }

    // (4) Aggregate yearly data as "Average or Max or Min"
    //     This results in year -> single value
    std::vector<std::pair<int, double>> yearlyData; // (year, temperature)

    for (const auto& kv : yearToTemps)
    {
        // kv.first = year, kv.second = vector<double>
        int year = kv.first;
        const std::vector<double>& temps = kv.second;

        if (temps.empty()) continue;

        // Aggregate
        double finalVal = 0.0;

        if (dataType == 1) {
            // Average: sum / count
            double sum = 0.0;
            for (double t : temps) sum += t;
            finalVal = sum / temps.size();
        }
        else if (dataType == 2) {
            // Max
            finalVal = *std::max_element(temps.begin(), temps.end());
        }
        else {
            // Min
            finalVal = *std::min_element(temps.begin(), temps.end());
        }

        yearlyData.emplace_back(year, finalVal);
    }

    if (yearlyData.empty()) {
        std::cout << "\nNo data available for the specified country code \"" << countryCode << "\".\n";
        return;
    }

    // Sort years in ascending order
    std::sort(yearlyData.begin(), yearlyData.end(),
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) -> bool {
            return a.first < b.first;
        }
    );

    // (5) Prepare scaling for histogram
    //     Find the maximum value on the Y-axis and draw vertical bars as text
    double maxVal = std::numeric_limits<double>::lowest();
    double minVal = std::numeric_limits<double>::max();
    for (const auto& p : yearlyData) {
        double val = p.second;
        if (val > maxVal) maxVal = val;
        if (val < minVal) minVal = val;
    }
    // To start from 0, you can set minVal = 0.0 if desired

    double range = maxVal - minVal;
    if (range <= 0.0) {
        std::cout << "\nAll data points have the same value. Cannot draw vertical histogram.\n";
        return;
    }

    // Height in vertical direction (set to 20 lines)
    const int chartHeight = 20;
    // Scaling
    double scale = static_cast<double>(chartHeight) / range;

    // (6) Draw each row from top to bottom
    std::cout << "\n===== Yearly "
              << ((dataType == 1) ? "Average" : (dataType == 2) ? "Max" : "Min")
              << " Temp for " << countryCode << " =====\n\n";

    // Ensure space for Y-axis labels (e.g., 6 characters)
    const int labelWidth = 6;

    // row: chartHeight ~ 0
    for (int row = chartHeight; row >= 0; --row)
    {
        // Calculate Y-axis label
        double currentVal = minVal + (range * row / chartHeight);
        // Format label with fixed width (1 decimal place)
        std::ostringstream labelStream;
        labelStream << std::fixed << std::setprecision(1) << currentVal;
        std::string label = labelStream.str();

        // Right-align the label
        std::cout << std::setw(labelWidth) << label << " ┃ ";

        // Loop through each year's data
        for (size_t i = 0; i < yearlyData.size(); ++i)
        {
            double val = yearlyData[i].second;
            // Height on scale
            int barHeight = static_cast<int>((val - minVal) * scale);

            // If current row is within the bar height, draw "#", else " "
            // Note: row == 0 is the baseline
            if (row <= barHeight - 1) {
                // Draw '#'
                std::cout << "  █  ";
            } else {
                std::cout << "     ";
            }
        }
        std::cout << "\n";
    }

    // (7) Output X-axis (year) labels
    //     Combine into one line below the histogram
    //     If there are too many years, it may overlap, so adjust as needed
    //     implementation: write yearInt on one line

    // Add space before X-axis labels (label width + ' | ')
    std::cout << std::setw(labelWidth + 2) << " " << " ";

    for (size_t i = 0; i < yearlyData.size(); ++i)
    {
        int yearInt = yearlyData[i].first;
        // Allocate approximately 5 characters for each year
        std::ostringstream oss;
        oss << std::setw(5) << yearInt;
        std::cout << oss.str();
    }
    std::cout << "\n";
}

// ─────────────────────────────────────────────
// (Menu 5) Predict Future Temperature (Linear Regression)
// ─────────────────────────────────────────────
void MerkelMain::predictFutureTemperature()
{
    std::string countryCode = getCountryCodeFromUser();
    if (countryCode.empty()) {
        return; // Input error
    }

    // Get temperature data for the specified country from CSV
    std::vector<Candlestick> candles = computeCandlestickDataForCountry(countryCode);
    if (candles.empty()) {
        std::cerr << "No candlestick data computed. "
                  << "Check if the country code is correct and data is available.\n";
        return;
    }

    // Extract data points as (year, average temperature)
    std::vector<std::pair<int, double>> dataPoints; // (year, average temperature)
    for (const auto& candle : candles) {
        int year = 0;
        try {
            year = std::stoi(candle.date.substr(0, 4));
        } catch (...) {
            continue; // Skip invalid year
        }
        dataPoints.emplace_back(year, candle.close); // Assuming 'close' represents average temperature
    }

    if (dataPoints.size() < 2) {
        std::cerr << "Not enough data points for regression analysis.\n";
        return;
    }

    // Calculate sums for regression
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    int n = dataPoints.size();
    for (const auto& point : dataPoints) {
        sumX += point.first;
        sumY += point.second;
        sumXY += point.first * point.second;
        sumX2 += point.first * point.first;
    }

    // linear regression formula: Y = slope * X + intercept
    double denominator = n * sumX2 - sumX * sumX;
    if (denominator == 0.0) {
        std::cerr << "Denominator is zero. Cannot perform regression.\n";
        return;
    }

    // Calculate regression coefficients
    double slope = (n * sumXY - sumX * sumY) / denominator;
    // Y-intercept
    double intercept = (sumY * sumX2 - sumX * sumXY) / denominator;

    std::cout << "\n=== Linear Regression ===\n";
    std::cout << "Equation: Y = " << slope << " * X + " << intercept << "\n";

    // Get the number of future years to predict from the user
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

    // Use the last data point's year as the base for prediction
    int lastYear = dataPoints.back().first;
    std::cout << "\n=== Predicted Temperatures ===\n";
    std::cout << "Year\tPredicted Temperature\n";
    std::vector<std::pair<int, double>> predictedData; // (year, predicted temperature)
    for (int i = 1; i <= futureYears; ++i) {
        int predictYear = lastYear + i;
        double predictedTemp = slope * predictYear + intercept;
        predictedData.emplace_back(predictYear, predictedTemp);
        std::cout << predictYear << "\t" << std::fixed << std::setprecision(3) << predictedTemp << "\n";
    }

    // (9) Plot the prediction
    plotPrediction(dataPoints, predictedData);
}

// ─────────────────────────────────────────────
// (Menu 5) Plot Prediction Data as Text
// ─────────────────────────────────────────────
void MerkelMain::plotPrediction(const std::vector<std::pair<int, double>>& pastData, const std::vector<std::pair<int, double>>& predictedData) const
{
    std::vector<std::pair<int, double>> allData = pastData;
    allData.insert(allData.end(), predictedData.begin(), predictedData.end());

    double minVal = std::numeric_limits<double>::max();
    double maxVal = std::numeric_limits<double>::lowest();
    for (const auto& p : allData) {
        if (p.second < minVal) minVal = p.second;
        if (p.second > maxVal) maxVal = p.second;
    }

    double range = maxVal - minVal;
    if (range <= 0.0) {
        std::cout << "There is no valid range to plot.\n";
        return;
    }

    const int chartHeight = 20;
    double scale = static_cast<double>(chartHeight - 1) / range;

    int dataCount = allData.size();

    std::vector<int> scaledValues;
    for (const auto& p : allData) {
        int scaled = static_cast<int>(std::round((p.second - minVal) * scale));
        // Limit scale values to [0, chartHeight - 1]
        if (scaled < 0) scaled = 0;
        if (scaled >= chartHeight) scaled = chartHeight - 1;
        scaledValues.push_back(scaled);
    }

    std::cout << "\n=== Temperature Prediction Plot ===\n\n";
    for (int row = chartHeight - 1; row >= 0; --row) {
        // Display Y-axis label
        double currentVal = minVal + (range * row / (chartHeight - 1));
        std::ostringstream labelStream;
        labelStream << std::fixed << std::setprecision(1) << currentVal;
        std::string label = labelStream.str();

        // Right-align the label
        std::cout << std::setw(6) << label << " ┃";

        // Plot each data point
        for (int i = 0; i < dataCount; ++i) {
            if (scaledValues[i] == row) {
                if (i < static_cast<int>(pastData.size())) {
                    // Blue '*' + space
                    std::cout << "\033[34m*\033[0m " << " ";
                }
                else {
                    // Green '*' + space
                    std::cout << "\033[32m*\033[0m " << " ";
                }
            }
            else {
                std::cout << "  " << " ";
            }
        }
        std::cout << "\n";
    }

    // Display X-axis labels (years)
    std::cout << "       ";
    for (const auto& p : allData) {
        // Display only the last two digits
        int lastTwoDigits = p.first % 100;
        std::cout << std::setw(2) << lastTwoDigits << " ";
    }
    std::cout << "\n";
}