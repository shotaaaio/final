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

	//�X�e�[�W�̓o�^
	void regist(Stage* stage) { stageArray.emplace_back(stage); }

	//�X�e�[�W�̎擾
	Stage* getStage(int index) { return stageArray.at(index); }
private:
	std::vector<Stage*>stageArray;
};