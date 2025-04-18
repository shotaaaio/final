#include"BulletManager.h"

//�R���X�g���N�^
BulletManager::BulletManager()
{

}

//�f�X�g���N�^
BulletManager::~BulletManager()
{
	clear();
}

//�X�V����
void BulletManager::update(float elapsedTime)
{
	for (Bullet* bullet : bulletArray)
	{
		bullet->update(elapsedTime);
	}

	//�j������
	for (Bullet* bullet : removes)
	{
		auto itr = std::find(bulletArray.begin(), bulletArray.end(), bullet);
		if (itr != bulletArray.end())
		{
			// �j�����ׂ� bullet �� bullets ���ɑ��݂���ꍇ�A�폜
			delete bullet;
			bulletArray.erase(itr);
		}
	}

	//�j�����X�g���N���A
	removes.clear();
}

//�`�揈��
void BulletManager::render(ID3D11DeviceContext* dc)
{
	for (Bullet* bullet : bulletArray)
	{
		bullet->render(dc);
	}
}

//�f�o�b�N�v���~�e�B�u�`��
void BulletManager::drawDebugPrimitive()
{
	for (Bullet* bullet : bulletArray)
	{
		bullet->drawDebugPrimitive();
	}
}

//�e�o�^
void BulletManager::regist(Bullet* bullet)
{
	bulletArray.emplace_back(bullet);
}

//�e�S����
void BulletManager::clear()
{
	for (Bullet* bullet : bulletArray)
	{
		delete bullet;
	}
	bulletArray.clear();
}

//�e�폜
void BulletManager::remove(Bullet* bullet)
{
	auto itr = std::find(removes.begin(), removes.end(), bullet);

	//�d�����Ă�����ǉ����Ȃ�
	if (itr != removes.end())return;

	//�j�����X�g�ɒǉ�
	removes.push_back(bullet);
}