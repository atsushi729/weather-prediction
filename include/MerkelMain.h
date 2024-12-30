#ifndef MERKELMAIN_H
#define MERKELMAIN_H

#include <vector>
#include <string>
#include "Candlestick.h"

/**
 * @brief Main class
 *        - CSV data loading
 *        - Candlestick calculation
 *        - Text-based rendering
 *        - Menu control
 */
class MerkelMain
{
public:
    // Constructor
    MerkelMain();

    // Start main loop
    void init();
private:
    // Display menu
    void printMenu();
    // Display help
    void printHelp();
    // Get user option
    int getUserOption();
    // Process based on user option
    void processUserOption(int userOption);

    // Get country code from user
    std::string getCountryCodeFromUser();

    // Compute Candlestick from CSV data
    std::vector<Candlestick> computeCandlestickDataForCountry(const std::string& countryCode);

    // ─────────────────────────────────────────────
    // (1) Compute and Display Candlestick Data (Menu 2)
    // ─────────────────────────────────────────────
    void computeAndDisplayCandlestickData();

    // ─────────────────────────────────────────────
    // (2) Compute Candlestick Data (Behind the Scenes) + Immediate Text Plotting (Menu 3)
    // ─────────────────────────────────────────────
    void computeCandlestickAndPlot();

    // ─────────────────────────────────────────────
    // (3) Text-based Rendering of Computed Candlestick Data
    // ─────────────────────────────────────────────
    void plotCandlestickData(const std::vector<Candlestick>& candles, int maxDisplayCount);

    // ─────────────────────────────────────────────
    // (4) Text-based Rendering of Computed Histogram
    // ─────────────────────────────────────────────
    void showYearlyHistogram();

    // Existing function: Select 1=Average, 2=Max Temperature, 3=Min Temperature
    int getDataTypeFromUser();

    // ─────────────────────────────────────────────
    // (5) Predict future temperature
    // ─────────────────────────────────────────────
    void predictFutureTemperature();

    // ─────────────────────────────────────────────
    // Member Variables
    // ─────────────────────────────────────────────
    // Recently computed candlestick data
    std::vector<Candlestick> lastComputedCandles;

    // Raw CSV data
    std::vector<std::vector<std::string>> csvData;
};

#endif // MERKELMAIN_H
