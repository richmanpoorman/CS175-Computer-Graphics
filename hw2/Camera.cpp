#include "Camera.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp> 

using namespace std;

struct UVWVectors {
	glm::vec3 u, v, w; 
};

Camera::Camera() {
	reset();
}

Camera::~Camera() {
}

void Camera::reset() {
	screenWidth = screenHeight = 200;
	screenWidthRatio = 1.0f;
	rotU = rotV = rotW = 0;

	lookVector = glm::vec3(0.0f, 0.0f, -1.0f);
	eyeVector = glm::vec3(0.0f, 0.0f, 1.0f);
	upVector = glm::vec3(0.0f, 1.0f, 0.0f);

	setNearPlane(NEAR_PLANE);
	setFarPlane(FAR_PLANE);

	orientLookVec(eyeVector, lookVector, upVector);

	setViewAngle(VIEW_ANGLE);
	// cout << viewAngle << endl;
}

//called by main.cpp as a part of the slider callback for controlling rotation
// the reason for computing the diff is to make sure that we are only incrementally rotating the camera
void Camera::setRotUVW(float u, float v, float w) {
	float diffU = u - rotU;
	float diffV = v - rotV;
	float diffW = w - rotW;
	rotateU(diffU);
	rotateV(diffV);
	rotateW(diffW);
	rotU = u;
	rotV = v;
	rotW = w;
}


UVWVectors uvwVectors(glm::vec3 lookVector, glm::vec3 upVector) {
	glm::vec3 w = -glm::normalize(lookVector);
	glm::vec3 u = glm::normalize(glm::cross(upVector, w));
	glm::vec3 v = glm::cross(w, u);
	return { u, v, w };
}

/* orientLookAt
* establishes camera orientation
* (given lookAt point)
*/
void Camera::orientLookAt(glm::vec3 eyePoint, glm::vec3 lookatPoint, glm::vec3 upVec) {
	eyeVector = eyePoint;
	lookVector = glm::normalize(lookatPoint - eyePoint);
	glm::vec3 rightVector = glm::normalize(glm::cross(lookVector, upVec));
	upVector = glm::cross(rightVector, lookVector);
}

/* orientLookVec
* establishes camer orientaiton
* (given look vector)
*/
void Camera::orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec) {
	eyeVector = eyePoint;
	lookVector = lookVec;
	glm::vec3 rightVector = glm::normalize(glm::cross(lookVector, upVec));
	upVector = glm::cross(rightVector, lookVector);
}

glm::mat4 Camera::getRotateMatrix() {
	UVWVectors uvw = uvwVectors(getLookVector(), getUpVector());
	glm::vec3 u = uvw.u, v = uvw.v, w = uvw.w;

	glm::mat4 rotateMatrix(1.0f);
	rotateMatrix[0] = glm::vec4(u, 0.0f);
	rotateMatrix[1] = glm::vec4(v, 0.0f);
	rotateMatrix[2] = glm::vec4(w, 0.0f);
	rotateMatrix = glm::transpose(rotateMatrix);

	return rotateMatrix;
}

glm::mat4 Camera::getTranslateMatrix() {
	glm::mat4 translateMatrix(1.0f);
	glm::vec4 translateVector = glm::vec4(-getEyePoint(), 1.0);
	translateMatrix[3] = translateVector;

	return translateMatrix;
}

glm::mat4 Camera::getScaleMatrix() {
	// Note that w / 2 = tan(theta_w / 2) * far [Same with height]
	glm::mat4 scaleMat4(1.0);
	scaleMat4[0][0] = 2.0 / getScreenWidth(); 
	scaleMat4[1][1] = 2.0 / getScreenHeight(); 
	scaleMat4[2][2] = 1.0 / getFarPlane(); 
	return scaleMat4;
}

glm::mat4 Camera::getInverseScaleMatrix() {
	// Note that w / 2 = tan(theta_w / 2) * far [Same with height]
	glm::mat4 invScaleMat4(1.0);
	invScaleMat4[0][0] = getScreenWidth() / 2.0;
	invScaleMat4[1][1] = getScreenHeight() / 2.0;
	invScaleMat4[2][2] = getFarPlane();
	return invScaleMat4;
}

