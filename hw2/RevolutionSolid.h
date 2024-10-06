#ifndef REVOLUTION_SHAPE_H
#define REVOLUTION_SHAPE_H

#include "Shape.h"
//#include "ClosedDipolePolygon.h"
#include "Geometry.h"

#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <utility> // For pair

class RevolutionSolid {
public:
	RevolutionSolid();
	RevolutionSolid(std::pair<float, float> domainRange, std::function<glm::vec2(float)> surfaceCurve, std::function<glm::vec2(float)> surfaceNormal);
	RevolutionSolid(std::pair<float, float> domainRange, std::function<glm::vec2(float)>  surfaceCurve, std::function<glm::vec2(float)> surfaceNormal, bool hasTopVertex, bool hasBottomVertex);
	~RevolutionSolid();

	OBJ_TYPE getType() {
		return SHAPE_MESH;
	};

	void draw();

	void drawNormal();
private:
	float tolerance = 1e-6;
	bool hasTopVertex, hasBottomVertex;
	std::function<glm::vec2(float)> curveFunction; // Function which takes in 2 ints and returns a Vertex
	std::function<glm::vec2(float)> normalFunction;
	Surface revolutionSurface; 
	std::pair<float, float> domain; 
	int tesselationX, tesselationY;

	void initialize(std::pair<float, float> domain, std::function<glm::vec2(float)> surfaceCurve, std::function<glm::vec2(float)> surfaceNormal,
				    bool hasTopVertex, bool hasBottomVertex);
	

	void updateSurface(); 

};

#endif