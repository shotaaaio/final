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

    // ���f�����傫���̂ŃX�P�[������
    scale = { 0.05f, 0.05f, 0.05f };

    radius = 0.5f;  // ��
    height = 1.0f;  // ����
}

Trash::~Trash()
{
    if (model != nullptr)
    {
        delete model;
        model = nullptr;
    }
}

// �X�V����
void Trash::update(float elapsedTime)
{
    // ���x�����X�V
    updateVelocity(elapsedTime);

    //inputLaunchBullet();

    // ���[���h�s��̍X�V
    updateTransform();
}

// �`�揈��
void Trash::render(ID3D11DeviceContext* dc)
{
    model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}