#ifndef CANDLESTICK_H
#define CANDLESTICK_H

#include <string>

class Candlestick {
public:
    std::string date; // "YYYY" format
    double open;
    double high;
    double low;
    double close;

    Candlestick(const std::string& date_, double open_, double high_, double low_, double close_);
};

#endif // CANDLESTICK_H
