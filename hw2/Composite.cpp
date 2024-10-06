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
* applies the composite shapes'
* transform matrix before 
* drawing all sub-shapes
*
*/
void Composite::draw() {
    // iterate through shapes and draw them in the correct position
    for (int i = 0; i < shapes.length(); i++) {
        // transformation for each shape
        glPushMatrix();
        
        // apply respective transformation matrix, then draw
        // transMatrices[i];
        // gl translate, rotate, scale, translate respectively each shape.

        shapes[i].draw();

        glPopMatrix();
    }
}

/* drawNormal
*
* applies the composite shapes'
* transform matrix before 
* drawing the normals of all sub-shapes
*
*/
void Composite::drawNormal() {

    glPushMatrix();

    // apply the current composite transform 
    glMultMatrixf(glm::value_ptr(transMatrix))

    // draw all the shapes' normals
    (for int i = 0; i < shapes.size(); i++) {
        shapes[i].drawNormal();
    }
    glPopMatrix();
}
