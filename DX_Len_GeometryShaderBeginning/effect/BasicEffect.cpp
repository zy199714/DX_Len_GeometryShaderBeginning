#include "Effects.h"
#include "../common/d3dUtil.h"
#include "EffectHelper.h"	// ��������Effects.h��d3dUtil.h����
#include "../common/DXTrace.h"
#include "../common/Vertex.h"
using namespace DirectX;
using namespace std::experimental;


//
// BasicEffect::Impl ��Ҫ����BasicEffect�Ķ���
//

class BasicEffect::Impl : public AlignedType<BasicEffect::Impl>
{
public:

	//
	// ��Щ�ṹ���ӦHLSL�Ľṹ�塣��Ҫ��16�ֽڶ���
	//

	struct CBChangesEveryInstanceDrawing
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldInvTranspose;
	};

	struct CBChangesEveryObjectDrawing
	{
		material_struct mat;
	};

	struct CBDrawingStates
	{
		DirectX::XMVECTOR fogColor;
		int fogEnabled;
		float fogStart;
		float fogRange;
		int textureUsed;
		int shadowEnabled;
		float shadowBits;
		float pad1;
		float pad2;
	};

	struct CBChangesEveryFrame
	{
		DirectX::XMMATRIX view;
		DirectX::XMVECTOR eyePos;
	};

	struct CBChangesOnResize
	{
		DirectX::XMMATRIX worldLightViewProj;
		DirectX::XMMATRIX proj;
	};

	struct CBChangesRarely
	{
		DirectionalLight dirLight[BasicEffect::maxLights];
		PointLight pointLight[BasicEffect::maxLights];
		SpotLight spotLight[BasicEffect::maxLights];
		int numDirLight;
		int numPointLight;
		int numSpotLight;
		int pad3;
	};

public:
	// ������ʽָ��
	Impl() = default;
	~Impl() = default;

public:
	// ��Ҫ16�ֽڶ�������ȷ���ǰ��
	CBufferObject<0, CBChangesEveryInstanceDrawing>	cbInstDrawing;		// ÿ��ʵ�����Ƶĳ���������
	CBufferObject<1, CBChangesEveryObjectDrawing>	cbObjDrawing;		// ÿ�ζ�����Ƶĳ���������
	CBufferObject<2, CBDrawingStates>				cbStates;			// ÿ�λ���״̬�ı�ĳ���������
	CBufferObject<3, CBChangesEveryFrame>			cbFrame;			// ÿ֡���Ƶĳ���������
	CBufferObject<4, CBChangesOnResize>				cbOnResize;			// ÿ�δ��ڴ�С����ĳ���������
	CBufferObject<5, CBChangesRarely>				cbRarely;			// �����������ĳ���������
	BOOL isDirty;											// �Ƿ���ֵ���
	std::vector<CBufferBase*> cBufferPtrs;					// ͳһ�����������еĳ���������


	ComPtr<ID3D11VertexShader> basicInstanceVS;
	ComPtr<ID3D11VertexShader> basicObjectVS;
	ComPtr<ID3D11VertexShader> shadowMapInstanceVS;
	ComPtr<ID3D11VertexShader> shadowMapObjectVS;

	ComPtr<ID3D11PixelShader> basicPS;
	ComPtr<ID3D11PixelShader> shadowPS;

	ComPtr<ID3D11InputLayout> instancePosNormalTexLayout;	
	ComPtr<ID3D11InputLayout> vertexPosNormalTexLayout;		

	ComPtr<ID3D11ShaderResourceView> textureDiffuse;				// ����������
	ComPtr<ID3D11ShaderResourceView> textureShadow;			// shadow map
};

//
// BasicEffect
//

namespace
{
	// BasicEffect����
	static BasicEffect * pInstance = nullptr;
}

BasicEffect::BasicEffect()
{
	if (pInstance)
		throw std::exception("BasicEffect is a singleton!");
	pInstance = this;
	pImpl = std::make_unique<BasicEffect::Impl>();
}

BasicEffect::~BasicEffect()
{
}

BasicEffect::BasicEffect(BasicEffect && moveFrom)
{
	pImpl.swap(moveFrom.pImpl);
}

BasicEffect & BasicEffect::operator=(BasicEffect && moveFrom)
{
	pImpl.swap(moveFrom.pImpl);
	return *this;
}

BasicEffect & BasicEffect::Get()
{
	if (!pInstance)
		throw std::exception("BasicEffect needs an instance!");
	return *pInstance;
}


