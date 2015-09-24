#ifndef _point2d_
#define _point2d_

#include "math.h"

class point2d {
public:
	double x;
	double y;
	int X();
	int Y();
	point2d();
	point2d(double, double);
	point2d(point2d, double, double); // point angle and length
	point2d(const point2d &);
	void setCoordinates(double setX, double setY);
	void setX(double setX);
	void setY(double setY);
	bool operator==(point2d);
	void operator=(point2d);
	point2d operator+(point2d);
	point2d operator-(point2d);
	point2d operator*(double);
	double distance(point2d);
	friend point2d operator*(double, point2d);

	friend class line;
};

#endif
