/*

*/

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
	// Metadata
	// Sim data
	//	vector<glucoseStep> glucStep;

	vector<string> variableString;
	string varName;
	string varValue;
};

struct batchData
{
	int index;
	int repetitions;
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

int userInterface();
int queueBatch();
int getBatchIndex();
int setParams();

int main( int argc , char* argv[] )
{
	int simCount = 0;
	
	rootShellStrings root;
	vector<batchData> batches;
	
	int startingBatchIndex = getBatchIndex();

//------------------------------------------------------------------------------------------------------------------------------------------//
	
	userInterface();														// Call user interface function

//------------------------------------------------------------------------------------------------------------------------------------------//
	// Set temporary data for testing. Will be deleted on completion
	simData tempSim;
	
	batchData tempBatch;
	tempBatch.sim.push_back(tempSim);						// add temporary sim to a batch
	tempBatch.index = startingBatchIndex;					// add index to temporary batch
	tempBatch.repetitions = 3;
	batches.push_back(tempBatch); 								// add temporary batch to batch list. All batches will go in this stack
	
//------------------------------------------------------------------------------------------------------------------------------------------//	
	// Create output files for each batch
	for(int i = 0; i < batches.size(); i++)										// Iterate through batches
	{
		stringstream mkdirSS;
		mkdirSS << "mkdir ~/IsletSimBatch/data/SimBatch" << setfill('0') << setw(4) << batches[i].index;
		root.mkdir.push_back(mkdirSS.str());
		
		for(int j = 1; j <= batches[i].sim.size(); j++)						// Iterate through sims in a batch
		{	
			mkdirSS.str("");
			mkdirSS << "mkdir ~/IsletSimBatch/data/SimBatch" << setfill('0') << setw(4) << batches[i].index << "/sim" << setfill('0') << setw(4) << j;
			root.mkdir.push_back(mkdirSS.str());
			
			
						
			string repTag = "";
			for(int k = 0; k <= batches[i].repetitions; k++)
			{		
				stringstream slurmSS, chmodSS, sbatchSS;			
				slurmSS << "slurm" << simCount << ".sh";				// index slurm files
				
				chmodSS << "chmod u+x " << slurmSS.str();
				root.chmod.push_back(chmodSS.str());
				
				sbatchSS << "sbatch " << slurmSS.str();
				root.sbatch.push_back(sbatchSS.str());
		
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
				slurmFile << "#SBATCH --output=../data/SimBatch" << setfill('0') << setw(4) << batches[i].index << "/S" << j << "/slurm.out \n";
				slurmFile << "cd $PBS_O_WORKDIR \n\n";
				slurmFile << "#Execute \n\n";
				if (batches[i].repetitions > 0)
				{
					mkdirSS.str("");
					mkdirSS << "mkdir ~/IsletSimBatch/data/SimBatch" << setfill('0') << setw(4) << batches[i].index << "/sim" << setfill('0') << setw(4) << j << "/rep" << setfill('0') << setw(2) << k+1;
					root.mkdir.push_back(mkdirSS.str());
					slurmFile << "~/IsletSimBatch/exe/simulator.exe ../data/SimBatch" << setfill('0') << setw(4) <<batches[i].index << "/sim" << setfill('0') << setw(4) << j << "/rep" << setfill('0') << setw(2) << k+1;
				}
				else
				{
					slurmFile << "~/IsletSimBatch/exe/simulator.exe ../data/SimBatch" << setfill('0') << setw(4) <<batches[i].index << "/sim" << setfill('0') << setw(4) << j;
				}
				slurmFile.close();
				
				//-----------------------------------------------------------------------------------------------------------------------------//
					// Make a section here that outputs metadata for the sim and batches to an index.csv file.
					
	
				//-----------------------------------------------------------------------------------------------------------------------------//
					// Make a section here that creates an input file for each simulation.
				
				simCount++;
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
			cout << "New batch index is " << setfill('0') << setw(4) << i;
		}
	}
	
	return i;
}

int queueBatch()
{
	bool inMenu = true;
	while (inMenu)
	{
		string selectionString;
		stringstream selectionSS;
		int selectionInt;
		
		cout << "Set simulation parameters:\n";
		cout << " 1. Set custom values for Beta cell and Islet variables. \n"; 
		cout << " 2. Simulation time.\n";
		cout << " 3. Step time (dt) for linear approximation.\n";
		cout << " 4. Change randomization settings.\n";
		cout << " 5. Change number of simulations per variable combination.\n";
		cout << " 6. Save changes and return.\n";
		cout << " 7. Cancel changes and return.\n\n";
		cout << "Enter Selection:";
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
				setParams();
			}
			
			else if (selectionInt == 2)

			{
				cout << "Set the amount of time to simulate in ms (default = 800000): ";
				cin;
			}
			else if (selectionInt == 3)
			{
				cout << "Set the step time (default = 0.18 ms): ";
				cin;
			}
			else if (selectionInt == 4)
			{
				cout << "'Change randomization settings' is not set up yet. \n";
			}
			else if (selectionInt == 6)
			{
				inMenu = false;
			}
			else if (selectionInt == 7)
			{
				inMenu = false;
			}
			else
			{
				cout << "Invalid selection\n\n";
			}
		}
	}
	return 0;
}

int userInterface()
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
		cout << " 3. Run queued batches and exit.\n";
		cout << " 4. Cancel simulations and exit.\n\n";
		cout << "Enter Selection:";
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
				queueBatch();
			}
			else if (selectionInt == 2)
			{
				cout << "View or modify a queued batch:";
			}
			else if (selectionInt == 3)
			{
				
			}
			else if (selectionInt == 4)
			{
				
			}
			else
			{
				cout << "Invalid selection\n\n";
			}
		}
	}	
	return 0;
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
