//#include "Trail.hlsli"
//VS_OUT main(VS_INPUT input)
//{
//    VS_OUT output;

//    //���[���h��Ԉʒu�i���E�s��� Identity �Ƃ��đz��j
//    float3 worldPos = input.Position;

//    //�m�C�Y�ɉ����āA�@�������փI�t�Z�b�g
//    float noise = sin(worldPos.y * NoiseAmount + Time);
//    worldPos += normalize(input.Normal) * noise * DistortionPower;

//    //�g���C���̕��𒲐�
//    worldPos.x += input.TexCoord.x * TrailWidth;

//    //�r���[�^�v���W�F�N�V�����ϊ�
//    float4 viewPos = mul(float4(worldPos, 1.0f), View);
//    output.Pos = mul(viewPos, Projection);

//    //�F���I�t�Z�b�g��K�p
//    float hueShift = HueOffset * input.TexCoord.y;
//    float3 shiftedColor = HSVtoRGB(hueShift, 1.0f, 1.0f); // HSV����RGB�ɕϊ�
//    output.Color = float4(shiftedColor * input.Color.rgb, input.Color.a);

//    //UV���W�͂��̂܂ܒʂ�
//    output.TexCoord = input.TexCoord;

//    //�@���� WorldPos ��n���Ă����i�K�v�Ȃ� PS �ŗ��p�\�j
//    output.Normal = input.Normal;
//    output.WorldPos = worldPos;

//    return output;
//}