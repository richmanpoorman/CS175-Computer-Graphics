#ifndef COMPOSITE_H
#define COMPOSITE_H

#include "Shape.h"
#include "Geometry.h"

using namespace std;

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
	vector<glm::mat4> transMatrices;
};

#endif