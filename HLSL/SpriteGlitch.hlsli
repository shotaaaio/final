struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};


cbuffer GLITCH_SHADER_CONSTANT_BUFFER : register(b0)
{
    float iTime;         //経過時間
    float density;       //密度
    float shift;         //ずらす量
    float line_inpact;   //一本線の影響度
    float2 x_shift;      //xの横をどの程度ずらすのか
    float2 y_shift;      //yの横をどの程度ずらすのか
    float rand_float;    //ランダム性
    float x_shifting;    //色収差のxの位置をずらす
    float y_shifting;    //色収差のyの位置をずらす
    float extension;     //uvの拡張
    float x_gage;        //ゲージのx幅色調の変化
    float uv_slider;     //左上に流す
    float brightness;    //明るさ
    float glitch_mask_radius;  //マスク半径
    int glitch_sampling_count; //サンプリング回数
    float2 center;             //中心
    float gage;
    float glitchRectWidth;     // 矩形ノイズの幅（UV空間でのサイズ）
    float glitchRectHeight;    // 矩形ノイズの高さ（UV空間でのサイズ）
    float glitchRectStrength;  // 矩形ノイズの強度（明るさへの影響度）
  
};