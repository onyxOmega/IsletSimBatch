

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip> 
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

struct glucoseStep
{	
	double time;
	double level;
};

struct simData
{
	vector<string> variableString;
};

struct batchData
{
	bool valid;
	int index;
	int repetitions;
	string wallTime;
	vector<simData> sim;
};

struct rootShellStrings
{
	vector<string> mkdir;
	vector<string> chmod;
	vector<string> sbatch;
};

struct allowedVariables
{
	string name, unit, min, max, def;
};

// Set constants
int const SIM_RATE = 1400; 																				// Approximate number of time steps per second the simulation can caluclate. Update periodically with sim tests. This is used to set wall-times in slurm control files.
int const defaultSimTime = 200000;
double const defaultStepTime = .14;

// Declare custom functions
bool userInterface();
batchData queueBatch();																					// queueBatch() function returns custom data type "batchData"
int getBatchIndex();
int setParams();
void displayBatchData(batchData);

// Set global vector of batchData structures
vector<batchData> batch;

int main( int argc , char* argv[] )
{
	int simCount = 0;
	rootShellStrings root;
	int startingBatchIndex = getBatchIndex();

//------------------------------------------------------------------------------------------------------------------------------------------//
	
	bool confirmBatches = userInterface();															// Call user interface function
	
	if (!confirmBatches)
	{
		return 0; 																									// Cancels program and closes
	}
	
//------------------------------------------------------------------------------------------------------------------------------------------//
	// Set temporary data for testing. Will be deleted on completion or while testing manual entries.
	
	for (int bI = 0; bI < 3; bI++)
	{
		batchData tempBatch;
		stringstream tempSS;

		for (int sI = 0; sI < 2; sI++)
		{
			simData tempSim;
			tempSS.str("");
			tempSS << "abc" << sI << "=" << bI*sI;
			tempSim.variableString.push_back(tempSS.str());							// add test variable string to the test sim.
			tempSS.str("");
			tempSS << "def" << sI << "=" << bI + sI;
			tempSim.variableString.push_back(tempSS.str());							// add test variable string to the test sim.
			tempBatch.sim.push_back(tempSim);													// add temporary sim to a batch
		}

		tempBatch.index = startingBatchIndex + bI;											// add index to temporary batch
		tempBatch.repetitions = 3 - bI;
		batch.push_back(tempBatch); 																// add temporary batch to batch list. All batches will go in this stack
	}	
	
	
//------------------------------------------------------------------------------------------------------------------------------------------//	
	// Create root_shell.sh file for all batches(which makes relevant directories then runs sbatch files) 
	// and create slurm batch files for each individual sim run
	for(int i = 0; i < batch.size(); i++)										// Iterate through batches
	{
		stringstream mkdirSS;
		mkdirSS << "mkdir ~/IsletSimBatch/data/SimBatch" << setfill('0') << setw(4) << batch[i].index;
		root.mkdir.push_back(mkdirSS.str());
		
		for(int j = 0; j < batch[i].sim.size(); j++)						// Iterate through sims in a batch
		{	
			// Create a parent directory for a given simulation set.
			mkdirSS.str("");
			mkdirSS << "mkdir ~/IsletSimBatch/data/SimBatch" << setfill('0') << setw(4) << batch[i].index << "/sim" << setfill('0') << setw(4) << j+1;
			root.mkdir.push_back(mkdirSS.str());
			string repTag = "";
			
			// Iterate through all the desired repetitions in a simulation set, create the folders, and populate root.vector<string> variables with paths and stuff.
			for(int k = 0; k < batch[i].repetitions; k++)
			{		
				// Add a line to change this run's slurm file permissions to the root.chmod vector<string>. Contents of this vector are output to root_shell.sh later.
				// 
				stringstream slurmSS, chmodSS, sbatchSS;			
				slurmSS << "slurm" << simCount << ".sh";
				chmodSS << "chmod u+x " << slurmSS.str();
				root.chmod.push_back(chmodSS.str());
				
				// Add a line to run this slurm file
				sbatchSS << "sbatch " << slurmSS.str();
				root.sbatch.push_back(sbatchSS.str());
		
				// Create slurm file and output the lines that are constant across all jobs in the batch
				ofstream slurmFile;														
				slurmFile.open(slurmSS.str());
				slurmFile << "#!/bin/bash \n";										
				slurmFile << "#Setting the name of the job \n";
				slurmFile << "#PBS -N IsletSimObjectiveTest \n";
				slurmFile << "#Setting a walltime for the job \n";
				slurmFile << "#PBS -l walltime=0:05:00 \n";
				slurmFile << "#Selecting processors \n";
				slurmFile << "#PBS -l nodes=1:ppn=12 \n";
				slurmFile << "#SBATCH --reservation=janus-serial \n";
				
				// iterate through repetitions of each sim. Create a new directory, and output repetition dependent lines to slurm files
				if (batch[i].repetitions > 1)										
				{
					mkdirSS.str("");
					mkdirSS << "mkdir ~/IsletSimBatch/data/SimBatch" << setfill('0') << setw(4) << batch[i].index << "/sim" << setfill('0') << setw(4) << j+1 << "/rep" << setfill('0') << setw(2) << k+1;
					root.mkdir.push_back(mkdirSS.str());
					
					slurmFile << "#SBATCH --output=../data/SimBatch" << setfill('0') << setw(4) << batch[i].index << "/sim" << setfill('0') << setw(4) << j+1 << "/slurmRep" << setfill('0') << setw(2) << k+1 <<".out \n";
					slurmFile << "cd $PBS_O_WORKDIR \n\n";
					slurmFile << "#Execute \n\n";
					
					// This line sets runtime parameters for the simulator.exe file. Batch#, Sim #, and Rep# are used by the simulator to build the strings for where data is coming from and going to.
					slurmFile << "~/IsletSimBatch/exe/simulator.exe Batch:" << batch[i].index << " Sim:" << j+1 << " Rep:" << k+1;  
				}
				else
				{
					slurmFile << "#SBATCH --output=../data/SimBatch" << setfill('0') << setw(4) << batch[i].index << "/sim" << setfill('0') << setw(4) << j+1 << "/slurm.out \n";
					slurmFile << "cd $PBS_O_WORKDIR \n\n";
					slurmFile << "#Execute \n\n";
					// This line sets the runtime parameters if there are no repetitions set. In this case, the data is output to the same directory as the inputVarialbes.txt file
					slurmFile << "~/IsletSimBatch/exe/simulator.exe Batch:" <<batch[i].index << " Sim:" << j+1;  
				}
				slurmFile.close();
				//---------------------------------------------------------------------------------------------------------------------------------//
					// Make a section here that outputs metadata for the sim and batches to an index.csv file.
					
				simCount++;
			}
			//-----------------------------------------------------------------------------------------------------------------------------------//
				// Create simulationVars.txt file with the custum parameters for the simulation set.
			
			stringstream simVarsSS;
			simVarsSS << "../input/Batch" << setfill('0') << setw(4) <<batch[i].index << "_Sim" << setfill('0') << setw(4) <<  j+1 << "Vars.txt";
			
			ofstream simVarsFile;
			simVarsFile.open(simVarsSS.str());
			
			// Iterate through the variable strings saved for a given simulation set. Output strings to the file
			for(int l = 0; l < batch[i].sim[j].variableString.size(); l++)
			{
				simVarsFile << batch[i].sim[j].variableString[l] << endl;
			}
			
		}
	}
	
//------------------------------------------------------------------------------------------------------------------------------------------//
	// This section is functional. Creates full "root_shell.sh" file from string vectors
	ofstream rootShell;
	rootShell.open("root_shell.sh");
	
	rootShell << "#!/bin/bash \n";

	for(int i = 0; i < root.mkdir.size(); i++)
	{
		rootShell << root.mkdir[i] << endl;
	}
	
	for(int i = 0; i < root.chmod.size(); i++)
	{
		rootShell << root.chmod[i] << endl;
	}
	
	for(int i = 0; i < root.sbatch.size(); i++)
	{
		rootShell << root.sbatch[i] << endl;
	}
	
	rootShell << "rm slurm* \n";
	rootShell.close();
	
	return 0;
}

