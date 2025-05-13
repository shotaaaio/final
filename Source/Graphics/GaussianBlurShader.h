#pragma once
#include "Shader.h"
#include "ShaderManager.h"
#include "GraphicsManager.h"
class GaussianBlurShader : public SpriteShader
{
	struct CBFilter
	{
		DirectX::XMFLOAT4 weights[MaxKernelSize * MaxKernelSize];
		float KernelSize;
		DirectX::XMFLOAT2 texcel;
		float dummy;
	};
public:
	GaussianBlurShader(ID3D11Device* device);
	~GaussianBlurShader() override {};

	void Begin(ID3D11DeviceContext* immdiaite_context);
	void Draw(ID3D11DeviceContext* immdiaite_context, const RenderContext& rc, const Sprite* sprite);
	void End(ID3D11DeviceContext* immdiaite_context);

private:
	//フィルター値計算
	void CalcGaussianFliter(CBFilter& cbFilter, const GaussianFilterData& gaussianFilterData);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>filterConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>inputLayout;
	Microsoft::WRL::ComPtr<ID3D11BlendState>blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>depthStancilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>samplerState;
};