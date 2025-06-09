//// Trail.hlsli
//#ifndef TRAIL_HLSLI
//#define TRAIL_HLSLI

//// 頂点シェーダーの入力構造体
//struct VS_INPUT
//{
//    float3 Position : POSITION; // 頂点位置
//    float3 Normal :   NORMAL;   // 頂点法線（歪み方向用）
//    float4 Color :    COLOR;    // 頂点色
//    float2 TexCoord : TEXCOORD0;// テクスチャ座標
//};

//// 頂点シェーダーの出力構造体／ピクセルシェーダーの入力
//struct VS_OUT
//{
//    float4 Pos :     SV_POSITION;// クリップ空間位置
//    float4 Color :   COLOR;     // インターポレートされる色
//    float2 TexCoord :TEXCOORD0; // インターポレートされる UV
//    float3 Normal :  NORMAL;    // 法線（歪み計算用）
//    float3 WorldPos :TEXCOORD1; // 世界座標位置
//};

//// 定数バッファ用構造
//cbuffer TrailCB : register(b0)
//{
//    row_major float4x4 View;
//    row_major float4x4 Projection;
    
//    float Time;
//    float TrailWidth;
//    float HueOffset;
//    float AlphaStart;
  
//    float AlphaEnd;
//    float DistortionPower;
//    float NoiseAmount;

//    float dummy;
//};

//float3 HSVtoRGB(float h, float s, float v)
//{
//    float c = v * s;
//    float x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
//    float m = v - c;

//    float3 rgb;

//    if (h < 60)
//        rgb = float3(c, x, 0);
//    else if (h < 120)
//        rgb = float3(x, c, 0);
//    else if (h < 180)
//        rgb = float3(0, c, x);
//    else if (h < 240)
//        rgb = float3(0, x, c);
//    else if (h < 300)
//        rgb = float3(x, 0, c);
//    else
//        rgb = float3(c, 0, x);

//    return rgb + m;
//}

//#endif // TRAIL_HLSLI
