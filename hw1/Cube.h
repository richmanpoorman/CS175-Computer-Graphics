#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"
#include <Geometry.h>

class Cube : public Shape {
public:
	Cube() {};
	~Cube() {};

	OBJ_TYPE getType() {
		return SHAPE_CUBE;
	}

    void tessellate(int segmentsX, int segmentsY);
    void draw() override;
    void drawNormal() override;

private:
    Surface surface;
};

#endif