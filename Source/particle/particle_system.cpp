#include"particle_system.h"
#include"Graphics/Buffer.h"

ParticleSystem::ParticleSystem(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view, int komax, int komay, int num)
	: komax(komax)
	, komay(komay)
	, num_particles(num)
{
	HRESULT hr;

	//	�p�[�e�B�N����񃊃X�g
	data = new ParticleData[num];
	ZeroMemory(data, sizeof(ParticleData) * num);

	//	���_��񃊃X�g
	v = new Vertex[num];
	ZeroMemory(v, sizeof(Vertex) * num);

	for (int i = 0; i < num_particles; i++) { data[i].type = -1; }

	//	�p�[�e�B�N���p�摜���[�h
	this->shader_resource_view = shader_resource_view;

	//	���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//	���_�����̃o�b�t�@
	bd.ByteWidth = sizeof(Vertex) * num_particles;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;


	hr = device->CreateBuffer(&bd, NULL, vertex_buffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	createBuffer<ParticleSystem::ConstantBufferForPerFrame>(device, constant_buffer.GetAddressOf());
	createBuffer<ParticleSystem::geometry_constants>(device, geometry_constant_buffer.GetAddressOf());

	//	���_�V�F�[�_�[
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PARAMETER", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	ShaderManager::instance()->createVsFromCso(device, "Shader/geometry_particle_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));

	//	�W�I���g���V�F�[�_�[
	ShaderManager::instance()->createGsFromCso(device, "Shader/geometry_particle_gs.cso", geometry_shader.ReleaseAndGetAddressOf());

	//	�s�N�Z���V�F�[�_�[
	ShaderManager::instance()->createPsFromCso(device, "Shader/geometry_particle_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
}


ParticleSystem::~ParticleSystem()
{
	delete[]data;
	delete[]v;
}

void ParticleSystem::Update(float elapsedTime)
{
	for (int i = 0;i < num_particles;i++)
	{
		if (data[i].type < 0)continue;

		data[i].vx += data[i].ax * elapsedTime;
		data[i].vy += data[i].ay * elapsedTime;
		data[i].vz += data[i].az * elapsedTime;

		data[i].x += data[i].vx * elapsedTime;
		data[i].y += data[i].vy * elapsedTime;
		data[i].z += data[i].vz * elapsedTime;

		data[i].timer -= elapsedTime;
		data[i].alpha = (sqrtf(data[i].timer));

		//�A�j��
		if (data[i].anime)
			data[i].type += elapsedTime * data[i].anime_speed;//speed�R�}/�b

		//�I������
		if (data[i].timer <= 0)
			data[i].type = -1;
	}
}

void ParticleSystem::Render(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	//�萔�o�b�t�@
	ConstantBufferForPerFrame cb;
	cb.size = { 0.1f,0.1f };
	bindBuffer(immediate_context, 3, constant_buffer.GetAddressOf(), &cb);
	immediate_context->GSSetConstantBuffers(3, 1, constant_buffer.GetAddressOf());

	geometry_constants gc;
	gc.view = view;
	gc.projection = projection;
	bindBuffer(immediate_context, 4, geometry_constant_buffer.GetAddressOf(), &gc);
	//immediate_context->GSSetConstantBuffers(4, 1, constant_buffer.GetAddressOf());

	//�_�`��ݒ�
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//�V�F�[�_�[�ݒ�
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->GSSetShader(geometry_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	//���̓��C�A�E�g
	immediate_context->IASetInputLayout(input_layout.Get());

	//�e�N�X�`���ݒ�
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

	//�p�[�e�B�N�����𒸓_�o�b�t�@
	int n = 0;//�p�[�e�B�N��������
	for (int i = 0;i < num_particles;i++)
	{
		if (data[i].type < 0)continue;

		v[n].position.x = data[i].x;
		v[n].position.y = data[i].y;
		v[n].position.z = data[i].z;
		v[n].texcoord.x = data[i].w;
		v[n].texcoord.y = data[i].h;
		v[n].color.x = v[n].color.y = v[n].color.z = 1.0f;
		v[n].color.w = data[i].alpha;

		v[n].param.x = 0;
		v[n].param.y = data[i].type;
		v[n].param.z = (float)komax; //���R�}��
		v[n].param.w = (float)komay; //�c�R�}��
		++n;
	}

	//���_�f�[�^�X�V
	immediate_context->UpdateSubresource(vertex_buffer.Get(), 0, nullptr, v, 0, 0);

	//�p�[�e�B�N���o�b�t�@�[���Z�b�g
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//�p�[�e�B�N����񕪕`��R�[��
	immediate_context->Draw(n, 0);

	//	�V�F�[�_������
	immediate_context->VSSetShader(nullptr, nullptr, 0);
	immediate_context->GSSetShader(nullptr, nullptr, 0);
	immediate_context->PSSetShader(nullptr, nullptr, 0);
}

void ParticleSystem::Set(int type, float timer, 
	DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v,
	DirectX::XMFLOAT3 f, DirectX::XMFLOAT2 size,
	bool anime, float anime_speed)
{
	for (int i = 0;i < num_particles;i++)
	{
		if (data[i].type >= 0)continue;
		data[i].type = (float)type;
		data[i].x = p.x;
		data[i].y = p.y;
		data[i].z = p.z;
		data[i].vx = v.x;
		data[i].vy = v.y;
		data[i].vz = v.z;
		data[i].ax = f.x;
		data[i].ay = f.y;
		data[i].az = f.z;
		data[i].w = size.x;
		data[i].h = size.y;
		data[i].alpha = 1.0f;
		data[i].timer = timer;
		data[i].anime = anime;
		data[i].anime_speed = anime_speed;
		break;
	}
}