//--------------------------------------------
//	RGB�F��Ԃ̐��l����P�x�l�ւ̕ϊ��֐�
//--------------------------------------------
//rgb:RGB�F��Ԃ̐��l
float RGB2Luminance(float3 rgb)
{
    static const float3 luminanceValue = float3(0.299f, 0.587f, 0.114f);
    return dot(luminanceValue, rgb);
}
