#include"Skymap.h"
#include"Texture.h"
#include"misc.h"
#include"Shader.h"
#include"Framework.h"
#include"Buffer.h"


Skymap::Skymap(ID3D11Device* device, const wchar_t* filename)
{
	//スカイマップ用のテクスチャ及びスプライトを準備
	TextureManager::instance()->loadTextureFromFile(
		device, filename,
		skymap_shader_resource_view.GetAddressOf(), &skymap_texture2d_desc
	);

	skymap_sprite = std::make_unique<Sprite>(device, skymap_shader_resource_view);

	//シェーダーの読みこみ
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ShaderManager* shader = ShaderManager::instance();
	shader->createVsFromCso(device, "Shader/SkymapVS.cso", skymap_vertex_shader.GetAddressOf(), skymap_input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
	shader->createPsFromCso(device, "Shader/SkymapPS.cso", skymap_pixel_shader.GetAddressOf());

	createBuffer<Skymap::sky_map_constants>(device, sky_map_buffer.GetAddressOf());
}

Skymap::~Skymap()
{

}

void Skymap::Render(ID3D11DeviceContext* immediate_context, 
	const DirectX::XMFLOAT3* camera_position, DirectX::XMFLOAT4X4& inverse_view_projection)
{

	//シェーダー設定
	immediate_context->IASetInputLayout(skymap_input_layout.Get());
	immediate_context->VSSetShader(skymap_vertex_shader.Get(),nullptr,0);
	immediate_context->PSSetShader(skymap_pixel_shader.Get(), nullptr, 0);

	//バッファーのバインド
	sky_map_constants smc;
	smc.camera_position.x = camera_position->x;
	smc.camera_position.y = camera_position->y;
	smc.camera_position.z = camera_position->z;
	smc.camera_position.w = 1.0;

	smc.inverse_view_projection = inverse_view_projection;

	bindBuffer(immediate_context, 2, sky_map_buffer.GetAddressOf(), &smc);

	//描画
	skymap_sprite->render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}