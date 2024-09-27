#ifndef CLOSED_DIPOLE_POLYGON_H
#define CLOSED_DIPOLE_POLYGON_H

#include "Shape.h"
#include "Geometry.h"

#include <glm/glm.hpp>
#include <vector>
#include <functional>

class ClosedDipolePolygon {
public:
	ClosedDipolePolygon();
	ClosedDipolePolygon(std::function<Vertex(int, int)> surfaceFunction, Vertex top, Vertex bottom);
	ClosedDipolePolygon(std::function<Vertex(int, int)> surfaceFunction, Vertex top, Vertex bottom,
					bool hasFlatTop, bool hasFlatBottom);
	~ClosedDipolePolygon();

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
	std::function<Vertex(int, int)> parameterizedSurface;  // Function which takes in 2 ints and returns a Vertex
	Surface surface; 

	int tesselationX = -1, tesselationY = -1;
	
	void initialize(std::function<Vertex(int, int)> surfaceFunction,
				   Vertex top, Vertex bottom,
				   bool shouldMakeNewVerticiesConnectToTop, bool shouldMakeNewVerticiesConnectToBottom);

	
};

#endif