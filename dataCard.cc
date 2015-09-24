/*
 * dataCard.cc
 *
 *  Created on: Nov 10, 2014
 *      Author: ppsadm01
 */

#include <dataCard.h>
#include <limits.h>
#include <iostream>
#include <line.h>
using namespace std;

/*
 * A constructor that assigns a bitmap and Class (rank of the card, i.e. second character in the filename)
 * from the corresponding image in the database directory
 */

dataCard::dataCard(string fname) {
	image.read(fname.c_str());
	numOfBlackPixels = numBlackTotal(image);
	distanceToQuery = 0;
	name = fname;
	Class = fname[57];
	detectRankEdges();
}

dataCard::dataCard(const dataCard& rhs){
	numOfBlackPixels = rhs.numOfBlackPixels;
	name = rhs.name;
	Class = rhs.Class;
	distanceToQuery = rhs.distanceToQuery;
	image = rhs.image;
	topLeft = rhs.topLeft;
	bottomRight = rhs.bottomRight;
}

/*
 * Counts total number of black pixels in the bitmap
 */

int dataCard::numBlackTotal (bitmap bmp) {
	int numBlackTotal = 0;
	for (int i = 0; i < bmp.getHeigth(); i++) {
		for (int j = 0; j < bmp.getWidth(); j++) {
			if (bmp.map[i][j].r == 0)
				numBlackTotal++;
		}
	}
	return numBlackTotal;

}

void dataCard::detectRankEdges() {
	bool edgeDetected = false;
	int X, Y;
	int width = image.getWidth();
	int height = image.getHeigth();
	point2d startLine;
	point2d endLine;

	// top edge
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (image.map[x][y].r == 0) {
				edgeDetected = true;
				Y = y;
				break;
			}
		}
		if (!edgeDetected) continue;
		X = x;
		break;
	}
	if (X < 0) X = 0;
	startLine.setCoordinates(X, 0);
	endLine.setCoordinates(X, width);
	line topEdge(startLine, endLine);

	edgeDetected = false;
	X = 0; Y = 0;

	//bottom edge

	for (int x = height - 1; x > 0; x--) {
		for (int y = 0; y < width; y++) {
			if (image.map[x][y].r == 0) {
				edgeDetected = true;
				Y = y;
				break;
			}
		}
		if (!edgeDetected) continue;
		X = x;
		break;
	}
	if (X >= height) X = height - 1;
	startLine.setCoordinates(X, 0);
	endLine.setCoordinates(X, width);
	line bottomEdge(startLine, endLine);

	edgeDetected = false;
	X = 0; Y = 0;

	// left edge

	for (int y = 0; y < width; y++) {
		for (int x = 0; x < height; x++) {
			if (image.map[x][y].r == 0) {
				edgeDetected = true;
				X = x;
				break;
			}
		}
		if (!edgeDetected) continue;
		Y = y;
		break;
	}
	if (Y < 0) Y = 0;
	startLine.setCoordinates(0, Y);
	endLine.setCoordinates(height, Y);
	line leftEdge(startLine, endLine);

	edgeDetected = false;
	X = 0; Y = 0;

	// right edge

	for (int y = width - 1; y >= 0; y--) {
		for (int x = 0; x < height; x++) {
			if (image.map[x][y].r == 0) {
				edgeDetected = true;
				X = x;
				break;
			}
		}
		if (!edgeDetected) continue;
		Y = y;
		break;
	}
	if (Y >= width) Y = width - 1;
	startLine.setCoordinates(0, Y);
	endLine.setCoordinates(height, Y);
	line rightEdge(startLine, endLine);

	topLeft = topEdge.intersection(leftEdge);
	bottomRight = bottomEdge.intersection(rightEdge);
}

string dataCard::getName() {
	return name;
}

void dataCard::setClass(char c) {
	Class = c;
}

char dataCard::getClass() {
	return Class;
}

void dataCard::setDistance(int distance) {
	distanceToQuery = distance;
}

int dataCard::getDistance () {
	return distanceToQuery;
}

void dataCard::resetDistance () {
	distanceToQuery = INT_MAX;
}


