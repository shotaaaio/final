#pragma once

#include <vector>
#include "Obj.h"

// �G�Ǘ�
class ObjManager
{
private:
    ObjManager() {}
    ~ObjManager() {}

    // �G���m�̏Փˏ���
    void collisionEnemiesAndEnemies();

public:
    // singleton
    static ObjManager* instance()
    {
        static ObjManager inst;
        return &inst;
    }

    // �X�V����
    void update(float elapsedTime);

    // �`�揈��
    void render(ID3D11DeviceContext* dc);

    // �G�̓o�^
    void regist(Obj* enemy);

    // �G�̔j��
    void clear();

    // �f�o�b�O�v���~�e�B�u�`��
    //void drawDebugPrimitive();

    // �G�̑������擾
    int getObjCount() const { return static_cast<int>(enemies.size()); }

    // �G���擾
    Obj* getObj(int index) { return enemies.at(index); }

    Obj* searchEnemy(DirectX::XMFLOAT3* position)
    {
        // ��ԋ߂��̓G���^�[�Q�b�g�ɐݒ�
        float dist = FLT_MAX;
        Obj* closestEnemy = nullptr;
        int enemyCount = getObjCount();
        for (int i = 0; i < enemyCount; ++i)
        {
            // �G�Ƃ̋����𔻒�
            Obj* enemy = getObj(i);
            DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(position);
            DirectX::XMVECTOR eneVec = DirectX::XMLoadFloat3(enemy->getPosition());
            DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(eneVec, posVec);
            DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vec);
            float lengthSq;
            DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
            if (lengthSq < dist)
            {
                dist = lengthSq;
                closestEnemy = enemy;
            }
        }
        return closestEnemy;
    }

    // �G�폜
    void remove(Obj* enemy);

    Obj* getEnemyByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd);

private:
    std::vector<Obj*>	enemies;
    std::vector<Obj*> removes;
};