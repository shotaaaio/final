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

    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
    {
        bool hitDetected = false;
        for (Stage* stage : stageArray) {
            if (stage->raycast(start, end, hit)) {
                hitDetected = true;

                // ���̃I�u�W�F�N�g�⃂�f���Ƃ̏Փ˔��������
                for (Stage* otherStage : stageArray) {
                    if (otherStage != stage) {
                        HitResult tempHit;
                        // ���̃X�e�[�W���̃��f���ɑ΂��郌�C�L���X�g���s��
                        if (otherStage->raycast(start, end, tempHit)) {
                            // �Փ˂����o���ꂽ�ꍇ�A�K�؂ɕ␳�ʒu���X�V
                            if (tempHit.distance < hit.distance) {
                                hit = tempHit; // ���߂��Փ˂�I��
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