#pragma once
#include "../ew/ewMath/ewMath.h"
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/vec4.h"

namespace ml {
	// Identity Matrix
	inline ew::Mat4 Identity() {
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}

	// Translate x, y, z
	inline ew::Mat4 Translate(ew::Vec3 t) {
		return ew::Mat4(
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1
		);
	}

	// Scale on x, y, z axis
	inline ew::Mat4 Scale(ew::Vec3 s) {
		return ew::Mat4(
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		);
	}

	// Rotation around x axis (pitch) in radians
	inline ew::Mat4 RotateX(float rad) {
		return ew::Mat4(
			1, 0, 0, 0,
			0, cos(rad), -sin(rad), 0,
			0, sin(rad), cos(rad), 0,
			0, 0, 0, 1
		);
	}

	// Rotation around y axis (yaw) in radians
	inline ew::Mat4 RotateY(float rad) {
		return ew::Mat4(
			cos(rad), 0, sin(rad), 0,
			0, 1, 0, 0,
			-sin(rad), 0, cos(rad), 0,
			0, 0, 0, 1
		);
	}

	// Rotation around z axis (roll) in radians
	inline ew::Mat4 RotateZ(float rad) {
		return ew::Mat4(
			cos(rad), -sin(rad), 0, 0,
			sin(rad), cos(rad), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}

	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f); // Euler angles (degrees)
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
		ew::Mat4 getModelMatrix() const {
			return ml::Translate(position) * ml::RotateY(rotation.y) * ml::RotateX(rotation.x) * ml::RotateZ(rotation.z) * ml::Scale(scale);
		}
	};

	inline ew::Mat4 LookAt(ew::Vec3 eye, ew::Vec3 target, ew::Vec3 up) {
		ew::Vec3 f = ew::Normalize(eye - target);
		ew::Vec3 r = ew::Normalize(ew::Cross(up, f));
		ew::Vec3 u = ew::Normalize(ew::Cross(f, r));
		return ew::Mat4(
			r.x, r.y, r.z, -1 * ew::Dot(r, eye),
			u.x, u.y, u.z, -1 * ew::Dot(u, eye),
			f.x, f.y, f.z, -1 * ew::Dot(f, eye),
			0, 0, 0, 1
		);
	}

	// Orthographic projection
	inline ew::Mat4 Orthographic(float height, float aspect, float near, float far) {
		float width = height * aspect;
		float r = width / 2;
		float t = height / 2;
		float l = -r;
		float b = -t;
		return ew::Mat4(
			2 / (r - l), 0, 0, -(r + l) / (r - l),
			0, 2 / (t - b), 0, -(t + b) / (t - b),
			0, 0, -2 / (far - near), -(far + near) / (far - near),
			0, 0, 0, 1
		);
	}

	inline ew::Mat4 Perspective(float fov, float aspect, float near, float far) {
		fov = ew::Radians(fov);
		return ew::Mat4(
			1 / (tan(fov / 2) * aspect), 0, 0, 0,
			0, 1 / tan(fov / 2), 0, 0,
			0, 0, (near + far) / (near - far), (2 * far * near) / (near - far),
			0, 0, -1, 0
		);
	};
};