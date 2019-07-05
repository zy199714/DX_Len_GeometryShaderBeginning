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

//�������Ҫ���õĿ�
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

	HINSTANCE AppInst() const;			//��ȡyӦ��ʵ�����
	HWND MainWnd() const;				//	��ȡ����ʵ�����
	float AspectRatio() const;				//��ȡ��Ļ��߱�

	int Run();				//���г���ִ����Ϣѭ��

	virtual bool Init();				//��ʼ�����ڡ�Direct3D��Direct2D����
	virtual void OnResize();	//���ڴ�С�ı��ǵ���
	virtual void UpdateScene(float dt) = 0;				//���ÿһ֡�ĸ���
	virtual void DrawScene() = 0;				//���ÿһ֡�Ļ���
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);		//��Ϣ�ص�

protected:
	bool InitMainWindow();
	bool InitDirect2D();
	bool InitDirect3D();

	void CalculateFrameStats();		//����ÿ��֡�����ڴ�����ʾ

protected:
	HINSTANCE mhAppInst;			//Ӧ��ʵ�����
	HWND mhMainWnd;				//����ʵ�����
	bool mAppPaused;					//�Ƿ���ͣ
	bool mMinimized;					//�Ƿ���С��
	bool mMaximized;					//�Ƿ����
	bool mResizing;						//�Ƿ񴰿ڴ�С�仯
	bool mEnable4xMsaa;				//�Ƿ���4�����ز���
	UINT m4xMsaaQuality;			//MSAA֧�ֵ������ȼ�

	GameTimer mTimer;		//��ʱ��

	// ʹ��ģ�����(C++11)��������
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
	//������Դ
	ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> mRenderTargetView;
	ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;				//�ӿ�
	//��������
	std::unique_ptr<DirectX::Mouse> mMouse;
	DirectX::Mouse::ButtonStateTracker mMouseTracker;
	std::unique_ptr<DirectX::Keyboard> mKeyboard;
	DirectX::Keyboard::KeyboardStateTracker mKeyboardTracker;
	// ������Ӧ���ڹ��캯�����ú���Щ�Զ���ĳ�ʼ����
	std::wstring mMainWndCaption;		                   // �����ڱ���
	int mClientWidth;                                           // �ӿڿ��
	int mClientHeight;                                          // �ӿڸ߶�
};
#endif // !D3DAPP_H