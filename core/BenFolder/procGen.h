#pragma once
#include "../ew/procGen.h"
#include "../ew/ewMath/ewMath.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/vec2.h"
#include "../ew/mesh.h"

namespace MyLib 
{
	ew::MeshData createSphere(float radius, int numSegments);
	ew::MeshData createCylinder(float height, float radius, int numSegments);
	ew::MeshData createPlane(float size, int subdivisions);
}