#pragma once
#include"Object.h"

//�e�Ǘ��̑O���錾
class BulletManager;

//�e
class Bullet : public Object
{
public:
	Bullet(BulletManager* manager);
	virtual ~Bullet(){}

	//�X�V����
	virtual void update(float elapedTime) = 0;

	//�`�揈��
	virtual void render(ID3D11DeviceContext* dc) = 0;

	//�f�o�b�N�v���~�e�B�u�`��
	virtual void drawDebugPrimitive();

	void updateTransform() override;

	//�����擾
	const DirectX::XMFLOAT3* getDirection() { return &direction; }

	//�����̐ݒ�
	void setDirection(const DirectX::XMFLOAT3& d) { this->direction = d; }

	//���a�擾
	float getRadius()const { return radius; }
	//�j��
	void destroy();
protected:
	DirectX::XMFLOAT3 direction{ 0,0,1 }; //����

	//�e�Ǘ�
	BulletManager* manager;

	//���a
	float radius = 0.5;
};