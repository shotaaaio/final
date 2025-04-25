#include "Goal.h"
#include "DeviceManager.h"
#include "Input/InputManager.h"
#include "StraightBullet.h"
#include "HomingBullet.h"
#include "Player.h"

Goal::Goal()
{
    model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Obj\\Flag.fbx");

    // ���f�����傫���̂ŃX�P�[������
    scale = { 0.007f, 0.007f, 0.007f };

    radius = 0.5f;  // ��
    height = 1.0f;  // ����
}

Goal::~Goal()
{
    if (model != nullptr)
    {
        delete model;
        model = nullptr;
    }
}

// �X�V����
void Goal::update(float elapsedTime)
{
    // ���x�����X�V
    //updateVelocity(elapsedTime);

    //inputLaunchBullet();

    // ���[���h�s��̍X�V
    updateTransform();
}

// �`�揈��
void Goal::render(ID3D11DeviceContext* dc)
{
    model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}