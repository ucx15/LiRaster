#include "tris.hpp"


// ------ Tris3D Class ------
// Ctors and Dtors
Tris3D::Tris3D() {}
Tris3D::Tris3D(Vec3 a,  Vec3 b,  Vec3 c) : v1(a), v2(b), v3(c) {}
Tris3D::~Tris3D() {}

// Methods
Vec3 Tris3D::getCenter() {
	return (v1 + v2 + v3) / 3.0f;
}


// ------ Tris2D Class ------
// Ctors and Dtors
Tris2D::Tris2D() {}
Tris2D::Tris2D(Vec2 a,  Vec2 b,  Vec2 c) : v1(a), v2(b), v3(c) {}
Tris2D::~Tris2D() {}

// Methods
Vec2 Tris2D::getCenter() {
	return (v1 + v2 + v3) / 3.0f;
}


// ------ Tris2D_i Class ------
// (No methods, just a data structure)
