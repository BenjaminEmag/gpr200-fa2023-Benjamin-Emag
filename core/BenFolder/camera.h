#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "transformations.h"

namespace ml {
	struct Camera {
		ew::Vec3 position; //Camera body position
		ew::Vec3 target; //Position to look at
		float fov; //Vertical field of view in degrees
		float aspectRatio; //Screen width / Screen height
		float nearPlane; //Near plane distance (+Z)
		float farPlane; //Far plane distance (+Z)
		bool orthographic; //Perspective or orthographic?
		float orthoSize; //Height of orthographic frustum
		ew::Mat4 ViewMatrix() //World->View
		{
			return ml::LookAt(position, target, ew::Vec3(0, 1, 0));
		}

		ew::Mat4 ProjectionMatrix() {
			if (orthographic) {
				return Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
			}
			else {
				return Perspective(fov, aspectRatio, nearPlane, farPlane);
			}
		}
	};
}