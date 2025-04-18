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
private:
	std::vector<Stage*>stageArray;
};