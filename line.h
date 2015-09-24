#ifndef _line_
#define _line_

#include "point2d.h"

class line {
public:
	point2d p1;
	point2d p2;
	line();
	line(point2d, point2d);
	line(const line &);
	double distance(point2d);
	point2d intersection(line);
	static bool in_line(point2d, point2d, point2d, float tolerance = 1.0);
};

#endif
