#define NUM_OPENGL_LIGHTS 8

#include "MyGLCanvas.h"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <vector>

using namespace std;

int Shape::m_segmentsX;
int Shape::m_segmentsY;

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
	parser = NULL;
	camera = NULL;

	objType = SHAPE_CUBE;
	cube = new Cube();
	cylinder = new Cylinder();
	cone = new Cone();
	sphere = new Sphere();
	shape = cube;

	resetScene();
}

MyGLCanvas::~MyGLCanvas() {
	delete cube;
	delete cylinder;
	delete cone;
	delete sphere;
	if (camera != NULL) {
		delete camera;
	}
	if (parser != NULL) {
		delete parser;
	}
}

void MyGLCanvas::resetScene() {
	glm::vec3 eyePosition = glm::vec3(2.0f, 2.0f, 2.0f);

	wireframe = 0;
	fill = 1;
	normal = 0;
	smooth = 0;
	segmentsX = segmentsY = 10;

	shape->setSegments(segmentsX, segmentsY);

	if (camera != NULL) {
		delete camera;
		camera = NULL;
	}
	camera = new Camera();
	camera->orientLookAt(eyePosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	if (parser != NULL) {
		delete parser;
		parser = NULL;
	}
}

void MyGLCanvas::setShape(OBJ_TYPE type) {
	objType = type;
	printf("set shape to: %d\n", type);
}


void MyGLCanvas::renderShape(OBJ_TYPE type) {
	switch (type) {
	case SHAPE_CUBE:
		shape = cube;
		break;
	case SHAPE_CYLINDER:
		shape = cylinder;
		break;
	case SHAPE_CONE:
		shape = cone;
		break;
	case SHAPE_SPHERE:
		shape = sphere;
		break;
	case SHAPE_SPECIAL1:
		shape = cube;
		break;
	default:
		shape = cube;
	}

	shape->setSegments(segmentsX, segmentsY);
	shape->draw();
}

void MyGLCanvas::loadSceneFile(const char* filenamePath) {
	if (parser != NULL) {
		delete parser;
	}
	parser = new SceneParser(filenamePath);

	bool success = parser->parse();
	cout << "success? " << success << endl;
	if (success == false) {
		delete parser;
		parser = NULL;
	}
	else {
		SceneCameraData cameraData;
		parser->getCameraData(cameraData);

		camera->reset();
		camera->setViewAngle(cameraData.heightAngle);
		if (cameraData.isDir == true) {
			camera->orientLookVec(cameraData.pos, cameraData.look, cameraData.up);
		}
		else {
			camera->orientLookAt(cameraData.pos, cameraData.lookAt, cameraData.up);
		}
	}
}


void MyGLCanvas::setSegments() {
	shape->setSegments(segmentsX, segmentsY);
}

void MyGLCanvas::draw() {
	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		/**********************************************/
		/*    Enable normalizing normal vectors       */
		/*    (e.g. normals not affected by glScalef) */
		/**********************************************/

		glEnable(GL_NORMALIZE);

		/****************************************/
		/*          Enable z-buferring          */
		/****************************************/

		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);
		glFrontFace(GL_CCW); //make sure that the ordering is counter-clock wise
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glm::mat4 projectionMat4 = camera->getProjectionMatrix();
	glMultMatrixf(glm::value_ptr(projectionMat4));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glm::mat4 modelViewMat4 = camera->getModelViewMatrix();
	glMultMatrixf(glm::value_ptr(modelViewMat4));

	if (smooth) {
		glShadeModel(GL_SMOOTH);
	}
	else {
		glShadeModel(GL_FLAT);
	}

	if (objType == SHAPE_SCENE) {
		drawScene();
	}
	else {
		drawObject(objType);
	}

	drawAxis();
}

