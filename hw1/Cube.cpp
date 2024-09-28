#include "Cube.h"
#include "Shape.h"
#include <iostream>

using namespace std;

Cube::Cube() {
    //tessellate(Shape::m_segmentsX, Shape::m_segmentsY);
}

Cube::~Cube() {}

/* tessellate 
* 
* generates the vertices for a tesselated 3D 
* cube with the provided number of subdivisions 
* (segments) along each axis
*
*/
void Cube::tessellate(int segmentsX, int segmentsY) {
        /* make a surface object for the cube */
        surface = Surface();

        /* the "size" of each subdivision */
        float stepX = 1.0f / segmentsX;
        float stepY = 1.0f / segmentsY;

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
                        case 0: position = glm::vec3(x, y, z); normal = glm::vec3(0, 0, 1); break;  // Front
                        case 1: position = glm::vec3(x, y, -z); normal = glm::vec3(0, 0, -1); break; // Back
                        case 2: position = glm::vec3(x, z, -y); normal = glm::vec3(0, 1, 0); break;  // Top
                        case 3: position = glm::vec3(x, -z, y); normal = glm::vec3(0, -1, 0); break; // Bottom
                        case 4: position = glm::vec3(z, y, -x); normal = glm::vec3(1, 0, 0); break;  // Right
                        case 5: position = glm::vec3(-z, y, x); normal = glm::vec3(-1, 0, 0); break; // Left
                    }

                    surface.addVertex(Vertex(position, normal));
                }
            }
        }

        /* create two triangle faces for each tessellated
        * square, using its four vertices. populate faces
        * vector */
        int verticesPerFace = (segmentsX + 1) * (segmentsY + 1);
        for (int face = 0; face < 6; ++face) {
            for (int i = 0; i < segmentsX; ++i) {
                for (int j = 0; j < segmentsY; ++j) {
                    int baseIndex = face * verticesPerFace + i * (segmentsY + 1) + j;
                    int v1 = baseIndex;
                    int v2 = baseIndex + 1;
                    int v3 = baseIndex + (segmentsY + 1);
                    int v4 = baseIndex + (segmentsY + 1) + 1;

                    surface.makeFace(v1, v2, v3);
                    surface.makeFace(v2, v4, v3);
                }
            }
        }
    }

