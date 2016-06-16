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
		// Output filenames. Will be dynamic in the future.		
		char const* obj_timeOutput = "objOutput/time.txt";				
		char const* obj_potentialOutput = "objOutput/potential.txt";			// membrane potential	
		char const* obj_calciumOutput = "objOutput/calcium.txt";			// intracellular calcium
		char const* obj_sodiumOutput = "objOutput/sodium.txt";				// intracellular sodium
		char const* obj_potassiumOutput = "objOutput/potassium.txt";	// intracellular potassium
		char const* obj_caerOutput = "objOutput/caer.txt";						// endoplasmic reticulum calcim
		char const* obj_atpOutput = "objOutput/atp.txt";						// intracellular ATP
		char const* obj_adpOutput = "objOutput/adp.txt";						// intracellular ADP
		char const* obj_IRPOutput = "objOutput/IRP.txt"; 					 	// immediately releasable pool		x[k+22] 
		char const* obj_PPOutput = "objOutput/PP.txt";							// primed pool								x[k+23] 
		char const* obj_DPOutput = "objOutput/DP.txt";							// docked pool								x[k+24]
		char const* obj_FIPOutput = "objOutput/FIP.txt";						// fused pool (FHP in the paper)
		char const* obj_RIPOutput = "objOutput/RIP.txt";						// releasing pool (RHP in the paper)
		char const* obj_capOutput = "objOutput/cap.txt";						// ??? I don't know what this one is, it's Variable 28 in the X vector
		char const* obj_noiseOutput = "objOutput/noise.txt";					// ??? I don't know how this works, var 29 in X vector
		
		// Input filenames. Will be dynamic in the future.
		string userVarsFile;
		char const* cellPropertiesFile = "input/vars5exo.txt";
		char const* nnFile = "input/NN10A.txt";
		char const* randomVarsFile = "input/RandomVars.txt";
		char const* cellPositionFile = "input/XYZpos.txt";
	
	public:
	
		void constructPath(int, int, int);
		void ObjectiveOutputPurgeFiles();
		void ObjectiveOutputDataBlock(stringstream*);
		void set_userVarsFile(string);
		
		string get_userVarsFile();
		char const* get_cellPropertiesFile();
		char const* get_cellPositionFile();
		char const* get_nnFile();
		char const* get_randomVarsFile();
};

# endif