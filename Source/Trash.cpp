#include "Trash.h"
#include "DeviceManager.h"
#include "Input/InputManager.h"
#include "StraightBullet.h"
#include "HomingBullet.h"
//#include "PlayerManager.h"
#include "Player.h"

Trash::Trash()
{
    model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Obj\\trash.fbx");

    // モデルが大きいのでスケール調整
    scale = { 0.05f, 0.05f, 0.05f };

    radius = 0.5f;  // 幅
    height = 1.0f;  // 高さ
}

Trash::~Trash()
{
    if (model != nullptr)
    {
        delete model;
        model = nullptr;
    }
}

// 更新処理
void Trash::update(float elapsedTime)
{
    // 速度処理更新
    updateVelocity(elapsedTime);

    //inputLaunchBullet();

    // ワールド行列の更新
    updateTransform();
}

// 描画処理
void Trash::render(ID3D11DeviceContext* dc)
{
    model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}