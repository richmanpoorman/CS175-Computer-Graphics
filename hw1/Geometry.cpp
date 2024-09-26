#include "Geometry.h"

#include <vector>
#include <unordered_set>

using namespace std; 



/*
 *  Vertex
 */

// Initializes with (0, 0, 0) with the zero normal
Vertex::Vertex() { initialize(0, 0, 0, glm::vec3(0, 0, 0)); }
// Initializes with the point with the zero normal
Vertex::Vertex(float x, float y, float z) { initialize(x, y, z, glm::vec3(0, 0, 0)); }
// Initializes with the point with the given vector as a direction
Vertex::Vertex(float x, float y, float z, glm::vec3 normal) { initialize(x, y, z, normal); }
Vertex::~Vertex() {}
void Vertex::initialize(float x, float y, float z, glm::vec3 normal) {
	positionVector = glm::vec3(x, y, z);
	normalVector   = glm::normalize(normal);
}

glm::vec3 Vertex::position() { return positionVector; }
glm::vec3 Vertex::normal() { return normalVector; }

void Vertex::setPosition(glm::vec3 newPosition) { positionVector = positionVector; }
void Vertex::setNormal(glm::vec3 directionVector) { normalVector = glm::normalize(directionVector); }


/*
 *
 *     MOST IMPORTANT INFORMATION IS ABOVE; THE BELOW ARE EXTRA STRUCTURES TO KEEP TRACK OF EVERYTHING
 * 
 */

 /*
  *  Face
  */
// Sets the verticies to (-1, -1, -1)
Face::Face() { initialize(-1, -1, -1); }
// Sets the verticies to the given point
Face::Face(VertexID vertexIndex1, VertexID vertexIndex2, VertexID vertexIndex3) { initialize(vertexIndex1, vertexIndex2, vertexIndex3); }
Face::~Face() {}
void Face::initialize(VertexID vertexIndex1, VertexID vertexIndex2, VertexID vertexIndex3) {
	vertexIndicies[0] = vertexIndex1;
	vertexIndicies[1] = vertexIndex2;
	vertexIndicies[2] = vertexIndex3;
}

vector<int> Face::verticies() { return { vertexIndicies[0], vertexIndicies[1], vertexIndicies[2] }; }

int Face::setVerticies(int newVertexIndicies[3]) {
	for (int i = 0; i < 3; i++)
		vertexIndicies[i] = newVertexIndicies[i];
}


/*
 *  Surface
 */
Surface::Surface() { nextFaceID = nextVertexID = 1;  }
Surface::~Surface() {}

VertexID Surface::addVertex(Vertex vertex) {
	VertexID id = nextVertexID++; 

	vertexMap[id] = vertex;
	vertexInFace[id] = unordered_set<FaceID>();

	return id; 
}
Vertex Surface::removeVertex(VertexID vertexID) {
	assert(hasVertex(vertexID)); // Makes sure that the key is actually in the dictionary

	Vertex vertex = vertexMap[vertexID];

	for (FaceID faceID : vertexInFace[vertexID])
		removeFace(faceID);

	vertexInFace.erase(vertexID);
	vertexMap.erase(vertexID);

	return vertex;
}

FaceID Surface::makeFace(VertexID vertex1ID, VertexID vertex2ID, VertexID vertex3ID) {
	assert(hasVertex(vertex1ID) and hasVertex(vertex2ID) and hasVertex(vertex3ID)); // Makes sure that the verticies are in the dictionary 
	FaceID faceID = nextFaceID++;

	Face face = Face(vertex1ID, vertex2ID, vertex3ID);

	vertexInFace[vertex1ID].insert(faceID);
	vertexInFace[vertex2ID].insert(faceID);
	vertexInFace[vertex3ID].insert(faceID);

	faceMap[faceID] = face;
	
	return faceID;
}
Face Surface::removeFace(FaceID faceID) {
	assert(hasFace(faceID));

	Face face = faceMap[faceID];

	faceMap.erase(faceID);

	for (VertexID vertexID : face.verticies())
		vertexInFace.erase(faceID);

	return face;
}

bool Surface::hasVertex(VertexID vertexID) { return vertexMap.count(vertexID); }
bool Surface::hasFace(FaceID faceID) { return faceMap.count(faceID); }