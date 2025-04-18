#include"Skymap.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float4 color : COLOR,
	float2 texcoord : TEXCOORD)
{
	VS_OUT vout;
	vout.position = position;
	vout.color = color;
	vout.texcoord = texcoord;

	//  position�ɂ킽���Ă�����W��NDC��Ԃ̍��W�Ȃ̂ŁA�A
	//  �����~���e�s��̋t�s����g���ă��[���h��Ԃ̍��W�ɕϊ�����
	position = mul(position, inverse_view_projection);
	vout.world_position = position / position.w;
	return vout;
}