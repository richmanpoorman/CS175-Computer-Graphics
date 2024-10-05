#ifndef REVOLUTION_SHAPE_H
#define REVOLUTION_SHAPE_H

#include "Shape.h"
#include "ClosedDipolePolygon.h"
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <utility> // For pair

class RevolutionSolid {
public:
	RevolutionSolid();
	RevolutionSolid(std::pair<float, float> domain, std::function<glm::vec2(float)> surfaceCurve, std::function<glm::vec2(float)> surfaceNormal);
	RevolutionSolid(std::pair<float, float> domain, std::function<glm::vec2(float)>  surfaceCurve, std::function<glm::vec2(float)> surfaceNormal, bool hasFlatTop, bool hasFlatBottom);
	~RevolutionSolid();

	OBJ_TYPE getType() {
		return SHAPE_MESH;
	};

	void draw();

	void drawNormal();
private:
	
	bool isFlatTop, isFlatBottom;
	std::function<glm::vec2(float)> curveFunction; // Function which takes in 2 ints and returns a Vertex
	std::function<glm::vec2(float)> normalFunction;
	ClosedDipolePolygon closedPolygon; 
	float leftT, rightT; 

	void initialize(std::pair<float, float> domain, std::function<glm::vec2(float)> surfaceCurve, std::function<glm::vec2(float)> surfaceNormal,
				    bool hasFlatTop, bool hasFlatBottom);
	Vertex drawCurve(int thetaStep, int yStep);

};

#endif