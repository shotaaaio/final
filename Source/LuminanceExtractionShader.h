#pragma once
#include "Graphics/ShaderManager.h"
#include "Shader.h"

class LumianceExtractionShader : public SpriteShader
{
public:
	LumianceExtractionShader(ID3D11Device* device);
	~LumianceExtractionShader() override {};

	void Begin(ID3D11DeviceContext* immediate_context)override;
	void Draw(ID3D11DeviceContext* immediate_context, const RenderContext& rc, const Sprite* sprite)override;
	void End(ID3D11DeviceContext* immediate_context)override;

private:
	struct CBLuminanceExtrantiion
	{
		float threshold;
		float intensity;
		DirectX::XMFLOAT2 dummy;
	}; 

	Microsoft::WRL::ComPtr<ID3D11Buffer>				luminanceExtractionConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;

};