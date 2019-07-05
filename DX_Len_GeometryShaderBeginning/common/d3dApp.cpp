#include "d3dApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include <sstream>

namespace
{
	D3DApp* gd3dApp = 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance)
	: mhAppInst(hInstance),
	mMainWndCaption(L"Blending"),
	mClientWidth(800),
	mClientHeight(600),
	mhMainWnd(nullptr),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false),
	mEnable4xMsaa(true),
	m4xMsaaQuality(0),
	md3dDevice(nullptr),
	md3dImmediateContext(nullptr),
	mSwapChain(nullptr),
	mDepthStencilBuffer(nullptr),
	mRenderTargetView(nullptr),
	mDepthStencilView(nullptr)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));


	// 让一个全局指针获取这个类，这样我们就可以在Windows消息处理的回调函数
	// 让这个类调用内部的回调函数了
	gd3dApp = this;
}

D3DApp::~D3DApp()
{
	// 恢复所有默认设定
	if (md3dImmediateContext)
		md3dImmediateContext->ClearState();
}

HINSTANCE D3DApp::AppInst()const
{
	return mhAppInst;
}

HWND D3DApp::MainWnd()const
{
	return mhMainWnd;
}

float D3DApp::AspectRatio()const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

int D3DApp::Run()
{
	MSG msg = { 0 };

	mTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mTimer.Tick();
			if (!mAppPaused)
			{
				CalculateFrameStats();
				UpdateScene(mTimer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return msg.wParam;
}

bool D3DApp::Init()
{
	mMouse = std::make_unique<DirectX::Mouse>();
	mKeyboard = std::make_unique<DirectX::Keyboard>();

	if (!InitMainWindow())
		return false;

	if (!InitDirect2D())
		return false;

	if (!InitDirect3D())
		return false;

	return true;
}

void D3DApp::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// 释放交换链的相关资源
	mRenderTargetView.Reset();
	mDepthStencilView.Reset();
	mDepthStencilBuffer.Reset();

	ComPtr<ID3D11Texture2D> backBuffer;
	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	HR(md3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, mRenderTargetView.GetAddressOf()));
	backBuffer.Reset();

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		//24位深度缓冲，8位模板缓冲
	//4xMSAA ?
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 0;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//创建深度缓冲区、深度模板视图
	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, mDepthStencilBuffer.GetAddressOf()));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, mDepthStencilView.GetAddressOf()));
	// 将渲染目标视图和深度/模板缓冲区结合到管线
	md3dImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	//设置视口变换
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Height = (float)mClientHeight;
	mScreenViewport.Width = (float)mClientWidth;
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//激活或取消激活窗口时发送WM_ACTIVATE。
		//当窗口停用时我们暂停游戏
		//当它变得活跃时取消暂停
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (md3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					//如果用户拖动调整大小栏，我们不会调整大小
					//这里的缓冲区因为用户不断
					//拖动调整大小条，WM_SIZE消息流
					//发送到窗口，这将是毫无意义的（和慢）
					//为从拖动接收的每个WM_SIZE消息调整大小
					//调整大小的条形图。 所以相反，我们在用户之后重置
					//完成窗口大小调整并释放调整大小条
					//发送WM_EXITSIZEMOVE消息。
				}
				else // API 调用 SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE 调节窗口大小.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

		//当用户释放调整大小条时发送WM_EXITSIZEMOVE。
		//这里我们根据新的窗口尺寸重置所有内容。
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

		// 销毁窗口
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// alt-enter 时不要发出哔哔声
		return MAKELRESULT(0, MNC_CLOSE);

		// 防止窗口变得太小
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

		// 监测这些键盘/鼠标事件
	case WM_INPUT:

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:

	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		mMouse->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		mKeyboard->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_ACTIVATEAPP:
		mMouse->ProcessMessage(msg, wParam, lParam);
		mKeyboard->ProcessMessage(msg, wParam, lParam);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// 将窗口调整到中心
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//计算窗口大小
	RECT R = { 0,0,mClientWidth,mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"D3DWndClassName", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, (screenWidth - width) / 2, (screenHeight - height) / 2, width, height, 0, 0, mhAppInst, 0);

	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

bool D3DApp::InitDirect2D()
{
	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, md2dFactory.GetAddressOf()));
	HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(mdwriteFactory.GetAddressOf())));

	return true;
}

bool D3DApp::InitDirect3D()
{
	HRESULT hr = S_OK;

	//创建D3D设备、D3D设备上下文
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 驱动类型数组
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	// 特性等级数组
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL featureLevel;
	D3D_DRIVER_TYPE d3dDriverType;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, md3dDevice.GetAddressOf(), &featureLevel, md3dImmediateContext.GetAddressOf());

		if (hr == E_INVALIDARG)
		{
			// Direct3D 11.0 的API不承认D3D_FEATURE_LEVEL_11_1，所以我们需要尝试特性等级11.0以及以下的版本
			hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, md3dDevice.GetAddressOf(), &featureLevel, md3dImmediateContext.GetAddressOf());
		}

		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	if (FAILED(hr))
	{
		MessageBox(0, TEXT("D3D11CreateDevice Failed."), 0, 0);
		return false;
	}

	// 检测是否支持特性等级11.0或11.1
	if (featureLevel != D3D_FEATURE_LEVEL_11_0 && featureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBox(0, TEXT("Direct3D Feature Level 11 unsupported."), 0, 0);
		return false;
	}

	//检查MSAA支持的质量等级
	md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_SNORM, 4, &m4xMsaaQuality);
	assert(m4xMsaaQuality > 0);

	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
	ComPtr<IDXGIFactory1> dxgiFactory1 = nullptr;	// D3D11.0(包含DXGI1.1)的接口类

	// 为了正确创建 DXGI交换链，首先我们需要获取创建 D3D设备 的 DXGI工厂，否则会引发报错：
	// "IDXGIFactory::CreateSwapChain: This function is being called with a device from a different IDXGIFactory."
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgiDevice.GetAddressOf())));
	HR(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory1.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//标志指示栅格用于在曲面上创建图像的方法。
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	//标志指示图像如何拉伸以适合给定监视器的分辨率
	// 是否开启4倍多重采样？
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			//缓冲区用途
	sd.BufferCount = 1;				// 后备缓冲区数目
	sd.OutputWindow = mhMainWnd;		//输出窗口
	sd.Windowed = TRUE;			//是否窗口化
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;		//交换效果
	sd.Flags = 0;				//DXGI_SWAP_CHAIN_FLAG枚举类型的成员，描述交换链行为的选项
	HR(dxgiFactory1->CreateSwapChain(md3dDevice.Get(), &sd, mSwapChain.GetAddressOf()));

	// 可以禁止alt+enter 全屏
	dxgiFactory1->MakeWindowAssociation(mhMainWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

	// 每当窗口被重新调整大小的时候，都需要调用这个OnResize函数。现在调用
	// 以避免代码重复
	OnResize();

	return true;
}

void D3DApp::CalculateFrameStats()
{
	// 该代码计算每秒帧速，并计算每一帧渲染需要的时间，显示在窗口标题
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
