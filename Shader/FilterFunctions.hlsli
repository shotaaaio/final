//--------------------------------------------
//	RGB色空間の数値から輝度値への変換関数
//--------------------------------------------
//rgb:RGB色空間の数値
float RGB2Luminance(float3 rgb)
{
    static const float3 luminanceValue = float3(0.299f, 0.587f, 0.114f);
    return dot(luminanceValue, rgb);
}
