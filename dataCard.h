/*
 * dataCard.h
 *
 *  Created on: Nov 10, 2014
 *      Author: Dmitry Kazakov
 */


#ifndef DATACARD_H_
#define DATACARD_H_
#include <card.h>
#include <string>
using namespace std;

class dataCard {
private:
	//const char* name; // name according to the filename
	char Class; // Classes: 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A
	int distanceToQuery;


public:
	string name; // name according to the filename
	bitmap image;
	int numOfBlackPixels;
	point2d topLeft;
	point2d bottomRight;

private:
	void detectRankEdges();

public:
	dataCard(string fname);
	dataCard(const dataCard& data);
	dataCard(): name(0), Class(0), distanceToQuery(0), numOfBlackPixels(0), topLeft(0,0), bottomRight(0,0){};
	~dataCard(){};
	void resetDistance();
	void resetDistance(vector<dataCard> dataBase);
	int numBlackTotal (bitmap bmp);
	string getName();
	void setClass (char c);
	char getClass();
	void setDistance(int distance);
	int getDistance ();

};

#endif /* DATACARD_H_ */
