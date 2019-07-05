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

		// ƽ��
		void Strafe(float d);
		// ֱ��(ƽ���ƶ�)
		void Walk(float d);
		// ǰ��(��ǰ���ƶ�)
		void MoveForward(float d);
		// ���¹۲�
		void Pitch(float rad);
		// ���ҹ۲�
		void Roll(float rad);
	};
}