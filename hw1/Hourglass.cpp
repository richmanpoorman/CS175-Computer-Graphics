#include "Hourglass.h"
#include "Geometry.h"
#include "RevolutionSolid.h"
#include "Shape.h"
#include <iostream>
#include <glm/glm.hpp>


using namespace std;

/* HourglassCurve
*
* curve function for a Hourglass is a 2D semi-circle 
* with radius 0.5 */
float normalizingX = sqrt(2) - 0.5;
glm::vec2 hourglassCurve (float t) {
    return glm::vec2((1 / cos(t)) - normalizingX,0.5 * tan(t));
}

/* HourglassNormal
* the normal of any point on a Hourglass is the same 
* as the vector from the Hourglass's center to that 
* point */
glm::vec2 hourglassNormal(float t) {
    return glm::normalize(glm::vec2(0.5 / (cos(t) * cos(t)), -tan(t) / cos(t)));
}

Hourglass::Hourglass() {
	hourglass = RevolutionSolid({-0.25 * PI, 0.25 * PI}, &hourglassCurve, &hourglassNormal);
};

Hourglass::~Hourglass() {};

void Hourglass::draw() { hourglass.draw(); }

void Hourglass::drawNormal() { hourglass.drawNormal(); }