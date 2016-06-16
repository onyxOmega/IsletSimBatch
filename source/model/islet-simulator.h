/* Constructor for IsletSimulator custom class.

	This class stores islet specific data (including an array of Beta
	Cell objects within the simulated islet), and calculates property
	changes over time using a euler first order linar approximation on
	a series of interdependant ODEs.
	
	Authors: Matt Wescott, William Fischer
*/

#ifndef NUM_CORES
#define NUM_CORES 2
#endif

#ifndef ISLETSIMULATOR_H
#define ISLETSIMULATOR_H



#include "islet-file-handler.h"
#include "islet-data-structures.h"


#include <string>
#include <vector>


using namespace std;

class IsletSimulatorClass
{
	private:
		//int const NUM_CORES = 12;						// set to 2 for local runs, 12 for Janus to optimize parallel processing. (local system probably has 4 cores, but not enough memory to use all of them for this)
	
		IsletFileHandlerClass fileHandler;
		
		string userVarMatrix[2][10];					// User defined variable matrix
		
		double seed;												// randomization seed. Defaults to time stamp based seed without user input.
		double runTime;											// simulation time in miliseconds
		double stepTime;										// time step interval for linear approximations
		int outInterval = 100;								// time interval between data outputs
		int outBufferSize = 20;								// number of output rows stored in a buffer before sending it to a file
		
		// other variables
		double Glucose;
		int cellNumber;
		double Icoup;
		
		// Beta Cell vector
		IsletStructure islet;
		vector<BetaCellStructure> betaCells;
		
		// Islet simulation constants
		const double R = 8.3143;
		const double Tem = 310.15;
		const double F = 96.4867;
		const double RTF = R*Tem/F;
		const double RTF2 = RTF/2;
		const double yini0 = -69.8663703359279;
		const double yini1 = 7.92502913389466;
		const double yini2 = 125.248586232226;
		const double yini3 = 7.56138347594955E-05;
		const double yini4 = 0.0047417301517704;
		
		// Buffers an output data string array
		stringstream dataOutputStream[14];
		
	public:
		IsletSimulatorClass(IsletFileHandlerClass);					// Class constructor
		void setDefaultVars();
		void setInitialBetaCellVars();
		void setUserDefinedVars();
		void simulationLoop();
		void setNearestNeighbors();
};

#endif