//--------------------------------------------------------------Functions-------------------------------------------------------------------//	

int getBatchIndex()
{	
	cout << "Batch Indexer test \n";

	struct stat info;
	bool batchExists = true;
	int i;
	
	for (i = 1; batchExists; i++)
	{
		stringstream pathSS;
		pathSS << "../data/SimBatch" << setfill('0') << setw(4) << i;
		string pathString;
		pathSS >> pathString;
		const char* batchChars = pathString.c_str();
		
		if(stat(batchChars, &info ) != 0)
		{
			batchExists = false;
		}
		else if(info.st_mode & S_IFDIR )
		{
			batchExists = true;
		}
		else
		{
			batchExists = false;
		}
	}
	return (i - 1);
}

batchData queueBatch()					// queueBatch function returns custum data structure type "batchData"
{
	batchData newBatch;
	newBatch.valid = true;
	
	bool inMenu = true;
	while (inMenu)
	{
		string selectionString;
		stringstream selectionSS;
		int selectionInt;
		
		// set default parameters for simulations.
		int simTime = defaultSimTime;
		double stepTime = defaultStepTime;
		int repetitions = 1;
		int totalSims = 1;
		
		cout << "Set simulation parameters (Skip for defaults):\n";
		cout << " 1. Custom values for Beta cell and Islet variables.\n"; 
		cout << " 2. Set Simulation time. Default: " << defaultSimTime << " ms.\n";
		cout << " 3. Step time (dt) for linear approximation. Default: " << defaultStepTime << " ms.\n";
		cout << " 4. Change randomization settings. Default: Time based random seed. \n";
		cout << " 5. Change number of simulations per variable combination. Default: 1. \n";
		cout << " 6. Save changes and return.\n";
		cout << " 7. Cancel changes and return.\n\n";
		cout << "Enter Selection: ";
		cin >> selectionString;
		cout << endl;
		selectionSS << selectionString;
		selectionSS >> selectionInt;
		if (selectionSS.fail())
		{
			cout << "Invalid selection\n\n";
		}
		else
		{
			// Set and validate parameters for new simulation batch.
			if (selectionInt == 1)
			{
				setParams();
			}
			
			// Set and validate time for new sim batch.
			else if (selectionInt == 2)
			{
				string runTimeString;
				stringstream runTimeSS;		
				int runTimeInt;
				cout << "Set the amount of time to simulate in ms (default = " << defaultSimTime <<" ms): ";
				bool valid = false;
				while(!valid)
				{
					cin >> runTimeString;
					cout << endl;
					runTimeSS << runTimeString;
					runTimeSS >> runTimeInt;
					
					if (runTimeSS.fail())
					{
						cout << "Invalid data type for entry. Please enter an integer greater than 500: ";
					}
					else
					{
						if (runTimeInt < 500)
						{
							cout << "Invalid simulation time. Please enter a value greater than 500 ms: ";
						}
						else
						{
							simTime = runTimeInt;
							cout << "Simulation time set to " << simTime << " ms.\n\n";
							valid = true;
						}
					}
				}
			}
			
			// Set and validate dt value for new sim batch.
			else if (selectionInt == 3)
			{
				string stepString;
				stringstream stepSS;
				double stepDouble;
				cout << "Set the step time (default = " << defaultStepTime << " ms): ";
				bool valid = false;
				while(!valid)
				{
					cin >> stepString;
					stepSS << stepString;
					stepSS >> stepDouble;
					
					if (stepSS.fail())
					{
						cout << "Invalid data type for entry. Please enter a real number between 0.01 and 0.5: ";
					}
					else
					{
						if (stepDouble < .01 || stepDouble > 0.5)
						{
							cout << "Invalid step time. Please enter a value between 0.01 and 0.5 ms: ";
						}
						else
						{
							stepTime = stepDouble;
							cout << "Simulation time set to " << setprecision(2) << stepTime << " ms.\n\n";
							valid = true;
						}
					}
				}
			}
			else if (selectionInt == 4)
			{
				cout << "'Change randomization settings' is not set up yet. \n";
			}
			else if (selectionInt == 6)
			{
				int timeSteps = int(simTime/stepTime);
			
				int wallTime = timeSteps/(SIM_RATE);			// est seconds of wall time. Need to convert to hours, minutes, seconds.
				double totalWallTime = wallTime * repetitions;
				
				cout << "Actual wall time: " << wallTime << endl;
				// Add a buffer to wallTime to make sure enough is reserved
				wallTime = int(wallTime*1.2);
				cout << "Reserved wall time: " << wallTime << endl;
				
				int wallSeconds = wallTime%60;
				int wallMinutes = ((wallTime - wallSeconds)%3600)/60;
				int wallHours = wallTime/3600;
				stringstream wallTimeSS("");
				string wallTimeString;
				wallTimeSS << setfill('0') << setw(2) << wallHours << ":" << setfill('0') << setw(2) << wallMinutes << ":" << setfill('0') << setw(2) << wallSeconds << endl;
				wallTimeSS >> wallTimeString;
				
				newBatch.wallTime = wallTimeString;
				cout << "Batch created:\n";
				displayBatchData(newBatch);
				inMenu = false;
			}
			else if (selectionInt == 7)
			{
				inMenu = false;
				
				newBatch.valid = false;
				return newBatch;
			}
			else
			{
				cout << "Invalid selection\n\n";
			}
		}
	}
	return newBatch;
}

