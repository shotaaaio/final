//#pragma once
//#include "Graphics/ShaderManager.h"
//#include "Graphics/Shader.h"
//
//class TrailShader
//{
//public:
//    TrailShader(ID3D11Device* device);
//    ~TrailShader() = default;
//
//    void Begin(ID3D11DeviceContext* immediate_context, const RenderContext& rc);
//    void SetTrailData(ID3D11DeviceContext* immediate_context, const std::vector<DirectX::XMFLOAT3>& tips, const std::vector<DirectX::XMFLOAT3>& roots);
//    void End(ID3D11DeviceContext* immediate_context);
//
//private:
//    struct TrailVertex
//    {
//        DirectX::XMFLOAT3 Position;  // 頂点位置
//        DirectX::XMFLOAT3 Normal;    // 法線
//        DirectX::XMFLOAT4 Color;     // 頂点色
//        DirectX::XMFLOAT2 TexCoord;  // テクスチャ座標
//    };
//    static const int MAX_POLYGON = 16;
//
//    struct SwordTrailConstants
//    {
//        DirectX::XMFLOAT4X4 View;
//        DirectX::XMFLOAT4X4 Projection;
//        float Time;
//        float TrailWidth;
//        float HueOffset;
//        float AlphaStart;
//        float AlphaEnd;
//        float DistortionPower;
//        float NoiseAmount;
//        float dummy;
//    };
//
//    SwordTrailConstants stConst;
//
//    Microsoft::WRL::ComPtr<ID3D11Buffer> TrailConstantBuffer;
//    Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
//    Microsoft::WRL::ComPtr<ID3D11VertexShader> TrailVertexShader;
//    Microsoft::WRL::ComPtr<ID3D11PixelShader> TrailPixelShader;
//    Microsoft::WRL::ComPtr<ID3D11InputLayout> TrailLayout;
//    Microsoft::WRL::ComPtr<ID3D11BlendState> TrailBlendState;
//    Microsoft::WRL::ComPtr<ID3D11RasterizerState> TrailRasterizerState;
//    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> TrailStencilState;
//    Microsoft::WRL::ComPtr<ID3D11SamplerState> TrailSamplerState;
//};