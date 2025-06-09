#include"CameraController.h"
#include"Camera.h"
#include"Input/InputManager.h"
#include"Mathf.h"
#include<algorithm>
#include"Player.h"
#include"Mathf.h"

//更新処理
void CameraController::update(float elapsedTime,float rot)
{
	//カメラの回転
	rotateCameraAngle(elapsedTime);

	//びたびたのカメラ位置
	DirectX::XMFLOAT3 eye = computeEye(rot);

	//ラープで滑らかにする
	eye = Mathf::Leap(eye, target, elapsedTime);

	//カメラの視点と注視点を設定
	Camera::instance()->setLookAt(eye, target, { 0,1,0 });
}

void CameraController::rotateCameraAngle(float elapsedTime)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	float ax = gamePad->getAxisRX(); //JキーとLキー
	float ay = gamePad->getAxisRY(); //IキーとKキー

	//カメラの回転速度
	constexpr float rollSpeed = DirectX::XMConvertToRadians(90);
	float speed = rollSpeed * elapsedTime;

	//スティックに合わせて回転
	angle.x += ay * speed; //ｘ軸回転
	angle.y += ax * speed; //Y軸回転

	constexpr float maxAngle = DirectX::XMConvertToRadians(45);
	constexpr float minAngle = DirectX::XMConvertToRadians(-45);

	//x軸のカメラ回転制限
	if (angle.x < minAngle) angle.x = minAngle;
	if (angle.x > maxAngle) angle.x = maxAngle;

	//Y 軸の回転値を -3.14 ～ 3.14 に収まるよう調整
	if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
	if (angle.y < DirectX::XM_PI) angle.y += DirectX::XM_2PI;
}


DirectX::XMFLOAT3 CameraController::computeEye(float rot)
{
	//カメラ回転値を回転行列に変換
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//カメラの回転行列から前方法ベクトルを取り出す
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, transform.r[2]);

	//注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める(eye = target - front * range)
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	return eye;
}

DirectX::XMFLOAT3 CameraController::computeEyeTPS()
{
	// 回転行列から前方ベクトルを取得（Z軸＝前方）
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, transform.r[2]);

	// Y方向の上下ぶれを無視して、XZ方向だけ使う
	DirectX::XMFLOAT3 flatFront = { front.x, 0.0f, front.z };

	// 正規化しておく（安全）
	DirectX::XMVECTOR vec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&flatFront));
	DirectX::XMStoreFloat3(&flatFront, vec);

	// カメラ距離・高さ設定
	float backDistance = 5.0f;    // 後方距離
	float cameraHeight = 3.0f;    // プレイヤーより高い位置

	// 注視点のY座標（腰位置）＋高さ
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - flatFront.x * backDistance;
	eye.z = target.z - flatFront.z * backDistance;
	eye.y = target.y + cameraHeight;

	return eye;
}