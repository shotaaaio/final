#pragma once
#include<DirectXMath.h>

//���w�n�̊֐��B
class Mathf
{
public:
	//���`�⊮
	static float Lerp(float a, float b, float t);
	static DirectX::XMFLOAT3 Leap(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, float t);

	//����
	static float Proportion(int a, int b);

	//�w��͈͂̃����_���l���v�Z
	static float RandomRange(float min, float max);

	//�O�����̎擾
	static DirectX::XMFLOAT3 GetForwardDirection(float angleY);
};