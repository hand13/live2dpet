#pragma once
#include <memory>
#include <DirectXTK/SimpleMath.h>
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Matrix;

struct MVP {
	Matrix model;
	Matrix view;
	Matrix perspective;
};

struct Data {
	void * data;
	Data():data(nullptr){}
	~Data() {
		if(data != nullptr) {
			free(data);
		}
	}
};

class Buffer {
	private:
	std::shared_ptr<Data> buffer;
	size_t dataLength;
	size_t bufferLength;
	size_t resize(size_t size);
	public:
	~Buffer();
	Buffer();
	size_t pushData(void * data,size_t length);
	void * getData()const;
	size_t getDataLength()const;
};

Buffer readFileToBufferA(const char* filename,int length);
Buffer readFileToBuffer(const wchar_t* filename);

class Timer {
	private:
	int current;
	int startTime;
	public:
	Timer():current(0),startTime(0){}
	void start();
	int delta();
	int elapsedFromStart();
	static int now();
};

struct Camera {
	private:
	Vector3 position;
	Vector3 up;
	float yaw;
	float pitch;
	public:
	Camera(
	const Vector3 & position, 
	float yaw,
	float pitch,
	const Vector3 & up);
	Camera();
	static Camera defaultCamera();
	DirectX::SimpleMath::Matrix getViewMatrix();
	void moveFront(float delta);
	void moveRight(float delta);
	void addYaw(float delta);
	void addPitch(float delta);
	Vector3 getPosition()const;
	Vector3 getDirection()const;
};

inline void clamp(float start,float end,float & value) {
	if(value < start) {
		value = start;
	}
	if(value > end) {
		value = end;
	}
}

template<typename T>
struct Point{
	T x;
	T y;
	Point():x(0),y(0){}
	Point(T x,T y):x(x),y(y){}
	Point(const Point & o) {
		x= o.x;
		y= o.y;
	}
	Point operator+(const Point & o) const{
		Point r(0,0);
		r.x = x + o.x;
		r.y = y + o.y;
		return r;
	}
};
typedef  Point<float> PointF;

struct Light {
	Vector3 position;
	Vector3 color;
	float strength;
};