bool BasicEffect::InitAll(ComPtr<ID3D11Device> device)
{
	if (!device)
		return false;

	if (!pImpl->cBufferPtrs.empty())
		return true;

	if (!RenderStates::IsInit())
		throw std::exception("RenderStates need to be initialized first!");

	ComPtr<ID3DBlob> blob;

	// ʵ�����벼��
	D3D11_INPUT_ELEMENT_DESC basicInstLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "World", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "World", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "World", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "World", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "WorldInvTranspose", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "WorldInvTranspose", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "WorldInvTranspose", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "WorldInvTranspose", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	// ******************
	// ����������ɫ��
	//
	HR(CreateShaderFromFile(L"HLSL\\BasicObject_VS.cso", L"HLSL\\BasicObject_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->basicObjectVS.GetAddressOf()));
	HR(CreateShaderFromFile(L"HLSL\\ShadowMapObject_VS.cso", L"HLSL\\ShadowMapObject_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->shadowMapObjectVS.GetAddressOf()));
	// �������㲼��
	HR(device->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->vertexPosNormalTexLayout.GetAddressOf()));

	HR(CreateShaderFromFile(L"HLSL\\BasicInstance_VS.cso", L"HLSL\\BasicInstance_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->basicInstanceVS.GetAddressOf()));
	HR(CreateShaderFromFile(L"HLSL\\ShadowMapInstance_VS.cso", L"HLSL\\ShadowMapInstance_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->shadowMapInstanceVS.GetAddressOf()));
	// �������㲼��
	HR(device->CreateInputLayout(basicInstLayout, ARRAYSIZE(basicInstLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->instancePosNormalTexLayout.GetAddressOf()));

	// ******************
	// ����������ɫ��
	//ShadowMap_PS
	HR(CreateShaderFromFile(L"HLSL\\Basic_PS.cso", L"HLSL\\Basic_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->basicPS.GetAddressOf()));
	HR(CreateShaderFromFile(L"HLSL\\ShadowMap_PS.cso", L"HLSL\\ShadowMap_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->shadowPS.GetAddressOf()));

	pImpl->cBufferPtrs.assign({
		&pImpl->cbInstDrawing,
		&pImpl->cbObjDrawing, 
		&pImpl->cbStates,
		&pImpl->cbFrame, 
		&pImpl->cbOnResize, 
		&pImpl->cbRarely});

	// ��������������
	for (auto& pBuffer : pImpl->cBufferPtrs)
	{
		HR(pBuffer->CreateBuffer(device));
	}

	return true;
}


void BasicEffect::SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext, RenderType type)
{
	if (type == RenderInstance)
	{
		deviceContext->IASetInputLayout(pImpl->instancePosNormalTexLayout.Get());
		deviceContext->VSSetShader(pImpl->basicInstanceVS.Get(), nullptr, 0);
	}
	else
	{
		deviceContext->IASetInputLayout(pImpl->vertexPosNormalTexLayout.Get());
		deviceContext->VSSetShader(pImpl->basicObjectVS.Get(), nullptr, 0);
	}
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->RSSetState(nullptr);
	deviceContext->PSSetShader(pImpl->basicPS.Get(), nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, RenderStates::SSLinearWrap.GetAddressOf());
	deviceContext->PSSetSamplers(1, 1, RenderStates::SSLinearBorder.GetAddressOf());
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void BasicEffect::SetRenderShadowMap(ComPtr<ID3D11DeviceContext> deviceContext, RenderType type)
{
	if (type == RenderInstance)
	{
		deviceContext->IASetInputLayout(pImpl->instancePosNormalTexLayout.Get());
		deviceContext->VSSetShader(pImpl->shadowMapInstanceVS.Get(), nullptr, 0);
	}
	else
	{
		deviceContext->IASetInputLayout(pImpl->vertexPosNormalTexLayout.Get());
		deviceContext->VSSetShader(pImpl->shadowMapObjectVS.Get(), nullptr, 0);
	}
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->RSSetState(nullptr);
	deviceContext->PSSetShader(pImpl->shadowPS.Get(), nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, RenderStates::SSLinearWrap.GetAddressOf());
	deviceContext->PSSetSamplers(1, 1, RenderStates::SSLinearBorder.GetAddressOf());
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void XM_CALLCONV BasicEffect::SetWorldMatrix(DirectX::FXMMATRIX W)
{
	auto& cBuffer = pImpl->cbInstDrawing;
	cBuffer.data.world = XMMatrixTranspose(W);
	cBuffer.data.worldInvTranspose = XMMatrixInverse(nullptr, W);
	pImpl->isDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV BasicEffect::SetViewMatrix(FXMMATRIX V)
{
	auto& cBuffer = pImpl->cbFrame;
	cBuffer.data.view = XMMatrixTranspose(V);
	pImpl->isDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV BasicEffect::SetProjMatrix(FXMMATRIX P)
{
	auto& cBuffer = pImpl->cbOnResize;
	cBuffer.data.proj = XMMatrixTranspose(P);
	pImpl->isDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV BasicEffect::SetLightViewProjMatrix(DirectX::FXMMATRIX LVP)
{
	auto& cBuffer = pImpl->cbOnResize;
	cBuffer.data.worldLightViewProj = XMMatrixTranspose(LVP);
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetDirLight(size_t pos, const DirectionalLight & dirLight)
{
	auto& cBuffer = pImpl->cbRarely;
	cBuffer.data.dirLight[pos] = dirLight;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetPointLight(size_t pos, const PointLight & pointLight)
{
	auto& cBuffer = pImpl->cbRarely;
	cBuffer.data.pointLight[pos] = pointLight;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetSpotLight(size_t pos, const SpotLight & spotLight)
{
	auto& cBuffer = pImpl->cbRarely;
	cBuffer.data.spotLight[pos] = spotLight;
	pImpl->isDirty = cBuffer.isDirty = true;
}
 
void BasicEffect::SetNumLight(int dir, int point, int spot)
{
	auto& cBuffer = pImpl->cbRarely;
	cBuffer.data.numDirLight = dir;
	cBuffer.data.numPointLight = point;
	cBuffer.data.numSpotLight = spot;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetMaterial(const material_struct & mat)
{
	auto& cBuffer = pImpl->cbObjDrawing;
	cBuffer.data.mat = mat;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetTextureUsed(bool isUsed)
{
	auto& cBuffer = pImpl->cbStates;
	cBuffer.data.textureUsed = isUsed;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetTextureDiffuse(ComPtr<ID3D11ShaderResourceView> textureDiffuse)
{
	pImpl->textureDiffuse = textureDiffuse;
}

void BasicEffect::SetTextureShadow(ComPtr<ID3D11ShaderResourceView> textureShadow)
{
	pImpl->textureShadow = textureShadow;
}

void XM_CALLCONV BasicEffect::SetEyePos(FXMVECTOR eyePos)
{
	auto& cBuffer = pImpl->cbFrame;
	cBuffer.data.eyePos = eyePos;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetFogState(bool isOn)
{
	auto& cBuffer = pImpl->cbStates;
	cBuffer.data.fogEnabled = isOn;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetFogStart(float fogStart)
{
	auto& cBuffer = pImpl->cbStates;
	cBuffer.data.fogStart = fogStart;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetFogColor(DirectX::XMVECTOR fogColor)
{
	auto& cBuffer = pImpl->cbStates;
	cBuffer.data.fogColor = fogColor;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetFogRange(float fogRange)
{
	auto& cBuffer = pImpl->cbStates;
	cBuffer.data.fogRange = fogRange;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetShadowEnabled(bool isOn)
{
	auto& cBuffer = pImpl->cbStates;
	cBuffer.data.shadowEnabled = isOn;
	pImpl->isDirty = cBuffer.isDirty = true;
}
void BasicEffect::SetShadowBits(float bits)
{
	auto& cBuffer = pImpl->cbStates;
	cBuffer.data.shadowBits = bits;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::Apply(ComPtr<ID3D11DeviceContext> deviceContext)
{
	auto& pCBuffers = pImpl->cBufferPtrs;
	// ���������󶨵���Ⱦ������
	pCBuffers[0]->BindVS(deviceContext);
	pCBuffers[3]->BindVS(deviceContext);
	pCBuffers[4]->BindVS(deviceContext);

	pCBuffers[1]->BindPS(deviceContext);
	pCBuffers[2]->BindPS(deviceContext);
	pCBuffers[3]->BindPS(deviceContext);
	pCBuffers[4]->BindPS(deviceContext);
	pCBuffers[5]->BindPS(deviceContext);

	// ��������
	deviceContext->PSSetShaderResources(0, 1, pImpl->textureDiffuse.GetAddressOf());
	deviceContext->PSSetShaderResources(1, 1, pImpl->textureShadow.GetAddressOf());

	if (pImpl->isDirty)
	{
		pImpl->isDirty = false;
		for (auto& pCBuffer : pCBuffers)
		{
			pCBuffer->UpdateBuffer(deviceContext);
		}
	}
}


