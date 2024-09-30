#include "Sphere.h"
#include "Geometry.h"
#include "RevolutionSolid.h"
#include "Shape.h"
#include <iostream>
#include <glm/glm.hpp>

#define radius 0.5

using namespace std;

/* sphereCurve
*
* curve function for a sphere is a 2D semi-circle 
* with radius 0.5 */
glm::vec2 sphereCurve (float t) {
    return glm::vec2(radius * sin(t), radius * cos(t));
}

/* sphereNormal
* the normal of any point on a sphere is the same 
* as the vector from the sphere's center to that 
* point */
glm::vec2 sphereNormal(float t) {
    return sphereCurve(t);
}

Sphere::Sphere() {
	sphere = RevolutionSolid({PI, 0}, &sphereCurve, &sphereNormal);
};

Sphere::~Sphere() {};

void Sphere::draw() { sphere.draw(); }

void Sphere::drawNormal() { sphere.drawNormal(); }