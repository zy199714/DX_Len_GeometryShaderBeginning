#include "Effects.h"
#include "../common/d3dUtil.h"
#include "EffectHelper.h"	// 必须晚于Effects.h和d3dUtil.h包含
#include "../common/DXTrace.h"
#include "../common/Vertex.h"
using namespace DirectX;
using namespace std::experimental;

class MiniMapEffect::Impl: public AlignedType<MiniMapEffect::Impl>
{
public:
	struct  CBChangesEveryFrame
	{
		XMVECTOR eyePos;
	};

public:
	Impl() = default;
	~Impl() = default;

public:
	CBufferObject<0, CBChangesEveryFrame> cbFrame;		// 每帧修改的常量缓冲区

	BOOL isDirty;										// 是否有值变更
	std::vector<CBufferBase*> cBufferPtrs;				// 统一管理上面所有的常量缓冲区

	ComPtr<ID3D11VertexShader> minimapVS;
	ComPtr<ID3D11PixelShader> minimapPS;

	ComPtr<ID3D11InputLayout> vertexPosTexLayout;

	ComPtr<ID3D11ShaderResourceView> texture;			// 用于淡入淡出的纹理
};

namespace
{
	// MinimapEffect单例
	static MiniMapEffect * pInstance = nullptr;
}

MiniMapEffect::MiniMapEffect()
{
	if (pInstance)
		throw std::exception("MinimapEffect is a singleton!");
	pInstance = this;
	pImpl = std::make_unique<MiniMapEffect::Impl>();
}

MiniMapEffect::~MiniMapEffect()
{
}

MiniMapEffect::MiniMapEffect(MiniMapEffect && moveFrom)
{
	pImpl.swap(moveFrom.pImpl);
}

MiniMapEffect & MiniMapEffect::operator=(MiniMapEffect && moveFrom)
{
	pImpl.swap(moveFrom.pImpl);
	return *this;
}

MiniMapEffect & MiniMapEffect::Get()
{
	if (!pInstance)
		throw std::exception("MinimapEffect needs an instance!");
	return *pInstance;
}

bool MiniMapEffect::InitAll(ComPtr<ID3D11Device> device)
{
	if (!device)
		return false;

	if (!pImpl->cBufferPtrs.empty())
		return true;

	if (!RenderStates::IsInit())
		throw std::exception("RenderStates need to be initialized first!");

	ComPtr<ID3DBlob> blob;

	// ******************
	// 创建顶点着色器
	//

	HR(CreateShaderFromFile(L"HLSL\\MiniMap_VS.cso", L"HLSL\\MiniMap_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->minimapVS.GetAddressOf()));
	// 创建顶点布局
	HR(device->CreateInputLayout(VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->vertexPosTexLayout.GetAddressOf()));

	// ******************
	// 创建像素着色器
	//
	HR(CreateShaderFromFile(L"HLSL\\MiniMap_PS.cso", L"HLSL\\MiniMap_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->minimapPS.GetAddressOf()));


	pImpl->cBufferPtrs.assign({
		&pImpl->cbFrame,
		});

	// 创建常量缓冲区
	for (auto& pBuffer : pImpl->cBufferPtrs)
	{
		HR(pBuffer->CreateBuffer(device));
	}

	return true;
}

void MiniMapEffect::SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext)
{
	deviceContext->IASetInputLayout(pImpl->vertexPosTexLayout.Get());
	deviceContext->VSSetShader(pImpl->minimapVS.Get(), nullptr, 0);
	deviceContext->PSSetShader(pImpl->minimapPS.Get(), nullptr, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->RSSetState(nullptr);

	deviceContext->PSSetSamplers(0, 1, RenderStates::SSLinearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(RenderStates::DSSNoDepthTest.Get(), 0);	// 关闭深度测试
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void MiniMapEffect::SetTextureDiffuse(ComPtr<ID3D11ShaderResourceView> texture)
{
	pImpl->texture = texture;
}

void MiniMapEffect::Apply(ComPtr<ID3D11DeviceContext> deviceContext)
{
	auto& pCBuffers = pImpl->cBufferPtrs;
	// 将缓冲区绑定到渲染管线上
//	pCBuffers[0]->BindPS(deviceContext);
	// 设置SRV
	deviceContext->PSSetShaderResources(0, 1, pImpl->texture.GetAddressOf());

	if (pImpl->isDirty)
	{
		pImpl->isDirty = false;
		for (auto& pCBuffer : pCBuffers)
		{
			pCBuffer->UpdateBuffer(deviceContext);
		}
	}
}