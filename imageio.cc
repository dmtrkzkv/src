#include <stdio.h>
#include <iostream>
#include <math.h>
#include "imageio.h"
#include "point2d.h"

using namespace std;

bitmap::bitmap(int _type, int _heigth, int _width, int _maxcolor) :
	type(0), data(0), maxcolor(0), pr(255), pg(0), pb(0), width(0), heigth(0), map(0) {
	if (_type == 6 && _maxcolor < 256) {
		int size = _width * _heigth;
		data = new rgb[size];
		if (!data) {
			cerr << "Memory allocation error (bitmap conversion constructor)." << endl;
			return;
		}
		for (int i = 0; i < size; i++) {
			data[i].r = 0;
			data[i].g = 0;
			data[i].b = 0;
		}
	} else {
		cerr << "Bitmap type " << _type << " with " << _maxcolor + 1
				<< " colors not supported (bitmap conversion constructor)." << endl;
		return;
	}
	type = _type;
	width = _width;
	heigth = _heigth;
	maxcolor = _maxcolor;
	initMap();

}

void bitmap::initMap() {
	typedef rgb* rgbptr;
	if (map)
		delete[] map;
	map = new rgbptr[heigth];
	for (int i = 0; i < heigth; i++)
		map[i] = data + i * width;
}

bitmap::bitmap(const char* fname) :
	type(0), data(0), maxcolor(0), pr(255), pg(0), pb(0), width(0), heigth(0), map(0) {
	if (!read(fname))
		cerr << "Error reding " << fname << " (conversion constructor)." << endl;
}

bitmap::~bitmap() {
	if (data)
		delete[] data;
	if (map)
		delete[] map;
}

void bitmap::setPen(int r, int g, int b) {
	pr = r;
	pg = g;
	pb = b;
}

void bitmap::dot(int row, int col, int size) {
	for (int i = row - size; i <= row + size; i++)
		for (int j = col - size; j <= col + size; j++) {
			if (i < 0 || i >= heigth || j < 0 || j >= width)
				continue;
			map[i][j].r = pr;
			map[i][j].g = pg;
			map[i][j].b = pb;
		}
}

void bitmap::line(int row1, int col1, int row2, int col2) {
	const float dr = row2 - row1;
	const float dc = col2 - col1;
	const float length = hypot(dr, dc);
	const float dt = 1.0 / length;
	for (float t = 0; t <= 1; t += dt) {
		int row = row1 + (int) (t * dr + 0.5);
		int col = col1 + (int) (t * dc + 0.5);
		if (row < 0 || row >= heigth || col < 0 || col >= width)
			continue;
		map[row][col].r = pr;
		map[row][col].g = pg;
		map[row][col].b = pb;
	}
}

inline int bitmap::nextvalidchar(FILE* f) {
	bool weiter = true;
	register int ch;
	while (weiter) {
		ch = getc(f);
		if (ch >= '0' && ch <= '9') {//next valid char found
			weiter = false;
			break;
		} else if (ch == '#') {//comment begining, read out comment chars until the end of the line
			do {
				ch = getc(f);
			} while (ch != '\n' && ch != EOF);
		} else if (ch == EOF) {//EOF appears early, file header truncated
			weiter = false;
			cerr << "File header truncated (bitmap::nextvalidchar)." << endl;
			break;
		} else if (ch != ' ' && ch != '\t' && ch != '\n') {
			weiter = false;
			cerr << "Wrong characters in the file header (bitmap::nextvalidchar)." << endl;
			break;
		}
	}
	return ch;
}

