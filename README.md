# Project Title

A Temperature Data Visualization and Prediction Tool

## Project Structure

```
/project
├── include
│   ├── MerkelMain.h
│   ├── CSVReader.h
│   ├── CandlestickCalculator.h
│   └── Candlestick.h
├── src
│   ├── MerkelMain.cpp
│   ├── CSVReader.cpp
│   ├── CandlestickCalculator.cpp
│   └── Candlestick.cpp
└── weather_data.csv
```

## Installation

1. Prepare dataset (Please set up dataset under oop-midterm folder)
   ```bash
   weather_data.csv
   ```
2. Move to src folder
   ```bash
   cd src
   ```
3. rcBuild the project (requires a C++ compiler):
   ```bash
   g++ -std=c++11 -I../include -o main *.cpp
   ```
4. Run the application:
   ```bash
   ./main
   ```

## Usage

1. Upon running, choose from the following options in the main menu:
   - Compute Candlestick Data
   - Create a Text-Based Candlestick Chart
   - Show Yearly Temperature Histogram
   - Predict Future Temperatures
2. Follow the prompts to input country codes, data ranges, or other parameters as required.

## How It Works

- **Candlestick Data Calculation**: Extracts and aggregates temperature data for open, high, low, and close values.
- **Visualization**: Renders data in text-based formats for simplicity and portability.
- **Prediction**: Implements a linear regression model to extrapolate future temperature trends.

## Example Output

### Candlestick Data

```
Candle data : AT
Date    Open    High    Low     Close
1980    6.049   29.132  -14.507 6.049
1981    6.049   29.419  -16.219 7.192
...
```

### Text-Based Histogram

```
===== Yearly Average Temperature (AT) =====
50.0 |    █  █  
40.0 |  █      
...
```

### Predicted Temperatures

```
=== Predicted Temperatures ===
Year    Predicted Temperature
2025    8.867
2026    8.910
...
```

## Future Enhancements

- Add graphical visualization support.
- Integrate advanced predictive models like ARIMA.
- Allow data export in various formats.

## Credits

- Developed by Atsushi Hatakeyama
- Data Source: `weather_data.csv`
