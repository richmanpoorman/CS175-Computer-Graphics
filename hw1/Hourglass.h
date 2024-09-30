#ifndef HOURGLASS_H
#define HOURGLASS_H

#include "Shape.h" 
#include "RevolutionSolid.h"


class Hourglass : public Shape {
public:
	Hourglass();
	~Hourglass();

	OBJ_TYPE getType() {
		return SHAPE_SPECIAL1;
	}

	void draw();

	void drawNormal();

private:
	RevolutionSolid hourglass;
};

#endif