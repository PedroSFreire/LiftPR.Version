#pragma once

#include  "Lift/Core.h"

namespace lift {

	class Input {
	public:
		virtual ~Input() = default;
		inline static bool IsKeyPressed(int key_code);
		inline static bool IsMouseButtonPressed(int button);
		inline static std::pair<float, float> GetMousePos();
		inline static float GetMouseX();
		inline static float GetMouseY();

	protected:
		virtual bool IsKeyPressedImpl(int key_code) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		static Input* instance_;
	};

	inline bool Input::IsKeyPressed(const int key_code) {
		return instance_->IsKeyPressedImpl(key_code);
	}

	inline bool Input::IsMouseButtonPressed(const int button) {
		return instance_->IsMouseButtonPressedImpl(button);
	}

	inline std::pair<float, float> Input::GetMousePos() {
		return instance_->GetMousePosImpl();
	}

	inline float Input::GetMouseX() {
		return instance_->GetMouseXImpl();
	}

	inline float Input::GetMouseY() {
		return instance_->GetMouseYImpl();
	}

}
