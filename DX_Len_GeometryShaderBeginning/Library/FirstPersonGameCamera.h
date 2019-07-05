#pragma once
#include "GameCamera.h"

namespace Library
{
	class FirstPersonGameCamera : public GameCamera
	{
	public:
		FirstPersonGameCamera(float width, float height);
		FirstPersonGameCamera(float fieldOfView, float width, float height, float nearPlaneDistance, float farPlaneDistance);

		virtual ~FirstPersonGameCamera();

		// 平移
		void Strafe(float d);
		// 直行(平面移动)
		void Walk(float d);
		// 前进(朝前向移动)
		void MoveForward(float d);
		// 上下观察
		void Pitch(float rad);
		// 左右观察
		void Roll(float rad);
	};
}