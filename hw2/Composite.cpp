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
* 
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
* 
*
*/
void Composite::drawNormal() {
    
}
