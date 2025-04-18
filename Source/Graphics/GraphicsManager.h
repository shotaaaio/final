#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <functional>
#include <memory>

#include "DebugRenderer.h"
#include "LineRenderer.h"
#include "Buffer.h"

enum class SAMPLER_STATE
{
	POINT,
	LINEAR,
	ANISOTROPIC,
};

enum class DEPTH_STENCIL_STATE
{
	ON_ON,		// �[�x�e�X�g�� ON �[�x�������݂� ON
	ON_OFF,		// �[�x�e�X�g�� ON �[�x�������݂� OFF
	OFF_ON,		// �[�x�e�X�g�� OFF �[�x�������݂� ON
	OFF_OFF,	// �[�x�e�X�g�� OFF �[�x�������݂� OFF
};

enum class BLEND_STATE
{
	NONE,
	ALPHABLENDING,
	ADD,
	MULTIPLE
};

enum class RASTERIZER_STATE
{
	SOLID_CULLBACK,
	WIRE_CULLBACK,
	SOLID_CULLNONE,
	WIRE_CULLNONE
};

// �����_�[�R���e�L�X�g
struct RenderContext
{
	// �T���v���[�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[3];
	// �[�x�X�e���V���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[4];
	// �u�����h�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[4];
	// ���X�^���C�U�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[4];
};

class GraphicsManager
{
private:
	GraphicsManager() {}
	~GraphicsManager() {}

public:
	static GraphicsManager* instance()
	{
		static GraphicsManager inst;
		return &inst;
	}

	bool initialize(ID3D11Device* device, ID3D11DeviceContext* dc);

	Microsoft::WRL::ComPtr<ID3D11SamplerState> getSamplerState(SAMPLER_STATE state) const { return renderContext.samplerStates[static_cast<int>(state)]; }

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> getDepthStencilStates(DEPTH_STENCIL_STATE state) const { return renderContext.depthStencilStates[static_cast<int>(state)]; }

	Microsoft::WRL::ComPtr<ID3D11BlendState> getBlendStates(BLEND_STATE state) const { return renderContext.blendStates[static_cast<int>(state)]; }

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> getRasterizerStates(RASTERIZER_STATE state) const { return renderContext.rasterizerStates[static_cast<int>(state)]; }

	void SettingRenderContext(void(*settingCallback)(ID3D11DeviceContext*, RenderContext*));

	DebugRenderer* getDebugRenderer() const { return debugRenderer.get(); }

	LineRenderer* getLineRenderer() const { return lineRenderer.get(); }

private:
	ID3D11DeviceContext* deviceContext = nullptr;
	RenderContext renderContext;
	
	std::unique_ptr<DebugRenderer> debugRenderer;
	std::unique_ptr<LineRenderer> lineRenderer;

};