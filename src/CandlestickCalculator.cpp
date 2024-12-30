#include "CandlestickCalculator.h"
#include <unordered_map>
#include <limits>
#include <sstream>
#include <algorithm>
#include <iostream>

// Structure to hold temperature data for each year
struct TemperatureData {
    double sum;    // Annual temperature sum
    int count;     // Number of temperature data points
    double high;   // Annual maximum temperature
    double low;    // Annual minimum temperature

    TemperatureData()
        : sum(0.0), count(0),
          high(std::numeric_limits<double>::lowest()),
          low(std::numeric_limits<double>::max()) {}
};

// Function to compute candlestick data from CSV data for a given country
std::vector<Candlestick> CandlestickCalculator::computeCandlestickData(const std::vector<std::vector<std::string>>& csvData, const std::string& countryCode) {
    std::vector<Candlestick> candlesticks;

    if (csvData.empty()) {
        std::cerr << "Error: CSV data is empty." << std::endl;
        return candlesticks;
    }

    // Identify the column index for the country code from the header
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

    // Map to store yearly temperature data
    std::unordered_map<int, TemperatureData> yearlyData;

    // Process each data row
    for (size_t i = 1; i < csvData.size(); ++i) { // Skip header
        const std::vector<std::string>& row = csvData[i];
        if (static_cast<int>(row.size()) <= targetIndex) {
            continue; // If the specified column does not exist
        }

        // Extract the year from the timestamp
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

        // Retrieve temperature data
        double temperature = 0.0;
        try {
            temperature = std::stod(row[targetIndex]);
        } catch (...) {
            continue; // Invalid temperature data
        }

        // Update yearly data
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

    // Create a list of years to sort
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
            continue; // Skip years with no data
        }

        double average = data.sum / data.count;
        double open = hasPrevious ? previousAverage : average; // For the first year, use its own average
        double close = average;
        double high = data.high;
        double low = data.low;

        // Set the year in "YYYY" format
        std::string yearStr = std::to_string(year);

        Candlestick candle(yearStr, open, high, low, close);
        candlesticks.push_back(candle);

        previousAverage = average;
        hasPrevious = true;
    }

    return candlesticks;
}
