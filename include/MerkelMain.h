#pragma once
#include <vector>
#include <string>
#include "Candlestick.h"

class MerkelMain
{
public:
    MerkelMain();
    void init();

private:
    void printMenu();
    void printHelp();
    int getUserOption();
    void processUserOption(int userOption);

    // ローソク足データの計算・表示関数
    void computeAndDisplayCandlestickData();

    // ▼ 新メソッド: 「3」で裏側の計算＋即プロットをする関数
    void computeCandlestickAndPlot();

    // テキストベースのキャンドルスティック描画関数
    void plotCandlestickData(const std::vector<Candlestick>& candles, int maxDisplayCount);

    // --- 計算結果を保持するための変数を追加 ---
    std::vector<Candlestick> lastComputedCandles;

    // CSVデータ格納用
    std::vector<std::vector<std::string>> csvData;
};
