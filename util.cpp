#include "util.h"
#include <filesystem>
#include <stdio.h>
#include <Windows.h>
namespace fs = std::filesystem;

Buffer::Buffer():buffer(std::make_shared<Data>()),dataLength(0),bufferLength(0) {
}
size_t Buffer::pushData(void * dat,size_t length) {

	if(length <= 0){
		return 0;
	}

	resize(dataLength + length);
	memcpy((char*)buffer->data + dataLength,dat,length);
	dataLength += length;
	return dataLength;
}

size_t Buffer::resize(size_t size) {
	if(size <= bufferLength) {
		return bufferLength;
	}

	if(buffer->data == nullptr) {
		buffer->data = malloc(size);
	}else {
		buffer->data = realloc(buffer->data,size);
	}
	bufferLength = size;
	return bufferLength;
}
void * Buffer::getData()const {
	return buffer->data;
}
size_t Buffer::getDataLength()const {
	return dataLength;
}

Buffer::~Buffer() {
}

Buffer readFileToBuffer(const wchar_t* filename) {
	Buffer result;
	fs::path ps(filename);
	auto size = fs::file_size(ps);
	void * buffer = malloc(size);
	FILE * file;

	if(_wfopen_s(&file,filename,L"rb") != 0) {
		return result;
	}

	size_t readNum = 0;
	size_t tmp = 0;

	do{
		tmp = fread((char*)buffer + readNum,1,size-readNum,file);
		readNum += tmp;
	}while (readNum != size && tmp != 0);
	
	result.pushData(buffer,size);
	free(buffer);
	return result;
}

void Timer::start() {
	current = Timer::now();
	startTime = current;
}
int Timer::delta() {
	int now = Timer::now();
	int d = now - current;
	current = now;
	return d;
}

int Timer::now() {
	return GetTickCount();
}
int Timer::elapsedFromStart() {
	return now() - startTime;
}

Camera::Camera() {
}
Camera::Camera(const Vector3 & position,float yaw,float pitch,const Vector3& up)
:position(position),yaw(yaw),pitch(pitch),up(up){
}

Camera Camera::defaultCamera() {

	Vector3 pos = {0.f,0.f,10.f};
	Vector3 up = {0.f,1.f,0.f};
	return Camera(pos,-(DirectX::XM_PI/2.0),0,up);
}

Matrix Camera::getViewMatrix(){
	return Matrix::CreateLookAt(position,Vector3(position + getDirection()),up);
}

void Camera::moveFront(float delta) {
	position +=  getDirection() * delta;
}

void Camera::moveRight(float delta) {
	Vector3 right = getDirection().Cross(up);
	right.Normalize();
	position +=  right * delta;
}

void Camera::addYaw(float delta) {
	yaw += delta;
}
//need to update up vector
void Camera::addPitch(float delta) {
	pitch += delta;
	clamp(-(DirectX::XM_2PI/4 - 0.1f),DirectX::XM_2PI/4 - 0.1f,pitch);
}
Vector3 Camera::getDirection()const{
	Vector3 result;
	result.x = cos(yaw) * cos(pitch);
	result.y = sin(pitch);
	result.z = sin(yaw) * cos(pitch);
	return result;
}
Vector3 Camera::getPosition()const {
	return position;
}