void displayBatchData(batchData batch)
{
	
}

bool userInterface()
{
	cout << endl << "     Islet of Langerhans Beta Cell Simulator, V2.0\n";
	cout << "Authors: Matt Westacott, Aleena Notary, William Fischer \n\n";
	
	bool inMenu = true;
	
	while (inMenu)
	{
		string selectionString;
		stringstream selectionSS;
		int selectionInt;
		
		cout << "Please select from the following options:\n";
		cout << " 1. Queue a new simulation batch.\n";
		cout << " 2. View or modify queued batches.\n";
		cout << " 3. Cancel simulations and exit.\n";
		cout << " 4. Run queued batches and exit.\n\n";
		cout << "Enter Selection: ";
		cin >> selectionString;
		cout << endl;
		selectionSS << selectionString;
		selectionSS >> selectionInt;
		if (selectionSS.fail())
		{
			cout << "Invalid selection\n\n";
		}
		else
		{
			if (selectionInt == 1)
			{
				batchData newBatch = queueBatch();
				if(newBatch.valid)
				{
					batch.push_back(newBatch);
				}
				else
				{
					cout << "Batch Cancelled\n\n";
				}
			}
			else if (selectionInt == 2)
			{
				cout << "View or modify a queued batch:";
			}
			else if (selectionInt == 3)
			{
				return false;
			}
			else if (selectionInt == 4)
			{
				return true;
			}
			else
			{
				cout << "Invalid selection\n\n";
			}
		}
	}	
}