bool bitmap::read(const char* fname) {

	FILE* f = fopen(fname, "r");

	if (!f) {
		cerr << "Error opening " << fname << endl;
		return false;
	}

	register int ch = getc(f);
	if (ch != 'P') {
		fclose(f);
		cerr << fname << " is not a .pnm image file, missing P." << endl;
		return false;
	}

	ch = getc(f);
	if (ch < '0' || ch > '6') {
		fclose(f);
		cerr << fname << " has an illegal image format (P" << ch << ")" << endl;
		return false;
	} else
		type = ch - '0';

	if (type != 6) {
		cerr << "Error: pnm type P" << type << " not supported yet (bitmap::read)." << endl;
		return false;
	}
	//     else
	//       cout<<"pnm type P"<<type<<", ";


	//looking for width:
	ch = nextvalidchar(f);
	if (ch == EOF) {
		fclose(f);
		cerr << "Error reading " << fname << ", header truncated before width information (bitmap::read)." << endl;
		return false;
	}
	width = 0;
	while (ch >= '0' && ch <= '9') {
		width = 10 * width + (ch - '0');
		ch = getc(f);
	}
	//    cout<<width<<" x ";


	//looking for heigth:
	ch = nextvalidchar(f);
	if (ch == EOF) {
		fclose(f);
		cerr << "Error reading " << fname << ", header truncated before heigth information (bitmap::read)." << endl;
		return false;
	}
	heigth = 0;
	while (ch >= '0' && ch <= '9') {
		heigth = 10 * heigth + (ch - '0');
		ch = getc(f);
	}

	//    cout<<heigth<<", ";


	//looking for maxcolor:
	ch = nextvalidchar(f);
	if (ch == EOF) {
		fclose(f);
		cerr << "Error reading " << fname << ", header truncated before maxcolor information (bitmap::read)." << endl;
		return false;
	}
	maxcolor = 0;
	while (ch >= '0' && ch <= '9') {
		maxcolor = 10 * maxcolor + (ch - '0');
		ch = getc(f);
	}
	//    cout<<maxcolor+1<<" colors"<<endl;


	//read in the whole line (meaningless data)
	while (ch != '\n')
		ch = getc(f);

	//read image data (depending on the image format):
	if (type == 6 && maxcolor < 256) {
		if (data)
			delete[] data;
		size_t size = width * heigth;
		data = new rgb[size];
		size_t check = fread(data, 3, size, f);
		if (check != size) {
			delete[] data;
			data = 0;
			cerr << "Error reading " << fname << ", data field truncated (bitmap::read)." << endl;
			fclose(f);
			return false;
		}

	} else
		cerr << "File type P" << type << " with " << maxcolor + 1 << " colors not supported yet (bitmap::read)" << endl;

	fclose(f);

	initMap();

	return true;
}

bool bitmap::write(const char* fname) {
	if (type != 6 || maxcolor > 255) {
		cerr << "File type P" << type << " with " << maxcolor + 1 << " colors not supported yet (bitmap::write)."
				<< endl;
		return false;
	}

	bool ret = true;

	FILE* f;
	f = fopen(fname, "w");

	if (!f) {
		cerr << "Error opening " << fname << " (bitmap::write)" << endl;
		return false;
	}

	fprintf(f, "P%d\n", type);
	fprintf(f, "%d %d\n", width, heigth);
	fprintf(f, "%d\n", maxcolor);

	size_t size = width * heigth;

	size_t check = fwrite(data, 3, size, f);

	if (check != size) {
		cerr << "Error opening " << fname << " (bitmap::write)" << endl;
		ret = false;
	}

	fflush(f);
	fclose(f);

	return ret;

}

void bitmap::removeRedChannel() {
	for (int r = 0; r < heigth; r++)
		for (int c = 0; c < width; c++)
			map[r][c].r = 0;
}

void bitmap::removeGreenChannel() {
	for (int r = 0; r < heigth; r++)
		for (int c = 0; c < width; c++)
			map[r][c].g = 0;
}

void bitmap::removeBlueChannel() {
	for (int r = 0; r < heigth; r++)
		for (int c = 0; c < width; c++)
			map[r][c].b = 0;
}

void bitmap::operator=(bitmap bmp) {
	int size = bmp.width * bmp.heigth;
	if (data)
		delete[] data;
	data = new rgb[size];
	if (!data) {
		cerr << "Memory allocation error (bitmap::operator=)." << endl;
		return;
	}
	for (int i = 0; i < size; i++)
		data[i] = bmp.data[i];
	type = bmp.type;
	width = bmp.width;
	heigth = bmp.heigth;
	maxcolor = bmp.maxcolor;
	pr = bmp.pr;
	pg = bmp.pg;
	pb = bmp.pb;

	initMap();
}

bitmap::bitmap(const bitmap& bmp) :
	type(0), data(0), maxcolor(0), pr(255), pg(0), pb(0), width(0), heigth(0), map(0) {

	int size = bmp.width * bmp.heigth;
	data = new rgb[size];
	if (!data) {
		cerr << "Memory allocation error (bitmap::operator=)." << endl;
		return;
	}
	for (int i = 0; i < size; i++)
		data[i] = bmp.data[i];
	type = bmp.type;
	width = bmp.width;
	heigth = bmp.heigth;
	maxcolor = bmp.maxcolor;
	pr = bmp.pr;
	pg = bmp.pg;
	pb = bmp.pb;

	initMap();
}
