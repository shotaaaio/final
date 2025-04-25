#include "Ivy.h"
#include "DeviceManager.h"
#include "Input/InputManager.h"
#include "StraightBullet.h"
#include "HomingBullet.h"
//#include "PlayerManager.h"
#include "Player.h"

Ivy::Ivy()
{
    model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Obj\\plants1.mdl");

    // ���f�����傫���̂ŃX�P�[������
    scale = { 0.001f, 0.002f, 0.002f };

    radius = 0.5f;  // ��
    height = 1.0f;  // ����
}

Ivy::~Ivy()
{
    if (model != nullptr)
    {
        delete model;
        model = nullptr;
    }
}

// �X�V����
void Ivy::update(float elapsedTime)
{
    // ���x�����X�V
    updateVelocity(elapsedTime);

    //inputLaunchBullet();

    // ���[���h�s��̍X�V
    updateTransform();
}

// �`�揈��
void Ivy::render(ID3D11DeviceContext* dc)
{
    model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}