void MyGLCanvas::drawObject(OBJ_TYPE type) {
	//disable all the lights, and then enable each one...
	for (int i = 0; i < NUM_OPENGL_LIGHTS; i++) {
		glDisable(GL_LIGHT0 + i);
	}

	GLfloat light_pos0[] = { 1.0, 0.5, 2.0, 0.0f };
	GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if (wireframe) {
		glDisable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glColor3f(1.0f, 1.0f, 0.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		renderShape(objType);
		glEnable(GL_LIGHTING);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(0.5f, 0.5f, 0.5f);
		renderShape(objType);
	}
}

/* primitiveToShape
*
* creates actual Shapes from their primitives 
*/
Shape *primitiveToShape(ScenePrimitive* primitive) {

	Shape *subshape;

	OBJ_TYPE type = primitive->type;
	switch (type) {
		case SHAPE_CUBE:
			Cube *cube;
			cube = new Cube();
			subshape = cube;
			break;
		case SHAPE_CYLINDER:
			Cylinder *cylinder;
			cylinder = new Cylinder();
			subshape = cylinder;
			break;
		case SHAPE_CONE:
			Cone *cone;
			cone = new Cone();
			subshape = cone;
			break;
		case SHAPE_SPHERE:
			Sphere *sphere;
			sphere = new Sphere();
			subshape = sphere;
			break;
		default:
			return nullptr;

	}	
	
	return shape;
}


glm::mat4 SceneTransf_to_Matrix(SceneTransformation transf) {
	switch (transf.type) {
	case TRANSFORMATION_TRANSLATE:
		cout << glm::to_string(transf.matrix) << endl;
	case  TRANSFORMATION_SCALE:
		cout << glm::to_string(transf.matrix) << endl;
	case TRANSFORMATION_ROTATE:
		cout << glm::to_string(transf.matrix) << endl;
	case TRANSFORMATION_MATRIX:
		// wtf are you
		cout << glm::to_string(transf.matrix) << endl;
	default:
		std::cout << "what";
	}
}

// TODO: Our function to traverse the parser's output
vector<pair<Shape, glm::mat4>> flattenSceneGraph(SceneNode* root) {
	if (root == nullptr) return {};
	vector<pair<Shape, glm::mat4>> result = vector<pair<Shape, glm::mat4>>();
	glm::mat4 transform = glm::mat4(1.0f);
	flattenTraversal(root, transform, result);
	return result;
}

void flattenTraversal(SceneNode* current, glm::mat4 &transformations, vector<pair<Shape, glm::mat4>>& result) {
	vector<SceneNode*> children = current->children; 
	vector<ScenePrimitive*> primitives = current->primitives;
	vector<SceneTransformation*> currentTransformations = current->transformations;

	glm::mat4 nodeTransformation = SceneTransf_to_Matrix(currentTransformations);
	glm::mat4 newMatrix = transformations * nodeTransformation; 

	for (ScenePrimitive* primitive : primitives) {
		Shape *shape = primitiveToShape(primitive);
		pair<Shape, glm::mat4> primitivePair = make_pair(shape, newMatrix);
		result.push_back(primitivePair);
	}

	for (SceneNode *child : children) {
		flattenTraversal(child, newMatrix, result);
	}
}

void MyGLCanvas::drawScene() {
	if (parser == NULL) {
		return;
	}

	glPushMatrix();

	//disable all the lights, and then enable each one...
	for (int i = 0; i < NUM_OPENGL_LIGHTS; i++) {
		glDisable(GL_LIGHT0 + i);
	}

	SceneNode* root = parser->getRootNode();
	glm::mat4 compositeMatrix(1.0f);

	if (wireframe) {
		glColor3f(1.0, 1.0, 0.0);
		glDisable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//TODO: draw wireframe of the scene
		// note that you don't need to applyMaterial, just draw the geometry
		glEnable(GL_LIGHTING);
	}

	glDisable(GL_COLOR_MATERIAL);
	int numLights = parser->getNumLights();
	for (int i = 0; i < numLights; i++) {
		SceneLightData lightData;
		parser->getLightData(i, lightData);
		setLight(lightData);
	}

	glColor3f(1, 1, 1);
	glEnable(GL_LIGHTING);
	if (fill == 1) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//TODO: render the scene
		// note that you should always applyMaterial first before drawing each geometry
	}
	glDisable(GL_LIGHTING);

	glPopMatrix();
}

