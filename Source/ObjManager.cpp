#include "ObjManager.h"
#include "Collision.h"

// �X�V����
void ObjManager::update(float elapsedTime)
{
    for (Obj* enemy : objs)
    {
        enemy->update(elapsedTime);
    }

    // �j������
    for (Obj* bullet : removes)
    {
        auto itr = std::find(objs.begin(), objs.end(), bullet);
        if (itr != objs.end())
        {
            // �j�����ׂ� bullet �� bullets ���ɑ��݂���ꍇ�A�폜
            delete bullet;
            objs.erase(itr);
        }
    }
    // �j�����X�g���N���A
    removes.clear();
    collisionEnemiesAndEnemies();
}

// �`�揈��
void ObjManager::render(ID3D11DeviceContext* dc)
{
    for (Obj* obj : objs)
    {
        obj->render(dc);
    }
}

// �G��o�^
void ObjManager::regist(Obj* obj)
{
    objs.emplace_back(obj);
}

// �G�̑S�폜
void ObjManager::clear()
{
    for (Obj* obj : objs)
    {
        delete obj;
    }
    objs.clear();
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
    int objCount = getObjCount();
    for (int i = 0; i < objCount; i++)
    {
        Obj* objA = objs.at(i);
        for (int j = i + 1; j < objCount; j++)
        {
            Obj* objB = objs.at(j);

            DirectX::XMFLOAT3 outVec;

            if (Collision::intersectCylinderAndCylinder(
                *objA->getPosition(),
                objA->getRadius(),
                objA->getHeight(),
                *objB->getPosition(),
                objB->getRadius(),
                objB->getHeight(),
                outVec))
            {
                // �Փ˂��Ă���ꍇ�AEnemyB�̈ʒu���C��
                float radius = objA->getRadius() + objB->getRadius();
                DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), radius);
                vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(objA->getPosition()), vec);
                DirectX::XMFLOAT3 pos;
                DirectX::XMStoreFloat3(&pos, vec);
                pos.y = objB->getPosition()->y;
                objB->setPosition(pos);
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

Obj* ObjManager::getObjByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd)
{
    DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&rayStart);
    DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&rayEnd);
    DirectX::XMVECTOR rayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(endVec, startVec));

    float closestDist = FLT_MAX;
    Obj* closestobj = nullptr;

    for (int i = 0; i < getObjCount(); ++i)
    {
        Obj* obj = getObj(i);
        DirectX::XMFLOAT3 objPos = *obj->getPosition();
        DirectX::XMVECTOR objVec = DirectX::XMLoadFloat3(&objPos);

        DirectX::XMVECTOR toobjVec = DirectX::XMVectorSubtract(objVec, startVec);
        float t;
        DirectX::XMStoreFloat(&t, DirectX::XMVector3Dot(toobjVec, rayDir));

        if (t < 0)
            continue;

        DirectX::XMVECTOR closestPoint = DirectX::XMVectorAdd(startVec, DirectX::XMVectorScale(rayDir, t));
        DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(objVec, closestPoint);

        float distSq;
        DirectX::XMStoreFloat(&distSq, DirectX::XMVector3LengthSq(diffVec));

        float radius = obj->getRadius();
        if (distSq <= radius * radius && t < closestDist)
        {
            closestDist = t;
            closestobj = obj;
        }
    }
    return closestobj;
}