using namespace std;
#include <card.h>
#include <stdlib.h>
#include <line.h>
#include <math.h>
#include <iostream>
#include <limits.h>
#include <exception>
#include <vector>
#include <imageio.h>

double const pi = 3.14159265359;
//bitmap rotatedBmp(6, 115, 100, 255);

//card::card(int type, int heigth, int wide, int maxcolor) : bitmap(type, heigth, wide, maxcolor)


int card::totalBlackPixels() {
	int height = processed.getHeigth();
	int width = processed.getWidth();
	int counter = 0;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (processed.map[x][y].r == 0) counter++;
		}
	}

	return counter;
}
card::card(const char* s) {
	bmp.read(s);
	processed = processBitmap(bmp);
	detectRankEdges();
	numBlackTotal = totalBlackPixels();
}

void card::RefreshBitmap() { //oeffnet bild
	// uncomment when you want to use a new snapshot from the camera instead of using the same image over and over ...
	// system("/home/ppsadm/NewAndBetter/requestframe.sh"); // image will be copied to ./frame.pnm

	bmp.read("/scratch_net/biwidl104/pps_inderst_kazakov/data/card_images_new/h6.1.pnm");
	//bmp.read("captured.pnm");
}

void card::RefreshBitmap(const char* s) {

	bmp.read(s);
}


int card::getValue(void) {


	return 5;
}

bool card::table(point2d p, int redthreshold) { //false falls der punkt p nicht weiss und im Bild ist, sonst true
	if (p.X() >= 0 && p.X() < bmp.getHeigth() && p.Y() >= 0 && p.Y() < bmp.getWidth()) {
		if (bmp.map[p.X()][p.Y()].r >= redthreshold)
			return false;
		else
			return true;
	} else
		return true;
}

point2d* card::getBoundaryPoints(point2d firstPoint, int threshold, float distance, int& num) { //speichert im array result alle punkte, die an der grenze der karte liegen
	const int maxsize = 200;
	const double pi = 3.14159265;
	const double delta_angle = 5.0 / 180.0 * pi;//5.0 degree
	num = 0;
	point2d* result = new point2d[maxsize];

	//get out from the table:
	double angle = pi / 2.0;//90 degree
	while (angle < 10.0 && !table(point2d(firstPoint, angle, distance), threshold))//waehle den angle sodass es zur karte zeigt
		angle += (pi / 2.0);

	point2d center;
	point2d nextPoint = firstPoint;

	do {
		//advance:
		center = nextPoint;

		//store center:
		result[num] = center;
		num++;
		//find next point:
		//move outside if necessary:
		while (!table(point2d(center, angle, distance), threshold))//geht weiter solange es gruen ist
			angle -= delta_angle;
		//move inside if necessary:
		while (table(point2d(center, angle, distance), threshold))//geht weiter solange es weiss ist
			angle += delta_angle;
		nextPoint = point2d(center, angle, distance);
	} while (firstPoint.distance(nextPoint) > 0.75 * distance);

	return result;
}

//origin ist der Ecke der am obersten ist (und vor der lange Kante)
point2d card::getOrigin(point2d its1, point2d its2, point2d its3, point2d its4) {

	if(its1.distance(its2) > its2.distance(its3)){
		if(its1.x < its3.x) {
			return its1;
		}
		else if(its1.x == its3.x){
			if(its1.y < its3.y)
				return its1;
			else
				return its3;
		}
		else {
			return its3;
		}
	}
	else {
		if(its2.x < its4.x)
			return its2;
		else if(its2.x == its4.x) {
			if(its2.y < its4.y)
				return its2;
			else
				return its4;
		}
		else
			return its4;

	}
}
/*
 *  This function computes angle at the origin
 */
double card::getAngle(point2d origin, point2d its1, point2d its2, point2d its3, point2d its4) {
	point2d ref;

	if(origin == its1)
		ref = its4;
	else if(origin == its2)
		ref = its1;
	else if(origin == its3)
		ref = its2;
	else if(origin == its4)
		ref = its3;


	double dx = ref.x - origin.x;
	double dy = ref.y - origin.y;

		if(dy > 0) {

			double delta=dx/dy;

			return atan(delta);

		}

		else {

			double delta = (-dy/dx);
			return atan(delta) + pi/2;

		}

}

/*
 * This function uses the average angle value, delivered by the averageAngle function, to
 * transform it into an average angle that will be passed to the rotateBmp function.
 * This angle can take negative values and values > pi/2
 */

