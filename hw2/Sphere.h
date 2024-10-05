#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h" 
#include "RevolutionSolid.h"


class Sphere : public Shape {
public:
	Sphere();
	~Sphere();

	OBJ_TYPE getType() {
		return SHAPE_SPHERE;
	}

	void draw();

	void drawNormal();

private:
	RevolutionSolid sphere;
};

#endif