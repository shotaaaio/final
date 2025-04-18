#pragma once
#include<d3d11.h>
#include"Character.h"

class EnemyManager;
//�G
class Enemy : public Character
{
public:
	Enemy() {};
	~Enemy()override{}

	//�X�V����
	virtual void update(float elapsedTime) = 0;

	//�`�揈��
	virtual void render(ID3D11DeviceContext* dc) = 0;

	//�f�o�b�N�v���~�e�B�u�`��
	virtual void drawDebugPrimitive();

	void destroy();
};