double card::getAngle(double avAngle, point2d origin, point2d its1, point2d its2, point2d its3, point2d its4) {
	point2d ref;

	if(origin == its1)
		ref = its4;
	else if(origin == its2)
		ref = its1;
	else if(origin == its3)
		ref = its2;
	else if(origin == its4)
		ref = its3;


	double dx = ref.x - origin.x;
	double dy = ref.y - origin.y;

		if(dy > 0) {
			if (dx >= 0)
				return avAngle;
			else
				return -avAngle;

		}

		else {
			if (dx >= 0)
				return pi - avAngle;
			else
				return avAngle - pi;

		}

}

/*
 * Average angle computes the angles adjacent to all four sides of the card and thus
 * increases the precision of angle determination. The angle delivered is always <= pi/2
 * and >= 0.
 */

double card::averageAngle(point2d its1, point2d its2, point2d its3, point2d its4) {
	double dx, dy;
	double tangens1, tangens2, tangens3, tangens4;
		if (its1.distance(its4) < its1.distance(its2)) {
			dx = abs(its1.x - its4.x);
			dy = abs(its1.y - its4.y);
			tangens1 = dx/dy;
			dx = abs(its1.x - its2.x);
			dy = abs(its1.y - its2.y);
			tangens2 = dy/dx;
			dx = abs(its2.x - its3.x);
			dy = abs(its2.y - its3.y);
			tangens3 = dx/dy;
			dx = abs(its4.x - its3.x);
			dy = abs(its4.y - its3.y);
			tangens4 = dy/dx;
		}
		else {
			dx = abs(its1.x - its4.x);
			dy = abs(its1.y - its4.y);
			tangens1 = dy/dx;
			dx = abs(its1.x - its2.x);
			dy = abs(its1.y - its2.y);
			tangens2 = dx/dy;
			dx = abs(its2.x - its3.x);
			dy = abs(its2.y - its3.y);
			tangens3 = dy/dx;
			dx = abs(its4.x - its3.x);
			dy = abs(its4.y - its3.y);
			tangens4 = dx/dy;
		}

	return atan((tangens1 + tangens2 + tangens3 + tangens4)/4);
}



double card::getLength(point2d its,point2d its2,point2d its4){ //gibt die laenge der kuerzere seite zurueck
	if(its.distance(its2) > its.distance(its4))
		return its.distance(its4);
	else
		return its.distance(its2);

}

/*
 * topPoint is the point with smallest x-value (i.e. the "highest" point). Not to confuse with the
 * origin!
 */
point2d card::getTopPoint(point2d its1, point2d its2, point2d its3, point2d its4) { //DIMA
	point2d points [3] = {its2, its3, its4};
	int minX = its1.X();
	point2d newOrigin = its1;
	for (int i = 0; i <= 2; i++) {
		if (points[i].X() < minX) {
			newOrigin = points[i];
			minX = points[i].X();
		}
	}
	return newOrigin;
}

/*
 * Returns true if the card should be rotated counterclockwise. False otherwise.
 * At the time being the function is not used anywhere.
 */
bool card::rotateMathPositive (point2d origin, point2d its1, point2d its2, point2d its3, point2d its4) {//DIMA
	if (origin == its1) {
		if (its1.distance(its4) < its1.distance(its2))
			return true;
		else return false;
	}
	else if (origin == its2) {
			if (its2.distance(its1) < its1.distance(its3))
				return true;
			else return false;
		}
	else if (origin == its3) {
			if (its3.distance(its2) < its3.distance(its4))
				return true;
			else return false;
		}
	else {
			if (its4.distance(its3) < its4.distance(its1))
				return true;
			else return false;
		}
}

void card::rotateBmp(bitmap* newBmp, point2d origin,double angle,double length){

	int width = newBmp->getWidth();
	int height = newBmp->getHeigth();

	//double scale = length/width;
	double scale = length/(5 * width);

	  for (int i = 0; i < height; i++) {
	     for (int j = 0; j < width; j++) { //i und j sind Koordinaten im newBmp

	    	int _i = (int) (scale * (cos(angle) * i + sin(angle) * j) + origin.x);//entsprechende Punkte im alten bmp
	    	int _j = (int) (scale * (-sin(angle) * i + cos(angle) * j) + origin.y);

	    	newBmp->map[i][j].r=bmp.map[_i][_j].r;
	    	newBmp->map[i][j].g=bmp.map[_i][_j].g;
	    	newBmp->map[i][j].b=bmp.map[_i][_j].b;
	  		}
	  }

	  //cout << "scale: " << scale << " origin: " << origin.x << ";" << origin.y << endl;

}

