#include "Cylinder.h"
#include "Geometry.h"
#include "RevolutionSolid.h"
#include "Shape.h"
#include <iostream>
#include <glm/glm.hpp>


#define height 1.0
#define radius 0.5

using namespace std;

// function for revolution solid interface
glm::vec2 cylinderCurve(float t) {
	return glm::vec2(radius, t);
}

// like the worksheet, the normal is always on the x axis around the cylinder 
glm::vec2 cylinderNormal(float t) {
	return glm::vec2(1, 0);
}

Cylinder::Cylinder() {
	// use the Reovlution Solid interface to do the rest for us
	cylinder = RevolutionSolid({-1.0 * height / 2.0, height / 2.0}, &cylinderCurve, &cylinderNormal);
}
Cylinder::~Cylinder() {};

void Cylinder::draw() { cylinder.draw(); }

void Cylinder::drawNormal() { cylinder.drawNormal(); }

#undef height 
#undef radius