#include "Cylinder.h"
#include "Geometry.h"
#include "RevolutionSolid.h"
#include "Shape.h"
#include <iostream>


#define height 1.0
#define radius 0.5

using namespace std;

Vertex cylinderFunction(int yStep, int thetaStep) {
	
	int totalThetaSteps = Shape::m_segmentsX,
		totalYSteps     = Shape::m_segmentsY;
	glm::vec3 position = glm::vec3(
		cos(2.0 * thetaStep * PI / totalThetaSteps),
		-height / 2.0 + 1.0 * height * yStep / totalYSteps, 
		sin(2.0 * PI * thetaStep / totalThetaSteps)
	);
	glm::vec3 normal = glm::normalize(position);
	Vertex vertex = Vertex(position, normal);
	//cout << "THETA-STEP: " << thetaStep << " Y-STEP: " << yStep << endl;
	//cout << position.x << ", " << position.y << ", " << position.z << endl;
	return vertex;
}

Cylinder::Cylinder() {
	Vertex top    = Vertex(glm::vec3(0.0, height / 2.0, 0.0), glm::vec3(0.0, 1.0, 0.0)),
		   bottom = Vertex(glm::vec3(0.0, -height / 2.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	
	cylinder = RevolutionSolid(&cylinderFunction, top, bottom, true, true);
}
Cylinder::~Cylinder() {};

void Cylinder::draw() {
	cylinder.draw(); 
}

void Cylinder::drawNormal() { cylinder.drawNormal(); }

#undef height 
#undef radius