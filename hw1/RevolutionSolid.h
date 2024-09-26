#ifndef REVOLUTION_SHAPE_H
#define REVOLUTION_SHAPE_H

#include "Shape.h"
#include "Geometry.h"

#include <glm/glm.hpp>
#include <vector>

class RevolutionSolid : public Shape {
public:
	RevolutionSolid(int tesselateX, int tesselateY, Vertex(*surfaceFunction)(int x, int y), Vertex top, Vertex bottom);
	RevolutionSolid(int tesselateX, int tesselateY, Vertex(*surfaceFunction)(int x, int y), Vertex top, Vertex bottom,
					bool shouldMakeNewVerticiesConnectToTop, bool shouldMakeNewVerticiesConnectToBottom);
	~RevolutionSolid() {};

	OBJ_TYPE getType() {
		return SHAPE_SPECIAL1;
	}

	void draw() {
	};

	void drawNormal() {
	};

private:
	glm::vec3 topPoint, bottomPoint; 
	vector<vector<Vertex>> revolutionSurface;
	Vertex(*surfaceFunction)(int x, int y);
};

#endif