#include "DrawableGameComponent.h"
#include "GameTime.h"

namespace Library
{
	DrawableGameComponent::DrawableGameComponent()
		: GameComponent(), mVisible(true), mCamera(nullptr)
	{
	}

	DrawableGameComponent::DrawableGameComponent(GameCamera& camera)
		: mVisible(true), mCamera(&camera)
	{
	}

	DrawableGameComponent::~DrawableGameComponent()
	{
	}

	bool DrawableGameComponent::Visible() const
	{
		return mVisible;
	}

	void DrawableGameComponent::SetVisible(bool visible)
	{
		mVisible = visible;
	}

	GameCamera* DrawableGameComponent::GetCamera()
	{
		return mCamera;
	}

	void DrawableGameComponent::SetCamera(GameCamera* camera)
	{
		mCamera = camera;
	}

	void DrawableGameComponent::Draw(const GameTime& gameTime)
	{

	}
}