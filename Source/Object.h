#pragma once
#include<DirectXMath.h>
#include"Graphics/SkinnedMesh.h"
#include "Graphics/Sprite.h"

class Object
{
public:
	Object() {}
	virtual ~Object() {}

	//�s��X�V����
	virtual void updateTransform();

	//�ʒu�擾
	const DirectX::XMFLOAT3* getPosition()const { return &position; }
	//�ʒu�ݒ�
	void setPosition(const DirectX::XMFLOAT3& p) { this->position = p; }

	//�p�x�擾
	const DirectX::XMFLOAT3* getAngle()const { return &angle; }
	//�p�x�ݒ�
	void setAngle(const DirectX::XMFLOAT3& a) { this->angle = a; }

	//�g��k���擾
	const DirectX::XMFLOAT3* getScale()const { return &scale; }
	//�g��k���ݒ�
	void setScale(DirectX::XMFLOAT3& s) { this->scale = s; }

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 }; //�ʒu
	DirectX::XMFLOAT3 angle = { 0,0,0 };	//��]
	DirectX::XMFLOAT3 scale = { 1,1,1 };	//�g��k��

	// �p���s��
	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	//���f��
	std::unique_ptr<SkinnedMesh>model;

	std::unique_ptr<Sprite> shizukuImage;

	std::unique_ptr<Sprite> wakuImage;
};