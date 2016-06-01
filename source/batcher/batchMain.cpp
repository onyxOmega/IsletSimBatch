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

//---------------------------------------------------------------------------------------------------------------------------------//
// Stores stepped glucose data. Not implemented, low priority
struct glucoseStep
{	
	double time;
	double level;
};

//---------------------------------------------------------------------------------------------------------------------------------//
// Each "variableString" in the vector will contain one string with the variable and it's value for the given sim. This is passed
// into the input files for the actual simulations when they're run.
// Example:
// sim[#].variableString[0] = "stepTime=.16;"
struct simData
{
	vector<string> variableString;
};

//---------------------------------------------------------------------------------------------------------------------------------//
// Stores the list of values for a single variable
struct varPermutations	
{
	string name;
	vector<double> value; 
};

//---------------------------------------------------------------------------------------------------------------------------------//
struct batchData
{
	bool valid;
	int index;
	int repetitions;
	double stepTime;
	int simTime;
	string wallTime;
	vector<simData> sim;
	
	// Matrix containing all the permutations for every parameter in a batch.
	vector<varPermutations> paramMatrix;	
};

//---------------------------------------------------------------------------------------------------------------------------------//
struct rootShellStrings
{
	vector<string> mkdir;
	vector<string> chmod;
	vector<string> sbatch;
};

//---------------------------------------------------------------------------------------------------------------------------------//
struct allowedVariables
{
	string name, unit; 
	double min, max, def;
};

//----------------------------------------------------------Constants-------------------------------------------------------------//
// SIM_RATE is the number of time steps performed per second
int const SIM_RATE = 1400; 																			 
int const defaultSimTime = 200000;
double const defaultStepTime = .16;

//---------------------------------------------------------------------------------------------------------------------------------//
// Declare custom functions
bool userInterface();
int getBatchIndex();
void displayBatchData();
void changeBatchData(batchData);
void makeFiles();
void makeTestBatch(int);
batchData queueBatch();																					
varPermutations setParams();

//---------------------------------------------------------------------------------------------------------------------------------//
// Set global vector of batchData structures
vector<batchData> batches;

//-----------------------------------------------------------Main funtion----------------------------------------------------------//
int main( int argc , char* argv[] )
{
	int startingBatchIndex = getBatchIndex();
	makeTestBatch(startingBatchIndex);
	
	// Call user interface function. Returns true if the user chooses to confirm their selections and the data is valid
	bool confirmBatches = userInterface();														
	
	// Cancels program and closes if the user doesn't confirm running the set simulations.
	if (!confirmBatches)
	{
		return 0; 																									
	}
	

	makeFiles();

	return 0;
}

