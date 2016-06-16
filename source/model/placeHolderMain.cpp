/*This program is a barebones program meant to test simultaneous scripted batching in Janus
	It will spend 2 minutes looping from initial run time and output timestamp data to the
	output file specified on the command line.
*/


#include <iostream>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip> 


using namespace std;

int main( int argc , char* argv[] )
{
	string outputPath;
	if (argc < 2) 
		{
			cerr << "Usage: " << argv[0] << " <OUTPUT FILE>" << endl;
			return 1;
		}
	else
	{
		outputPath = argv[1];
		cout << "Data output folder: " << outputPath << endl;
	}
	
	// sample code for sending to an output folder in the command line declared path
	// integrate into model once the rest of the stuff works.
	ofstream testOut;
	stringstream ss;
	ss << outputPath << "testOut.txt";
	testOut.open(ss.str(),ios::app);
	
	return 0;
}