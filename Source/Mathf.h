#pragma once
#include<DirectXMath.h>

//数学系の関数達
class Mathf
{
public:
	//線形補完
	static float Lerp(float a, float b, float t);
	static DirectX::XMFLOAT3 Leap(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, float t);

	//割合
	static float Proportion(int a, int b);

	//指定範囲のランダム値を計算
	static float RandomRange(float min, float max);

	//前方向の取得
	static DirectX::XMFLOAT3 GetForwardDirection(float angleY);
};