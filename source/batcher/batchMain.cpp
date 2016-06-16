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

struct timeStamp
{
	int hour, min, sec;
	string str;
};
//---------------------------------------------------------------------------------------------------------------------------------//
struct batchData
{
	bool valid;
	// queue is used to index the number of batches queued in a given run, while index is for overall sorting of all
	// the batches ever run.
	int index, queue;
	int repetitions, simCount;
	double stepTime;
	int simTime;
	timeStamp wallTime, totalWallTime;
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
timeStamp getWallTime(int);
batchData updateTimestamp(batchData);
batchData setupSims(batchData);
batchData queueBatch(batchData);	
varPermutations setParams();	
void checkEOR(int, vector<varPermutations>);
void listBatches();
void displayBatchData(batchData);
void changeBatchData(batchData);
void makeFiles();
void makeTestBatch();
void updateConfig();

void pressEnter();
bool confirm(string, string, string, string);
int getValidInt(int, int, string, string);
																			


//---------------------------------------------------------------------------------------------------------------------------------//
// Set globals
vector<batchData> batches;
vector<int> place;
int batchIndex;
int tempCount;


//-----------------------------------------------------------Main funtion----------------------------------------------------------//
int main( int argc , char* argv[] )
{
	place.reserve(20);
	
	batchIndex = getBatchIndex();
	
	//makeTestBatch();
	
	// Call user interface function. Returns true if the user chooses to confirm their selections and the data is valid
	bool confirmBatches = userInterface();														
	
	// Cancels program and closes if the user doesn't confirm running the set simulations.
	if (!confirmBatches)
	{
		return 0; 																									
	}
	

	makeFiles();
	updateConfig();
	
	return 0;
}

//------------------------------------------------------------Functions-----------------------------------------------------------//
// Find out what the current batch index is by counting through previously created batches	
int getBatchIndex()
{	
	ifstream configFile;
	configFile.open("../conf/.config.ini");
	
	int index;
	stringstream convertSS;
	string toss, keep;
	getline(configFile, toss, '=');
	getline(configFile, keep, ';');
	convertSS << keep;
	convertSS >> index;
	configFile.close();
	return index;
}

void updateConfig()
{
	ofstream configFile;
	configFile.open("../conf/.config.ini");
	cout << "closing index = " <<  batchIndex << endl;
	configFile << "lastIndex=" << batchIndex << ";" << endl;
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Create a new simulation batch
batchData queueBatch(batchData newBatch)					
{
	// Set standard parameters for a new batch that hasn't been previously validated. This step is skipped
	// when editing existing batches.
	if (!newBatch.valid)
	{
		newBatch.valid = true;
		newBatch.queue = batches.size() + 1;
		batchIndex++;
		newBatch.index = batchIndex;		
		newBatch.simTime = defaultSimTime;
		newBatch.stepTime = defaultStepTime;
		newBatch.repetitions = 1;
		int totalSims = 1;
	}
	
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
		cout << " 6. View current batch configuration.\n";
		cout << " 7. Save changes and return.\n";
		cout << " 8. Cancel changes and return.\n\n";
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
				bool varIsUnique = true;
				varPermutations newParameterSet = setParams();
				
				/* Testing some stuff 
				cout << "new set name: " << newParameterSet.name << endl;
				cout << "set names already listed: ";
				for(int i = 0; i < newBatch.paramMatrix.size(); i++)
				{
					cout << newBatch.paramMatrix[i].name << ", ";
				}
				cout << endl;
				*/
				
				if (newParameterSet.value.size() > 0)
				{
					for(int i = 0; i < newBatch.paramMatrix.size(); i++)
					{
						if (newBatch.paramMatrix[i].name == newParameterSet.name)
						{
							varIsUnique = false;
							cout << "You have already specified values for this variable. Would you like to ..." << endl;
							cout << "  1: Add values to the set." << endl;
							cout << "  2: Replace the set with new values." << endl;
							cout << "  3: Cancel and go back." << endl;
							cout << "  4: Delete parameter set and go back." << endl;
							cout << endl << "Enter Selection: ";
							
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
									cout << "Invalid selection\n\n";
								}
								else
								{
									if (selectionInt == 1)
									{
										valid = true;
										for (int j = 0; j < newParameterSet.value.size(); j++)
										{
											newBatch.paramMatrix[i].value.push_back(newParameterSet.value[j]);
										}
										sort(newBatch.paramMatrix[i].value.begin(), newBatch.paramMatrix[i].value.end());
										newBatch.paramMatrix[i].value.erase( unique( newBatch.paramMatrix[i].value.begin(), newBatch.paramMatrix[i].value.end() ), newBatch.paramMatrix[i].value.end() );
										cout << "The batch will run the following values for " << newParameterSet.name << ": ";
										for (int k = 0; k < newBatch.paramMatrix[i].value.size(); k++)
										{
											cout << newBatch.paramMatrix[i].value[k];
											if (k != newBatch.paramMatrix[i].value.size()-1)
											{
												cout << ", ";
											}
										}
										cout << ";\n\n";
									}
									else if (selectionInt == 2)
									{
										valid = true;
										newBatch.paramMatrix[i] = newParameterSet;
										cout << "The batch will run the following values for " << newParameterSet.name << ": ";
										for (int k = 0; k < newBatch.paramMatrix[i].value.size(); k++)
										{
											cout << newBatch.paramMatrix[i].value[k];
											if (k != newBatch.paramMatrix[i].value.size()-1)
											{
												cout << ", ";
											}
										}
										cout << ";\n\n";
									}
									else if (selectionInt == 3)
									{
										valid = true;
										cout << "Operation cancelled. \n";
										cout << "The batch will run the following values for " << newParameterSet.name << ": ";
										for (int k = 0; k < newBatch.paramMatrix[i].value.size(); k++)
										{
											cout << newBatch.paramMatrix[i].value[k];
											if (k != newBatch.paramMatrix[i].value.size()-1)
											{
												cout << ", ";
											}
										}
										cout << ";\n\n";
									}
									else if (selectionInt == 4)
									{
										valid = true;
										newBatch.paramMatrix.erase(newBatch.paramMatrix.begin()+i);
									}
									else
									{
										cout << "Invalid selection. Please enter an integer from 1 to 4: ";
									}
								}
							}						
						}
					}
					
					if(varIsUnique)
					{
						newBatch.paramMatrix.push_back(newParameterSet);
						cout << "The batch will run the following values for " << newParameterSet.name << ": ";
						for (int i = 0; i < newParameterSet.value.size(); i++)
						{
							cout << newParameterSet.value[i];
							if (i != newParameterSet.value.size() -1)
							{
								cout << ", ";
							}
						}
						cout << ";\n\n";
					}
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
			
			else if (selectionInt == 5)
			{
				cout << "Enter the number of repetitions to perform for each simulation (max of 100): ";
				bool valid = false;
				while (!valid)
				{
					string repsString;
					stringstream repsSS;
					int repsInt;
					cin >> repsString;
					repsSS << repsString;
					repsSS >> repsInt;
					if (repsSS.fail())
					{
						cout << "Invalid selection\n\n";
					}
					else
					{
						if (repsInt > 0 && repsInt <= 100)
						{
							valid = true;
							newBatch.repetitions = repsInt;
						}
						else
						{
							cout << "Invalid selection. Please enter an integer from 1 to 100: "; 
						}
					}
				}
			}
						
			else if (selectionInt == 6)
			{
				newBatch = setupSims(newBatch);
				newBatch = updateTimestamp(newBatch);
				displayBatchData(newBatch);
				pressEnter();
			}
			
			//	Fill in empty values with defaults, then commit changes
			else if (selectionInt == 7)
			{
				newBatch = setupSims(newBatch);
				newBatch = updateTimestamp(newBatch);
				cout << "Queuing the following batch: \n";
				displayBatchData(newBatch);
				pressEnter();
				inMenu = false;	
				return newBatch;
			}
			else if (selectionInt == 8)
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
// Iterate through all variable combinations and make a sim for each
batchData setupSims(batchData currentBatch)
{
	int simCount = 1;
	currentBatch.sim.clear();
	place.clear();
	
	for (int i = 0; i < currentBatch.paramMatrix.size(); i++)
	{
		place.push_back(0);
		simCount = simCount * currentBatch.paramMatrix[i].value.size();
	}
		
	// Iterate through each combination of variables
	for (int j = 0; j < simCount; j++)
	{	
		simData newSim;
		
		// Create the variable list for a single sim, add each line to the simData.variableString vector
		for (int i = 0; i < place.size(); i++)
		{
			// Create a sim value vector for a distinct combination
			stringstream simSS;
			string simString;
			simSS << currentBatch.paramMatrix[i].name << "=" << currentBatch.paramMatrix[i].value[place[i]] << ";";
			simSS >> simString;
			newSim.variableString.push_back(simString);
			
			// Start the iteration process at the end of the place vector
			if (i == place.size() - 1)
			{
				checkEOR(i, currentBatch.paramMatrix);
			}
			
		
		}	
		// Add simTime, stepTime, etc, for a single sim
		stringstream simSS;
		string simString;
		simSS << "stepTime=" << currentBatch.stepTime << ";";
		simSS >> simString;
		newSim.variableString.push_back(simString);
		
		simSS.str("");
		simString = "";
		simSS << "simTime=" << currentBatch.simTime << ";";
		simSS >> simString;
		newSim.variableString.push_back(simString);
		
		// Adds the list of variables for each unique simulation to the batch's sim vector
		currentBatch.sim.push_back(newSim);
	}
	
	currentBatch.simCount = currentBatch.sim.size() * currentBatch.repetitions;
	return currentBatch;
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Recursive function to see if the paramMatrix.value vector is at the end of a given row. Iterates to the next step either way.
void checkEOR(int i, vector<varPermutations> paramMatrix)
{
	if (place[i] == paramMatrix[i].value.size() - 1)
	{
		place[i] = 0;
		i--;
		checkEOR(i, paramMatrix);
	}
	else
	{
		place[i]++;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Calculate new time estimates based on the total number of sims running, the runtime, and the number of repetitions.
batchData updateTimestamp(batchData currentBatch)
{
	// calculate number of time steps
	int timeSteps = int(currentBatch.simTime/currentBatch.stepTime);
	
	// est seconds of wall time. Need to convert to hours, minutes, seconds.
	int wallSeconds = timeSteps/(SIM_RATE);
	int totalWallSeconds = int(wallSeconds * currentBatch.simCount);
	int reservedWallSeconds = int(wallSeconds*1.2);
	int totalReservedWallSeconds = int(totalWallSeconds*1.2);
	
	// timeStamp wallTime = getWallTime(wallSeconds);
	// timeStamp totalWallTime = getWallTime(totalWallSeconds);
	currentBatch.wallTime = getWallTime(reservedWallSeconds);
	currentBatch.totalWallTime = getWallTime(totalReservedWallSeconds);
	return currentBatch;
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Get a string value for the wall time in HH:MM:SS format
timeStamp getWallTime(int totalSeconds)
{
	timeStamp newWallTime;
	newWallTime.sec = totalSeconds % 60;
	int remainingSeconds = totalSeconds - newWallTime.sec;
	int totalMinutes = remainingSeconds/60;
	newWallTime.min = totalMinutes%60;
	int remainingMinutes = totalMinutes - newWallTime.min;
	newWallTime.hour = remainingMinutes/60;
	
	stringstream wallTimeSS;
	wallTimeSS << setfill('0') << setw(2) << newWallTime.hour << ":" << setfill('0') << setw(2) << newWallTime.min << ":" << setfill('0') << setw(2) << newWallTime.sec;
	wallTimeSS >> newWallTime.str;
	return newWallTime;
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Show details about queued batches
void listBatches()
{
	if (batches.size() != 0)
		{
			for(int i = 0; i < batches.size(); i++)
		{
			displayBatchData(batches[i]);
		}
		
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
	else
	{
		cout << "No batches are queued. ";
		pressEnter();
	}	
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Show details about a single queued batch
void displayBatchData(batchData currentBatch)
{
	cout << "Batch #" << currentBatch.queue << endl;
	cout << "  Global index: " << currentBatch.index << endl;
	cout << "  Valid batch: " << boolalpha << currentBatch.valid << endl;
	cout << "  Simulation time: " << currentBatch.simTime << endl;
	cout << "  Step time: " << currentBatch.stepTime << endl;
	cout << "  Number of unique simulations: " <<  currentBatch.sim.size() << endl;
	cout << "  Number of repetitions of each simulation: " << currentBatch.repetitions << endl;
	cout << "  Total number of simulations: " << currentBatch.simCount << endl;
	cout << "  Reserved wall time per simulation: " << currentBatch.wallTime.str << endl;
	cout << "  Reserved total wall time: " << currentBatch.totalWallTime.str << endl;
	
	if (currentBatch.paramMatrix.size() == 0)
	{
		cout << "  No custom parameters set. Using simulation defaults.\n\n";
	}
	
	for (int j = 0; j < currentBatch.paramMatrix.size(); j++)
	{
		if(j == 0)
		{
			cout << "  Custom parameters to run...\n";
		}
		cout <<"    " << currentBatch.paramMatrix[j].name << " = ";
		for (int k = 0; k < currentBatch.paramMatrix[j].value.size(); k++)
		{
			cout << currentBatch.paramMatrix[j].value[k];
			if (k < currentBatch.paramMatrix[j].value.size() - 1)
			{
				cout << ", ";
			}
			else
			{
				cout << ";\n";
			}
		}
		
		if(j == currentBatch.paramMatrix.size()-1)
		{
			cout << endl;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Modify details about queued batches
void changeBatchData(batchData currentBatch)
{
	cout << "Change batch data... \n\n";				
	displayBatchData(currentBatch);
	batches[currentBatch.queue - 1] = queueBatch(currentBatch);
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
				batchData emptyBatch;
				emptyBatch.valid = false;
				batchData newBatch = queueBatch(emptyBatch);
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
				listBatches();
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
	varCheckFile.open("../conf/.var_check.txt");
	
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
	
	cout << "Choose from the following parameters: \n";
	cout << setw(colWidth+8) << "Var Name";
	cout << setw(colWidth) << "Min Value";
	cout << setw(colWidth) << "Max Value";
	cout << setw(colWidth) << "Default";
	cout << setw(colWidth) << "Units" << endl;
	
	for(int i = 0; i < okVars.size(); i++)
	{
		cout << setw(numWidth - 2) << i + 1 << ": ";
		cout << setw(colWidth - numWidth + 8) << okVars[i].name;
		cout << setw(colWidth) << fixed << setprecision(2) << okVars[i].min;
		cout << setw(colWidth) << fixed << setprecision(2) << okVars[i].max;
		cout << setw(colWidth) << fixed << setprecision(2) << okVars[i].def; 
		cout << setw(colWidth) << okVars[i].unit << endl; 
	}
	cout << " q: Cancel selection and return.\n\n";
	cout << "Enter the number for the variable you want to specify: "; 
	
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
	valuesSS << valuesString;
	
	while (valuesSS)
	{
		valuesSS >> tempValue;
		if (!valuesSS.fail())	// if casting to double succeeds
		{
			if (tempValue <= okVars[selectionInt-1].max &&  tempValue >= okVars[selectionInt-1].min)
			{
				newVariable.value.push_back(tempValue);
			}
		}
	}	
	cout << endl;
	
	sort(newVariable.value.begin(), newVariable.value.end());
	newVariable.value.erase( unique( newVariable.value.begin(), newVariable.value.end() ), newVariable.value.end() );

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
				slurmFile << "#PBS -l walltime=" << batches[i].wallTime.str << "\n";
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
				cout << " batch " << i << ", sim " << j << ", var string " << l;
				simVarsFile << batches[i].sim[j].variableString[l] << endl;
			}
			
		}
	}
	
	// This section is functional. Creates 
	ofstream dirSetupFile;
	ofstream runBatchFile;
	dirSetupFile.open("directory-setup.sh");
	runBatchFile.open("run-batches.sh");
	
	dirSetupFile << "#!/bin/bash \n";
	runBatchFile << "#!/bin/bash \n";

	for(int i = 0; i < root.mkdir.size(); i++)
	{
		dirSetupFile << root.mkdir[i] << endl;
	}
	for(int i = 0; i < root.chmod.size(); i++)
	{
		dirSetupFile << root.chmod[i] << endl;
	}
	dirSetupFile.close();
	
	for(int i = 0; i < root.sbatch.size(); i++)
	{
		runBatchFile << root.sbatch[i] << endl;
	}
	
	runBatchFile << "rm slurm* \n";
	runBatchFile.close();
}

//---------------------------------------------------------------------------------------------------------------------------------//
// Create temporary batch data for testing;
void makeTestBatch()
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
											
		tempBatch.repetitions = 3 - bI;
		
		// Initial batchIndex value is derived from current folder population. The first one used in the new batch is one increment up.
		batchIndex++;
		tempBatch.index = batchIndex;
		tempBatch.queue = batches.size() + 1;
		tempBatch = updateTimestamp(tempBatch);
		
		// add temporary batch to batch list. All batches will go in this stack
		batches.push_back(tempBatch); 																
	}	
}

void pressEnter()
{
	cout << "Press ENTER to continue.";
	cin.ignore();
	getchar();
	cout << endl;	
}

bool confirm(string message, string error, string confirmStr, string cancelStr)
{
	string input;
	bool boolAnswer;
	
	cout << message << " (" << confirmStr << " or " << cancelStr << ") ";
	cin >> input;
	
	if (input == confirmStr)
	{

	}
	else if (input == cancelStr)
	{
		
	}

	return boolAnswer;
}

int getValidInt(int min, int max, string message, string error)
{
	int validValue = 1;
	return validValue;
}

