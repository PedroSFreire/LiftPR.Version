﻿#pragma once

class Camera {
public:
	Camera();

	Camera(const vec3& eye, const vec3& look_at, const vec3& up, const float fovy, const float aspect_ratio);

	[[nodiscard]] vec3 Direction() const { return normalize(look_at_ - eye_); }
	void SetDirection(const vec3& direction) { look_at_ = eye_ + length(look_at_ - eye_); }

	[[nodiscard]] const vec3& Eye() const { return eye_; };
	void SetEye(const vec3& eye) { eye_ = eye; }
	[[nodiscard]] const vec3& LookAt() const { return look_at_; }
	void SetLookAt(const vec3& look_at) { look_at_ = look_at; }
	[[nodiscard]] const vec3& Up() const { return up_; }
	void SetUp(const vec3& up) { up_ = up; }
	[[nodiscard]] float Fovy() const { return fovy_; }
	void SetFovy(const float fovy) { fovy_ = fovy; }
	[[nodiscard]] float AspectRatio() const { return aspect_ratio_; }
	void SetAspectRatio(const float& aspect_ratio) { aspect_ratio_ = aspect_ratio; }


	[[nodiscard]] const vec3& VectorU() const { return vector_u_; }
	[[nodiscard]] const vec3& VectorV() const { return vector_v_; }
	[[nodiscard]] const vec3& VectorW() const { return vector_w_; }

	void OnUpdate();
	void Orbit(const float dx, const float dy);
	void Strafe(const float dx, const float dy);
	void Zoom(const float amount);

private:
	vec3 eye_;
	vec3 look_at_;
	vec3 up_;
	float fovy_;
	float aspect_ratio_;

	vec3 vector_u_ {1.0f};
	vec3 vector_v_ {1.0f};
	vec3 vector_w_ {1.0f};

	float mouse_orbit_speed_ {0.01f};
	float mouse_strafe_speed_ {0.001f};
	float mouse_zoom_speed_ {0.1f};

	bool changed_ = true;

};