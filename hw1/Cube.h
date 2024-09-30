#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"
#include "Geometry.h"

class Cube : public Shape {
public:
	Cube();
	~Cube();

	OBJ_TYPE getType() {
		return SHAPE_CUBE;
	}

    void tessellate();
	void draw();
	void drawNormal();

private:
    Surface surface;
};

#endif