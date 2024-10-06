#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <FL/gl.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>

typedef int VertexID;
typedef int FaceID;

class Vertex {
public:
	Vertex(); 
	Vertex(float x, float y, float z);
	Vertex(float x, float y, float z, glm::vec3 normal);
	Vertex(glm::vec3 position, glm::vec3 normal);
	~Vertex();

	glm::vec3 position();
	glm::vec3 normal();
	
	void setPosition(glm::vec3 newPosition);
	void setNormal(glm::vec3 directionVector);
private:
	void initialize(glm::vec3 position, glm::vec3 normal);
	glm::vec3 positionVector; 
	glm::vec3 normalVector;
};

class Face {
public:
	Face(); 
	Face(VertexID vertexIndex1, VertexID vertexIndex2, VertexID vertexIndex3);
	~Face();
	
	std::vector<VertexID> verticies();
	void setVerticies(VertexID newVertexIndicies[3]);
	
private:
	void initialize(VertexID vertexIndex1, VertexID vertexIndex2, VertexID vertexIndex3);
	VertexID vertexIndicies[3] = {};
};

class Surface {
public:
	Surface();
	~Surface();

	std::vector<Vertex> verticies();
	std::vector<Face>   faces();

	void draw();
	void drawNormal();

	Vertex vertex(VertexID vertexID);
	VertexID addVertex(Vertex vertex);

	Face face(FaceID faceID);
	FaceID makeFace(VertexID vertex1ID, VertexID vertex2ID, VertexID vertex3ID);

	bool hasVertex(VertexID vertexID);
	bool hasFace(FaceID faceID);

private:
	std::unordered_map<VertexID, Vertex> vertexMap;
	std::unordered_map<FaceID, Face> faceMap;
	std::vector<FaceID> faceList; 
	std::vector<VertexID> vertexList;
	VertexID nextVertexID;
	FaceID nextFaceID;
};

#endif