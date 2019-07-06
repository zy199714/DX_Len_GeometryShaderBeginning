#include "GameApp.h"
#include "common/d3dUtil.h"
#include "common/DXTrace.h"
#include <wincodec.h>

using namespace DirectX;
using namespace std::experimental;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

GameApp::~GameApp()
{
	delete mCamera;
	delete mShadowCamrea;
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	// ����ȳ�ʼ��������Ⱦ״̬���Թ��������Чʹ��
	RenderStates::InitAll(md3dDevice);

	if (!mBasicEffect.InitAll(md3dDevice))
		return false;

	if (!mMiniMapEffect.InitAll(md3dDevice))
		return false;

	if (!InitResource())
		return false;

	// ��ʼ����꣬���̲���Ҫ
	mMouse->SetWindow(mhMainWnd);
	mMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	 
	return true;
}

void GameApp::OnResize()
{
	assert(md2dFactory);
	assert(mdwriteFactory);
	// �ͷ�D2D�������Դ
	mColorBrush.Reset();
	md2dRenderTarget.Reset();

	D3DApp::OnResize();

	// ΪD2D����DXGI������ȾĿ��
	ComPtr<IDXGISurface> surface;
	HR(mSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = md2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, md2dRenderTarget.GetAddressOf());
	surface.Reset();

	if (hr == E_NOINTERFACE)
	{
		OutputDebugString(L"\n���棺Direct2D��Direct3D�������Թ������ޣ��㽫�޷������ı���Ϣ�����ṩ������ѡ������\n"
			"1. ����Win7ϵͳ����Ҫ������Win7 SP1������װKB2670838������֧��Direct2D��ʾ��\n"
			"2. �������Direct3D 10.1��Direct2D�Ľ�����������ģ�"
			"3. ʹ�ñ������⣬����FreeType��\n\n");
	}
	else if (hr == S_OK)
	{
		// �����̶���ɫˢ���ı���ʽ
		HR(md2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			mColorBrush.GetAddressOf()));
		HR(mdwriteFactory->CreateTextFormat(L"����", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn",
			mTextFormat.GetAddressOf()));
	}
	else
	{
		// �����쳣����
		assert(md2dRenderTarget);
	}
}

void GameApp::UpdateScene(float dt)
{

	// ��������¼�����ȡ���ƫ����
	Mouse::State mouseState = mMouse->GetState();
	Mouse::State lastMouseState = mMouseTracker.GetLastState();
	mMouseTracker.Update(mouseState);

	Keyboard::State keyState = mKeyboard->GetState();
	mKeyboardTracker.Update(keyState);

		// ********************
		// ����������Ĳ���
		//
		// �����ƶ�
	if (mKeyboardTracker.IsKeyPressed(Keyboard::N))
	{
		if (count == 0)
		{
			mContonl = mShadowCamrea;
			count = 1;
		}
		else
		{
			mContonl = mCamera;
			count = 0;
		}
	}

		if (keyState.IsKeyDown(Keyboard::W))
			mContonl->MoveForward(dt * spend);
		if (keyState.IsKeyDown(Keyboard::S))
			mContonl->MoveForward(dt * -spend);
		if (keyState.IsKeyDown(Keyboard::A))
			mContonl->Strafe(dt * -spend);
		if (keyState.IsKeyDown(Keyboard::D))
			mContonl->Strafe(dt * spend);

		if (mKeyboardTracker.IsKeyPressed(Keyboard::D1))
		{
			mShadowEnabled = !mShadowEnabled;
			mBasicEffect.SetShadowEnabled(mShadowEnabled);
		}
		if (mKeyboardTracker.IsKeyPressed(Keyboard::D2))
		{
			pp = !pp;
		}

		if (mKeyboardTracker.IsKeyPressed(Keyboard::Q))
		{
			mShadowBits += 0.0000001f;
			mBasicEffect.SetShadowBits(mShadowBits);
		}
		if (mKeyboardTracker.IsKeyPressed(Keyboard::E))
		{
			mShadowBits -= 0.0000001f;
			mBasicEffect.SetShadowBits(mShadowBits);
		}

		if (mKeyboardTracker.IsKeyPressed(Keyboard::I))
		{
			pitch += thet;
			//dirLight.Direction += 0.01f;
		}
		if (mKeyboardTracker.IsKeyPressed(Keyboard::O))
		{
			yaw += thet;
		}
		if (mKeyboardTracker.IsKeyPressed(Keyboard::P))
		{
			roll += thet;
		}
		if (mKeyboardTracker.IsKeyPressed(Keyboard::J))
		{
			pitch -= thet;
		}
		if (mKeyboardTracker.IsKeyPressed(Keyboard::K))
		{
			yaw -= thet;
		}
		if (mKeyboardTracker.IsKeyPressed(Keyboard::L))
		{
			roll -= thet;
		}
		if (mKeyboardTracker.IsKeyPressed(Keyboard::M))
		{
			pitch = yaw = roll = 0.0f;
		}
		mArrow.SetRotate(pitch, yaw, roll);
		
		// ��Ұ��ת����ֹ��ʼ�Ĳ�ֵ�����µ�ͻȻ��ת
		mContonl->Pitch(mouseState.y * dt * 1.25f);
		mContonl->Roll(mouseState.x * dt * 1.25f);

	// ���¹۲����
	mContonl->UpdateViewMatrix();
	mBasicEffect.SetViewMatrix(mContonl->ViewMatrix());
	mBasicEffect.SetEyePos(mContonl->PositionVector());

	mBasicEffect.SetLightViewMatrix(mShadowCamrea->ViewMatrix());
	mBasicEffect.SetLightProjMatrix(mShadowCamrea->OrthogonalProjectionMatrix());
	mBasicEffect.SetLightPos(mShadowCamrea->PositionVector());
		
	// ���ù���ֵ				
	mMouse->ResetScrollWheelValue();

	// �˳�����
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Escape))
	{
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
	}
}

void GameApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	if (pp)
	{	
		mRenderShadow->Begin(md3dImmediateContext);

		//ʵ������
		mBasicEffect.SetRenderShadowMap(md3dImmediateContext, BasicEffect::RenderInstance);
		DrawTree();
		//��������
		mBasicEffect.SetRenderShadowMap(md3dImmediateContext, BasicEffect::RenderObject);

		//mTrees.Draw(md3dImmediateContext, mBasicEffect);
		// �������
		//ComPtr<ID3D11Texture2D> backBuffer;
		//mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		//HR(SaveWICTextureToFile(md3dImmediateContext.Get(), backBuffer.Get(), GUID_ContainerFormatPng, L"Screenshot\\output.png"));
		//pp = false;
		mBasicEffect.SetTextureShadow(mRenderShadow->GetOutputTextureRT());
		mRenderShadow->End(md3dImmediateContext);
	}
	
	// ******************
	// ����Direct3D����
	//
	// Ԥ����պ󱸻�����
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ���Ƶ���,��
	mBasicEffect.SetRenderDefault(md3dImmediateContext, BasicEffect::RenderObject);
	mGround.Draw(md3dImmediateContext, mBasicEffect);
	//mTrees.Draw(md3dImmediateContext, mBasicEffect);
	mArrow.Draw(md3dImmediateContext, mBasicEffect);

	mBasicEffect.SetRenderDefault(md3dImmediateContext, BasicEffect::RenderInstance);
	DrawTree();


	mMiniMapEffect.SetRenderDefault(md3dImmediateContext);
	mMiniMapEffect.SetTextureDiffuse(mRenderShadow->GetOutputTextureRT());
	mMiniMap.Draw(md3dImmediateContext, mMiniMapEffect);

	// ******************
	// ����Direct2D����
	//
	if (md2dRenderTarget != nullptr)
	{
		md2dRenderTarget->BeginDraw();
		std::wstring text = L"��ǰ�����ģʽ: ��һ�˳�  Esc�˳�\n"
			"����ƶ�������Ұ W/S/A/D �ƶ�\n"
			"��Ӱ:";
		text += (mShadowEnabled ? L"����" : L"�ر�");
		text += L"\nBits:"+std::to_wstring(mShadowBits);
		text += L"\nPitch:" + std::to_wstring((int)(pitch * 180 / XM_PI ));
		text += L"\nYaw:" + std::to_wstring((int)(yaw * 180 / XM_PI ));
		text += L"\nRoll:" + std::to_wstring((int)(roll * 180 / XM_PI ));

		md2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), mTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, mColorBrush.Get());
		HR(md2dRenderTarget->EndDraw());
	}

	HR(mSwapChain->Present(0, 0));
}