//------------------------------------------------------------Functions------------------------------------------------------------//
// Find out what the current batch index is by counting through previously created batches	
int getBatchIndex()
{	
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

//---------------------------------------------------------------------------------------------------------------------------------//
// Create a new simulation batch
batchData queueBatch()					
{
	batchData newBatch;
	newBatch.valid = true;
	newBatch.simTime = defaultSimTime;
	newBatch.stepTime = defaultStepTime;
	newBatch.repetitions = 1;
	int totalSims = 1;
	bool inMenu = true;
	
	while (inMenu)
	{
		string selectionString;
		stringstream selectionSS;
		int selectionInt;
				
		cout << "Set simulation parameters (Skip for defaults):\n";
		cout << " 1. Add custom values for a Beta cell or Islet variable.\n"; 
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
				varPermutations newVariable = setParams();
				if (newVariable.value.size() > 0)
				{
					newBatch.paramMatrix.push_back(newVariable);
				}
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
							newBatch.simTime = runTimeInt;
							cout << "Simulation time set to " << newBatch.simTime << " ms.\n\n";
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
							newBatch.stepTime = stepDouble;
							cout << "Simulation time set to " << setprecision(2) << newBatch.stepTime << " ms.\n\n";
							valid = true;
							
							cout << newBatch.stepTime;
						}
					}
				}
			}
			else if (selectionInt == 4)
			{
				cout << "'Change randomization settings' is not set up yet. \n";
			}
			
			//	Fill in empty values with defaults, then commit changes
			else if (selectionInt == 6)
			{		
				cout << "Sim Time = " << newBatch.simTime << ". Step time = " << newBatch.stepTime << endl;
				int timeSteps = int(newBatch.simTime/newBatch.stepTime);
				
				// est seconds of wall time. Need to convert to hours, minutes, seconds.
				int wallTime = timeSteps/(SIM_RATE);			
				double totalWallTime = wallTime * newBatch.repetitions;
				
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

				cout << "Batch created... \n\n";
				inMenu = false;	
				return newBatch;
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
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Show details about queued batches
void displayBatchData()
{
	for(int i = 0; i < batches.size(); i++)
	{
		cout << "Batch " << i + 1 << endl;
		cout << "  Simulation time: " << batches[i].simTime << endl;
		cout << "  Step time: " << batches[i].stepTime << endl;
		cout << "  Number of repetitions of each simulation: " << batches[i].repetitions << endl;
		cout << "  Total number of simulations: ... \n";
		cout << "  Estimated wall time per simulation: ... \n";
		cout << "  Estimated total wall time: ...\n";
		if (batches[i].paramMatrix.size() == 0)
		{
			cout << "  No custom parameters set. Using simulation defaults.\n\n";
		}
		for (int j = 0; j < batches[i].paramMatrix.size(); j++)
		{
			if(j == 0)
			{
				cout << "  Custom parameters to run...\n";
			}
			cout <<"    " << batches[i].paramMatrix[j].name << " = ";
			for (int k = 0; k < batches[i].paramMatrix[j].value.size(); k++)
			{
				cout << batches[i].paramMatrix[j].value[k];
				if (k < batches[i].paramMatrix[j].value.size() - 1)
				{
					cout << ", ";
				}
				else
				{
					cout << ";\n";
				}
			}
			
			if(j == batches[i].paramMatrix.size()-1)
			{
				cout << endl;
			}
		}
	}
	cout << endl;
	cout << "Enter a batch to edit (q to cancel): ";
	bool valid = false;
	
	while (!valid)
	{
		string selectionString;
		stringstream selectionSS;
		int selectionInt;
		cin >> selectionString;
		cout << endl;
		selectionSS << selectionString;
		selectionSS >> selectionInt;
		if (selectionSS.fail())
		{
			if(selectionString == "q")
			{
				return;
			}
			else
			{
				cout << "Invalid input. Please enter the batch number for the batch you would like to edit, or enter q to cancel: ";
			}
		}
		else
		{
			if(selectionInt > 0 && selectionInt <= batches.size())
			{
				changeBatchData(batches[selectionInt-1]);
				valid = true;
			}
			else
			{
				cout << "Invalid selection. Please enter the batch number for the batch you would like to edit, or enter q to cancel: ";
			}	
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Modify details about queued batches
void changeBatchData(batchData batch)
{
	cout << "Change batch data... \n\n";
	displayBatchData();
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Run top level of user interface	
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
					batches.push_back(newBatch);
				}
				else
				{
					cout << "Batch Cancelled\n\n";
				}
			}
			else if (selectionInt == 2)
			{
				displayBatchData();
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

//---------------------------------------------------------------------------------------------------------------------------------//
// Add a list of values for a single variable to a batch.
varPermutations setParams()
{
	vector<allowedVariables> okVars;
	varPermutations newVariable;
	
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
	
	int colWidth = 14;
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
		cout << setw(colWidth) << fixed << setprecision(2) << okVars[i].min;
		cout << setw(colWidth) << fixed << setprecision(2) << okVars[i].max;
		cout << setw(colWidth) << fixed << setprecision(2) << okVars[i].def; 
		cout << setw(colWidth) << okVars[i].unit << endl; 
	}
	cout << " q: cancel selection and exit menu.\n\n";
	cout << "Enter the number for the variable you want to change: "; 
	
	int selectionInt;
	bool valid = false;
		
	while (valid == false)
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
				return newVariable;
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
	}	
	
	cout << "Enter a list of up to 15 '" << okVars[selectionInt-1].name << "' values to run, separated by spaces (only valid values will be used): \n";
	newVariable.name = okVars[selectionInt-1].name;
	
	vector<double> valuesVector;
	string valuesString;
	stringstream valuesSS;
	stringstream bufferSS;
	double tempValue;
	cin.ignore();							// removes \n frim the input buffer so the getline reads the next user input.
	getline(cin, valuesString);
	
	cout << "valuesString is " << valuesString << endl;
	cout << "selectionInt " << selectionInt << endl;
	valuesSS << valuesString;
	
	while (valuesSS)
	{
		valuesSS >> tempValue;
		if (!valuesSS.fail())	// if casting to double succeeds
		{
			if (tempValue <= okVars[selectionInt-1].max &&  tempValue >= okVars[selectionInt-1].min)
			{
				cout << tempValue << " ";
				newVariable.value.push_back(tempValue);
			}
		}
	}	
	cout << endl;
	
	sort(newVariable.value.begin(), newVariable.value.end());
	
	cout << newVariable.name << " = ";
	for (int i = 0; i < newVariable.value.size(); i++)
	{
		cout << newVariable.value[i] << ", ";
	}
	
	cout << endl;
	
	return newVariable;
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Use data from the "batches" vector to make the necessary files to run the queue.
void makeFiles()
{
	int simCount = 0;
	rootShellStrings root;
	
	// Iterate through batches
	for(int i = 0; i < batches.size(); i++)										
	{
		stringstream mkdirSS;
		mkdirSS << "mkdir ~/IsletSimBatch/data/SimBatch" << setfill('0') << setw(4) << batches[i].index;
		root.mkdir.push_back(mkdirSS.str());
		
		// Iterate through sims in a batch
		for(int j = 0; j < batches[i].sim.size(); j++)						
		{	
			// Create a parent directory for a given simulation set.
			mkdirSS.str("");
			mkdirSS << "mkdir ~/IsletSimBatch/data/SimBatch" << setfill('0') << setw(4) << batches[i].index << "/sim" << setfill('0') << setw(4) << j+1;
			root.mkdir.push_back(mkdirSS.str());
			string repTag = "";
			
			// Iterate through all the desired repetitions in a simulation set, create the folders, and populate root.vector<string> variables with paths and stuff.
			for(int k = 0; k < batches[i].repetitions; k++)
			{		
				// Add a line to change this run's slurm file permissions to the root.chmod vector<string>. Contents of this vector are output to root_shell.sh later.
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
				if (batches[i].repetitions > 1)										
				{
					mkdirSS.str("");
					mkdirSS << "mkdir ~/IsletSimBatch/data/SimBatch" << setfill('0') << setw(4) << batches[i].index << "/sim" << setfill('0') << setw(4) << j+1 << "/rep" << setfill('0') << setw(2) << k+1;
					root.mkdir.push_back(mkdirSS.str());
					
					slurmFile << "#SBATCH --output=../data/SimBatch" << setfill('0') << setw(4) << batches[i].index << "/sim" << setfill('0') << setw(4) << j+1 << "/slurmRep" << setfill('0') << setw(2) << k+1 <<".out \n";
					slurmFile << "cd $PBS_O_WORKDIR \n\n";
					slurmFile << "#Execute \n\n";
					
					// This line sets runtime parameters for the simulator.exe file. Batch#, Sim #, and Rep# are used by the simulator to build the strings for where data is coming from and going to.
					slurmFile << "~/IsletSimBatch/exe/simulator.exe Batch:" << batches[i].index << " Sim:" << j+1 << " Rep:" << k+1;  
				}
				else
				{
					slurmFile << "#SBATCH --output=../data/SimBatch" << setfill('0') << setw(4) << batches[i].index << "/sim" << setfill('0') << setw(4) << j+1 << "/slurm.out \n";
					slurmFile << "cd $PBS_O_WORKDIR \n\n";
					slurmFile << "#Execute \n\n";
					// This line sets the runtime parameters if there are no repetitions set. In this case, the data is output to the same directory as the inputVarialbes.txt file
					slurmFile << "~/IsletSimBatch/exe/simulator.exe Batch:" <<batches[i].index << " Sim:" << j+1;  
				}
				slurmFile.close();
				
				/* ###Make a section here that outputs metadata for the sim and batches to an index.csv file### */
					
				simCount++;
			}
			
			// Create simulationVars.txt file with the custum parameters for the simulation set.
			stringstream simVarsSS;
			simVarsSS << "../input/Batch" << setfill('0') << setw(4) <<batches[i].index << "_Sim" << setfill('0') << setw(4) <<  j+1 << "Vars.txt";
			
			ofstream simVarsFile;
			simVarsFile.open(simVarsSS.str());
			
			// Iterate through the variable strings saved for a given simulation set. Output strings to the file
			for(int l = 0; l < batches[i].sim[j].variableString.size(); l++)
			{
				simVarsFile << batches[i].sim[j].variableString[l] << endl;
			}
			
		}
	}
	
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
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Create temporary batch data for testing;
void makeTestBatch(int startingBatchIndex)
{
	for (int bI = 0; bI < 3; bI++)
	{
		batchData tempBatch;
		stringstream tempSS;

		for (int sI = 0; sI < 2; sI++)
		{
			simData tempSim;
			tempSS.str("");
			tempSS << "abc" << sI << "=" << bI*sI;
			
			// add test variable string to the test sim.
			tempSim.variableString.push_back(tempSS.str());							
			tempSS.str("");
			tempSS << "def" << sI << "=" << bI + sI;
			
			// add test variable string to the test sim.
			tempSim.variableString.push_back(tempSS.str());		
			
			// add temporary sim to a batch			
			tempBatch.sim.push_back(tempSim);													
		}

		// add index to temporary batch
		tempBatch.stepTime = .1;
		tempBatch.simTime = bI * 10;
		tempBatch.index = startingBatchIndex + bI;											
		tempBatch.repetitions = 3 - bI;
		
		// add temporary batch to batch list. All batches will go in this stack
		batches.push_back(tempBatch); 																
	}	
}