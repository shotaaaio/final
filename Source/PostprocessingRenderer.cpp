//#include "PostprocessingRenderer.h"
//#include "imgui.h"
//#include "DeviceManager.h"
//PostprosessingRenderer::PostprosessingRenderer()
//{
//	//高輝度抽出用描画ターゲットを生成
//	DeviceManager* deviceM = DeviceManager::instance();
//
//	//高輝度抽出用の描画に使うスプライト
//	//renderSprite = std::make_unique<Sprite>(graphics->GetDevice(),nullptr);
//
//	UINT width = static_cast<UINT>(deviceM->getScreenWidth()) / 2;
//	UINT height = static_cast<UINT>(deviceM->getScreenHeight()) / 2;
//
//	//高輝度抽出用描画ターゲット生成
//	luminanceExtractRenderTarget = std::make_unique<RenderTarget>(width, height,DXGI_FORMAT_R8G8B8A8_UNORM);
//}
//
//void PostprosessingRenderer::Render(ID3D11DeviceContext* dec)
//{
//	GraphicsManager* graphicsManager = GraphicsManager::instance();
//
//	//現在設定されているバッファを退避して初期化しておく
//	UINT       cacheViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
//	D3D11_VIEWPORT cacheViewports{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
//	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>cacheRenderTargetView;
//	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>cacheDepthStencilView;
//	{
//		dec->RSGetViewports(&cacheViewportCount, &cacheViewports);
//		dec->OMGetRenderTargets(1,
//			cacheRenderTargetView.ReleaseAndGetAddressOf(),
//			cacheDepthStencilView.ReleaseAndGetAddressOf());
//	}
//	RenderContext rc;
//	rc.luminanceExtractionData = bloomData.luminanceExtractionData;
//
//
//	//高輝度抽出用バッファに描画先を変更して高輝度抽出
//	{
//		//描画先を変更
//		ID3D11RenderTargetView* rtv = luminanceExtractRenderTarget->GetRenderTargetView().Get();
//		FLOAT color[] = { 0.0f,0.0f,0.0f,0.0f };
//		dec->ClearRenderTargetView(rtv,color);
//		dec->OMSetRenderTargets(1, &rtv, nullptr);
//
//		D3D11_VIEWPORT viewport{};
//		viewport.Width  = static_cast<float>(luminanceExtractRenderTarget->GetWidth());
//		viewport.Height = static_cast<float>(luminanceExtractRenderTarget->GetHeight());
//		viewport.MinDepth = 0.0f;
//		viewport.MaxDepth = 1.0f;
//		dec->RSSetViewports(cacheViewportCount, &cacheViewports);
//
//		//高輝度抽出処理
//		SpriteShader* shader = graphicsManager->GetShader(SpriteShaderID::LuminanceExtraction);
//		shader->Begin(dec);
//		//抽出されたテクスチャとそのサイズを描画対象にセット
//		renderSprite->SetShaderResourceView(luminanceExtractRenderTarget->GetShaderResorceView(),
//			luminanceExtractRenderTarget->GetWidth(),
//			luminanceExtractRenderTarget->GetHeight());
//		renderSprite->render(dec, 
//			0.0f, 0.0f,
//			static_cast<float>(sceneData.width),
//			static_cast<float>(sceneData.height));
//		shader->Draw(dec,rc,renderSprite.get());
//		shader->End(dec);
//	}
//	//元のバッファに戻す
//	{
//		dec->RSSetViewports(cacheViewportCount, &cacheViewports);
//		dec->OMSetRenderTargets(1, cacheRenderTargetView.GetAddressOf(), cacheDepthStencilView.Get());
//	}
//
//	//抽出した高輝度描画対象を書き込む
//	{
//		//高輝度抽出関数を書き込む
//		SpriteShader* shader = graphicsManager->GetShader(SpriteShaderID::Default);
//		shader->Begin(dec);
//
//		//描画対象を変更
//		renderSprite->SetShaderResourceView(luminanceExtractRenderTarget->GetShaderResorceView(),
//			luminanceExtractRenderTarget->GetWidth(),
//			luminanceExtractRenderTarget->GetHeight());
//		renderSprite->render(dec,
//			0.0f, 0.0f,
//			static_cast<float>(luminanceExtractRenderTarget->GetWidth()),
//			static_cast<float>(luminanceExtractRenderTarget->GetHeight()));
//		shader->Draw(dec, rc, renderSprite.get());
//		shader->End(dec);	
//	}
//}
//
//void PostprosessingRenderer::SetSceneData(ShaderResourceViewData srvData)
//{
//	sceneData = srvData;
//}
//
//void PostprosessingRenderer::DrawDebugGUI()
//{
//	if (ImGui::TreeNode("PostProcess"))
//	{
//		if (ImGui::TreeNode("Bloom"))
//		{
//			ImGui::SliderFloat("threshold", &bloomData.luminanceExtractionData.threshold, 0.0f, 1.0f);
//			ImGui::SliderFloat("intensity", &bloomData.luminanceExtractionData.intensity, 0.0f, 10.0f);
//			ImGui::TreePop();
//		}
//		ImGui::TreePop();
//	}
//
//}
