#include "Cone.h"
#include "Geometry.h"
#include "RevolutionSolid.h"
#include "Shape.h"
#include <iostream>
#include <glm/glm.hpp>

#define radius 0.5
#define height 1.0
using namespace std;

/* coneCurve
*
* the curve function for a cone
* is a right triangle
*/
glm::vec2 coneCurve (float t) {
    return glm::vec2(t, height / 2 - t * height / radius);
}

/* coneNormal
*
* the normal of any point on the line is just
* the orthogonal slope
*/
glm::vec2 inverseSlopeDirection = glm::normalize(glm::vec2(height, radius));
glm::vec2 coneNormal(float t) {
    return inverseSlopeDirection;
}

/* constructor
*
* provides the domain, surface curve, and 
* normal curve to draw a sphere as a 
* RevolutionSolid object
*
*/
Cone::Cone() {
	cone = RevolutionSolid({radius, 0}, &coneCurve, &coneNormal);
};

/* destructor
*/
Cone::~Cone() {};

/* draw
*
* tessellates the cone and then draws
* the triangles connecting all the 
* vertices 
*/
void Cone::draw() { cone.draw(); }

/* drawNormal
*
* tessellates the cone and then draws the 
* normal vectors off each vertex in the sphere
*
*/
void Cone::drawNormal() { cone.drawNormal(); }

#undef radius 
#undef height