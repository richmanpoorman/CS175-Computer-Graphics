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
* curve function for a line
*/
glm::vec2 coneCurve (float t) {
    return glm::vec2(t, height / 2 - t * height / radius);
}

/* coneNormal
* the normal of any point on the line is just the orthogonal slope
*/
glm::vec2 inverseSlopeDirection = glm::normalize(glm::vec2(height, radius));
glm::vec2 coneNormal(float t) {
    return inverseSlopeDirection;
}

Cone::Cone() {
    //cout << "CONE" << endl;
	cone = RevolutionSolid({radius, 0}, &coneCurve, &coneNormal);
};

Cone::~Cone() {};

void Cone::draw() { cone.draw(); }

void Cone::drawNormal() { cone.drawNormal(); }

#undef radius 
#undef height