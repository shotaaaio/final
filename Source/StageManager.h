#pragma once

#include"Stage.h"
#include<vector>

class StageManager
{
public:
	StageManager() {}
	~StageManager() { stageArray.clear(); }

	static StageManager* instance()
	{
		static StageManager inst;
		return&inst;
	}

	//ステージの登録
	void regist(Stage* stage) { stageArray.emplace_back(stage); }

	//ステージの取得
	Stage* getStage(int index) { return stageArray.at(index); }

    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
    {
        bool hitDetected = false;
        for (Stage* stage : stageArray) {
            if (stage->raycast(start, end, hit)) {
                hitDetected = true;

                // 他のオブジェクトやモデルとの衝突判定を強化
                for (Stage* otherStage : stageArray) {
                    if (otherStage != stage) {
                        HitResult tempHit;
                        // 他のステージ内のモデルに対するレイキャストを行う
                        if (otherStage->raycast(start, end, tempHit)) {
                            // 衝突が検出された場合、適切に補正位置を更新
                            if (tempHit.distance < hit.distance) {
                                hit = tempHit; // より近い衝突を選択
                            }
                        }
                    }
                }
            }
        }
        return hitDetected;
    }
private:
	std::vector<Stage*>stageArray;
};