#pragma once

#include"Graphics/SkinnedMesh.h"
#include"Enemy.h"

//�X���C��
class EnemyPlantune : public Enemy
{
public:
	EnemyPlantune();
	~EnemyPlantune();

	//�X�V����
	void update(float elapsedTime);

	//�`�揈��
	void render(ID3D11DeviceContext* dc) override;

	//���S�����Ƃ��ɌĂ΂��
	void onDead()override;
};