#include "TextureRender.h"
#include "DXTrace.h"
using namespace Microsoft::WRL;

TextureRender::TextureRender(ComPtr<ID3D11Device> device, int texWidth, int texHeight, bool generateMips)
	: mGenerateMips(generateMips)
{
	// ******************
	// 1. ��������
	//

	ComPtr<ID3D11Texture2D> texture;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = texWidth;
	texDesc.Height = texHeight;
	texDesc.MipLevels = (mGenerateMips ? 0 : 1);	// 0Ϊ����mipmap��
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	//texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// ����texture�����½�����
	HR(device->CreateTexture2D(&texDesc, nullptr, texture.ReleaseAndGetAddressOf()));

	// ******************
	// 2. ���������Ӧ����ɫ����Դ��ͼ
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;	// -1ʹ�����е�mip�ȼ�
	HR(device->CreateShaderResourceView(texture.Get(), &srvDesc, mOutputTextureSRV.GetAddressOf()));


	// ******************
	// 3. ����������ȿ�ߵ����/ģ�建�����Ͷ�Ӧ����ͼ
	//

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(device->CreateDepthStencilView(texture.Get(), &dsvDesc, mOutputTextureDSV.GetAddressOf()));
	
	// ******************
	// 4. ���������Ӧ����ȾĿ����ͼ
	//	
	ComPtr<ID3D11Texture2D> renderTex;
	texDesc.Width = texWidth;
	texDesc.Height = texHeight;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	device->CreateTexture2D(&texDesc, nullptr, renderTex.GetAddressOf());

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	HR(device->CreateRenderTargetView(renderTex.Get(), &rtvDesc, mOutputTextureRTV.GetAddressOf()));

	// ******************
	// 5. ��ʼ���ӿ�
	//
	mOutputViewPort.TopLeftX = 0.0f;
	mOutputViewPort.TopLeftY = 0.0f;
	mOutputViewPort.Width = static_cast<float>(texWidth);
	mOutputViewPort.Height = static_cast<float>(texHeight);
	mOutputViewPort.MinDepth = 0.0f;
	mOutputViewPort.MaxDepth = 1.0f;
}

TextureRender::~TextureRender()
{
}

void TextureRender::Begin(ComPtr<ID3D11DeviceContext> deviceContext)
{
	// ������ȾĿ������ģ����ͼ
	deviceContext->OMGetRenderTargets(1, mCacheRTV.GetAddressOf(), mCacheDSV.GetAddressOf());
	// �����ӿ�
	UINT numViewports = 1;
	deviceContext->RSGetViewports(&numViewports, &mCacheViewPort);


	// ��ջ�����
	float wite[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
	float black[4] = { 0.0f, 0.0f, 0.0f,1.0f };
	deviceContext->ClearRenderTargetView(mOutputTextureRTV.Get(), wite);
	deviceContext->ClearDepthStencilView(mOutputTextureDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// ������ȾĿ������ģ����ͼ
	deviceContext->OMSetRenderTargets(1, mOutputTextureRTV.GetAddressOf(), mOutputTextureDSV.Get());
	// �����ӿ�
	deviceContext->RSSetViewports(1, &mOutputViewPort);
}

void TextureRender::End(ComPtr<ID3D11DeviceContext> deviceContext)
{
	// �ָ�Ĭ���趨
	deviceContext->RSSetViewports(1, &mCacheViewPort);
	deviceContext->OMSetRenderTargets(1, mCacheRTV.GetAddressOf(), mCacheDSV.Get());

	// ��֮ǰ��ָ����Ҫmipmap����������
	if (mGenerateMips)
	{
		deviceContext->GenerateMips(mOutputTextureSRV.Get());
	}
	
	// �����ʱ�������ȾĿ����ͼ�����ģ����ͼ
	mCacheDSV.Reset();
	mCacheRTV.Reset();
}

ComPtr<ID3D11ShaderResourceView> TextureRender::GetOutputTexture()
{
	return mOutputTextureSRV;
}
