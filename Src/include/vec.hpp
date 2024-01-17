#pragma once
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define Vec3 glm::vec3
#define Vec4 glm::vec4

/*
class Vec3 {
	public:
		float x,y,z;

		// Constructors
		Vec3(float x_, float y_, float z_);
		Vec3(float x_);
		Vec3();

		// Unary Operators
		Vec3 operator-();

		// Binary Operators
		bool operator==(Vec3 v);
		Vec3 operator+(Vec3 v);
		Vec3 operator-(Vec3 v);
		Vec3 operator*(float f);
		Vec3 operator/(float f);

		float magnitude();
		float magnitude_sq();

		float dot(Vec3 v);
		Vec3 cross(Vec3 v);		
		Vec3 normalize();
};
*/