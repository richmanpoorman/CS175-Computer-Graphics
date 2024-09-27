#include "RevolutionSolid.h"
#include "Shape.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <utility> // For pair
#include <functional>

using namespace std;

glm::vec2 defaultFunction(float t) { return glm::vec2(0, 0); }

RevolutionSolid::RevolutionSolid() { initialize({ 0, 0 }, defaultFunction, defaultFunction, false, false); }
RevolutionSolid::RevolutionSolid(pair<float, float> domain, std::function<glm::vec2(float)> surfaceCurve, std::function<glm::vec2(float)> surfaceNormal) { initialize(domain, surfaceCurve, surfaceNormal, true, true); }
RevolutionSolid::RevolutionSolid(pair<float, float> domain, std::function<glm::vec2(float)> surfaceCurve, std::function<glm::vec2(float)>  surfaceNormal, bool hasFlatTop, bool hasFlatBottom) { initialize(domain, surfaceCurve, surfaceNormal, hasFlatTop, hasFlatBottom); }
RevolutionSolid::~RevolutionSolid() {}

void RevolutionSolid::draw() { closedPolygon.draw(); }
void RevolutionSolid::drawNormal() { closedPolygon.drawNormal(); }

function<Vertex(int, int)> makeCurveFunction(float leftT, float rightT, function<glm::vec2(float)> curveFunction, function<glm::vec2(float)> normalFunction) {
	
	function<Vertex(int, int)> curveFunc = [=](int yStep, int thetaStep) {
		int totalThetaSteps = Shape::m_segmentsY,
			totalYSteps = Shape::m_segmentsX;

		// Calcuate angle and y based off of step
		float portionOfTheWay = (1.0 * yStep / totalYSteps);
		float t = leftT * (1.0 - portionOfTheWay) + portionOfTheWay * rightT; // Parameterize linear transition between two points
		float angle = 2.0 * PI * thetaStep / totalThetaSteps;

		// Find 4x4 matrix representing a rotation by the given angle
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(0, 1, 0));

		// Get the 2D points and normal of the curve that will be rotated around the y axis'

		glm::vec2 onCurve = curveFunction(t);
		glm::vec2 normalOnCurve = normalFunction(t);

		// Compute the vertex arrow after rotating
		glm::vec4 positionVector = rotationMatrix * glm::vec4(onCurve.x, onCurve.y, 0, 0);
		glm::vec4 normalVector = rotationMatrix * glm::vec4(normalOnCurve.x, normalOnCurve.y, 0, 0);

		// Turn it into a vertex
		Vertex vertex = Vertex(glm::vec3(positionVector), glm::vec3(normalVector));
		return vertex;
	};
	return curveFunc;
}

void RevolutionSolid::initialize(pair<float, float> domain, function<glm::vec2(float)> surfaceCurve, std::function<glm::vec2(float)> surfaceNormal,
								 bool hasFlatTop, bool hasFlatBottom) {
	leftT = domain.first, rightT = domain.second; 
	curveFunction = surfaceCurve;
	normalFunction = surfaceNormal; 
	isFlatTop = hasFlatTop, isFlatBottom = hasFlatBottom;

	float leftTResultY = curveFunction(leftT).y, 
		  rightTResultY = curveFunction(rightT).y;
	Vertex topVertex    = Vertex(glm::vec3(0, max(leftTResultY, rightTResultY), 0), glm::vec3(0,  1, 0));
	Vertex bottomVertex = Vertex(glm::vec3(0, min(leftTResultY, rightTResultY), 0), glm::vec3(0, -1, 0));
	function<Vertex(int, int)> curveFunc = makeCurveFunction(leftT, rightT, curveFunction, normalFunction);
	closedPolygon = ClosedDipolePolygon(curveFunc, topVertex, bottomVertex, isFlatTop, isFlatBottom);
}

