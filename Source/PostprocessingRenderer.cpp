//#include "PostprocessingRenderer.h"
//
//PostprosessingRenderer::PostprosessingRenderer()
//{
//	const auto& graphicsManager = GraphicsManager::instance();
//
//	renderSprite = std::make_unique<Sprite>();
//
//	//���P�x���o�p�`��^�[�Q�b�g�𐶐�
//	luminanceExtractRenderTarget = std::make_unique<RenderTarget>(
//		static_cast<UINT>(graphicsManager->GetScreenWidth()),
//		static_cast<UINT>(graphicsManager->GetScreenWidth()),DXGI_FORMAT_R8G8_UNORM);
//}
//
//void PostprosessingRenderer::Render(ID3D11DeviceContext* dec)
//{
//	GraphicsManager* graphicsManager = graphicsManager->instance();
//
//	//���ݐݒ肳��Ă���o�b�t�@��ޔ����ď��������Ă���
//	UINT       cacheViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
//	D3D11_VIEWPORT cacheViewports{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
//	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>cacheRenderTargetView;
//	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>cacheDepthStencilView;
//	{
//		dec->RSGetViewports(&cacheViewportCount, &cacheViewports);
//
//
//	}
//}
//
//void PostprosessingRenderer::SetSceneData(ShaderResourceViewData srvData)
//{
//}
//
//void PostprosessingRenderer::DrawDebugGUI()
//{
//}
