#include "Geometry.h"

#include <vector>
#include <unordered_set>

#define normalSize 0.1f

using namespace std; 



/*
 *  Vertex
 */

// Initializes with (0, 0, 0) with the zero normal
Vertex::Vertex() { initialize(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)); }
// Initializes with the point with the zero normal
Vertex::Vertex(float x, float y, float z) { initialize(glm::vec3(x, y, z), glm::vec3(0, 0, 0)); }
// Initializes with the point with the given vector as a direction
Vertex::Vertex(float x, float y, float z, glm::vec3 normal) { initialize(glm::vec3(x, y, z), normal); }
// Initializes with the given position and direction vector 
Vertex::Vertex(glm::vec3 position, glm::vec3 normal) { initialize(position, normal); }
Vertex::~Vertex() {}
void Vertex::initialize(glm::vec3 position, glm::vec3 normal) {
	positionVector = position;
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

void Face::setVerticies(VertexID newVertexIndicies[3]) {
	for (int i = 0; i < 3; i++)
		vertexIndicies[i] = newVertexIndicies[i];
}


/*
 *  Surface
 */
Surface::Surface() { nextFaceID = nextVertexID = 1;  }
Surface::~Surface() {}

Vertex Surface::vertex(VertexID vertexID) {
	assert(hasVertex(vertexID));
	return vertexMap[vertexID];
}
VertexID Surface::addVertex(Vertex vertex) {
	VertexID id = nextVertexID++; 

	vertexMap[id] = vertex;
	vertexList.push_back(id);
	return id; 
}


Face Surface::face(FaceID faceID) {
	assert(hasFace(faceID));
	return faceMap[faceID];
}
FaceID Surface::makeFace(VertexID vertex1ID, VertexID vertex2ID, VertexID vertex3ID) {
	assert(hasVertex(vertex1ID) and hasVertex(vertex2ID) and hasVertex(vertex3ID)); // Makes sure that the verticies are in the dictionary 
	FaceID faceID = nextFaceID++;

	Face face = Face(vertex1ID, vertex2ID, vertex3ID);

	faceList.push_back(faceID);

	faceMap[faceID] = face;
	faceList.push_back(faceID);
	
	return faceID;
}

bool Surface::hasVertex(VertexID vertexID) { return vertexMap.count(vertexID); }
bool Surface::hasFace(FaceID faceID) { return faceMap.count(faceID); }

vector<Vertex> Surface::verticies() { 

	vector<Vertex> fullVertexList = vector<Vertex>();
	for (VertexID vertexID : vertexList) {
		Vertex currentVertex = vertex(vertexID); 
		fullVertexList.push_back(currentVertex);
	}

	return fullVertexList;
}
vector<Face> Surface::faces() { 
	
	vector<Face> fullFaceList = vector<Face>();
	for (FaceID faceID : faceList) {
		Face currentFace = face(faceID);
		fullFaceList.push_back(currentFace);
	}
	return fullFaceList;
}

void Surface::draw() {
	//vector<Vertex> vertexList = verticies();
	glBegin(GL_TRIANGLES);
	for (Face face : faces()) {
		//cout << "FACE" << endl;
		for (VertexID vertexID : face.verticies()) {
			Vertex currentVertex = vertex(vertexID);
			glm::vec3 position = currentVertex.position(), normal = currentVertex.normal();
			glVertex3f(position.x, position.y, position.z);
			glNormal3f(normal.x, normal.y, normal.z);
			//cout << "Vertex: " << vertexID << " : " << position.x << ", " << position.y << ", " << position.z << endl;
		}

	}

	glEnd();
}

void Surface::drawNormal() {
	
	vector<Vertex> vertexList = verticies();
	glBegin(GL_LINES);
	for (Vertex vertex : vertexList) {
		glm::vec3 position = vertex.position(),
			normal = vertex.normal();

		glm::vec3 normalEnd = position + normalSize * normal;
		glm::vec3 normalStart = position;
		glVertex3f(normalStart.x, normalStart.y, normalStart.z);
		glVertex3f(normalEnd.x, normalEnd.y, normalEnd.z);
	}

	glEnd();
}

#undef normalSize