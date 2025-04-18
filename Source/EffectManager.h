#pragma once

#include<DirectXMath.h>
#include<Effekseer.h>
#include<EffekseerRendererDX11.h>

//�G�t�F�N�g�Ǘ�
class EffectManager
{
private:
	EffectManager(){}
	~EffectManager(){}
public:
	static EffectManager* instance()
	{
		static EffectManager inst;
		return &inst;
	}

	//����������
	void initialize();

	//�I������
	void finalize();

	//�X�V����
	void update(float elapsedTime);

	//�`�揈��
	void render(const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection);

	//Effekseer�Ǘ��̎擾
	Effekseer::ManagerRef getEffekseerMgr() { return effekseerMgr; }
private:
	//Effekseer�Ǘ�
	Effekseer::ManagerRef effekseerMgr;

	// Effekseer �`�����
	EffekseerRenderer::RendererRef effekseerRenderer;
};