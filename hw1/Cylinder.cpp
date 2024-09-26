#include "Cylinder.h"
#include "Geometry.h"
#include "RevolutionSolid.h"
#include "Shape.h"
#include <iostream>


#define height 1.0
#define radius 0.5

using namespace std;

Vertex cylinderFunction(int yStep, int thetaStep) {
	
	int totalThetaSteps = Shape::m_segmentsY,
		totalYSteps     = Shape::m_segmentsX;
	//cout << "TESSELATION: " << thetaStep << "/" << totalThetaSteps << ", " << yStep << "/" << totalYSteps << endl;
	float x = cos(2.0 * thetaStep * PI / totalThetaSteps),
		  y = -height / 2.0 + 1.0 * height * yStep / totalYSteps,
		  z = sin(2.0 * PI * thetaStep / totalThetaSteps);
	//cout << "XYZ (" << x << ", " << y << ", " << z << ")" << endl;;
	glm::vec3 position = glm::vec3(x, y, z);
	glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));
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