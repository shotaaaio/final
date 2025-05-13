#include "Misc.h"
#include "Glitch_Effect_Shader.h"
#include "Graphics/Shader.h"
#include "Graphics/Buffer.h"
GlitchEffectShader::GlitchEffectShader(ID3D11Device* device)
{
	// 頂点シェーダー
	{
		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	
		ShaderManager* shader = ShaderManager::instance();
		shader->createVsFromCso(device, "Shader/Glitch_Effect_vs.cso", Glitch_Vertex_Shader.GetAddressOf(),Glitch_Input_Layout.GetAddressOf(), inputElementDesc,_countof(inputElementDesc));
		shader->createPsFromCso(device, "Shader/Glitch_Effect_ps.cso", Glitch_Pixel_Shader.GetAddressOf());

		createBuffer<GlitchEffectShader::glitch_shader_constants>(device, Glitch_Constant_Buffer.GetAddressOf());
	}
}

void GlitchEffectShader::Render(ID3D11DeviceContext* idc, const DirectX::XMFLOAT3* camera_position, DirectX::XMFLOAT4X4 inverse_view_projection)
{
	idc

}