int setParams()
{
	vector<allowedVariables> okVars;
	
	ifstream varCheckFile;
	varCheckFile.open("../data/.var_check.txt");
	
	string inputString;
	getline(varCheckFile, inputString); 	// dumps first line, column header
	
	while (!varCheckFile.eof())
	{	allowedVariables tempStruct;
		varCheckFile >> tempStruct.name;
		varCheckFile >> tempStruct.min;
		varCheckFile >> tempStruct.max;
		varCheckFile >> tempStruct.def;
		varCheckFile >> tempStruct.unit;
		okVars.push_back(tempStruct);
	}
	
	int colWidth = 11;
	int numWidth = 4;
	
	cout << "Choose from the following parameters: \n\n";
	cout << setw(colWidth) << "Var Name";
	cout << setw(colWidth) << "Min Value";
	cout << setw(colWidth) << "Max Value";
	cout << setw(colWidth) << "Default";
	cout << setw(colWidth) << "Units" << endl;
	
	for(int i = 0; i < okVars.size(); i++)
	{
		cout << setw(numWidth - 2) << i + 1 << ": ";
		cout << setw(colWidth - numWidth) << okVars[i].name;
		cout << setw(colWidth) << okVars[i].min;
		cout << setw(colWidth) << okVars[i].max;
		cout << setw(colWidth) << okVars[i].def; 
		cout << setw(colWidth) << okVars[i].unit << endl; 
	}
	cout << " q: cancel selection and exit menu.\n\n";
	cout << "Enter the number for the variable you want to change: "; 
	
	bool valid;	
	int selectionInt;

	do
	{
		string selectionString;
		stringstream selectionSS;

		cin >> selectionString;
		selectionSS << selectionString;
		selectionSS >> selectionInt;
			
		if (selectionSS.fail())
		{
			if (selectionString == "q" || selectionString == "Q")
			{
				valid = true;
				return 0;
			}
			else
			{
				cout << "Invalid selection, please try again: ";
				valid = false;
			}
		}
		else
		{
			if (selectionInt <= okVars.size() && selectionInt > 0)
			{
				cout << "Valid Selection!\n";
				valid = true;
			}
			else
			{
				cout << "Invalid selection, please try again: ";
			}
		}
	} while (!valid);
	
	cout << "Enter a list of '" << okVars[selectionInt-1].name << "' values to run, separated by commas: \n";
	
	// Next: Parse out 
	
	do
	{
		string selectionString;
		stringstream selectionSS;
		int selectionInt;

		cin >> selectionString;
		selectionSS << selectionString;
		selectionSS >> selectionInt;
	} while (!valid);
	
}
