#include "procGen.h"

ew::MeshData MyLib::createSphere(float radius, int numSegments)
{
	ew::MeshData mesh;
	float thetaStep = 2 * ew::PI / numSegments;
	float phiStep = ew::PI / numSegments;
	float theta;
	float phi;

	for (int row = 0; row <= numSegments; row++)
	{
		phi = row * phiStep;
		for (int col = 0; col <= numSegments; col++)
		{
			theta = col * thetaStep;

			// vertex position
			ew::Vertex v;
			v.pos.x = radius * cos(theta) * sin(phi);
			v.pos.y = radius * cos(phi);
			v.pos.z = radius * sin(theta) * sin(phi);

			// texture coordinates
			v.uv = ew::Vec2(ew::PI * radius / numSegments * col, ew::PI * radius / numSegments * (numSegments - row));
			v.normal = ew::Normalize(v.pos);
			mesh.vertices.push_back(v);
		}
	}

	// indices for the sphere mesh
	int poleStart = 0;
	int sideStart = numSegments + 1;

	for (int i = 0; i < numSegments; i++)
	{
		mesh.indices.push_back(sideStart + i);
		mesh.indices.push_back(poleStart + i);
		mesh.indices.push_back(sideStart + i + 1);
	}

	int start;
	int columns = numSegments + 1;

	// indices for the sides of the sphere
	for (int row = 1; row < numSegments - 1; row++)
	{
		for (int col = 0; col < numSegments; col++)
		{
			start = (row * columns) + col;

			mesh.indices.push_back(start);
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns);
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns + 1);
			mesh.indices.push_back(start + columns);
		}
	}

	// indices for the bottom pole of the sphere
	poleStart = mesh.vertices.size() - numSegments;
	sideStart = mesh.vertices.size() - 1;

	for (int i = 0; i < numSegments; i++)
	{
		mesh.indices.push_back(sideStart + i + 1);
		mesh.indices.push_back(poleStart + i);
		mesh.indices.push_back(sideStart + i);
	}
	return mesh;
}



ew::MeshData MyLib::createCylinder(float height, float radius, int numSegments)
{
	ew::MeshData mesh;
	ew::Vertex v;
	float thetaStep = 2 * ew::PI / numSegments;
	float theta;
	int columns = numSegments + 1;
	float topY = height / 2;
	float bottomY = -topY;

	// Top center vertex
	v.pos.x = 0;
	v.pos.y = topY;
	v.pos.z = 0;
	v.normal = ew::Vec3(0, 1, 0);
	v.uv = ew::Vec2(0.5, 0.5);
	mesh.vertices.push_back(v);

	// Top ring vertices
	for (int i = 0; i <= numSegments; i++) {
		theta = i * thetaStep;
		v.pos.x = cos(theta) * radius;
		v.pos.z = sin(theta) * radius;
		v.pos.y = topY;
		v.normal = ew::Vec3(0, 1, 0);
		v.uv = ew::Vec2(cos(theta), sin(theta));
		v.uv = (v.uv + 1) / 2;
		mesh.vertices.push_back(v);
	}

	// Top side ring vertices
	for (int i = 0; i <= numSegments; i++) {
		theta = i * thetaStep;
		v.pos.x = cos(theta) * radius;
		v.pos.z = sin(theta) * radius;
		v.pos.y = topY;
		v.normal = ew::Vec3(cos(theta), 0, sin(theta));
		v.uv = ew::Vec2(1 / static_cast<float>(numSegments) * i, 1);
		mesh.vertices.push_back(v);
	}

	// Bottom side ring vertices
	for (int i = 0; i <= numSegments; i++) {
		theta = i * thetaStep;
		v.pos.x = cos(theta) * radius;
		v.pos.z = sin(theta) * radius;
		v.pos.y = bottomY;
		v.normal = ew::Vec3(cos(theta), 0, sin(theta));
		v.uv = ew::Vec2(1 / static_cast<float>(numSegments) * i, 0);
		mesh.vertices.push_back(v);
	}

	// Bottom ring vertices
	for (int i = 0; i <= numSegments; i++) {
		theta = i * thetaStep;
		v.pos.x = cos(theta) * radius;
		v.pos.z = sin(theta) * radius;
		v.pos.y = bottomY;
		v.normal = ew::Vec3(0, -1, 0);
		v.uv = ew::Vec2(cos(theta), sin(theta));
		mesh.vertices.push_back(v);
	}

	// Bottom center vertex
	v.pos.x = 0;
	v.pos.y = bottomY;
	v.pos.z = 0;
	v.normal = ew::Vec3(0, -1, 0);
	v.uv = ew::Vec2(0, 0);
	mesh.vertices.push_back(v);

	// Top indices
	int start = 1;
	int center = 0;
	for (int i = 0; i <= numSegments; i++) {
		mesh.indices.push_back(start + i);
		mesh.indices.push_back(center);
		mesh.indices.push_back(start + i + 1);
	}

	// Side indices
	int sideStart = numSegments + 1;
	for (int i = 0; i <= columns; i++) {
		int start = sideStart + i;

		// Triangle 1
		mesh.indices.push_back(start);
		mesh.indices.push_back(start + 1);
		mesh.indices.push_back(start + columns);

		// Triangle 2
		mesh.indices.push_back(start + 1);
		mesh.indices.push_back(start + columns + 1);
		mesh.indices.push_back(start + columns);
	}

	// Bottom indices
	center = mesh.vertices.size() - 1;
	start = center - columns;

	for (int i = 0; i <= numSegments; i++) {
		mesh.indices.push_back(start + i);
		mesh.indices.push_back(center);
		mesh.indices.push_back(start + i + 1);
	}

	return mesh;
}

ew::MeshData MyLib::createPlane(float size, int subdivisions)
{
	ew::MeshData mesh;
	ew::Vertex v;

	int row;
	int col;
	int columns = subdivisions + 1;

	// vertices
	for (row = 0; row <= subdivisions; row++) {
		for (col = 0; col <= subdivisions; col++) {
			// vertex position
			v.pos.x = size * (static_cast<float>(col) / subdivisions);
			v.pos.z = -size * (static_cast<float>(row) / subdivisions);

			v.normal = ew::Vec3(0, 1, 0);

			// texture coordinates
			v.uv = ew::Vec2(size / subdivisions * row, size / subdivisions * col);

			mesh.vertices.push_back(v);
		}
	}

	// indices for triangles
	for (row = 0; row < subdivisions; row++) {
		for (col = 0; col < subdivisions; col++) {
			int start = row * columns + col;

			// Bottom right triangle
			mesh.indices.push_back(start);
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns + 1);

			// Top left triangle
			mesh.indices.push_back(start);
			mesh.indices.push_back(start + columns + 1);
			mesh.indices.push_back(start + columns);
		}
	}

	return mesh;
}