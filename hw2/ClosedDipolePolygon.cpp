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

/*
 * Purpose    : Draws the shape using FLTK to draw the shape 3-dimensionally
 * Parameters : (None)
 * Return     : (None)
 */
void ClosedDipolePolygon::draw() {
	// Update if tesselation has changed 
	if (tesselationX != Shape::m_segmentsX or tesselationY != Shape::m_segmentsY)
		createSurface();

	// Draw the shape's surface
	surface.draw();
}

/*
 * Purpose    : Draws lines representing the normals on the surface of the shape 
 * Parameters : (None)
 * Return     : (None)
 */
void ClosedDipolePolygon::drawNormal() {
	// Update the surface if the tessleation has changed
	float normalSize = 0.1;
	if (tesselationX != Shape::m_segmentsX or tesselationY != Shape::m_segmentsY)
		createSurface();

	// Draw the normals
	surface.drawNormal();
}

/*
 * Purpose    : Uses a function to define the surface, as well as a definition of the top and bottom of the shape (the two dipoles)
 * Parameters : (int * int -> Vertex) surfaceFunction := A function which maps x, y to a point in space (including normals) 
 *				(Vertex)              top             := Top point, representing the top dipole
 *			    (Vertex)              bottom          := Bottom point, representing the bottom dipole 
 *				(bool)                hasFlatTop      := Whether the top of the surface is flat 
 *				(bool)                hasFlatBottom   := Whether the bottom of the surface is flat
 * Return     : (None)
 */
void ClosedDipolePolygon::initialize(std::function<Vertex(int, int)> surfaceFunction,
								 Vertex top, Vertex bottom,
								 bool hasFlatTop, bool hasFlatBottom) {
	// Initializes all the values, then creates the surface
	parameterizedSurface   = surfaceFunction;
	topPoint               = top;
	bottomPoint            = bottom; 
	isFlatTop              = hasFlatTop;
	isFlatBottom           = hasFlatBottom;
	
	createSurface();
}

/*
 * Purpose    : Creates the verticies to our double array (representing the x, y samples), using the function to put the points from (x, y) -> (x, y, z)
 * Parameters : (Surface)                  surface       := The container to add the new points to
 *				(vector<vector<VertexID>>) verticies     := The 2D array of verticies to help keep track of; used later to connect and create the faces
 *				(bool)                     hasFlatTop    := Whether the top of the shape is flat or not; used to keep the tesselation count consistant
 *				(bool)                     hasFlatBottom := Whether the bottom of the shape is flat or not; used to keep the tesselation count consistant
 * Return     : (None)
 */
void createVerticies(Surface &surface, vector<vector<VertexID>> &verticies, function<Vertex(int, int)> vertexFunction, bool hasFlatTop, bool hasFlatBottom) {
	int numRow = Shape::m_segmentsX + (hasFlatTop ? 1 : 0);
	int numCol = Shape::m_segmentsY; // Note that we exclude when column = Shape::m_segments because that last face connects to the first
	int rowStart = (hasFlatBottom ? 0 : 1); 
	int colStart = 0;
	// Add all of the new verticies
	for (int x = rowStart; x < numRow; x++) {
		// Creates a new row of verticies in the array
		verticies.push_back(vector<VertexID>());

		// Adds a vertex for each (x, y) we want to sample in the range
		for (int y = colStart; y < numCol; y++) {
			Vertex   newVertex = vertexFunction(x, y);
			VertexID vertexID  = surface.addVertex(newVertex);

			verticies.back().push_back(vertexID);
		}
	}
	
}


/*
 * Purpose    : Adds the triangle faces that don't connect to the dipoles; wraps around to get a closed shape
 * Parameters : (Surface)                  surface   := The surface to add the faces to 
 *				(vector<vector<VertexID>>) verticies := The verticies to use to make the faces 
 * Return     : (None)
 */
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

/*
* Purpose    : Adds the faces between the top/bottom of the drawn faces and the top/bottom vertex
* Parameters : (Surface)                  surface      := The surface to add the faces to 
*			   (vector<vector<VertexID>>) verticies    := The verticies that are already added
*			   (VertexID)                 topVertex    := The top vertex to connect to 
*			   (VertexID)                 bottomVertex := The bottom vertex to connect to 
*			   (bool)                     isTopFlat    := If the top is flat 
*			   (bool)                     isBottomFlat := If the bottom is flat 
* Return     : (None)
*/
void connectTopAndBottom(Surface& surface, vector<vector<VertexID>>& verticies, VertexID topVertex, VertexID bottomVertex, bool isTopFlat, bool isBottomFlat) {

	vector<VertexID> topRow = verticies.back(), // Note that the top is when the rows are at the highest
		bottomRow = verticies[0]; // Note that the bottom is when the row is "0" (at the base)

	assert(topRow.size() == bottomRow.size());
	int numCol = topRow.size();
	
	// If the top is flat, make a new array of points for the top faces
	if (isTopFlat) {
		// Create a new point for everything in the top
		vector<VertexID> newTopRow = vector<VertexID>();
		for (VertexID vertexID : topRow) {
			Vertex copy = surface.vertex(vertexID);
			glm::vec3 direction = glm::vec3(0.0, 1.0, 0.0);
			Vertex   newVertex = Vertex(copy.position(), glm::normalize(direction));
			VertexID vertID = surface.addVertex(newVertex);
			newTopRow.push_back(vertID);
		}
		topRow = newTopRow;
	}

	// If the bottom is flat, make a new array of points for the bottom faces
	if (isBottomFlat) {
		// Create a new point for everything in the bottom row
		vector<VertexID> newBottomRow = vector<VertexID>();
		for (VertexID vertexID : bottomRow) {

			Vertex copy = surface.vertex(vertexID);
			glm::vec3 direction = glm::vec3(0.0, -1.0, 0.0);
			Vertex   newVertex = Vertex(copy.position(), glm::normalize(direction));
			VertexID vertID = surface.addVertex(newVertex);
			newBottomRow.push_back(vertID);
		}
		bottomRow = newBottomRow;
	}
	assert(topRow.size() == bottomRow.size());
	// Connect to the top point
	for (int i = 0, n = topRow.size(); i < n; i++) {
		
		VertexID top2 = topRow[i],
				 top1 = topRow[(i + 1) % n];
		surface.makeFace(topVertex, top1, top2);
	}
	// Connect to the bottom point
	for (int i = 0, n = bottomRow.size(); i < n; i++) {
		VertexID bottom1 = bottomRow[i],
			     bottom2 = bottomRow[(i + 1) % n];
		surface.makeFace(bottom1, bottom2, bottomVertex);
	}
}

/*
 * Purpose    : Does all of the steps to calculate the verticies and faces of the shape
 * Parameters : (None)
 * Return     : (None)
 */
void ClosedDipolePolygon::createSurface() {
	
	tesselationX = Shape::m_segmentsX; 
	tesselationY = Shape::m_segmentsY;

	surface = Surface();
	revolutionSurface = vector<vector<VertexID>>();

	// Creates all verticies between the top and bottom
	createVerticies(surface, revolutionSurface, parameterizedSurface, isFlatTop, isFlatBottom);

	// Create the top and bottom
	VertexID topID    = surface.addVertex(topPoint);
	VertexID bottomID = surface.addVertex(bottomPoint);

	// Create the faces of side
	setTriangleFaces(surface, revolutionSurface);
	
	// Create top and bottom faces
	connectTopAndBottom(surface, revolutionSurface, topID, bottomID, isFlatTop, isFlatBottom);
	
}

