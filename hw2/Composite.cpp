#include "Composite.h"
#include "Shape.h"
#include <iostream>

using namespace std;

/* constructor
*
* 
*/
Composite::Composite() {

}

/* destructor
*/
Composite::~Composite() {}



/* draw
*
* applies the composite shape's
* transform matrix before 
* drawing all sub-shapes
*
*/
void Composite::draw() {

    glPushMatrix();

    // glMultMatrixf allows us to apply
    // the current transformation
    // on top of the ones already on the stack
    glMultMatrixf(glm::value_ptr(transMatrix))

    // draw all the sub-shapes in this composite shape
    (for int i = 0; i < shapes.size(); i++) {
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
* applies the composite shape's
* transform matrix before 
* drawing the normals of all sub-shapes
*
*/
void Composite::drawNormal() {

    glPushMatrix();

    // apply this composite shape's transformation
    glMultMatrixf(glm::value_ptr(transMatrix))

    // draw all the sub-shapes' normals
    (for int i = 0; i < shapes.size(); i++) {
        shapes[i].drawNormal();
    }
    glPopMatrix();
}