glm::mat4 Camera::getUnhingeMatrix() {

	float c = -getNearPlane() / getFarPlane(); 

	/*
		[ 1 0  0             0             ]
		[ 0 1  0             0             ]
		[ 0 0 -(1 / [c + 1]) (c / [c + 1]) ]
		[ 0 0 -1             0             ]
	*/
	glm::mat4 unhingeMat4(1.0f); 
	unhingeMat4[2][2] = -1 / (1 + c);
	unhingeMat4[3][2] = c / (1 + c);
	unhingeMat4[2][3] = -1;
	unhingeMat4[3][3] = 0;
	
	return unhingeMat4;
}


glm::mat4 Camera::getProjectionMatrix() {
	glm::mat4 projMat4 = getUnhingeMatrix() * getScaleMatrix();
	return projMat4;
}

glm::mat4 Camera::getInverseModelViewMatrix() {
	return glm::inverse(getModelViewMatrix());
}

// View Angle is the height
void Camera::setViewAngle (float _viewAngle) {
	viewAngle = _viewAngle;
	float newHeight = 2 * tan((PI * viewAngle)/ 360) * getFarPlane(); 
	float newWidth = newHeight / getScreenWidthRatio();

	screenWidth  = (int)newWidth;
	screenHeight = (int)newHeight;
}

void Camera::setNearPlane (float _nearPlane) {
	nearPlane = _nearPlane;
	filmPlanDepth = getFarPlane() - nearPlane;
}

void Camera::setFarPlane (float _farPlane) {
	farPlane = _farPlane;
	filmPlanDepth = farPlane - getNearPlane();
}

void Camera::setScreenSize (int _screenWidth, int _screenHeight) {
	screenWidth      = _screenWidth; 
	screenHeight     = _screenHeight; 
	screenWidthRatio = 1.0f * screenHeight / screenWidth; 
}

glm::mat4 Camera::getModelViewMatrix() {
	glm::mat4 modelViewMatrix = getRotateMatrix() * getTranslateMatrix();
	return modelViewMatrix;
}


void Camera::rotateV(float degrees) {
	glm::vec3 v = uvwVectors(getLookVector(), getUpVector()).v;
	glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1.0f), PI * degrees / 180.0f, v);
	lookVector = rotationMatrix * lookVector;
}

void Camera::rotateU(float degrees) {
	glm::vec3 u = uvwVectors(getLookVector(), getUpVector()).u;
	glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1.0f), PI * degrees / 180.0f, u);
	lookVector = rotationMatrix * lookVector;
}

void Camera::rotateW(float degrees) {
	glm::vec3 w = uvwVectors(getLookVector(), getUpVector()).w;
	glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1.0f), PI * degrees / 180.0f, w);
	upVector = rotationMatrix * upVector;
}

void Camera::translate(glm::vec3 v) {
	glm::mat3 translationMatrix = glm::translate(glm::mat4(1.0f), v);
	eyeVector = translationMatrix * eyeVector;
}

void Camera::rotate(glm::vec3 point, glm::vec3 axis, float degrees) {
	glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1.0f), PI * degrees / 180.0f, axis);
	lookVector = rotationMatrix * lookVector;
}


glm::vec3 Camera::getEyePoint() {
	return eyeVector;
}

glm::vec3 Camera::getLookVector() {
	return lookVector;
}

glm::vec3 Camera::getUpVector() {
	return upVector; 
}

float Camera::getViewAngle() {
	return viewAngle;
}

float Camera::getNearPlane() {
	return nearPlane;
}

float Camera::getFarPlane() {
	return farPlane;
}

int Camera::getScreenWidth() {
	return screenWidth;
}

int Camera::getScreenHeight() {
	return screenHeight;
}

float Camera::getScreenWidthRatio() {
	return screenWidthRatio;
}
