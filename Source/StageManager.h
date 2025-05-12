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

    //�X�e�[�W���m�̃��C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
    {
		return false;
    }
private:
	std::vector<Stage*>stageArray;
};