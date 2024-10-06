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
RevolutionSolid::RevolutionSolid() {
	initialize({ 0, 0 }, defaultFunction, defaultFunction, false, false);
}
RevolutionSolid::RevolutionSolid(pair<float, float> domain, function<glm::vec2(float)> surfaceCurve, function<glm::vec2(float)> surfaceNormal) {
	// Tries to detect whether the surface has flat top or point top
	glm::vec2 startPoint = surfaceCurve(domain.second),
			  endPoint   = surfaceCurve(domain.first);
	float topX    = startPoint.x,
		  bottomX = endPoint.x;
		
	// Auto checks if there should or should not be a flat top and bottom
	initialize(domain, surfaceCurve, surfaceNormal, abs(topX) < tolerance, abs(bottomX) < tolerance);
}
RevolutionSolid::RevolutionSolid(pair<float, float> domain, function<glm::vec2(float)> surfaceCurve, function<glm::vec2(float)> surfaceNormal, bool hasTopVertex, bool hasBottomVertex) {
	initialize(domain, surfaceCurve, surfaceNormal, hasTopVertex, hasBottomVertex);
}
RevolutionSolid::~RevolutionSolid() {}

void RevolutionSolid::initialize(pair<float, float> domainRange, function<glm::vec2(float)> surfaceCurve, function<glm::vec2(float)> surfaceNormal, bool curveHasTop, bool curveHasBottom) {
	domain          = domainRange;
	curveFunction   = surfaceCurve;
	normalFunction  = surfaceNormal; 
	hasTopVertex    = curveHasTop; 
	hasBottomVertex = curveHasBottom;

	updateSurface();
}

void RevolutionSolid::draw() {
	if (Shape::m_segmentsX != tesselationX or Shape::m_segmentsY != tesselationY)
		updateSurface(); 

	revolutionSurface.draw();
}

void RevolutionSolid::drawNormal() {
	if (Shape::m_segmentsX != tesselationX or Shape::m_segmentsY != tesselationY)
		updateSurface();

	revolutionSurface.drawNormal();
}


glm::vec3 vectorTolerance(glm::vec3 vector, float tolerance) {
	bool isZeroX = abs(vector.x) < tolerance,
		 isZeroY = abs(vector.y) < tolerance,
		 isZeroZ = abs(vector.z) < tolerance; 

	if (not isZeroX and not isZeroY and not isZeroZ)
		return vector; 

	float x = isZeroX ? 0 : vector.x,
		  y = isZeroY ? 0 : vector.y,
		  z = isZeroZ ? 0 : vector.z; 

	return glm::vec3(x, y, z);
}


glm::vec3 directionTolerance(glm::vec3 vector, float tolerance) {
	return glm::normalize(vectorTolerance(vector, tolerance));
}

vector<pair<glm::vec4, glm::vec4>> curvePositionsAndNormals(pair<float, float> domain, int numSteps, function<glm::vec2(float)> curvePosition, function<glm::vec2(float)> curveNormal) {
	float start = domain.first, end = domain.second; 
	vector<pair<glm::vec4, glm::vec4>> positionsAndNormals = vector<pair<glm::vec4, glm::vec4>>();
	for (int step = 0; step <= numSteps; step++) {
		float portionOfTheWay = 1.0 * step / numSteps;
		float t = start * portionOfTheWay - end * (1 - portionOfTheWay);
		
		glm::vec2 position = curvePosition(t),
				  normal   = curveNormal(t);
		
		// Puts the 2D space to 3D space (4D vector to use matrix transformations in the future
		glm::vec4 position3D = glm::vec4(position.x, position.y, 0, 1); 
		glm::vec4 normal3D   = glm::vec4(normal.x, normal.y, 0, 0);
		
		positionsAndNormals.push_back({ position3D, normal3D }); 
	}
	return positionsAndNormals; 
}

