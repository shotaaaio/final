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

    //ステージ同士のレイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
    {
		return false;
    }
private:
	std::vector<Stage*>stageArray;
};