#include"Mathf.h"
#include<stdlib.h>

float Mathf::Lerp(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}

DirectX::XMFLOAT3 Mathf::Leap(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b,float t)
{
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&a), DirectX::XMLoadFloat3(&b), t));
    return result;
}

float Mathf::Proportion(int a, int b)
{
    return a / (a + b);
}


//�w��͈͂̃����_���l���v�Z����
float Mathf::RandomRange(float min, float max)
{
    // 0����1�̊Ԃ̃����_���Ȓl�𐶐�
    float random01 = static_cast<float>(rand()) / RAND_MAX;

    // min����max�͈̔͂̃����_���Ȓl���v�Z���ĕԂ�
    return min + (max - min) * random01;
}

//�O�����̎擾
DirectX::XMFLOAT3 Mathf::GetForwardDirection(float angleY)
{
    DirectX::XMFLOAT3 dir;
    dir.x = sinf(angleY);
    dir.y = 0.0f;
    dir.z = cosf(angleY);
    return dir;
}