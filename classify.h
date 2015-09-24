/*
 * classify.h

 *
 *  Created on: Nov 10, 2014
 *      Author: ppsadm01
 */

using namespace std;
#include <card.h>
#include <vector>
#include <imageio.h>
#include <list>
#include <climits>
#include <dataCard.h>
#include <string>

#ifndef CLASSIFY_H_
#define CLASSIFY_H_
using namespace std;


const char classes [] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'}; // list of classes
const int kNN = 4;
const int totalPixels = 11500;

class classify {
public:
	vector<dataCard> database; // it is more handy to have database right in the classify object. Thus we need
							   // to instantiate a classify object only once in the main function.

private:
	list<dataCard> neighbors; // for each new card on the table (query object) a data object from database is loaded into this container
							  // after performing classification algorithm. All data objects are inserted in a sorted order
							  // correspondingly to their distances from the query object.

	int numBlackTotal (bitmap bmp);
	int numMatchingBlack (card query, dataCard dataImage); 	   // query is of type card. It should contain
	double computeDistance (card query, dataCard neighbor);    // a processed image of the card (i.e. rotated,
	void loadData();									       // scaled, cut, binarized, with known rank edges)
	void insertNeighbor (dataCard neighbor);
	void loadImage(string dirName, dataCard dataImage);


public:
	classify(){};
	classify(vector<dataCard> data);
	virtual ~classify(){};
	char getClassQuery(card query);

};

#endif /* CLASSIFY_H_ */
