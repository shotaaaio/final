#pragma once
#include<DirectXMath.h>

//カメラ操作
class CameraController
{
public:
	CameraController() {}
	~CameraController(){}

	//更新処理
	void update(float elapsedTime,float rot);

	//ターゲット位置設定
	void setTarget(const DirectX::XMFLOAT3& t) { this->target = t; }
	
	//実際に来てほいEyeの位置
	DirectX::XMFLOAT3 computeEye(float rot);
	DirectX::XMFLOAT3 computeEyeTPS();

	//カメラの回転
	void rotateCameraAngle(float elapsedTime);
private:
	DirectX::XMFLOAT3 target = { 0,0,0 }; //ターゲット位置
	DirectX::XMFLOAT3 angle  = { 0.5f,0,0 }; //カメラの角度
	float range = 10.0f;
};