void card::blackWhite(bitmap* bmp){
	int width = bmp->getWidth();
	int heigth = bmp->getHeigth();

	int thresholdGreen=128;
	//int thresholdRed = 90;

	for (int i = 0; i < heigth; i++) {
		     for (int j = 0; j < width; j++) {
		    	 if(bmp->map[i][j].g > thresholdGreen){//wir haben weiss oder gruen, konvertieren es in weiss
		    		 bmp->map[i][j].r=255;
		    		 bmp->map[i][j].g=255;
		    		 bmp->map[i][j].b=255;

		    	 }
		    	 else {
		    		 bmp->map[i][j].r=0;
					 bmp->map[i][j].g=0;
					 bmp->map[i][j].b=0;
		    	 }
		     }
	}


	//"Ecke-Vorbereitung": color top left black angle on the card into white

	for (int i = 0; i < heigth; i++) {
		if (bmp->map[i][0].r == 255) break;
		for(int j = 0; j < width; j++) {
			if (bmp->map[i][j].r == 255) break;
			bmp->map[i][j].r = 255;
			bmp->map[i][j].g = 255;
			bmp->map[i][j].b = 255;
		}

	}
	// paint the first n left columns into white
	for(int j = 0; j<15; j++) {
		for (int k = 0; k < heigth; k++) {
			bmp->map[k][j].r = 255;
			bmp->map[k][j].g = 255;
			bmp->map[k][j].b = 255;

			bmp->map[k][width-1-j].r = 255;
			bmp->map[k][width-j-1].g = 255;
			bmp->map[k][width-j-1].b = 255;
		}
	}
	// paint the first n top rows into white
	for(int j = 0; j<20; j++) {
		for (int k = 0; k < width; k++) {
				bmp->map[j][k].r = 255;
				bmp->map[j][k].g = 255;
				bmp->map[j][k].b = 255;
		}
	}
}
/*
 * Searches for the card on the table
 */
point2d card::detectBegin(int frame, bitmap bmp, point2d begin) {

	int stepOffset = 60;
	int direction = -1;
	int nextStep = 0;
	int i;

	while (begin.Y() >= 0 && begin.Y() <= bmp.getWidth()) {
		i = frame;
		while (i < bmp.getHeigth() - frame && bmp.map[i][begin.Y()].r < 128)
			i++;
		if (i < bmp.getHeigth() - frame) {
			begin.setX(i);
			return begin;
		}
		else {
			nextStep = direction * (abs(nextStep) + stepOffset);
			begin.setY(begin.Y() + nextStep);
			direction = -direction;
		}
	}
	if (begin.Y() == 0 || begin.Y() == bmp.getWidth()) throw ("There is no card on the table.");

	return begin;
}

void card::detectRankEdges() {
	bool edgeDetected = false;
	int X, Y;
	const int width = processed.getWidth();
	const int height = processed.getHeigth();
	point2d startLine;
	point2d endLine;

	// top edge
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (processed.map[x][y].r == 0) {
				edgeDetected = true;
				Y = y;
				break;
			}
		}
		if (!edgeDetected) continue;
		X = x - 1;
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
			if (processed.map[x][y].r == 0) {
				edgeDetected = true;
				Y = y;
				break;
			}
		}
		if (!edgeDetected) continue;
		X = x + 1;
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
			if (processed.map[x][y].r == 0) {
				edgeDetected = true;
				X = x;
				break;
			}
		}
		if (!edgeDetected) continue;
		Y = y - 1;
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
			if (processed.map[x][y].r == 0) {
				edgeDetected = true;
				X = x;
				break;
			}
		}
		if (!edgeDetected) continue;
		Y = y + 1;
		break;
	}
	if (Y >= width) Y = width - 1;
	startLine.setCoordinates(0, Y);
	endLine.setCoordinates(height, Y);
	line rightEdge(startLine, endLine);

	topLeft = topEdge.intersection(leftEdge);
	bottomRight = bottomEdge.intersection(rightEdge);
}


