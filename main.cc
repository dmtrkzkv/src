#include <imageio.h>
#include <stdlib.h>
#include <iostream>
#include <card.h>
//#include <vbl_timer.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <classify.h>
#include <dataCard.h>
using namespace std;
/*
 * TODO
 * Names sind noch nicht alle korrekt. Bis bevor man die neigbors pusht (insertNeighbor()) stimmt noch alles, danach nicht mehr: ein paar Karten
 * bekommen ein sehr komischen sting als name
 * Falls wir diese Karten aus dem Database nehmen, werden andere Karten das Problem sein, und wenn wir sie wiedereinfuegen sind sie es wieder,
 * aber nicht in der selbe "reihenfolge".
 * Wenn wir das ganze Programm laufen lassen wird es ausgefuehrt, wenn wir die namen ausgeben wollen wir es aber blockiert.
 *
 * */

#define DATABASE

void display(const char* timetype, long ms) {
	long s = 0, min = 0, h = 0;

	s = ms / 1000;
	ms -= 1000 * s;
	min = s / 60;
	s -= 60 * min;
	h = min / 60;
	min -= 60 * h;

	printf("%s (h:min:sec.ms) %ld:%02ld:%02ld.%03ld\n", timetype, h, min, s, ms);
}

bool anotherCard(int totalVal) {
	return totalVal < 20;
}


void createDatabase() {

	card crd;
	DIR* dirp = opendir("/scratch_net/biwidl104/pps_inderst_kazakov/data/card_images_new/");

	if (dirp == NULL)
		cout << "Could not open directory, database not created!" << endl;
	else {
		dirent* dp;
		char name[100]; // genuegend lang
		while ((dp = readdir(dirp)) != NULL){ // liest naechster Eintrag im Verzeichnis
			if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 ) { // nimmt nicht den Ornder selbst und der Vaterordner
				//cout << dp->d_name << " ; number: " <<(int)i << endl;
				strcpy (name,"/scratch_net/biwidl104/pps_inderst_kazakov/data/card_images_new/");
				strcat (name,dp->d_name);
				crd.RefreshBitmap(name);
				//cout << name << endl;
				strcpy (name,"/scratch_net/biwidl104/pps_inderst_kazakov/data/database/");
				strcat (name,dp->d_name);
				crd.Demo(name);
				//cout << name << endl << endl;
			}
		}
	}
	closedir(dirp);
}


// int main(int, const char **argv){
//   card crd;
//   int totalVal=0;
//   char a[100];
//   do
//   {
//     cout << "Enter druecken, sobald Karte bereit ist!" << endl;
//     cin.getline(a,10); 
//     crd.RefreshBitmap();

//     vbl_timer watch;
//hrtime_t start = gethrtime();

//     totalVal += crd.getValue();
//     cout << "Points: " << totalVal << endl;
//     display("Time: ",watch.real());
//hrtime_t end = gethrtime();
//cout<<"Time: "<<end-start<<" nanoseconds"<<endl;
//   } while (anotherCard( totalVal ) );
// }

//---------------demo------------------------

/*
 * TODO
 * Not to forget to include try-catch statement for the case when the card is not on the table
 * (exception is created within the detectBegin method of the card class)
 *
 */

/*
 *
 * TODO
 * Sort out what's going on with the names of the DataCards in the database when it is passed
 * as an argument to the classify constructor.
 *
 */

int main(int, const char **) {

#ifdef DATABASE
	//////////////////////////////////////////////////////
	vector<dataCard> dataset;
	DIR* dirp = opendir("/scratch_net/biwidl104/pps_inderst_kazakov/data/database/");
	if (dirp == NULL)
		cout << "ERROR: couldn't open database directory." << endl;
	else {
		dirent* dp;
		char name[100];
		while ((dp = readdir(dirp)) != NULL) {
			if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
				strcpy (name,"/scratch_net/biwidl104/pps_inderst_kazakov/data/database/");
				strcat (name,dp->d_name);
				const char* filename = (const char*) name;
				//dataCard entry(filename);
				dataCard entry(name);
				//cout << "name: "<< entry.name;
				dataset.push_back(entry);
			}
		}

	}
	cout << "Objects from database directory were successfully loaded into container." << endl;
	closedir(dirp);

	// for algorithm performance measurements
	//time_t timer;
	//double start;
	//double end;


	//

	classify classifier(dataset);

	dirp = opendir("/scratch_net/biwidl104/pps_inderst_kazakov/data/queries/");
	clock_t t;
	if (dirp == NULL)
		cout << "Could not open directory, database not created!" << endl;
	else {
		dirent* dp;
		char name[100];
		while ((dp = readdir(dirp)) != NULL){
			if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 ) {

				t = clock();
				strcpy (name,"/scratch_net/biwidl104/pps_inderst_kazakov/data/queries/");
				strcat (name,dp->d_name);
				card query(name);
				//start = time(&timer);

				char Class = classifier.getClassQuery(query);
				t = clock() - t;
				//end = time(&timer);
				cout << "name: " << name << ", Class: " << Class << ", took " << (float)t / CLOCKS_PER_SEC << " seconds." << endl;
			}
		}
	}
	closedir(dirp);




	//card query("/scratch_net/biwidl104/pps_inderst_kazakov/data/queries/s10.2.pnm");

	//cout << endl << "Class: " << classifier.getClassQuery(query) << endl;


	card crd;
	int totalVal=0;
	//classify test;

	/*while(anotherCard(totalVal)) {
		crd.RefreshBitmap();//oeffne bild
		try {
			crd.Demo(NULL);
		}
		catch (char const* msg) {
			cout << msg;
		}
		totalVal = totalVal + crd.getValue();
		cout << totalVal << endl;
	}
	cout << "Got: " << totalVal << endl;*/




#else

		cout << "Database not defined yet" << endl;

		createDatabase();

		cout <<  "Database created!" << endl;

#endif

	}
