#pragma once

#include "Common.h"

namespace Library
{
	class GameTime;

	class GameComponent
	{
	public:
		GameComponent();
		virtual ~GameComponent();

		bool Enabled() const;
		void SetEnabled(bool enabled);

		virtual void Initialize();
		virtual void Update(const GameTime& gameTime);
	protected:
		bool mEnabled;
	private:
		GameComponent(const GameComponent& rhs);
		GameComponent& operator=(const GameComponent& rhs);
	};
}