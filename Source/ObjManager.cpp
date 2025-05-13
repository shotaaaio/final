#include "ObjManager.h"
#include "Collision.h"

// �X�V����
void ObjManager::update(float elapsedTime)
{
    for (Obj* enemy : enemies)
    {
        enemy->update(elapsedTime);
    }

    // �j������
    for (Obj* bullet : removes)
    {
        auto itr = std::find(enemies.begin(), enemies.end(), bullet);
        if (itr != enemies.end())
        {
            // �j�����ׂ� bullet �� bullets ���ɑ��݂���ꍇ�A�폜
            delete bullet;
            enemies.erase(itr);
        }
    }
    // �j�����X�g���N���A
    removes.clear();
    collisionEnemiesAndEnemies();
}

// �`�揈��
void ObjManager::render(ID3D11DeviceContext* dc)
{
    for (Obj* enemy : enemies)
    {
        enemy->render(dc);
    }
}

// �G��o�^
void ObjManager::regist(Obj* enemy)
{
    enemies.emplace_back(enemy);
}

// �G�̑S�폜
void ObjManager::clear()
{
    for (Obj* enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}

//void ObjManager::drawDebugPrimitive()
//{
//    for (Obj* enemy : enemies)
//    {
//        enemy->drawDebugPrimitive();
//    }
//}

void ObjManager::collisionEnemiesAndEnemies()
{
    int enemyCount = getObjCount();
    for (int i = 0; i < enemyCount; i++)
    {
        Obj* EnemyA = enemies.at(i);
        for (int j = i + 1; j < enemyCount; j++)
        {
            Obj* EnemyB = enemies.at(j);

            DirectX::XMFLOAT3 outVec;

            if (Collision::intersectCylinderAndCylinder(
                *EnemyA->getPosition(),
                EnemyA->getRadius(),
                EnemyA->getHeight(),
                *EnemyB->getPosition(),
                EnemyB->getRadius(),
                EnemyB->getHeight(),
                outVec))
            {
                // �Փ˂��Ă���ꍇ�AEnemyB�̈ʒu���C��
                float radius = EnemyA->getRadius() + EnemyB->getRadius();
                DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), radius);
                vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(EnemyA->getPosition()), vec);
                DirectX::XMFLOAT3 pos;
                DirectX::XMStoreFloat3(&pos, vec);
                pos.y = EnemyB->getPosition()->y;
                EnemyB->setPosition(pos);
            }
        }
    }
}

void ObjManager::remove(Obj* enemy)
{
    auto itr = std::find(removes.begin(), removes.end(), enemy);

    // �d�����Ă�����ǉ����Ȃ�
    if (itr != removes.end())
        return;

    // �j�����X�g�ɒǉ�
    removes.push_back(enemy);
}

Obj* ObjManager::getEnemyByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd)
{
    DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&rayStart);
    DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&rayEnd);
    DirectX::XMVECTOR rayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(endVec, startVec));

    float closestDist = FLT_MAX;
    Obj* closestEnemy = nullptr;

    for (int i = 0; i < getObjCount(); ++i)
    {
        Obj* enemy = getObj(i);
        DirectX::XMFLOAT3 enemyPos = *enemy->getPosition();
        DirectX::XMVECTOR enemyVec = DirectX::XMLoadFloat3(&enemyPos);

        DirectX::XMVECTOR toEnemyVec = DirectX::XMVectorSubtract(enemyVec, startVec);
        float t;
        DirectX::XMStoreFloat(&t, DirectX::XMVector3Dot(toEnemyVec, rayDir));

        if (t < 0)
            continue;

        DirectX::XMVECTOR closestPoint = DirectX::XMVectorAdd(startVec, DirectX::XMVectorScale(rayDir, t));
        DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(enemyVec, closestPoint);

        float distSq;
        DirectX::XMStoreFloat(&distSq, DirectX::XMVector3LengthSq(diffVec));

        float radius = enemy->getRadius();
        if (distSq <= radius * radius && t < closestDist)
        {
            closestDist = t;
            closestEnemy = enemy;
        }
    }
    return closestEnemy;
}