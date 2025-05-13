#pragma once
#include<DirectXMath.h>
#include"Graphics/SkinnedMesh.h"
#include "Graphics/Sprite.h"

class Object
{
public:
	Object() {}
	virtual ~Object() {}

	//çsóÒçXêVèàóù
	virtual void updateTransform();

	//à íuéÊìæ
	const DirectX::XMFLOAT3* getPosition()const { return &position; }
	//à íuê›íË
	void setPosition(const DirectX::XMFLOAT3& p) { this->position = p; }

	//äpìxéÊìæ
	const DirectX::XMFLOAT3* getAngle()const { return &angle; }
	//äpìxê›íË
	void setAngle(const DirectX::XMFLOAT3& a) { this->angle = a; }

	//ägëÂèkè¨éÊìæ
	const DirectX::XMFLOAT3* getScale()const { return &scale; }
	//ägëÂèkè¨ê›íË
	void setScale(DirectX::XMFLOAT3& s) { this->scale = s; }

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 }; //à íu
	DirectX::XMFLOAT3 angle = { 0,0,0 };	//âÒì]
	DirectX::XMFLOAT3 scale = { 1,1,1 };	//ägëÂèkè¨

	// épê®çsóÒ
	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	//ÉÇÉfÉã
	std::unique_ptr<SkinnedMesh>model;

	std::unique_ptr<Sprite> shizukuImage;

	std::unique_ptr<Sprite> wakuImage;
};