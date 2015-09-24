#ifndef __imageio_h__
#define __imageio_h__

#include <stdio.h>

struct rgb {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

class bitmap {
	//data members:
	int type;
	rgb* data;
	int maxcolor;
	int pr;
	int pg;
	int pb;

	//for inner usage:
	inline int nextvalidchar(FILE* f);
	void initMap(void);

public:
	int width;
	int heigth;

	//default constructor:
	bitmap() :
		type(0), data(0), maxcolor(0), pr(255), pg(0), pb(0), width(0), heigth(0), map(0) {
	}
	//construct empty bitmap:
	bitmap(int _type, int _heigth, int _width, int _maxcolor);
	//construct bitmap from a file:
	bitmap(const char* fname);
	//copy constructor:
	bitmap(const bitmap& bmp);

	//destructor:
	~bitmap();

	//get width and heigth
	int getWidth() {
		return width;
	}
	int getHeigth() {
		return heigth;
	}

	//for accessing pixels.
	//Example:
	//   bitmap::map[row][column].r=150;
	//   bitmap::map[row][column].g=20;
	//   bitmap::map[row][column].b=210;
	rgb** map;

	//set color of the pen:
	void setPen(int r, int g = 0, int b = 0);
	//draw dot on the bitmp:
	void dot(int row, int col, int size = 2);
	//draw line on the bitmap:
	void line(int row1, int col1, int row2, int col2);

	//read bitmap from a file:
	bool read(const char* fname);
	//write bitmap into a file:
	bool write(const char* fname);

	//remove color channels:
	void removeGreenChannel();
	void removeBlueChannel();
	void removeRedChannel();

	void operator=(bitmap bmp);

};

#endif
