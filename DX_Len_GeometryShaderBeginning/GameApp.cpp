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

	// 务必先初始化所有渲染状态，以供下面的特效使用
	RenderStates::InitAll(md3dDevice);

	if (!mBasicEffect.InitAll(md3dDevice))
		return false;

	if (!mMiniMapEffect.InitAll(md3dDevice))
		return false;

	if (!InitResource())
		return false;

	// 初始化鼠标，键盘不需要
	mMouse->SetWindow(mhMainWnd);
	mMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	 
	return true;
}

void GameApp::OnResize()
{
	assert(md2dFactory);
	assert(mdwriteFactory);
	// 释放D2D的相关资源
	mColorBrush.Reset();
	md2dRenderTarget.Reset();

	D3DApp::OnResize();

	// 为D2D创建DXGI表面渲染目标
	ComPtr<IDXGISurface> surface;
	HR(mSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = md2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, md2dRenderTarget.GetAddressOf());
	surface.Reset();

	if (hr == E_NOINTERFACE)
	{
		OutputDebugString(L"\n警告：Direct2D与Direct3D互操作性功能受限，你将无法看到文本信息。现提供下述可选方法：\n"
			"1. 对于Win7系统，需要更新至Win7 SP1，并安装KB2670838补丁以支持Direct2D显示。\n"
			"2. 自行完成Direct3D 10.1与Direct2D的交互。详情参阅："
			"3. 使用别的字体库，比如FreeType。\n\n");
	}
	else if (hr == S_OK)
	{
		// 创建固定颜色刷和文本格式
		HR(md2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			mColorBrush.GetAddressOf()));
		HR(mdwriteFactory->CreateTextFormat(L"宋体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn",
			mTextFormat.GetAddressOf()));
	}
	else
	{
		// 报告异常问题
		assert(md2dRenderTarget);
	}
}

void GameApp::UpdateScene(float dt)
{

	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mMouse->GetState();
	Mouse::State lastMouseState = mMouseTracker.GetLastState();
	mMouseTracker.Update(mouseState);

	Keyboard::State keyState = mKeyboard->GetState();
	mKeyboardTracker.Update(keyState);

		// ********************
		// 自由摄像机的操作
		//
		// 方向移动
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
		
		// 视野旋转，防止开始的差值过大导致的突然旋转
		mContonl->Pitch(mouseState.y * dt * 1.25f);
		mContonl->Roll(mouseState.x * dt * 1.25f);

	// 更新观察矩阵
	mContonl->UpdateViewMatrix();
	mBasicEffect.SetViewMatrix(mContonl->ViewMatrix());
	mBasicEffect.SetEyePos(mContonl->PositionVector());

	mBasicEffect.SetLightViewMatrix(mShadowCamrea->ViewMatrix());
	mBasicEffect.SetLightProjMatrix(mShadowCamrea->OrthogonalProjectionMatrix());
	mBasicEffect.SetLightPos(mShadowCamrea->PositionVector());
		
	// 重置滚轮值				
	mMouse->ResetScrollWheelValue();

	// 退出程序
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

		//实例绘制
		mBasicEffect.SetRenderShadowMap(md3dImmediateContext, BasicEffect::RenderInstance);
		DrawTree();
		//单个绘制
		mBasicEffect.SetRenderShadowMap(md3dImmediateContext, BasicEffect::RenderObject);

		//mTrees.Draw(md3dImmediateContext, mBasicEffect);
		// 输出截屏
		//ComPtr<ID3D11Texture2D> backBuffer;
		//mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		//HR(SaveWICTextureToFile(md3dImmediateContext.Get(), backBuffer.Get(), GUID_ContainerFormatPng, L"Screenshot\\output.png"));
		//pp = false;
		mBasicEffect.SetTextureShadow(mRenderShadow->GetOutputTextureRT());
		mRenderShadow->End(md3dImmediateContext);
	}
	
	// ******************
	// 绘制Direct3D部分
	//
	// 预先清空后备缓冲区
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 绘制地面,树
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
	// 绘制Direct2D部分
	//
	if (md2dRenderTarget != nullptr)
	{
		md2dRenderTarget->BeginDraw();
		std::wstring text = L"当前摄像机模式: 第一人称  Esc退出\n"
			"鼠标移动控制视野 W/S/A/D 移动\n"
			"阴影:";
		text += (mShadowEnabled ? L"开启" : L"关闭");
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
	// 初始化游戏对象
	//

	// 创建随机的树
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
	// 初始化摄像机
	//

	// 默认摄像机
	mCameraMode = CameraMode::FirstPerson;
	mCamera = new FirstPersonGameCamera((float)mClientWidth,(float)mClientHeight);
	mCamera->Initialize();
	mCamera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
	
	mContonl = mCamera;
	// ******************
	// 初始化几乎不会变化的值
	//
	mShadowCamrea = new FirstPersonGameCamera((float)mClientWidth, (float)mClientHeight);
	mShadowCamrea->Initialize();
	mShadowCamrea->SetPosition(0.0f, 20.0f, 0.0f);
	//mShadowCamrea->SetDirection(0.0f, -1.0f,  0.0f);
	mShadowCamrea->UpdateViewMatrix();

	// 特效
	mBasicEffect.SetFogColor(XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f));
	mBasicEffect.SetFogStart(20.0f);
	mBasicEffect.SetFogRange(120.0f);

	// 方向光(默认)
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

	// 开启雾效
	mBasicEffect.SetFogState(false);
	mBasicEffect.SetViewMatrix(mCamera->ViewMatrix());
	mBasicEffect.SetProjMatrix(mCamera->ViewPerspectiveProjectionMatrix());

	// 关闭阴影
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
	// 初始化树
	mTrees.SetModel(md3dDevice.Get(), "Model\\tree.obj");
	XMMATRIX S = XMMatrixScaling(0.03f, 0.03f, 0.03f);

	// 随机生成随机朝向的树
	float theta = 0.0f;
	for (int i = 0; i < 4; ++i)
	{
		// 取5-95的半径放置随机的树
		for (int j =0; j < 2; ++j)
		{
			// 距离越远，树木越多
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
