#ifndef COMPOSITE_H
#define COMPOSITE_H

#include "Shape.h"
#include "Geometry.h"

class Composite : public Shape {
public:
	Composite();
	~Composite();

	OBJ_TYPE getType() {
		return SHAPE_SCENE;
	}


	void draw();
	void drawNormal();


private:
	vector<Surface> shapes;
	glm::mat4 transMatrix;
};

#endif