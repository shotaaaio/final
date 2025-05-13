#pragma once

#include<d3d11.h>
#include<wrl.h>
#include<DirectXMath.h>

#include<vector>

#include"./Graphics/Texture.h"
#include"./Graphics/ShaderManager.h"

class ParticleSystem
{
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position; //�ʒu
		DirectX::XMFLOAT2 texcoord; //UV
		DirectX::XMFLOAT4 color; //���_�F
		DirectX::XMFLOAT4 param; //�ėp�p�����[�^
	};

	struct ParticleData
	{
		float x, y, z;
		float w, h;
		float aw, ah;
		float vx, vy, vz;
		float ax, ay, az;
		float alpha;
		float timer;
		float anime_timer;
		float type;

		bool anime;
		float anime_speed;
	};

	//�萔�o�b�t�@�̒�`
	struct ConstantBufferForPerFrame
	{
		DirectX::XMFLOAT2 size;
		DirectX::XMFLOAT2 dummy;
	};

	struct geometry_constants
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

public:
	ParticleSystem(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>,
		int komax = 1, int komay = 1, int num = 1000);

	~ParticleSystem();
	void Update(float elapsed_time);

	void Render(ID3D11DeviceContext* immdiate_context,
		const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	void Set(
		int type,
		float time,
		DirectX::XMFLOAT3 p,
		DirectX::XMFLOAT3 v = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(1.0f, 1.0f),
		bool anime = false,
		float anime_speed = 24.0f
	);
	
private:
	ParticleData* data; //�p�[�e�B�N�����
	Vertex* v; //���_�o�b�t�@�������ݏ��
	int num_particles = 0; //�p�[�e�B�N����
	int komax, komay; //Texture�̏c��������

	Microsoft::WRL::ComPtr<ID3D11Buffer>vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>geometry_constant_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>shader_resource_view;
};