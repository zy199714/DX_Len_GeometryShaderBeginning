#include "GameComponent.h"
#include "GameTime.h"

namespace Library
{
	GameComponent::GameComponent()
		:mEnabled(true)
	{

	}

	GameComponent::~GameComponent()
	{

	}

	bool GameComponent::Enabled() const
	{
		return mEnabled;
	}

	void GameComponent::SetEnabled(bool enabled)
	{
		mEnabled = enabled;
	}

	void GameComponent::Initialize()
	{
	}

	void GameComponent::Update(const GameTime& gameTime)
	{
	}
}