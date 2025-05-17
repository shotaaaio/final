#include "SpriteGlitch.hlsli"
#include "math_utils.hlsli"

Texture2D image_map : register(t0);
SamplerState sampl : register(s0);

//Texture2D noize[2] : register(t1);やっても良かったけど、矩形と線分しかつかわないのでpass

//擬似ノイズ生成関数(時間依存)
float hash1(float x)
{
    return frac(sin(x * 12.9898 + 78.233) * 43758.5453);
}

float hash2(float2 uv)
{
    return frac(sin(dot(uv, float2(127.1, 311.7))) * 43758.5453);
}

// ランダム値生成関数（グリッド座標に基づく）
float Random(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

// 矩形ノイズをUVに応じて加算(白)
float3 ApplyRectangularNoise(float2 uv)
{
    float2 gridUV = floor(uv / float2(glitchRectWidth, glitchRectHeight)); // UVを矩形グリッドで分割
    float noise = Random(gridUV); // ランダムなノイズ値を生成

    noise = step(0.5f, noise); // 0.5以上なら1、未満なら0（ランダムなマスク）
    return float3(noise * glitchRectStrength, noise * glitchRectStrength, noise * glitchRectStrength);
}

// 矩形ノイズをUVに応じて加算(UVカラー)
float3 ApplyRectangularNoiseColorShift(float2 uv, float density)
{
    // densityに基づいてノイズをランダムに発生させる
    float noiseFactor = hash1(iTime * density); // 時間とdensityに基づいたノイズ係数

    // グリッド分割
    float2 gridUV = floor(uv / float2(glitchRectWidth, glitchRectHeight));
    
    // densityに基づいて、各チャンネルのオフセットを調整
    float2 offsetR = float2(hash1(dot(gridUV, float2(1.3, 2.1))), hash1(dot(gridUV, float2(3.7, 1.9)))) * noiseFactor * glitchRectStrength;
    float2 offsetG = float2(hash1(dot(gridUV, float2(5.1, 4.3))), hash1(dot(gridUV, float2(2.9, 6.5)))) * noiseFactor * glitchRectStrength;
    float2 offsetB = float2(hash1(dot(gridUV, float2(7.3, 8.1))), hash1(dot(gridUV, float2(9.4, 1.7)))) * noiseFactor * glitchRectStrength;

    // 各チャンネルを個別にずらしてサンプリング
    float r = image_map.Sample(sampl, uv + offsetR).r;
    float g = image_map.Sample(sampl, uv + offsetG).g;
    float b = image_map.Sample(sampl, uv + offsetB).b;

    return float3(r, g, b);
}


// ---- グリッチ用ピクセルシェーダ本体 ----
float4 main(VS_OUT pin) : SV_TARGET
{
    // density に基づいて時間をスケール
    float controlledTime = iTime * density;

    // スケール済み時間を用いて乱数sを生成
    float s = hash1(controlledTime);

    float2 uv = pin.texcoord.xy;
    float4 source_color = image_map.Sample(sampl, uv);

    // 線ずらし
    uv = interlaceY(image_map, uv, s * shift);

    // ランダムUVずらし
    uv += float2(hash2(uv + controlledTime) * rand_float, 0);

    // 色ずれ処理
    float3 color = colorSplit(image_map, sampl, uv, float2(s * x_shifting, y_shifting));

    // 明るさブレンド
    color = lerp(color, hash2(float2(extension + controlledTime * uv_slider, 0.0)).xxx, brightness);

    // 矩形ノイズ追加
    float3 rectNoise = ApplyRectangularNoiseColorShift(uv, density);
    color += rectNoise;

    // 中心からのフェードマスク
    float mask = saturate(length(pin.texcoord - center) / glitch_mask_radius);
    return lerp(source_color, float4(color, source_color.a), mask);
    
      //緑色成分だけを透明化
    //source_color.rgb のうち緑色と判断できる範囲を条件化
   // float isGreen = step(0.6, source_color.g) * step(source_color.r, 0.3) * step(source_color.b, 0.3);

    //緑だったらalphaを0にする
    //float alpha = lerp(source_color.a, 0.0, isGreen);
  
}
