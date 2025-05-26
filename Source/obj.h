//#pragma once
//
//#include <DirectXMath.h>
//#include "StageManager.h"
//
//// �L�����N�^�[
//class Obj
//{
//public:
//    Obj() {}
//    virtual ~Obj() {}
//
//    // �X�V����
//    virtual void update(float elapsedTime) = 0;
//
//    // �`�揈��
//    virtual void render(ID3D11DeviceContext* dc) = 0;
//
//    // �j��
//    void destroy();
//
//    // �s��X�V����
//    void updateTransform();
//
//    // �ʒu�擾
//    const DirectX::XMFLOAT3* getPosition() const { return &position; }
//
//    void setPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
//
//    const DirectX::XMFLOAT3* getAngle() const { return &angle; }
//
//    void setAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
//
//    const DirectX::XMFLOAT3* getScale() const { return &scale; }
//
//    void setScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
//
//    // ���a�̎擾
//    float getRadius() const { return radius; }
//
//    // ���n�������ǂ����̃t���O�̎擾
//    bool isGrounded() const { return groundedFlag; }
//
//    // �����擾
//    float getHeight() const { return height; }
//
//    void setHeight(float height) { this->height = height; }
//
//    // �Ռ���^����
//    void addImpulse(const DirectX::XMFLOAT3& impulse);
//
//protected:
//    // ���x�����X�V
//    void updateVelocity(float elpasedTime);
//
//private:
//    // �������x�X�V����
//    void updateVerticalVelocity(float elapsedTime);
//
//    // �����ړ��X�V����
//    void updateVerticalMove(float elapsedTime);
//
//    // �������x�X�V����
//    void updateHorizontalVelocity(float elapsedTime);
//
//    // �����ړ��X�V����
//    void updateHorizontalMove(float elapsedTime);
//
//protected:
//    DirectX::XMFLOAT3 position = { 0,0,0 };	// �ʒu
//    DirectX::XMFLOAT3 angle = { 0,0,0 };	// ��]
//    DirectX::XMFLOAT3 scale = { 1,1,1 };	// �g��k��
//
//    // �p���s��
//    DirectX::XMFLOAT4X4	transform = {
//        1,0,0,0,
//        0,1,0,0,
//        0,0,1,0,
//        0,0,0,1
//    };
//    float radius = 0.5f; // ���a
//    float gravity = -1.0f;	// �d�́i�t���[���P�ʂ̒l�j
//    DirectX::XMFLOAT3 velocity = { 0,0,0 };	// ���x
//    bool groundedFlag = false;	// true...���n����
//    float height = 2.0f;	// ����
//    int health = 5; // ���C�t�i���̃��C�t�Ƃ��ĂT�j
//    int maxHealth = 5;	// �ő僉�C�t
//    float friction = 0.5f; // ���C
//    float acceleration = 1.0f;  // �����x
//    float maxMoveSpeed = 5.0f;  // �ő�ړ����x
//    DirectX::XMFLOAT3 direction = { 0,0,0 };    // �ړ�����
//    float airControl = 0.3f;    // �󒆐���p�W��
//    float slopeRate = 1.0f; // �X�Η��i�f�t�H���g = 1.0f�j
//
//private:
//
//};