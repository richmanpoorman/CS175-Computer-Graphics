#include "RevolutionSolid.h"
#include "Shape.h"
#include <vector>
#include <iostream>
using namespace std;
RevolutionSolid::RevolutionSolid() {}
RevolutionSolid::RevolutionSolid(Vertex(*surfaceFunction)(int x, int y),
								 Vertex top, Vertex bottom) {
	initialize(surfaceFunction, top, bottom, false, false);
}
RevolutionSolid::RevolutionSolid(Vertex(*surfaceFunction)(int x, int y),
								 Vertex top, Vertex bottom,
								 bool hasFlatTop, bool hasFlatBottom) {
	initialize(surfaceFunction, top, bottom, hasFlatTop, hasFlatBottom);
}
RevolutionSolid::~RevolutionSolid() {};

void RevolutionSolid::draw() {
	if (tesselationX != Shape::m_segmentsX or tesselationY != Shape::m_segmentsY)
		createSurface();

	vector<Vertex> verticies = surface.verticies();
	glBegin(GL_TRIANGLES);
	for (Face face : surface.faces()) {
		//cout << "FACE" << endl;
		for (VertexID vertexID : face.verticies()) {
			Vertex vertex = surface.vertex(vertexID);
			glm::vec3 position = vertex.position(), normal = vertex.normal();
			glVertex3f(position.x, position.y, position.z);
			glNormal3f(normal.x, normal.y, normal.z);
			//cout << "Vertex: " << vertexID << " : " << position.x << ", " << position.y << ", " << position.z << endl;
		}

	}
	
	glEnd();
}

void RevolutionSolid::drawNormal() {
	float normalSize = 0.1;
	if (tesselationX != Shape::m_segmentsX or tesselationY != Shape::m_segmentsY)
		createSurface();

	vector<Vertex> verticies = surface.verticies();
	glBegin(GL_LINES);
	for (Vertex vertex : verticies) {
		glm::vec3 position = vertex.position(),
			      normal   = vertex.normal();

		glm::vec3 normalEnd   = position + normalSize * normal;
		glm::vec3 normalStart = position;
		glVertex3f(normalStart.x, normalStart.y, normalStart.z);
		glVertex3f(normalEnd.x, normalEnd.y, normalEnd.z);
	}

	glEnd();
}

void RevolutionSolid::initialize(Vertex(*surfaceFunction)(int row, int column),
								 Vertex top, Vertex bottom,
								 bool hasFlatTop, bool hasFlatBottom) {
	parameterizedSurface   = surfaceFunction;
	topPoint               = top;
	bottomPoint            = bottom; 
	isFlatTop              = hasFlatTop;
	isFlatBottom           = hasFlatBottom;
	
	createSurface();
}

void RevolutionSolid::createSurface() {
	int numRow = Shape::m_segmentsX + 1;
	int numCol = Shape::m_segmentsY + 1; 
	tesselationX = Shape::m_segmentsX; 
	tesselationY = Shape::m_segmentsY;

	surface = Surface();
	revolutionSurface = vector<vector<VertexID>>();

	// Add all of the new verticies
	for (int x = 0; x < numRow; x++) {

		revolutionSurface.push_back(vector<VertexID>());

		for (int y = 0; y < numCol; y++) {
			Vertex   newVertex = parameterizedSurface(x, y);
			VertexID vertexID  = surface.addVertex(newVertex);

			revolutionSurface.back().push_back(vertexID);
		}
	}
	// Add the top and bottom verticies
	VertexID topID    = surface.addVertex(topPoint);
	VertexID bottomID = surface.addVertex(bottomPoint);

	
	// Add the upper left triangle faces (Note that "o" is the fixed point)
	//  o - x
	//  | /
	//  x 
	for (int x = 0; x < numRow - 1; x++) for (int y = 0; y < numCol; y++) {
		VertexID v1 = revolutionSurface[x][y],
				 v2 = revolutionSurface[x + 1][y],
				 v3 = revolutionSurface[x][(y + 1) % numCol];
		//cout << "V1: " << v1 << " V2: " << v2 << " V3: " << v3 << endl;
		surface.makeFace(v1, v2, v3);
	}

	// Add the upper left triangle faces (Note that "o" is the fixed point)
	//      o
	//    / |
	//  x - x
	for (int x = 0; x < numRow - 1; x++) for (int y = 0; y < numCol; y++) {
		VertexID v1 = revolutionSurface[x][y],
				 v2 = revolutionSurface[x + 1][(y + numCol - 1) % numCol],
				 v3 = revolutionSurface[x + 1][y];
		surface.makeFace(v1, v2, v3);
	}
	
	vector<VertexID> topRow = revolutionSurface.back(), // Note that the top is when the rows are at the highest
					 bottomRow = revolutionSurface[0]; // Note that the bottom is when the row is "0" (at the base)
	
	if (isFlatTop) {
		// Create a new point for everything in the top
		vector<VertexID> newTopRow = vector<VertexID>();
		for (int i = 0; i < numCol; i++) {
			//Vertex copy = surface.vertex(topRow[i]),
			//	   toTheRight = surface.vertex(topRow[(i + 1) % numCol]);
			//
			//// Direction is assumed to be perpendicular to both the direction towards the top point and the next vector along the edge
			//glm::vec3 direction = glm::cross((topPoint.position() - copy.position()), 
			//								 (toTheRight.position() - copy.position()));

			Vertex copy = surface.vertex(topRow[i]);
			glm::vec3 direction = glm::vec3(0.0, 1.0, 0.0);
			Vertex   newVertex = Vertex(copy.position(), glm::normalize(direction));
			VertexID vertexID  = surface.addVertex(newVertex);
			newTopRow.push_back(vertexID);
		}
		topRow = newTopRow;
	}

	if (isFlatBottom) {
		// Create a new point for everything in the bottom row
		vector<VertexID> newBottomRow = vector<VertexID>();
		for (int i = 0; i < numCol; i++) {
			//Vertex copy = surface.vertex(bottomRow[i]),
			//	   toTheLeft = surface.vertex(topRow[(i + numCol - 1) % numCol]);

			//// Direction is assumed to be perpendicular to both the direction towards the top point and the next vector along the edge
			//glm::vec3 direction = glm::cross((bottomPoint.position() - copy.position()), 
			//								 (toTheLeft.position() - copy.position()));
			
			Vertex copy = surface.vertex(bottomRow[i]);
			glm::vec3 direction = glm::vec3(0.0, -1.0, 0.0);
			Vertex   newVertex = Vertex(copy.position(), glm::normalize(direction));
			VertexID vertexID = surface.addVertex(newVertex);
			newBottomRow.push_back(vertexID);
		}
		bottomRow = newBottomRow;
	}

	//cout << "TOP " << topRow.size() << " BOT: " << bottomRow.size() << endl;
	for (int i = 0; i < numCol; i++) {
		// Connect to the top point
		VertexID top1 = topRow[i],
				 top2 = topRow[(i + 1) % numCol];
		surface.makeFace(topID, top1, top2);
		

		// Connect to the bottom point
		VertexID bottom1 = bottomRow[i],
				 bottom2 = bottomRow[(i + 1) % numCol];
		surface.makeFace(bottomID, bottom1, bottom2);
	}
}