vector<vector<VertexID>> surfacePoints(Surface &surface, int numSteps, float tolerance, vector<pair<glm::vec4, glm::vec4>> &xyPlanePositionsAndNormals) {

	vector<glm::mat4> rotationMatricies = vector<glm::mat4>();
	for (int step = 0; step <= numSteps; step++) {
		float radianAngle = 2 * PI * step / numSteps;
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -radianAngle, glm::vec3(0, 1, 0));
		rotationMatricies.push_back(rotationMatrix);
	}

	vector<vector<VertexID>> verticies = vector<vector<VertexID>>();
	int numLayers = xyPlanePositionsAndNormals.size(); 
	for (pair<glm::vec4, glm::vec4> &positionAndNormal : xyPlanePositionsAndNormals) {
		glm::vec4 position = positionAndNormal.first,
			      normal = positionAndNormal.second; 
		verticies.push_back(vector<VertexID>());
		for (glm::mat4 &rotation : rotationMatricies) {
			// Does the rotation and casts back down to vec3 from vec4
			glm::vec3 rotatedPosition = rotation * position,
					  rotatedNormal   = rotation * normal;
			
			Vertex   vertex = Vertex(vectorTolerance(rotatedPosition, tolerance), directionTolerance(rotatedNormal, tolerance));
			VertexID vertexID = surface.addVertex(vertex);

			verticies.back().push_back(vertexID);
		}
	}
	return verticies;
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

void connectToSingleVertex(Surface& surface, vector<VertexID>& verticies, Vertex vertex) {
	for (int index = 0, size = verticies.size(); index < size; index++) {
		VertexID v1 = surface.addVertex(vertex);
		VertexID v3 = verticies[index],
			     v2 = verticies[(index + 1) % size];
		surface.makeFace(v1, v2, v3);
	}
}

void drawTopAndBottom(pair<float, float> domain, bool hasTopVertex, bool hasBottomVertex, Surface& surface, vector<vector<VertexID>> &verticies, function<glm::vec2(float)> curveFunction) {
	if (not hasTopVertex and not hasBottomVertex) return; 
	float yStart = curveFunction(domain.first).y,
		  yEnd   = curveFunction(domain.second).y;

	if (not hasTopVertex) { // Flat top
		glm::vec3 normalDirection = glm::vec3(0, yStart > yEnd ? 1 : -1, 0);
		Vertex vertex = Vertex(glm::vec3(0, yStart, 0), normalDirection);
		vector<VertexID> newVerticies = vector<VertexID>();
		for (VertexID vertexID : verticies[0]) {
			Vertex   newVertex   = Vertex(surface.vertex(vertexID).position(), normalDirection);
			VertexID newVertexID = surface.addVertex(newVertex);
			newVerticies.push_back(newVertexID);
		}
		connectToSingleVertex(surface, newVerticies, vertex);
	}

	if (not hasBottomVertex) { // Flat top
		glm::vec3 normalDirection = glm::vec3(0, yStart < yEnd ? 1 : -1, 0);
		Vertex vertex = Vertex(glm::vec3(0, yEnd, 0), normalDirection);
		vector<VertexID> newVerticies = vector<VertexID>();
		for (VertexID vertexID : verticies.back()) {
			Vertex   newVertex = Vertex(surface.vertex(vertexID).position(), normalDirection);
			VertexID newVertexID = surface.addVertex(newVertex);
			newVerticies.push_back(newVertexID);
		}
		reverse(newVerticies.begin(), newVerticies.end());
		connectToSingleVertex(surface, newVerticies, vertex);
	}
}

void RevolutionSolid::updateSurface() {
	tesselationX = Shape::m_segmentsX, tesselationY = Shape::m_segmentsY;
	revolutionSurface = Surface();

	vector<pair<glm::vec4, glm::vec4>> positionsAndNormals = curvePositionsAndNormals(domain, tesselationX, curveFunction, normalFunction);
	vector<vector<VertexID>> verticies = surfacePoints(revolutionSurface, tesselationY, tolerance, positionsAndNormals);
	setTriangleFaces(revolutionSurface, verticies);

	drawTopAndBottom(domain, hasTopVertex, hasBottomVertex, revolutionSurface, verticies, curveFunction);
}


