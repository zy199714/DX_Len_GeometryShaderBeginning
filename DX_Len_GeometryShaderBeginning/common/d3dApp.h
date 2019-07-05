#ifndef D3DAPP_H
#define D3DAPP_H

#include <wrl/client.h>
#include <string>
#include <d2d1.h>
#include <dwrite.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Mouse.h"
#include "Keyboard.h"
#include "GameTimer.h"

//添加所有要引用的库
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE AppInst() const;			//获取y应用实例句柄
	HWND MainWnd() const;				//	获取窗口实例句柄
	float AspectRatio() const;				//获取屏幕宽高比

	int Run();				//运行程序，执行消息循环

	virtual bool Init();				//初始化窗口、Direct3D、Direct2D部分
	virtual void OnResize();	//窗口大小改变是调用
	virtual void UpdateScene(float dt) = 0;				//完成每一帧的更新
	virtual void DrawScene() = 0;				//完成每一帧的绘制
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);		//消息回调

protected:
	bool InitMainWindow();
	bool InitDirect2D();
	bool InitDirect3D();

	void CalculateFrameStats();		//计算每秒帧数并在窗口显示

protected:
	HINSTANCE mhAppInst;			//应用实例句柄
	HWND mhMainWnd;				//窗口实例句柄
	bool mAppPaused;					//是否暂停
	bool mMinimized;					//是否最小化
	bool mMaximized;					//是否最大化
	bool mResizing;						//是否窗口大小变化
	bool mEnable4xMsaa;				//是否开启4倍多重采样
	UINT m4xMsaaQuality;			//MSAA支持的质量等级

	GameTimer mTimer;		//计时器

	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// Direct2D
	ComPtr<ID2D1Factory> md2dFactory;
	ComPtr<ID2D1RenderTarget> md2dRenderTarget;
	ComPtr<IDWriteFactory> mdwriteFactory;
	//Direct3D
	ComPtr<ID3D11Device> md3dDevice;
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;
	ComPtr<IDXGISwapChain> mSwapChain;
	//常用资源
	ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> mRenderTargetView;
	ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;				//视口
	//键鼠输入
	std::unique_ptr<DirectX::Mouse> mMouse;
	DirectX::Mouse::ButtonStateTracker mMouseTracker;
	std::unique_ptr<DirectX::Keyboard> mKeyboard;
	DirectX::Keyboard::KeyboardStateTracker mKeyboardTracker;
	// 派生类应该在构造函数设置好这些自定义的初始参数
	std::wstring mMainWndCaption;		                   // 主窗口标题
	int mClientWidth;                                           // 视口宽度
	int mClientHeight;                                          // 视口高度
};
#endif // !D3DAPP_H