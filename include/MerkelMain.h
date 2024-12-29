#pragma once

#include <vector>
#include <string>
#include "Candlestick.h"

/**
 * @brief メインクラス
 *        - CSVデータ読み込み
 *        - ローソク足計算
 *        - テキストベース描画
 *        - メニュー制御
 */
class MerkelMain
{
public:
    // コンストラクタ
    MerkelMain();
    
    // メインループ開始
    void init();

private:
    // メニュー表示
    void printMenu();
    // ヘルプ表示
    void printHelp();
    // ユーザーの選択肢取得
    int getUserOption();
    // ユーザーの選択肢に応じた処理
    void processUserOption(int userOption);

    // --------------------------
    // (1) ローソク足の計算・一覧表示 (メニュー2)
    // --------------------------
    void computeAndDisplayCandlestickData();

    // --------------------------
    // (2) ローソク足の計算（裏で実施）＋即時テキストプロット (メニュー3)
    // --------------------------
    void computeCandlestickAndPlot();

    // --------------------------
    // (3) 計算済みローソク足のテキストベース描画
    //     ここに「縦軸ラベル」の表示を追加
    // --------------------------
    void plotCandlestickData(const std::vector<Candlestick>& candles, int maxDisplayCount);

    // --------------------------
    // ヘルパー変数など
    // --------------------------
    // 直近で計算したローソク足データ
    std::vector<Candlestick> lastComputedCandles;

    // CSVの生データ
    std::vector<std::vector<std::string>> csvData;
};