void card::markRankEdges (bitmap* bmp) {
	bool edgeDetected = false;
	int X, Y;
	const int width = bmp->getWidth();
	const int height = bmp->getHeigth();
	// top edge
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (bmp->map[x][y].r == 0) {
				edgeDetected = true;
				Y = y;
				break;
			}
		}
		if (!edgeDetected) continue;
		X = x - 1;
		break;
	}
	point2d topStartLine(X, 0);
	point2d topEndLine(X, width);
	bmp->setPen(255,0,0);
	//bmp->line(topStartLine.X(), topStartLine.Y(), topEndLine.X(), topEndLine.Y());

	// bottom edge
	edgeDetected = false;
	X = 0;
	Y = 0;
	for (int x = height - 1; x > 0; x--) {
		for (int y = 0; y < width; y++) {
			if (bmp->map[x][y].r == 0) {
				edgeDetected = true;
				Y = y;
				break;
			}
		}
		if (!edgeDetected) continue;
		X = x + 1;
		break;
	}
	point2d bottomStartLine(X, 0);
	point2d bottomEndLine(X, width);
	//bmp->line(bottomStartLine.X(), bottomStartLine.Y(), bottomEndLine.X(), bottomEndLine.Y());

	// left edge
	X = 0;
	Y = 0;
	edgeDetected = false;
	for (int y = 0; y < width; y++) {
		for (int x = 0; x < height; x++) {
			if (bmp->map[x][y].r == 0) {
				edgeDetected = true;
				X = x;
				break;
			}
		}
		if (!edgeDetected) continue;
		Y = y;
		break;
	}
	point2d leftStartLine(0, Y);
	point2d leftEndLine(height, Y);
	//bmp->line(leftStartLine.X(), leftStartLine.Y(), leftEndLine.X(), leftEndLine.Y());

	// right edge
	X = 0;
	Y = 0;
	edgeDetected = false;
	for (int y = width - 1; y >= 0; y--) {
		for (int x = 0; x < height; x++) {
			if (bmp->map[x][y].r == 0) {
				edgeDetected = true;
				X = x;
				break;
			}
		}
		if (!edgeDetected) continue;
		Y = y;
		break;
	}
	point2d rightStartLine(0, Y);
	point2d rightEndLine(height, Y);
	//bmp->line(rightStartLine.X(), rightStartLine.Y(), rightEndLine.X(), rightEndLine.Y());
	line top(topStartLine, topEndLine);
	line bottom(bottomStartLine, bottomEndLine);
	line left(leftStartLine, leftEndLine);
	line right(rightStartLine, rightEndLine);
	point2d topLeft = top.intersection(left);
	point2d topRight = top.intersection(right);
	point2d bottomLeft = bottom.intersection(left);
	point2d bottomRight = bottom.intersection(right);
	bmp->line(topLeft.X(), topLeft.Y(), topRight.X(), topRight.Y());
	bmp->line(topLeft.X(), topLeft.Y(), bottomLeft.X(), bottomLeft.Y());
	bmp->line(topRight.X(), topRight.Y(), bottomRight.X(), bottomRight.Y());
	bmp->line(bottomLeft.X(), bottomLeft.Y(), bottomRight.X(), bottomRight.Y());
}

bitmap card::processBitmap(bitmap bmp) {
	const int frame = 5;

	point2d startDetect(0, bmp.getWidth() / 2);
	point2d begin = detectBegin(frame, bmp, startDetect);


	int boundarypointnum;
	point2d* boundary = getBoundaryPoints(begin, 128, 20, boundarypointnum);

	/////////////////////
	int it = 0;
	while (!line::in_line(boundary[it], boundary[it + 2], boundary[it + 4]))//geht weiter bis die 3 punkte auf der selbe linie sind
		it++;
	line first_line(boundary[it], boundary[it + 4]);
	it = it + 4;

	while (first_line.distance(boundary[it]) < 20)//geht weiter solange der punkt auf der linie ist
		it++;
	while (!line::in_line(boundary[it], boundary[it + 2], boundary[it + 4]))
		it++;
	line second_line(boundary[it], boundary[it + 4]);

	point2d its1 = first_line.intersection(second_line);//punkt zw erste und zweite linie

	while (second_line.distance(boundary[it]) < 20)//geht weiter solange der punkt auf der linie ist
		it++;
	while (!line::in_line(boundary[it], boundary[it + 2], boundary[it + 4]))
		it++;
	line third_line(boundary[it], boundary[it + 4]);

	point2d its2 = second_line.intersection(third_line);//punkt zw zweite und dritte linie

	while (third_line.distance(boundary[it]) < 20)//geht weiter solange der punkt auf der linie ist
		it++;
	while (!line::in_line(boundary[it], boundary[it + 2], boundary[it + 4]))
		it++;
	line fourth_line(boundary[it], boundary[it + 4]);

	point2d its3 = third_line.intersection(fourth_line);//punkt zw dritte und vierte linie

	point2d its4 = first_line.intersection(fourth_line);//punkt zw erste und vierte linie

	//Nun haben wir die 4 Ecken (its, its2, its3 und its4) sowie die 4 linien (first_line, second_line,...)

	/////////////////////

	point2d origin = getOrigin(its1,its2,its3,its4);
	//double angle = getAngle(origin,its1,its2,its3,its4);
	double avAngle = averageAngle(its1, its2, its3, its4);
	double angle = getAngle(avAngle, origin, its1, its2, its3, its4);
	double length = getLength(its1, its2, its4);

	//bitmap* newBmp = new bitmap(6, 115, 100, 255); //mit dem modifizierten Scale in rotateBmp bekommt man nun nur den Wert
	bitmap rotatedBmp1(6, 115, 100, 255);
	rotateBmp(&rotatedBmp1,origin,angle,length);
	blackWhite(&rotatedBmp1);
	markRankEdges(&rotatedBmp1);
	rotatedBmp = rotatedBmp1;
	return rotatedBmp;
}

