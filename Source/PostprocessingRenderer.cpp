//#include "PostprocessingRenderer.h"
//#include "imgui.h"
//#include "DeviceManager.h"
//PostprosessingRenderer::PostprosessingRenderer()
//{
//	//���P�x���o�p�`��^�[�Q�b�g�𐶐�
//	DeviceManager* deviceM = DeviceManager::instance();
//
//	//���P�x���o�p�̕`��Ɏg���X�v���C�g
//	//renderSprite = std::make_unique<Sprite>(graphics->GetDevice(),nullptr);
//
//	UINT width = static_cast<UINT>(deviceM->getScreenWidth()) / 2;
//	UINT height = static_cast<UINT>(deviceM->getScreenHeight()) / 2;
//
//	//���P�x���o�p�`��^�[�Q�b�g����
//	luminanceExtractRenderTarget = std::make_unique<RenderTarget>(width, height,DXGI_FORMAT_R8G8B8A8_UNORM);
//}
//
//void PostprosessingRenderer::Render(ID3D11DeviceContext* dec)
//{
//	GraphicsManager* graphicsManager = GraphicsManager::instance();
//
//	//���ݐݒ肳��Ă���o�b�t�@��ޔ����ď��������Ă���
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
//	//���P�x���o�p�o�b�t�@�ɕ`����ύX���č��P�x���o
//	{
//		//�`����ύX
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
//		//���P�x���o����
//		SpriteShader* shader = graphicsManager->GetShader(SpriteShaderID::LuminanceExtraction);
//		shader->Begin(dec);
//		//���o���ꂽ�e�N�X�`���Ƃ��̃T�C�Y��`��ΏۂɃZ�b�g
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
//	//���̃o�b�t�@�ɖ߂�
//	{
//		dec->RSSetViewports(cacheViewportCount, &cacheViewports);
//		dec->OMSetRenderTargets(1, cacheRenderTargetView.GetAddressOf(), cacheDepthStencilView.Get());
//	}
//
//	//���o�������P�x�`��Ώۂ���������
//	{
//		//���P�x���o�֐�����������
//		SpriteShader* shader = graphicsManager->GetShader(SpriteShaderID::Default);
//		shader->Begin(dec);
//
//		//�`��Ώۂ�ύX
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
