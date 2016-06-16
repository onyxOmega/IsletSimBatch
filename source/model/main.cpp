/*	Main source file for the simulation model for the Islet of Langerhans.
		
		Author: William Fischer
*/

#include "islet-file-handler.h"
#include "islet-simulator.h"
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <fstream>

using namespace std;

int main( int argc , char* argv[] )
{
	IsletFileHandlerClass fileHandler;																								// Create new File Handler object
	
	// Run validation and initial input processing
	if (argc < 1) 
	{
       // std::cerr << "Usage: " << argv[0] << " <INPUT FILE>. Use 'default' for all default values" << std::endl;
        return 1;
    }
	else
	{
		string batchString =  argv[1];
		stringstream batchSS(batchString);
		stringstream convertSS;
		string toss, keep;
		int batch = 0;
		int sim = 0;
		int rep = 0;
		
		getline(batchSS, toss, ':');
		getline(batchSS, keep, ',');
		convertSS << keep;
		convertSS >> batch;
		
		convertSS.clear();
		getline(batchSS, toss, ':');
		getline(batchSS, keep, ',');
		convertSS << keep;
		convertSS >> sim;
		
		convertSS.clear();
		
		if (getline(batchSS, toss, ':'))
		{
			getline(batchSS, keep, ',');
			convertSS << keep;
			convertSS >> rep;
		}
		cout << "Batch: " << batch << " Sim: " << sim << " Rep: " << rep << endl;
		
		fileHandler.constructPath(batch, sim, rep);
	}
	
	fileHandler.ObjectiveOutputPurgeFiles();																					// Remove old versions of output files from working output directory
	
	int objectStartTime = time(NULL);																								// Start runtime clock
	IsletSimulatorClass isletSimulator(fileHandler);																		// Create and construct new Islet Simulator object
	isletSimulator.simulationLoop();																									// Run the simulation loop
	int objectRunTime = time(NULL) - objectStartTime;																	// Stop runtime clock
	cout << "New version's total run time " << objectRunTime << " seconds." << endl;
	
	return 0;
}

