struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};


cbuffer GLITCH_SHADER_CONSTANT_BUFFER : register(b0)
{
    float iTime;         //�o�ߎ���
    float density;       //���x
    float shift;         //���炷��
    float line_inpact;   //��{���̉e���x
    float2 x_shift;      //x�̉����ǂ̒��x���炷�̂�
    float2 y_shift;      //y�̉����ǂ̒��x���炷�̂�
    float rand_float;    //�����_����
    float x_shifting;    //�F������x�̈ʒu�����炷
    float y_shifting;    //�F������y�̈ʒu�����炷
    float extension;     //uv�̊g��
    float x_gage;        //�Q�[�W��x���F���̕ω�
    float uv_slider;     //����ɗ���
    float brightness;    //���邳
    float glitch_mask_radius;  //�}�X�N���a
    int glitch_sampling_count; //�T���v�����O��
    float2 center;             //���S
    float gage;
    float glitchRectWidth;     // ��`�m�C�Y�̕��iUV��Ԃł̃T�C�Y�j
    float glitchRectHeight;    // ��`�m�C�Y�̍����iUV��Ԃł̃T�C�Y�j
    float glitchRectStrength;  // ��`�m�C�Y�̋��x�i���邳�ւ̉e���x�j
  
};