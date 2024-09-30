#include "ClosedDipolePolygon.h"
#include "Shape.h"
#include <vector>
#include <iostream>
using namespace std;
ClosedDipolePolygon::ClosedDipolePolygon() {}
ClosedDipolePolygon::ClosedDipolePolygon(std::function<Vertex(int, int)> surfaceFunction,
								 Vertex top, Vertex bottom) {
	initialize(surfaceFunction, top, bottom, false, false);
}
ClosedDipolePolygon::ClosedDipolePolygon(std::function<Vertex(int, int)> surfaceFunction,
								 Vertex top, Vertex bottom,
								 bool hasFlatTop, bool hasFlatBottom) {
	initialize(surfaceFunction, top, bottom, hasFlatTop, hasFlatBottom);
}
ClosedDipolePolygon::~ClosedDipolePolygon() {};

void ClosedDipolePolygon::draw() {
	if (tesselationX != Shape::m_segmentsX or tesselationY != Shape::m_segmentsY)
		createSurface();

	surface.draw();
}

void ClosedDipolePolygon::drawNormal() {
	float normalSize = 0.1;
	if (tesselationX != Shape::m_segmentsX or tesselationY != Shape::m_segmentsY)
		createSurface();

	surface.drawNormal();
}

void ClosedDipolePolygon::initialize(std::function<Vertex(int, int)> surfaceFunction,
								 Vertex top, Vertex bottom,
								 bool hasFlatTop, bool hasFlatBottom) {
	parameterizedSurface   = surfaceFunction;
	topPoint               = top;
	bottomPoint            = bottom; 
	isFlatTop              = hasFlatTop;
	isFlatBottom           = hasFlatBottom;
	
	createSurface();
}


void createVerticies(Surface &surface, vector<vector<VertexID>> &verticies, function<Vertex(int, int)> vertexFunction, bool hasFlatTop, bool hasFlatBottom) {
	int numRow = Shape::m_segmentsX + (hasFlatTop ? 1 : 0);
	int numCol = Shape::m_segmentsY; // Note that we exclude when column = Shape::m_segments because that last face connects to the first
	int rowStart = (hasFlatBottom ? 0 : 1); 
	int colStart = 0;
	// Add all of the new verticies
	for (int x = rowStart; x < numRow; x++) {

		verticies.push_back(vector<VertexID>());

		for (int y = colStart; y < numCol; y++) {
			Vertex   newVertex = vertexFunction(x, y);
			VertexID vertexID  = surface.addVertex(newVertex);

			verticies.back().push_back(vertexID);
		}
	}
	// Add the top and bottom verticies
	
}




void setTriangleFaces(Surface& surface, vector<vector<VertexID>>& verticies) {
	assert(not verticies.empty());
	int numRow = verticies.size(), numCol = verticies[0].size();
	// Add the upper left triangle faces (Note that "o" is the fixed point)
	//  v1 - v3
	//  |  /  | 
	//  v2 - v4
	for (int x = 0; x < numRow - 1; x++) for (int y = 0; y < numCol; y++) {
		int nextX = (x + 1) % numRow, nextY = (y + 1) % numCol;
		VertexID v1 = verticies[x][y],
			v2 = verticies[nextX][y],
			v3 = verticies[x][nextY], 
			v4 = verticies[nextX][nextY];
		//cout << "V1: " << v1 << " V2: " << v2 << " V3: " << v3 << endl;
		surface.makeFace(v3, v1, v2);
		surface.makeFace(v4, v3, v2);
	}
	
}
void connectTopAndBottom(Surface& surface, vector<vector<VertexID>>& verticies, VertexID topVertex, VertexID bottomVertex, bool isTopFlat, bool isBottomFlat) {

	vector<VertexID> topRow = verticies.back(), // Note that the top is when the rows are at the highest
		bottomRow = verticies[0]; // Note that the bottom is when the row is "0" (at the base)

	assert(topRow.size() == bottomRow.size());
	int numCol = topRow.size();
	//cout << "NUM COL: " << numCol << endl;
	if (isTopFlat) {
		// Create a new point for everything in the top
		vector<VertexID> newTopRow = vector<VertexID>();
		for (VertexID vertexID : topRow) {
			Vertex copy = surface.vertex(vertexID);
			glm::vec3 direction = glm::vec3(0.0, 1.0, 0.0);
			Vertex   newVertex = Vertex(copy.position(), glm::normalize(direction));
			VertexID vertexID = surface.addVertex(newVertex);
			newTopRow.push_back(vertexID);
		}
		topRow = newTopRow;
	}

	if (isBottomFlat) {
		// Create a new point for everything in the bottom row
		vector<VertexID> newBottomRow = vector<VertexID>();
		for (VertexID vertexID : bottomRow) {

			Vertex copy = surface.vertex(vertexID);
			glm::vec3 direction = glm::vec3(0.0, -1.0, 0.0);
			Vertex   newVertex = Vertex(copy.position(), glm::normalize(direction));
			VertexID vertexID = surface.addVertex(newVertex);
			newBottomRow.push_back(vertexID);
		}
		bottomRow = newBottomRow;
	}
	assert(topRow.size() == bottomRow.size());
	//cout << "TOP " << topRow.size() << " BOT: " << bottomRow.size() << endl;
	for (int i = 0, n = topRow.size(); i < n; i++) {
		// Connect to the top point
		VertexID top2 = topRow[i],
			top1 = topRow[(i + 1) % n];
		surface.makeFace(topVertex, top1, top2);

		//glm::vec3 top1Norm = surface.vertex(top1).position();
		//glm::vec3 top2Norm = surface.vertex(top2).position();
		//cout << "TOP 1: (" << top1Norm.x << ", " << top1Norm.y << ", " << top1Norm.z << ") TOP 2: (" << top2Norm.x << ", " << top2Norm.y << ", " << top2Norm.z << ")" << endl;
	}
	// Connect to the bottom point
	for (int i = 0, n = bottomRow.size(); i < n; i++) {
		VertexID bottom1 = bottomRow[i],
			     bottom2 = bottomRow[(i + 1) % n];
		surface.makeFace(bottom1, bottom2, bottomVertex);
	}
}

void ClosedDipolePolygon::createSurface() {
	
	tesselationX = Shape::m_segmentsX; 
	tesselationY = Shape::m_segmentsY;

	surface = Surface();
	revolutionSurface = vector<vector<VertexID>>();


	createVerticies(surface, revolutionSurface, parameterizedSurface, isFlatTop, isFlatBottom);

	VertexID topID    = surface.addVertex(topPoint);
	VertexID bottomID = surface.addVertex(bottomPoint);

	setTriangleFaces(surface, revolutionSurface);
	
	connectTopAndBottom(surface, revolutionSurface, topID, bottomID, isFlatTop, isFlatBottom);
	
}

