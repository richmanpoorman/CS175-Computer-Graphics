#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include "RevolutionSolid.h"

class Cylinder : public Shape {
public:
	Cylinder();
	~Cylinder();

	OBJ_TYPE getType() {
		return SHAPE_CYLINDER;
	}


	void draw();

	void drawNormal();


private:
	RevolutionSolid cylinder; 
};

#endif