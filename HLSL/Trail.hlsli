//// Trail.hlsli
//#ifndef TRAIL_HLSLI
//#define TRAIL_HLSLI

//// ���_�V�F�[�_�[�̓��͍\����
//struct VS_INPUT
//{
//    float3 Position : POSITION; // ���_�ʒu
//    float3 Normal :   NORMAL;   // ���_�@���i�c�ݕ����p�j
//    float4 Color :    COLOR;    // ���_�F
//    float2 TexCoord : TEXCOORD0;// �e�N�X�`�����W
//};

//// ���_�V�F�[�_�[�̏o�͍\���́^�s�N�Z���V�F�[�_�[�̓���
//struct VS_OUT
//{
//    float4 Pos :     SV_POSITION;// �N���b�v��Ԉʒu
//    float4 Color :   COLOR;     // �C���^�[�|���[�g�����F
//    float2 TexCoord :TEXCOORD0; // �C���^�[�|���[�g����� UV
//    float3 Normal :  NORMAL;    // �@���i�c�݌v�Z�p�j
//    float3 WorldPos :TEXCOORD1; // ���E���W�ʒu
//};

//// �萔�o�b�t�@�p�\��
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
