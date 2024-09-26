#ifndef REVOLUTION_SHAPE_H
#define REVOLUTION_SHAPE_H

#include "Shape.h"
#include "Geometry.h"

#include <glm/glm.hpp>
#include <vector>

class RevolutionSolid : public Shape {
public:
	RevolutionSolid();
	RevolutionSolid(Vertex(*surfaceFunction)(int x, int y), Vertex top, Vertex bottom);
	RevolutionSolid(Vertex(*surfaceFunction)(int x, int y), Vertex top, Vertex bottom,
					bool hasFlatTop, bool hasFlatBottom);
	~RevolutionSolid();

	OBJ_TYPE getType() {
		return SHAPE_MESH;
	};

	void draw();

	void drawNormal();

	void createSurface();

private:
	Vertex topPoint, bottomPoint; 
	std::vector<std::vector<VertexID>> revolutionSurface;
	bool isFlatTop, isFlatBottom;
	Vertex(*parameterizedSurface)(int x, int y); // Function which takes in 2 ints and returns a Vertex
	Surface surface; 

	int tesselationX = -1, tesselationY = -1;
	
	void initialize(Vertex(*surfaceFunction)(int row, int column), 
				   Vertex top, Vertex bottom,
				   bool shouldMakeNewVerticiesConnectToTop, bool shouldMakeNewVerticiesConnectToBottom);

	
};

#endif