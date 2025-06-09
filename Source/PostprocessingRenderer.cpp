#include "PostprocessingRenderer.h"
#include "imgui.h"
#include "DeviceManager.h"
PostprosessingRenderer::PostprosessingRenderer()
{
	//高輝度抽出用描画ターゲットを生成
	DeviceManager& deviceManager = *DeviceManager::instance();

	//高輝度抽出用の描画に使うスプライト
	renderSprite = std::make_unique<Sprite>(deviceManager.getDevice(), nullptr);

	//高輝度抽出用描画ターゲット生成
	luminanceExtractRenderTarget = std::make_unique<RenderTarget>(static_cast<UINT>(deviceManager.getScreenWidth()),
		static_cast<UINT>(deviceManager.getScreenWidth()),DXGI_FORMAT_R8G8B8A8_UNORM);
}

PostprosessingRenderer::~PostprosessingRenderer()
{
}

void PostprosessingRenderer::Render(ID3D11DeviceContext* dec)
{
    GraphicsManager* graphicsManager = GraphicsManager::instance();

    // バッファ退避
    UINT cacheViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    D3D11_VIEWPORT cacheViewports{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>cacheRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>cacheDepthStencilView;
    {
        dec->RSGetViewports(&cacheViewportCount, &cacheViewports);
        dec->OMGetRenderTargets(1,
            cacheRenderTargetView.ReleaseAndGetAddressOf(),
            cacheDepthStencilView.ReleaseAndGetAddressOf());
    }

    // 高輝度抽出用バッファに描画先を変更
    {
        ID3D11RenderTargetView* rtv = luminanceExtractRenderTarget->GetRenderTargetView().Get();
        FLOAT color[] = { 0.0f,0.0f,0.0f,0.0f };
        dec->ClearRenderTargetView(rtv, color);
        dec->OMSetRenderTargets(1, &rtv, nullptr);

        D3D11_VIEWPORT viewport{};
        viewport.Width = static_cast<float>(luminanceExtractRenderTarget->GetWidth());
        viewport.Height = static_cast<float>(luminanceExtractRenderTarget->GetHeight());
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        dec->RSSetViewports(1, &viewport);

        // 高輝度抽出シェーダーで描画
        SpriteShader* shader = graphicsManager->GetShader(SpriteShaderID::LuminanceExtraction);
        shader->Begin(dec);

        //描画対象を変更
        renderSprite->SetShaderResourceView(sceneData.srv, sceneData.width, sceneData.height);

        RenderContext rc;
        rc.luminanceExtractionData = bloomData.luminanceExtractionData;

        shader->Draw(dec, rc, renderSprite.get());
        shader->End(dec);
    }

    // バッファを元に戻す
    {
        dec->RSSetViewports(cacheViewportCount, &cacheViewports);
        dec->OMSetRenderTargets(1, cacheRenderTargetView.GetAddressOf(), cacheDepthStencilView.Get());
    }

    // 抽出した高輝度テクスチャを画面に描画
    {
        SpriteShader* shader = graphicsManager->GetShader(SpriteShaderID::Default);
        shader->Begin(dec);

        renderSprite->SetShaderResourceView(luminanceExtractRenderTarget->GetShaderResorceView(),
            luminanceExtractRenderTarget->GetWidth(),
            luminanceExtractRenderTarget->GetHeight());

        renderSprite->render(dec,
            0.0f, 0.0f,
            static_cast<float>(luminanceExtractRenderTarget->GetWidth()),
            static_cast<float>(luminanceExtractRenderTarget->GetHeight()));
        shader->Draw(dec, RenderContext{}, renderSprite.get());
        shader->End(dec);
    }
}

void PostprosessingRenderer::SetSceneData(ShaderResourceViewData srvData)
{
	sceneData = srvData;
}

void PostprosessingRenderer::DrawDebugGUI()
{
	if (ImGui::TreeNode("PostProcess"))
	{
		if (ImGui::TreeNode("Bloom"))
		{
			ImGui::SliderFloat("threshold", &bloomData.luminanceExtractionData.threshold, 0.0f, 1.0f);
			ImGui::SliderFloat("intensity", &bloomData.luminanceExtractionData.intensity, 0.0f, 10.0f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

}
