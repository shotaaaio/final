#pragma once
#include<DirectXMath.h>
#include"Graphics/SkinnedMesh.h"

class Object
{
public:
	Object(){}
	virtual ~Object(){}

	//行列更新処理
	virtual void updateTransform();

	//位置取得
	const DirectX::XMFLOAT3* getPosition()const { return &position; }
	//位置設定
	void setPosition(const DirectX::XMFLOAT3& p) { this->position = p; }

	//角度取得
	const DirectX::XMFLOAT3* getAngle()const { return &angle; }
	//角度設定
	void setAngle(const DirectX::XMFLOAT3& a) { this->angle = a; }

	//拡大縮小取得
	const DirectX::XMFLOAT3* getScale()const { return &scale; }
	//拡大縮小設定
	void setScale(DirectX::XMFLOAT3& s) { this->scale = s; }

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 }; //位置
	DirectX::XMFLOAT3 angle = { 0,0,0 };	//回転
	DirectX::XMFLOAT3 scale = { 1,1,1 };	//拡大縮小

	// 姿勢行列
	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	//モデル
	std::unique_ptr<SkinnedMesh>model;
};