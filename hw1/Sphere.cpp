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

/* constructor
*
* provides the domain, surface curve, and 
* normal curve to draw a sphere as a 
* RevolutionSolid object
*
*/
Sphere::Sphere() {
	sphere = RevolutionSolid({PI, 0}, &sphereCurve, &sphereNormal);
};

/* destructor
*/
Sphere::~Sphere() {};

/* draw
*
* tessellates the sphere and then draws
* the triangles connecting all the 
* vertices 
*/
void Sphere::draw() { sphere.draw(); }

/* drawNormal
*
* tessellates the cube and then draws the 
* normal vectors off each vertex in the sphere
*
*/
void Sphere::drawNormal() { sphere.drawNormal(); }