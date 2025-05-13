#pragma once
#include "Enemy.h"
#include <unordered_map>
#include <mutex>

class OcclusionQuery {
public:
	HRESULT BeginQuery(Enemy* enemy, ID3D11DeviceContext* dc);   //ピクセル測定開始
    HRESULT EndQuery(Enemy* enemy, ID3D11DeviceContext* context);//ピクセル測定終了
    void    CheckPixelCount(ID3D11DeviceContext* dc);            //ピクセル数取得
	void    CheckCountTimer(float elapsedTime);                  //タイムカウンター
	void    DrawImGui(); 						                 //デバッグ情報

private:
    struct QueryData {
        Microsoft::WRL::ComPtr<ID3D11Query> query;               //クエリ機能
        std::atomic<UINT64>pixelCount{ 0 };                      //描画ピクセル数
        std::atomic<bool>  started{ false };                     //クエリの開始状態
        std::atomic<bool>  ready{ false };                       //結果取得可能状態
        std::atomic<bool>  is_hide{ true };                      //可視性フラグ
        std::atomic<float> hideTime{ 0.0f };                     //敵が画面内から消えた時間
		std::atomic<float> hideTimeThereshold{ 5.0f };           //敵が画面内から消えた時間のしきい値
    };

    // デバイスとクエリデータ
	std::mutex mutex;                                            //スレッドセーフ用ミューテックス
    std::unordered_map<Enemy*,QueryData>queries;                 //遮蔽クエリをチェックするマップ          [キー:オブジェクトポインタ、値:データ]
    std::unordered_map<Enemy*,UINT64>   recentPixels;            //最近遮蔽クエリ結果をキャッシュするマップ[キー:オブジェクトポインタ、値:直近描画ピクセル数(可視性判定に使う)]
};