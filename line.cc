#include <line.h>
#include <iostream>

using namespace std;

line::line() {
	p1 = point2d(0, 0);
	p2 = point2d(0, 1);
}

line::line(point2d _p1, point2d _p2) {
	if (_p1 == _p2)
		cerr << "Error: line constructed from identical points" << endl;
	p1 = _p1;
	p2 = _p2;
}

line::line(const line & l) {
	p1 = l.p1;
	p2 = l.p2;
}

double line::distance(point2d p) {
	point2d u(p2 - p1);
	point2d v(u.y, -u.x); //v is perpendicular to u
	line l1(p, p + v);
	point2d intersec(intersection(l1));//intersection of this line and l1
	return p.distance(intersec);
}

bool line::in_line(point2d _p1, point2d _p2, point2d _p3, float tolerance) {//true falls _p2 auf der linie zw _p1 und _p3 ist
	line l(_p1, _p3);
	return (l.distance(_p2) < tolerance);
}

point2d line::intersection(line l) {
	point2d r1(p1);
	point2d u1(p2 - p1);
	point2d r2(l.p1);
	point2d u2(l.p2 - l.p1);

	float det = -u2.x * u1.y + u2.y * u1.x;
	if (det == 0.0) {
		cerr << "Error: the given lines do not intersect" << endl;
		return point2d(0, 0);
	}

	float par = (r2.x - r1.x) * u1.y + (r1.y - r2.y) * u1.x;
	par = par / det;

	return r2 + par * u2;
}
