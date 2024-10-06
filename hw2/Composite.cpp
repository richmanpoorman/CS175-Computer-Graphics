#include "Composite.h"
#include "Shape.h"
#include <iostream>

using namespace std;

/* constructor
*
* 
*/
Composite::Composite() {

    // populate the shapes vector with subshape surfaces

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

    glPushMatrix();

    (for int i = 0; i < shapes.size(); i++) {
        // note: glMultMatrixf allows us to apply
        // the current transformation on
        // top of the ones already on the stack
        glMultMatrixf(glm::value_ptr(transMatrices[i]));
        shapes[i].draw();
    }

    glPopMatrix();


    // // iterate through shapes and draw them in the correct position
    // for (int i = 0; i < shapes.length(); i++) {

    //     // transformation for each shape
    //     glPushMatrix();
        
    //     // apply respective transformation matrix, then draw
    //     // transMatrices[i];
    //     // gl translate, rotate, scale, translate respectively each shape.

    //     shapes[i].draw();

    //     glPopMatrix();
    // }
}

/* drawNormal
*
* accumulates the transform matrices and
* draws all the sub-shapes' normals
* in a composite shape
*
*/
void Composite::drawNormal() {

    glPushMatrix();

    (for int i = 0; i < shapes.size(); i++) {
        // compound each transform matrix
        glMultMatrixf(glm::value_ptr(transMatrices[i]))
        shapes[i].drawNormal();
    }
    glPopMatrix();
}
