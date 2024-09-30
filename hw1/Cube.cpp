#include "Cube.h"
#include "Shape.h"
#include <iostream>

#define FRONT 0 
#define BACK 1
#define TOP 2
#define BOTTOM 3
#define RIGHT 4
#define LEFT 5

using namespace std;


Cube::Cube() {
    tessellate();
}

Cube::~Cube() {}

/* tessellate 
* 
* generates the vertices for a tesselated 3D 
* cube with the provided number of subdivisions 
* (segments) along each axis
*
*/
void Cube::tessellate() {

        int segmentsX = Shape::m_segmentsX;
        int segmentsY = Shape::m_segmentsY;
        /* make a surface object for the cube */
        surface = Surface();
        /* the "size" of each subdivision */
        float stepX = 1.0f / segmentsX;
        float stepY = 1.0f / segmentsY;

        vector<vector<VertexID>> verticies[6] = {
            vector<vector<VertexID>>(segmentsX + 1, vector<VertexID>(segmentsY + 1, -1)),
            vector<vector<VertexID>>(segmentsX + 1, vector<VertexID>(segmentsY + 1, -1)),
            vector<vector<VertexID>>(segmentsX + 1, vector<VertexID>(segmentsY + 1, -1)),
            vector<vector<VertexID>>(segmentsX + 1, vector<VertexID>(segmentsY + 1, -1)),
            vector<vector<VertexID>>(segmentsX + 1, vector<VertexID>(segmentsY + 1, -1)),
            vector<vector<VertexID>>(segmentsX + 1, vector<VertexID>(segmentsY + 1, -1))
        };
        /* generate 3D vertices for all the points
        * in the shape. create six "faces" by 
        * changing the normal of all points on a
        * given face. populate vertices vector. */
        for (int face = 0; face < 6; ++face) {
            for (int i = 0; i <= segmentsX; ++i) {
                for (int j = 0; j <= segmentsY; ++j) {
                    float x = -0.5f + i * stepX;
                    float y = -0.5f + j * stepY;
                    float z = 0.5f;

                    glm::vec3 position;
                    glm::vec3 normal;

                    switch (face) {
                        case FRONT  : position = glm::vec3(x, y, z);  normal = glm::vec3( 0,  0,  1); break; // Front
                        case BACK   : position = glm::vec3(y, x, -z); normal = glm::vec3( 0,  0, -1); break; // Back
                        case TOP    : position = glm::vec3(x, z, -y); normal = glm::vec3( 0,  1,  0); break; // Top
                        case BOTTOM : position = glm::vec3(x, -z, y); normal = glm::vec3( 0, -1,  0); break; // Bottom
                        case RIGHT  : position = glm::vec3(z, y, -x); normal = glm::vec3( 1,  0,  0); break; // Right
                        case LEFT   : position = glm::vec3(-z, y, x); normal = glm::vec3(-1,  0,  0); break; // Left
                    }

                    VertexID vertexID = surface.addVertex(Vertex(position, normal));
                    verticies[face][i][j] = vertexID;
                }
            }
        }

        /* create two triangle faces for each tessellated
        * square, using its four vertices. populate faces
        * vector */
        for (int face = 0; face < 6; ++face) {
            for (int i = 0; i < segmentsX; ++i) {
                for (int j = 0; j < segmentsY; ++j) {
                    int v1 = verticies[face][i][j]; 
                    int v2 = verticies[face][i + 1][j];
                    int v3 = verticies[face][i][j + 1];
                    int v4 = verticies[face][i + 1][j + 1];

                    surface.makeFace(v1, v2, v3);
                    surface.makeFace(v2, v4, v3);
                }
            }
        }
    }


void Cube::draw() {
    tessellate();
    surface.draw();
}

void Cube::drawNormal() {
    tessellate();
    surface.drawNormal();
}

#undef FRONT 
#undef BACK 
#undef TOP 
#undef BOTTOM
#undef RIGHT 
#undef LEFT 