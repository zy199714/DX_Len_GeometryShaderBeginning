#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "common/d3dApp.h"
#include "Library/FirstPersonGameCamera.h"
#include "common/GameObject.h"
#include "common/TextureRender.h"
#include "common/ObjReader.h"

using namespace Library;

class GameApp : public D3DApp
{
public:
	// �����ģʽ
	enum class CameraMode { FirstPerson, ThirdPerson, Free };

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitResource();
	void CreateRandomTrees();

	void DrawTree();

private:
	int count = 0;
	float spend = 10.0f;
	float thet = (XM_PI / 180) * 10;
	
	ComPtr<ID2D1SolidColorBrush> mColorBrush;				// ��ɫ��ˢ
	ComPtr<IDWriteFont> mFont;								// ����
	ComPtr<IDWriteTextFormat> mTextFormat;					// �ı���ʽ

	GameObject mTrees;										// ��
	GameObject mGround;										// ����
	GameObject mPeople;
	GameObject mArrow;
	GameObject mMiniMap;
	std::vector<DirectX::XMMATRIX> mInstancedData;			// ����ʵ������

	BasicEffect mBasicEffect;								// ������Ⱦ��Ч����
	MiniMapEffect mMiniMapEffect;

	std::unique_ptr<TextureRender> mRenderShadow;  //shadow map
	bool mShadowEnabled;
	float mShadowBits;

	bool pp = false;

	float pitch = 0.0f;
	float roll = 0.0f;
	float yaw = 0.0f;

	FirstPersonGameCamera* mCamera;						// �����
	FirstPersonGameCamera* mContonl=nullptr;
	FirstPersonGameCamera* mShadowCamrea;
	CameraMode mCameraMode;									// �����ģʽ
};


#endif