bool GameApp::InitResource()
{
	mRenderShadow = std::make_unique<TextureRender>(md3dDevice, 800*4, 600*4, false);

	// ******************
	// ��ʼ����Ϸ����
	//

	// �����������
	CreateRandomTrees();

	mGround.SetModel(md3dDevice.Get(), "Model\\ground.obj");
	mGround.SetPosition(0.0f, 0.0f, 0.0f);


	mTrees.SetModel(md3dDevice.Get(), "Model\\tree.obj");
	//mTrees.SetPosition(0.0f, 20.0f, 0.0f);
	mTrees.SetScaling(0.02f, 0.02f, 0.02f);

	mArrow.SetModel(md3dDevice.Get(), "Model\\arrow.obj");
	mArrow.SetPosition(0.0f, 0.0f, 0.0f);

	mMiniMap.SetMesh(md3dDevice.Get(), Geometry::Create2DShow(0.7f, -0.7f, 0.3f, 0.3f));

	// ******************
	// ��ʼ�������
	//

	// Ĭ�������
	mCameraMode = CameraMode::FirstPerson;
	mCamera = new FirstPersonGameCamera((float)mClientWidth,(float)mClientHeight);
	mCamera->Initialize();
	mCamera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
	
	mContonl = mCamera;
	// ******************
	// ��ʼ����������仯��ֵ
	//
	mShadowCamrea = new FirstPersonGameCamera((float)mClientWidth, (float)mClientHeight);
	mShadowCamrea->Initialize();
	mShadowCamrea->SetPosition(0.0f, 20.0f, 0.0f);
	//mShadowCamrea->SetDirection(0.0f, -1.0f,  0.0f);
	mShadowCamrea->UpdateViewMatrix();

	// ��Ч
	mBasicEffect.SetFogColor(XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f));
	mBasicEffect.SetFogStart(20.0f);
	mBasicEffect.SetFogRange(120.0f);

	// �����(Ĭ��)
	directionalLight_struct dirLight;
	dirLight.Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dirLight.Direction = XMFLOAT3(-0.5f,-0.866f,  0.0f);
	mBasicEffect.SetDirLight(0, dirLight);

	pointLight_struct ponitLight;
	ponitLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	ponitLight.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	ponitLight.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	ponitLight.Position = XMFLOAT3(0.0f, 300.0f, 0.0f);;
	ponitLight.Att = XMFLOAT3(2.0f, 0.0f, 0.0f);
	ponitLight.Range = 1000.0f;
	mBasicEffect.SetPointLight(0, ponitLight);

	mBasicEffect.SetNumLight(1, 1, 0);

	material_struct mat;
	mat.Ambient = { 0.3f, 0.3f, 0.3f,1.0f };
	mat.Diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mat.Specular = { 0.1f, 0.1f, 0.1f, 1.0f };
	mBasicEffect.SetMaterial(mat);

	// ������Ч
	mBasicEffect.SetFogState(false);
	mBasicEffect.SetViewMatrix(mCamera->ViewMatrix());
	mBasicEffect.SetProjMatrix(mCamera->ViewPerspectiveProjectionMatrix());

	// �ر���Ӱ
	mShadowEnabled = 0;
	mShadowBits = 0.0000019f;
	mBasicEffect.SetShadowEnabled(mShadowEnabled);
	mBasicEffect.SetShadowBits(mShadowBits);

	mBasicEffect.SetLightViewMatrix(mShadowCamrea->ViewMatrix());
	mBasicEffect.SetLightProjMatrix(mShadowCamrea->OrthogonalProjectionMatrix());

	return true;
}

void GameApp::DrawTree()
{
	mTrees.DrawInstanced(md3dImmediateContext, mBasicEffect, mInstancedData);
}

void GameApp::CreateRandomTrees()
{
	srand((unsigned)time(nullptr));
	// ��ʼ����
	mTrees.SetModel(md3dDevice.Get(), "Model\\tree.obj");
	XMMATRIX S = XMMatrixScaling(0.03f, 0.03f, 0.03f);

	// �����������������
	float theta = 0.0f;
	for (int i = 0; i < 4; ++i)
	{
		// ȡ5-95�İ뾶�����������
		for (int j =0; j < 2; ++j)
		{
			// ����ԽԶ����ľԽ��
			for (int k = 0; k < 2 * j + 1; ++k)
			{
				float radius = (float)(rand() % 30 + 30 * j + 5);
				float randomRad = rand() % 72 / 72.0f * XM_2PI / 4;
				XMMATRIX T1 = XMMatrixTranslation(radius * cosf(theta + randomRad), 0.0f, radius * sinf(theta + randomRad));
				XMMATRIX R = XMMatrixRotationY(rand() % 72 / 72.0f * XM_2PI);
				XMMATRIX World = S * R * T1;
				mInstancedData.push_back(World);
			}
		}
		theta += XM_2PI / 4;
	}
}
