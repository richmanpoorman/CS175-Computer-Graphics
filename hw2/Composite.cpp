#include "Composite.h"
#include "Shape.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

/* constructor
*
* 
*/
Composite::Composite() {

    // populate the shapes vector with surfaces


}

/* destructor
*/
Composite::~Composite() {}



/* draw
*
* accumulates the transform matrices and
* draws all the sub-shapes in a composite shape
*
*/
void Composite::draw() {

    // iterate through shapes and draw them in the correct position
    for (int i = 0; i < shapes.size(); i++) {
        glPushMatrix();

        // note: glMultMatrixf allows us to apply
        // the current transformation on
        // top of the ones already on the stack
        // apply respective transformation matrix, then draw
        glMultMatrixf(glm::value_ptr(transMatrices[i]));
        shapes[i].draw();

        glPopMatrix();
    }


}

/* drawNormal
*
* accumulates the transform matrices and
* draws all the sub-shapes' normals
* in a composite shape
*
*/
void Composite::drawNormal() {

    for (int i = 0; i < shapes.size(); i++) {
        glPushMatrix();

        // apply respective transformation matrix, then draw
        glMultMatrixf(glm::value_ptr(transMatrices[i]));
        shapes[i].drawNormal();

        glPopMatrix();
    }
    
}
