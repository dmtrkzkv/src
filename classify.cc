/*
 * classify.cc
 *
 *  Created on: Nov 10, 2014
 *      Author: ppsadm01
 */
using namespace std;
#include <card.h>
#include <vector>
#include <imageio.h>
#include <classify.h>
#include <dataCard.h>
#include <iterator>
#include <iostream>
#include <dirent.h>
#include <string.h>

classify::classify(vector <dataCard> data) {
	database = data;
	cout << "Classifier created." << endl;
}

int classify::numBlackTotal (bitmap bmp) {
	int numBlackTotal = 0;
	for (int i = 0; i < bmp.getHeigth(); i++) {
		for (int j = 0; j < bmp.getWidth(); j++) {
			if (bmp.map[i][j].r == 0)
				numBlackTotal++;
		}
	}
	return numBlackTotal;
}

/*
 * TODO
 * Consider situations with draws between classes of the nearest neighbors: either initialize kNN
 * with a proper value or implement oneMoreNeighbor method
 *
 */
int classify::numMatchingBlack (card query, dataCard dataImage) {

	int numMatchingBlack = 0;

	int startQueryX = query.topLeft.X();
	//cout << "startQueryX: " << startQueryX;
	int startQueryY = query.topLeft.Y();
	//cout << " startQueryY: " << startQueryY << endl;
	int endQueryX = query.bottomRight.X();
	//cout << "endQueryX: " << endQueryX;
	int endQueryY = query.bottomRight.Y();
	//cout << " endQueryY: " << endQueryY << endl;

	int startDataImageX = dataImage.topLeft.X();
	//cout << "startDataImageX: " << startDataImageX;
	int startDataImageY = dataImage.topLeft.Y();
	//cout << " startDataImageY: " << startDataImageY << endl;
	int endDataImageX = dataImage.bottomRight.X();
	//cout << "endDataImageX: " << endDataImageX;
	int endDataImageY = dataImage.bottomRight.Y();
	//cout << " endDataImageY: " << endDataImageY << endl;

	int qx = startQueryX;
	int qy = startQueryY;
	int checkedPixels = 0;


	for (int x = startDataImageX; x <= endDataImageX; x++) {
		for (int y = startDataImageY; y <= endDataImageY; y++) {
			if (qy > endQueryY) break;
			checkedPixels++;
			if (dataImage.image.map[x][y].r == query.processed.map[qx][qy].r) {
				numMatchingBlack++;
				dataImage.image.map[x][y].g = 255; // mark matching pixels with green for later visual analysis
				dataImage.image.map[x][y].r = 0;
				dataImage.image.map[x][y].b = 0;
			}
			//cout << "x: " << x << " y: " << y << endl << "qx: " << qx << " qy: " << qy << endl;
			qy++;
		}
		qy = startQueryY;
		qx++;
		if (qx > endQueryX) break;
	}

	loadImage("/scratch_net/biwidl104/pps_inderst_kazakov/data/compared/", dataImage); // save compared image (green pixels show, which match)
	query.processed.write("/scratch_net/biwidl104/pps_inderst_kazakov/data/compared/query");
	//cout << dataImage.getClass() << ": " << numMatchingBlack << endl;
	return checkedPixels - numMatchingBlack;
}

/*
 * Saves bitmap into directory
 */
void classify::loadImage(string dirName, dataCard dataImage) {
		string filename  = dataImage.name.substr(57, 8);
		dirName += filename;
		dataImage.image.write(dirName.c_str());

}

double classify::computeDistance (card query, dataCard neighbor) {
	//int distance = totalPixels - numMatchingBlack(query, neighbor);
	//int distance = (query.numBlackTotal + neighbor.numOfBlackPixels) / numMatchingBlack (query, neighbor);
	int distance = numMatchingBlack(query, neighbor);
	neighbor.setDistance(distance);
	return distance;
}


/*
 * Inserting new neighbor in the neighbors container in increasing order
 */

void classify::insertNeighbor (dataCard neighbor) {
	if (neighbors.empty()) {
		neighbors.push_front(neighbor);
		return;
	}
	list<dataCard>::iterator it;
	for (it = neighbors.begin(); it != neighbors.end(); it++) {
		if (neighbor.getDistance() >= (*it).getDistance()) continue;
		neighbors.insert(it, neighbor);
		break;
	}
}



/*
 * Determine the most common class among kNN nearest neighbors from the neighbors container
 */

char classify::getClassQuery(card query) {

	// loading all neighbors to the list
	for (int i = 0; i < database.size(); i++) {
		dataCard neighbor = database[i];
		neighbor.setDistance(computeDistance(query, neighbor));
		insertNeighbor(neighbor);
	}
	int total[13]; // array for counting number of cards of each class
	for (int i = 0; i < 13; i++) {
		total[i] = 0;
	}
	list<dataCard>::iterator it = neighbors.begin();
	/*for (int i=0; i<5; i++) {
		cout << (*it).name << endl;
		it++;
	}*/
	/*for (it = neighbors.begin(); it != neighbors.end(); it++) {
		cout << (*it).name << endl;
	}*/
	bool draw = true;
	int kIndex = kNN;
	int index;
	it = neighbors.begin();
	while(draw) {

		for (int i = 0; i < kIndex; i++) {
			char c = (*it).getClass();
			//cout << "Neighbor " << i << " is " << (*it).name << endl;
			switch (c)
			{
				case '2': total[0]++; break;
				case '3': total[1]++; break;
				case '4': total[2]++; break;
				case '5': total[3]++; break;
				case '6': total[4]++; break;
				case '7': total[5]++; break;
				case '8': total[6]++; break;
				case '9': total[7]++; break;
				case '1': total[8]++; break;
				case 'j': total[9]++; break;
				case 'q': total[10]++; break;
				case 'k': total[11]++; break;
				case 'a': total[12]++; break;
			}
			it++;
		}

		// print out the kNN neighbors counts
		for (int i = 0; i < 13; i++) {
			cout << total[i] << " ";
		}

		int max = INT_MIN;


		for (int i = 0; i < 13; i++) {
			if (total[i] == max) {
				draw = true;
			}
			else if (total[i] > max) {
				max = total[i];
				index = i;
				draw = false;
			}
		}
		if(draw) {
			kIndex=1;
			cout << " draw " << endl;
		}
		if (it == neighbors.end()) {
			cout << "The card was not recognized! Shit happens...all the time" << endl;
			return 0;
		}
	}


	char Class = classes[index];

	neighbors.clear();

	return Class;
}





