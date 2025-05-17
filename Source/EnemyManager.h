#pragma once
#include"Enemy.h"
//�G�Ǘ�
class EnemyManager
{
private:
	EnemyManager(){}
	~EnemyManager() {}
public:
	//�V���O���g��
	static EnemyManager* instance()
	{
		static EnemyManager inst;
		return &inst;
	}

	//�X�V����
	void update(float elapsedTime);

	//�`�揈��
	void render(ID3D11DeviceContext* dc);

	//�G���m�̏Փ˔���
	void collisionEnemiesAndEnemies();

	//�G�̓o�^
	void regist(Enemy* enemy);

	//�G�̔j��
	void clear();

	//�f�o�b�N�v���~�e�B�u�`��
	void drawDebugPrimitive();

	//�G�̑������擾
	int getEnemyCount()const { return static_cast<int>(enemyArray.size()); }

	//�G���擾
	Enemy* getEnemy(int index) { return enemyArray.at(index); }

	//�����Ŗ�����|�W�V���������ԋ߂��G���T�[�`
	Enemy* searchEnemy(DirectX::XMFLOAT3* position);

	//�G�폜
	void remove(Enemy* enemy);

	Enemy* getEnemyByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd);
private:
	std::vector<Enemy*>enemyArray;
	std::vector<Enemy*>removeArray;	
};