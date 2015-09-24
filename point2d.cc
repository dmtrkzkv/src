/*
 class point2d
 {
 public:
 int x;
 int y;
 point2d();
 point2d(int, int);
 point2d(double, double); // angle and length
 point2d(const point2d &);
 bool operator==(point2d);
 void operator=(point2d);
 point2d operator+(point2d);
 point2d operator-(point2d);
 point2d operator*(double);
 double distance(point2d);
 };
 */

#include <point2d.h>

point2d::point2d() {
	x = 0;
	y = 0;
	return;
}

point2d::point2d(double a, double b) {
	x = a;
	y = b;
	return;
}

point2d::point2d(point2d p, double angle, double length) {
	x = p.x + length * cos(angle);
	y = p.y + length * sin(angle);
	return;
}

point2d::point2d(const point2d & a) {
	x = a.x;
	y = a.y;
	return;
}

void point2d::setCoordinates(double setX, double setY) {
	x = setX;
	y = setY;
}

void point2d::setX(double setX) {
	x = setX;
}

void point2d::setY(double setY) {
	y = setY;
}

bool point2d::operator==(point2d a) {
	if ((a.x == x) && (a.y == y)) {
		return true;
	} else {
		return false;
	}
}

void point2d::operator=(point2d a) {
	x = a.x;
	y = a.y;
	return;
}

point2d point2d::operator+(point2d a) {
	point2d b;
	b.x = x + a.x;
	b.y = y + a.y;
	return b;
}

point2d point2d::operator-(point2d a) {
	point2d b;
	b.x = x - a.x;
	b.y = y - a.y;
	return b;
}

point2d point2d::operator*(double skalar) {
	point2d a;
	a.x = x * skalar;
	a.y = y * skalar;
	return a;
}

double point2d::distance(point2d a) {
	return sqrt((a.x - x) * (a.x - x) + (a.y - y) * (a.y - y));
}

point2d operator*(double skalar, point2d p) {
	point2d a;
	a.x = p.x * skalar;
	a.y = p.y * skalar;
	return a;
}

int point2d::X() {
	return int(x + 0.5);
}

int point2d::Y() {
	return int(y + 0.5);
}
