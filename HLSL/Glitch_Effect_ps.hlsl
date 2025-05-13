#include "Glitch_Effect.hlsli"
#include "math_utils.hlsli"

Texture2D image_map : register(t0);
SamplerState sampl : register(s0);

//Texture2D noize[2] : register(t1);����Ă��ǂ��������ǁA��`�Ɛ�����������Ȃ��̂�pass

//�[���m�C�Y�����֐�(���Ԉˑ�)
float hash1(float x)
{
    return frac(sin(x * 12.9898 + 78.233) * 43758.5453);
}

float hash2(float2 uv)
{
    return frac(sin(dot(uv, float2(127.1, 311.7))) * 43758.5453);
}

// �����_���l�����֐��i�O���b�h���W�Ɋ�Â��j
float Random(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

// ��`�m�C�Y��UV�ɉ����ĉ��Z(��)
float3 ApplyRectangularNoise(float2 uv)
{
    float2 gridUV = floor(uv / float2(glitchRectWidth, glitchRectHeight)); // UV����`�O���b�h�ŕ���
    float noise = Random(gridUV); // �����_���ȃm�C�Y�l�𐶐�

    noise = step(0.5f, noise); // 0.5�ȏ�Ȃ�1�A�����Ȃ�0�i�����_���ȃ}�X�N�j
    return float3(noise * glitchRectStrength, noise * glitchRectStrength, noise * glitchRectStrength);
}

// ��`�m�C�Y��UV�ɉ����ĉ��Z(UV�J���[)
float3 ApplyRectangularNoiseColorShift(float2 uv, float density)
{
    // density�Ɋ�Â��ăm�C�Y�������_���ɔ���������
    float noiseFactor = hash1(iTime * density); // ���Ԃ�density�Ɋ�Â����m�C�Y�W��

    // �O���b�h����
    float2 gridUV = floor(uv / float2(glitchRectWidth, glitchRectHeight));
    
    // density�Ɋ�Â��āA�e�`�����l���̃I�t�Z�b�g�𒲐�
    float2 offsetR = float2(hash1(dot(gridUV, float2(1.3, 2.1))), hash1(dot(gridUV, float2(3.7, 1.9)))) * noiseFactor * glitchRectStrength;
    float2 offsetG = float2(hash1(dot(gridUV, float2(5.1, 4.3))), hash1(dot(gridUV, float2(2.9, 6.5)))) * noiseFactor * glitchRectStrength;
    float2 offsetB = float2(hash1(dot(gridUV, float2(7.3, 8.1))), hash1(dot(gridUV, float2(9.4, 1.7)))) * noiseFactor * glitchRectStrength;

    // �e�`�����l�����ʂɂ��炵�ăT���v�����O
    float r = image_map.Sample(sampl, uv + offsetR).r;
    float g = image_map.Sample(sampl, uv + offsetG).g;
    float b = image_map.Sample(sampl, uv + offsetB).b;

    return float3(r, g, b);
}


// �t���b�V���i���]�G�t�F�N�g�j��K�p
float3 ApplyFlash(float3 color)
{
    float flashPhase = frac(iTime / flashFrequency); // ���݂̃t���b�V���������̈ʒu�i0?1�j
    if (flashPhase < flashStrength)
    {
        return 1.0f - color; // �w�芄���̊Ԃ�����ʐF�𔽓]
    }
    return color;
}

// ---- �O���b�`�p�s�N�Z���V�F�[�_�{�� ----
float4 main(VS_OUT pin) : SV_TARGET
{
    float t = iTime;

    // �[���m�C�Y�ɂ�闐���l s �𐶐�
    float s = hash1(t * density);

    float2 uv = pin.texcoord.xy;
    float4 source_color = image_map.Sample(sampl, uv);
    if (source_color.a < 0)
        return source_color;

    // �����炵
    uv = interlaceY(image_map, uv, s * shift);

    // �����_��UV���炵
    uv += float2(hash2(uv + t) * rand_float, 0);

    // �F���ꏈ��
    float3 color = colorSplit(image_map, sampl, uv, float2(s * x_shifting, y_shifting));

    // ���邳�u�����h�ihash2�x�[�X�j
    color = lerp(color, hash2(float2(extension + t * uv_slider, 0.0)).xxx, brightness);

    // ��`�m�C�Y�����Z
    float3 rectNoise = ApplyRectangularNoiseColorShift(uv, density);
    color += rectNoise;

    // �t���b�V�������i��ʔ��]�j
    color = ApplyFlash(color);

    // ���S����̋����Ńt�F�[�h
    float mask = saturate(length(pin.texcoord - center) / glitch_mask_radius);
    return lerp(source_color, float4(color, source_color.a), mask);
}