int MyGLCanvas::handle(int e) {
	//printf("Event was %s (%d)\n", fl_eventnames[e], e);
	switch (e) {
	case FL_KEYUP:
		printf("keyboard event: key pressed: %c\n", Fl::event_key());
		break;
	case FL_MOUSEWHEEL:
		break;
	}

	return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
	Fl_Gl_Window::resize(x, y, w, h);
	puts("resize called");
}

void MyGLCanvas::drawAxis() {
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0, 0, 0); glVertex3f(1.0, 0, 0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0, 0, 0); glVertex3f(0.0, 1.0, 0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.0);
	glEnd();
	glEnable(GL_LIGHTING);
}

void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

	camera->setScreenSize(width, height);
}

/***************************************** applyMaterial() *****************/
void MyGLCanvas::applyMaterial(const SceneMaterial &material) {
	SceneGlobalData globalData;
	parser->getGlobalData(globalData);

	SceneMaterial material_local = material;
	material_local.cAmbient.r *= globalData.ka;
	material_local.cAmbient.g *= globalData.ka;
	material_local.cAmbient.b *= globalData.ka;
	material_local.cDiffuse.r *= globalData.kd;
	material_local.cDiffuse.g *= globalData.kd;
	material_local.cDiffuse.b *= globalData.kd;
	material_local.cSpecular.r *= globalData.ks;
	material_local.cSpecular.g *= globalData.ks;
	material_local.cSpecular.b *= globalData.ks;
	material_local.cReflective.r *= globalData.ks;
	material_local.cReflective.g *= globalData.ks;
	material_local.cReflective.b *= globalData.ks;
	material_local.cTransparent.r *= globalData.kt;
	material_local.cTransparent.g *= globalData.kt;
	material_local.cTransparent.b *= globalData.kt;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &material_local.cAmbient.r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &material_local.cDiffuse.r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &material_local.cSpecular.r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &material_local.cEmissive.r);
}

/***************************************** setLight() *****************/
void MyGLCanvas::setLight(const SceneLightData &light) {
	// Zero ambient light because the ambient light for the scene is
	// set with GL_LIGHT_MODEL_AMBIENT
	float zero[] = { 0, 0, 0, 0 };

	// There are 8 lights in OpenGL, GL_LIGHT0 to GL_LIGHT7, and
	// each GL_LIGHT* is one greater than the previous one.  This
	// means the nth light is GL_LIGHT0 + n.
	int id = GL_LIGHT0 + light.id;

	glLightfv(id, GL_AMBIENT, zero);
	glLightfv(id, GL_DIFFUSE, &light.color.r);
	glLightfv(id, GL_SPECULAR, &light.color.r);
	glLightf(id, GL_CONSTANT_ATTENUATION, light.function[0]);
	glLightf(id, GL_LINEAR_ATTENUATION, light.function[1]);
	glLightf(id, GL_QUADRATIC_ATTENUATION, light.function[2]);

	if (glIsEnabled(id))
		cout << "warning: GL_LIGHT" << light.id << " enabled more than once, not supposed to happen" << endl;

	switch (light.type) {
	case LIGHT_POINT: {
		glm::vec4 lightPos = glm::vec4(light.pos, 1.0f);
		glLightfv(id, GL_POSITION, glm::value_ptr(lightPos));
		glEnable(id);
		break;
	}

	case LIGHT_DIRECTIONAL: {
		//  make sure the direction vector is normalized (it isn't for a lot of scene files)
		glm::vec3 direction = -light.dir;
		direction = normalize(direction);
		glm::vec4 lightDir = glm::vec4(light.pos, 0.0f);
		glLightfv(id, GL_POSITION, glm::value_ptr(lightDir));
		glEnable(id);
		break;
	}
	}
}
