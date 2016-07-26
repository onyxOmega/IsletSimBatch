/* Custom Class: IsletFileHandler

	Handles input and output files used in the Islet of Langerhans 
	computer model.
	
	Authors: William Fischer, Matt Wescott
*/

#ifndef ISLETFILEHANDLER_H
#define ISLETFILEHANDLER_H

#include "islet-data-structures.h"

#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

class IsletFileHandlerClass
{
	private:		
		string	inpath, outpath,  timeOut, potentialOut, calciumOut, sodiumOut, potassiumOut, caerOut, 
					atpOut, adpOut, IRPOut, PPOut, DPOut, FIPOut, RIPOut, capOut, noiseOut, infoOut;
	
		string userVarsFile;
		char const* cellPropertiesFile = "../input/islet/vars5exo.txt";
		char const* nnFile = "../input/islet/NN10A.txt";
		//char const* randomVarsFile = "../input/RandomVars.txt";
		char const* cellPositionFile = "../input/islet/XYZpos.txt";
	
	public:
	
		void constructPath(int, int, int);
		void ObjectiveOutputPurgeFiles();
		void ObjectiveOutputDataBlock(stringstream*);
		void set_userVarsFile(string);
		void updateStatus(double);
		void finishedStatus
		
		string get_userVarsFile();
		char const* get_cellPropertiesFile();
		char const* get_cellPositionFile();
		char const* get_nnFile();
};

# endif