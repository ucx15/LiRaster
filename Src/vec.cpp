#include <math.h>

#include "vec.hpp"


// Constructors
Vec3::Vec3(float x_, float y_, float z_): x(x_), y(y_), z(z_) {}
Vec3::Vec3(float x_): x(x_), y(x_), z(x_) {}
Vec3::Vec3(): x(0.f), y(0.f), z(0.f) {}


// Operators
Vec3 Vec3::operator-() {
	return Vec3(-x, -y, -z);
}

bool Vec3::operator==(Vec3 v) {
	return ((x==v.x) && (y==v.y) && (z==v.z));
}

Vec3 Vec3::operator+(Vec3 v) {
	return Vec3(x+v.x, y+v.y, z+v.z);
}

Vec3 Vec3::operator-(Vec3 v) {
	return Vec3(x-v.x, y-v.y, z-v.z);
}

Vec3 Vec3::operator*(float f) {
	return Vec3(x*f, y*f, z*f);
}

Vec3 Vec3::operator/(float f) {
	return Vec3(x/f, y/f, z/f);
}


// Methods
float Vec3::magnitude() {
	return sqrtf((x*x) + (y*y) + (z*z));
}

float Vec3::magnitude_sq() {
	return ((x*x) + (y*y) + (z*z));
}

 float Vec3::dot(Vec3 v) {
	return (x*v.x) + (y*v.y) + (z*v.z);
}

Vec3 Vec3::cross(Vec3 v) {
	return Vec3((y*v.z - z*v.y), (z*v.x - x*v.z), (x*v.y - y*v.x));
}

Vec3 Vec3::normalize() {
	float mag = this->magnitude();
	return Vec3((float)x/mag, (float)y/mag, (float)z/mag);
}
