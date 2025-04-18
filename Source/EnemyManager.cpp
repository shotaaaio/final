#include"EnemyManager.h"
#include"Collision.h"

//�X�V����
void EnemyManager::update(float elapsedTime)
{
	for (Enemy* enemy : enemyArray)
	{
		enemy->update(elapsedTime);
	}

	//�j������
	for (Enemy* enemy : removeArray)
	{
		auto itr = std::find(enemyArray.begin(), enemyArray.end(), enemy);
		if (itr != enemyArray.end())
		{
			// �j�����ׂ� bullet �� bullets ���ɑ��݂���ꍇ�A�폜
			delete enemy;
			enemyArray.erase(itr);
		}
	}

	//�j�����X�g���N���A
	removeArray.clear();

	//�G���m�̏Փ˔���
	collisionEnemiesAndEnemies();
}

//�`�揈��
void EnemyManager::render(ID3D11DeviceContext* dc)
{
	for (Enemy* enemy : enemyArray)
	{
		enemy->render(dc);
	}
}

//�G��o�^
void EnemyManager::regist(Enemy* enemy)
{
	enemyArray.emplace_back(enemy);
}

void EnemyManager::clear()
{
	for (Enemy* enemy : enemyArray)
	{
		delete enemy;
	}
	enemyArray.clear();
}

void EnemyManager::drawDebugPrimitive()
{
	for (Enemy* enemy : enemyArray)
	{
		enemy->drawDebugPrimitive();
	}
}

void EnemyManager::collisionEnemiesAndEnemies()
{
	int max = getEnemyCount();
	for (int i = 0;i < max;++i)
	{
		Enemy* enemy1 = getEnemy(i);
		for (int j = 0;j < max;++j)
		{
			//�����Ǝ����Ŕ�ׂȂ��悤��
			if (i == j)continue;
			Enemy* enemy2 = getEnemy(j);
			DirectX::XMFLOAT3 outVec;

			if(Collision::intersectCylinderAndCylinder(
				*(enemy1->getPosition()),
				enemy1->getRadius(),
				enemy1->getHeight(),
				*(enemy2->getPosition()),
				enemy2->getRadius(),
				enemy2->getHeight(),
				outVec
			))
			{
				//���a�̍��v
				float range = enemy1->getRadius() + enemy2->getRadius();

				//�Փ˕����̃x�N�g�������v�̒����ɃX�P�[�����O
				DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

				//���̃x�N�g�����Փ˂��鑤�̈ʒu�ɑ���
				vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(enemy1->getPosition()), vec);

				//���̈ʒu�x�N�g�����Փ˂���鑤�ɐݒ肷��
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, vec);
				enemy2->setPosition(pos);
			}
		}
	}
}

Enemy* EnemyManager::searchEnemy(DirectX::XMFLOAT3* position)
{
	float dist = FLT_MAX;
	int enemyCount = getEnemyCount();
	Enemy* cacheEnemy = nullptr;
	for (int i = 0;i < enemyCount;++i)
	{
		//�G�Ƃ̋����𔻒�
		Enemy* enemy = getEnemy(i);
		DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(position);
		DirectX::XMVECTOR eneVec = DirectX::XMLoadFloat3(enemy->getPosition());
		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(eneVec, posVec);
		DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vec);
		float lengthSq;
		DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
		if (lengthSq < dist)
		{
			//�����̍X�V
			dist = lengthSq;
			//�����ԋ߂��G�l�~�[
			cacheEnemy = enemy;
		}
	}
	return cacheEnemy;
}

void EnemyManager::remove(Enemy* enemy)
{
	auto itr = std::find(removeArray.begin(), removeArray.end(), enemy);

	//�d�����Ă�����ǉ����Ȃ�
	if (itr != removeArray.end())return;

	//�j�����X�g�ɒǉ�
	removeArray.push_back(enemy);
}