//#include "RevolutionSolid.h"
//#include "Shape.h"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <vector>
//#include <iostream>
//#include <utility> // For pair
//#include <functional>
//
//using namespace std;
//
//glm::vec2 defaultFunction(float t) { return glm::vec2(0, 0); }
//
//RevolutionSolid::RevolutionSolid() { initialize({ 0, 0 }, defaultFunction, defaultFunction, false, false); }
//RevolutionSolid::RevolutionSolid(pair<float, float> domain, function<glm::vec2(float)> surfaceCurve, function<glm::vec2(float)> surfaceNormal) {
//	// Tries to detect whether the surface has flat top or point top
//	float tolerance = 1e-6;
//	glm::vec2 startPoint = surfaceCurve(domain.second),
//			  endPoint   = surfaceCurve(domain.first);
//	float topX    = startPoint.x,
//		  bottomX = endPoint.x;
//	
//	// Auto checks if there should or should not be a flat top and bottom
//	initialize(domain, surfaceCurve, surfaceNormal, abs(topX) > tolerance, abs(bottomX) > tolerance);
//}
//RevolutionSolid::RevolutionSolid(pair<float, float> domain, function<glm::vec2(float)> surfaceCurve, function<glm::vec2(float)>  surfaceNormal, bool hasFlatTop, bool hasFlatBottom) { initialize(domain, surfaceCurve, surfaceNormal, hasFlatTop, hasFlatBottom); }
//RevolutionSolid::~RevolutionSolid() {}
//
//void RevolutionSolid::draw() { closedPolygon.draw(); }
//void RevolutionSolid::drawNormal() { closedPolygon.drawNormal(); }
//
///*
// * Purpose    : Turns the curve function into a function which maps (x, y) -> (x, y, z) 
// * Parameters : (float)                 leftT          := The left of the parametric range 
// *				(float)                 rightT         := The right of the parametric range
// *				(function<vec2(float)>) curveFunction  := The function describing the curve to revolution around the y-axis
// *				(function<vec2(float)>) normalFunction := The function describing the normals of the curve
// * Return     : (None)
// */
//function<Vertex(int, int)> makeCurveFunction(float leftT, float rightT, function<glm::vec2(float)> curveFunction, function<glm::vec2(float)> normalFunction) {
//	
//	function<Vertex(int, int)> curveFunc = [=](int yStep, int thetaStep) {
//		int totalThetaSteps = Shape::m_segmentsY,
//			totalYSteps = Shape::m_segmentsX;
//
//		// Calcuate angle and y based off of step
//		float portionOfTheWay = (1.0 * yStep / totalYSteps);
//		float t = leftT * (1.0 - portionOfTheWay) + portionOfTheWay * rightT; // Parameterize linear transition between two points
//		float angle = 2.0 * PI * thetaStep / totalThetaSteps;
//
//		// Find 4x4 matrix representing a rotation by the given angle
//		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(0, 1, 0));
//
//		// Get the 2D points and normal of the curve that will be rotated around the y axis'
//
//		glm::vec2 onCurve = curveFunction(t);
//		glm::vec2 normalOnCurve = normalFunction(t);
//
//		// Compute the vertex arrow after rotating
//		glm::vec4 positionVector = rotationMatrix * glm::vec4(onCurve.x, onCurve.y, 0, 1);
//		glm::vec4 normalVector = rotationMatrix * glm::vec4(normalOnCurve.x, normalOnCurve.y, 0, 0);
//
//		// Turn it into a vertex
//		Vertex vertex = Vertex(glm::vec3(positionVector), glm::vec3(normalVector));
//		return vertex;
//	};
//	return curveFunc;
//}
//
///*
// * Purpose    : Initializes all of the values
// * Parameters : (pair<float, float>)    domain         := The range of the parametric function 
// *				(function<vec2(float)>) curveFunction  := The function describing the curve to revolution around the y-axis
// *				(function<vec2(float)>) normalFunction := The function describing the normals of the curve
// *				(bool)                  hasFlatTop     := If the top is flat or if it is a point 
// *				(bool)                  hasFlatBottom  := If the bottom is flat or if it is a point
// * Return     : (None)
// */
//void RevolutionSolid::initialize(pair<float, float> domain, function<glm::vec2(float)> surfaceCurve, function<glm::vec2(float)> surfaceNormal,
//								 bool hasFlatTop, bool hasFlatBottom) {
//	// Sets the values for the revolution solid
//	leftT = domain.first, rightT = domain.second; 
//	curveFunction = surfaceCurve;
//	normalFunction = surfaceNormal; 
//	isFlatTop = hasFlatTop, isFlatBottom = hasFlatBottom;
//
//	// Call the closed and calculate everything for the closed dipole
//	float leftTResultY  = curveFunction(leftT).y, 
//		  rightTResultY = curveFunction(rightT).y;
//	Vertex topVertex    = Vertex(glm::vec3(0, rightTResultY, 0), glm::vec3(0,  1, 0));
//	Vertex bottomVertex = Vertex(glm::vec3(0, leftTResultY, 0), glm::vec3(0, -1, 0));
//	function<Vertex(int, int)> curveFunc = makeCurveFunction(leftT, rightT, curveFunction, normalFunction);
//	closedPolygon = ClosedDipolePolygon(curveFunc, topVertex, bottomVertex, hasFlatTop, hasFlatBottom);
//}
//