void card::Demo(const char* name) {
	const int frame = 5;

	point2d startDetect(0, bmp.getWidth() / 2);
	point2d begin = detectBegin(frame, bmp, startDetect);


	int boundarypointnum;
	point2d* boundary = getBoundaryPoints(begin, 128, 20, boundarypointnum);

	/////////////////////
	int it = 0;
		while (!line::in_line(boundary[it], boundary[it + 2], boundary[it + 4]))//geht weiter bis die 3 punkte auf der selbe linie sind
			it++;
		line first_line(boundary[it], boundary[it + 4]);
		it = it + 4;

		while (first_line.distance(boundary[it]) < 20)//geht weiter solange der punkt auf der linie ist
			it++;
		while (!line::in_line(boundary[it], boundary[it + 2], boundary[it + 4]))
			it++;
		line second_line(boundary[it], boundary[it + 4]);

		point2d its1 = first_line.intersection(second_line);//punkt zw erste und zweite linie

		while (second_line.distance(boundary[it]) < 20)//geht weiter solange der punkt auf der linie ist
			it++;
		while (!line::in_line(boundary[it], boundary[it + 2], boundary[it + 4]))
			it++;
		line third_line(boundary[it], boundary[it + 4]);

		point2d its2 = second_line.intersection(third_line);//punkt zw zweite und dritte linie

		while (third_line.distance(boundary[it]) < 20)//geht weiter solange der punkt auf der linie ist
			it++;
		while (!line::in_line(boundary[it], boundary[it + 2], boundary[it + 4]))
			it++;
		line fourth_line(boundary[it], boundary[it + 4]);

		point2d its3 = third_line.intersection(fourth_line);//punkt zw dritte und vierte linie

		point2d its4 = first_line.intersection(fourth_line);//punkt zw erste und vierte linie

		//Nun haben wir die 4 Ecken (its, its2, its3 und its4) sowie die 4 linien (first_line, second_line,...)

		/////////////////////

		point2d origin = getOrigin(its1,its2,its3,its4);
		//double angle = getAngle(origin,its1,its2,its3,its4);
		double avAngle = averageAngle(its1, its2, its3, its4);
		double angle = getAngle(avAngle, origin, its1, its2, its3, its4);
		double length = getLength(its1, its2, its4);

		//bitmap* newBmp = new bitmap(6, 115, 100, 255); //mit dem modifizierten Scale in rotateBmp bekommt man nun nur den Wert
		bitmap rotatedBmp1(6, 115, 100, 255);
		rotateBmp(&rotatedBmp1,origin,angle,length);
		blackWhite(&rotatedBmp1);
		markRankEdges(&rotatedBmp1);
		rotatedBmp = rotatedBmp1;
		processed = rotatedBmp;

		if(name==NULL) {
			rotatedBmp.write("./newBmp.pnm");
			//system("eog ./newBmp.pnm");
		}
		else
			rotatedBmp.write(name);



	/////////////////////////////

#ifdef DISPLAY

	bmp.setPen(255,255,0);
	for (int i = 0; i < boundarypointnum; i++)
		bmp.dot(boundary[i].X(), boundary[i].Y());

	//////////////
	bmp.setPen(255,0,0);
	bmp.dot(its1.X(), its1.Y());

	bmp.setPen(0,255,0);
	bmp.line(first_line.p1.X(), first_line.p1.Y(), first_line.p2.X(), first_line.p2.Y());
	bmp.setPen(0,0,255);
	bmp.line(second_line.p1.X(), second_line.p1.Y(), second_line.p2.X(), second_line.p2.Y());
	////////////////////////////////////


	//bmp.write("home/zsemlye/Images/copy.pnm");
	//bmp.write("./copy.pnm");


#endif




}
