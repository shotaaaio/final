#pragma once
#include"Graphics/SkinnedMesh.h"
#include"Enemy.h"

//�X���C��
class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime();

	//�X�V����
	void update(float elapsedTime);

	//�`�揈��
	void render(ID3D11DeviceContext* dc) override;

	//���S�����Ƃ��ɌĂ΂��
	void onDead()override;
};