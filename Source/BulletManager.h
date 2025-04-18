#pragma once
#include<vector>
#include"Buller.h"

//�e�Ǘ�
class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	//�X�V����
	void update(float elapsedTime);

	//�`�揈��
	void render(ID3D11DeviceContext* dc);
	
	//�f�o�b�N�v���~�e�B�u
	void drawDebugPrimitive();

	//�e�o�^
	void regist(Bullet* bullet);

	//�e�S����
	void clear();

	// �e���擾
	int getBulletCount() const { return static_cast<int>(bulletArray.size()); }

	// �e�擾
	Bullet* getBullet(int index) { return bulletArray.at(index); }

	//�e�폜
	void remove(Bullet* bullet);
private:
	std::vector<Bullet*>bulletArray;
	std::vector<Bullet*>removes;
};