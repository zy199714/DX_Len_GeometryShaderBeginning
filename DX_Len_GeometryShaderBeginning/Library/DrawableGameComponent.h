#pragma once

#include "GameComponent.h"

namespace Library
{
	class GameCamera;

	class DrawableGameComponent : public GameComponent
	{
	public:
		DrawableGameComponent();
		DrawableGameComponent(GameCamera& camera);
		virtual ~DrawableGameComponent();

		bool Visible() const;
		void SetVisible(bool visible);

		GameCamera* GetCamera();
		void SetCamera(GameCamera* camera);

		virtual void Draw(const GameTime& gameTime);

	protected:
		bool mVisible;
		GameCamera* mCamera;

	private:
		DrawableGameComponent(const DrawableGameComponent& rhs);
		DrawableGameComponent& operator=(const DrawableGameComponent& rhs);
	};
}