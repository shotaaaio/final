//#pragma once
//
//#include "Misc.h"
//#include "TrailShader.h"
//#include "Graphics/Buffer.h"
//
//// 線形補間関数
//inline float lerp(float a, float b, float t)
//{
//    return a + t * (b - a);
//}
//
//TrailShader::TrailShader(ID3D11Device* device)
//{
//        // 入力レイアウト
//        D3D11_INPUT_ELEMENT_DESC inputDesc[] =
//        {
//            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
//              offsetof(TrailVertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
//            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
//              offsetof(TrailVertex, Normal),   D3D11_INPUT_PER_VERTEX_DATA, 0 },
//           { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
//              offsetof(TrailVertex, Color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
//            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0,
//              offsetof(TrailVertex, TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
//        };
//       
//		ShaderManager* shader = ShaderManager::instance();
//        shader->createVsFromCso(device, "Shader/Trail_vs.cso", TrailVertexShader.GetAddressOf(), TrailLayout.GetAddressOf(), inputDesc,_countof(inputDesc));
//        shader->createPsFromCso(device, "Shader/Trail_ps.cso", TrailPixelShader.GetAddressOf());
//
//		createBuffer<SwordTrailConstants>(device, TrailConstantBuffer.GetAddressOf());
//    
//
//    // 定数バッファ
//    {
//        D3D11_BUFFER_DESC desc = {};
//        desc.Usage = D3D11_USAGE_DEFAULT;
//        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//        desc.ByteWidth = sizeof(SwordTrailConstants);
//
//        device->CreateBuffer(
//            &desc, nullptr,
//            TrailConstantBuffer.GetAddressOf());
//    }
//
//    // 頂点バッファ動的更新用
//    {
//        D3D11_BUFFER_DESC vbDesc = {};
//        vbDesc.Usage = D3D11_USAGE_DYNAMIC;
//        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//        vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//
//        // スプライン補間を考慮してバッファサイズを計算
//        const int interpolatedPointsPerSegment = 10; // 補間点の数
//        const int totalInterpolatedPoints = (MAX_POLYGON - 3) * interpolatedPointsPerSegment;
//        vbDesc.ByteWidth = sizeof(TrailVertex) * totalInterpolatedPoints * 2; // 2はTipとRootのため
//
//        device->CreateBuffer(&vbDesc, nullptr, VertexBuffer.GetAddressOf());
//    }
//
//    // ブレンドステート
//    {
//        D3D11_BLEND_DESC desc = {};
//        desc.RenderTarget[0].BlendEnable = true;
//        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
//        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//
//        device->CreateBlendState(
//            &desc, TrailBlendState.GetAddressOf());
//    }
//
//    // 深度ステンシルステート
//    {
//        D3D11_DEPTH_STENCIL_DESC desc = {};
//        desc.DepthEnable = true;
//        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//
//        device->CreateDepthStencilState(
//            &desc, TrailStencilState.GetAddressOf());
//    }
//
//    // ラスタライザーステート
//    {
//        D3D11_RASTERIZER_DESC desc = {};
//        desc.DepthClipEnable = true;
//        desc.MultisampleEnable = true;
//        desc.FillMode = D3D11_FILL_SOLID;
//        desc.CullMode = D3D11_CULL_BACK;
//
//        device->CreateRasterizerState(
//            &desc, TrailRasterizerState.GetAddressOf());
//    }
//
//    // サンプラーステート
//    {
//        D3D11_SAMPLER_DESC desc = {};
//        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//        desc.MinLOD = -FLT_MAX;
//        desc.MaxLOD = FLT_MAX;
//
//        device->CreateSamplerState(
//            &desc, TrailSamplerState.GetAddressOf());
//       
//    }
//}
//void TrailShader::Begin(ID3D11DeviceContext* immediate_context, const RenderContext& rc)
//{
//   
//    //バッファ用データの設定
//	stConst.View = rc.camera->getView();
//    stConst.Projection = camera->getProjection();
//    stConst.TrailWidth = rc.swordTrailData.trailWidth;
//    stConst.HueOffset = rc.swordTrailData.hueOffset;
//    stConst.AlphaStart = rc.swordTrailData.alphaStart;
//    stConst.AlphaEnd = rc.swordTrailData.alphaEnd;
//    stConst.DistortionPower = rc.swordTrailData.distortionPower;
//    stConst.NoiseAmount = rc.swordTrailData.noiseAmount;
//
//    // 2) 定数バッファの更新とバインド
//    dc->UpdateSubresource(TrailConstantBuffer.Get(), 0, nullptr, &stConst, 0, 0);
//    dc->VSSetConstantBuffers(0, 1, TrailConstantBuffer.GetAddressOf());
//    dc->PSSetConstantBuffers(0, 1, TrailConstantBuffer.GetAddressOf());
//
//    // 3) シェーダーと入力レイアウトの設定
//    dc->VSSetShader(TrailVertexShader.Get(), nullptr, 0);
//    dc->PSSetShader(TrailPixelShader.Get(), nullptr, 0);
//    dc->IASetInputLayout(TrailLayout.Get());
//    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//    // 4) 描画ステート設定
//    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
//    dc->OMSetBlendState(TrailBlendState.Get(), blendFactor, 0xFFFFFFFF);
//    dc->OMSetDepthStencilState(TrailStencilState.Get(), 0);
//    dc->RSSetState(TrailRasterizerState.Get());
//    dc->PSSetSamplers(0, 1, TrailSamplerState.GetAddressOf());
//}
//
//void TrailShader::SetTrailData(
//    const RenderContext& rc,
//    const std::vector<DirectX::XMFLOAT3>& tips,
//    const std::vector<DirectX::XMFLOAT3>& roots)
//{
//    auto dc = rc.deviceContext;
//
//    //マップ
//	// 頂点バッファのマッピング
//    D3D11_MAPPED_SUBRESOURCE msr;
//	// VertexBufferのサイズを取得
//    dc->Map(VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
//    auto verts = reinterpret_cast<TrailVertex*>(msr.pData);
//
//    //補間点数とバッファ容量
//    const size_t N = min(tips.size(), roots.size());
//    //バッファの最大頂点数 (Tip+Root で 2 倍)
//    const size_t capacity = (MAX_POLYGON - 3) * 10 * 2;
//	//頂点数がバッファ容量を超える場合は制限
//    size_t vi = 0;
//
//    for (size_t i = 0; i < N; ++i)
//    {
//        //頂点バッファが一杯なら抜ける
//        if (vi + 2 > capacity)
//            break;
//
//        float t = (N > 1)
//            ? static_cast<float>(i) / static_cast<float>(N - 1)
//            : 0.0f;
//
//        // ── Tip側 ──
//        {
//            auto& V = verts[vi++];
//            // 位置
//            V.Position = tips[i];
//
//            // 法線を Tip→Root の方向で計算
//            DirectX::XMVECTOR tipVec = DirectX::XMLoadFloat3(&tips[i]);
//            DirectX::XMVECTOR rootVec = DirectX::XMLoadFloat3(&roots[i]);
//            DirectX::XMVECTOR dirVec = DirectX::XMVectorSubtract(rootVec, tipVec);
//            DirectX::XMVECTOR normVec = DirectX::XMVector3Normalize(dirVec);
//            DirectX::XMStoreFloat3(&V.Normal, normVec);
//
//            // 色 (先端→根元でαグラデーション)
//            V.Color = DirectX::XMFLOAT4(1, 0, 0,
//                lerp(stConst.AlphaStart, stConst.AlphaEnd, t));
//            
//
//            // UV.x=0 で Tip
//            V.TexCoord = DirectX::XMFLOAT2(0.0f, t);
//        }
//
//        // ── Root側 ──
//        {
//            auto& V = verts[vi++];
//            // 位置
//            V.Position = roots[i];
//
//            // 同じ法線
//            DirectX::XMVECTOR tipVec = DirectX::XMLoadFloat3(&tips[i]);
//            DirectX::XMVECTOR rootVec = DirectX::XMLoadFloat3(&roots[i]);
//            DirectX::XMVECTOR dirVec = DirectX::XMVectorSubtract(rootVec, tipVec);
//            DirectX::XMVECTOR normVec = DirectX::XMVector3Normalize(dirVec);
//            DirectX::XMStoreFloat3(&V.Normal, normVec);
//
//            // 色
//            V.Color = DirectX::XMFLOAT4(
//                1.0f, 0.0f, .0f,
//                lerp(stConst.AlphaStart, stConst.AlphaEnd, t)
//            );
//
//            // UV.x=1 で Root
//            V.TexCoord = DirectX::XMFLOAT2(1.0f, t);
//        }
//    }
//
//    // 3) アンマップ
//    dc->Unmap(VertexBuffer.Get(), 0);
//
//    // 4) バインド＆Draw
//    UINT stride = sizeof(TrailVertex);
//    UINT offset = 0;
//    dc->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
//    dc->Draw(static_cast<UINT>(vi), 0);
//}
//
//void TrailShader::End(const RenderContext& rc)
//{
//    auto dc = rc.deviceContext;
//    dc->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
//    dc->OMSetDepthStencilState(nullptr, 0);
//}
