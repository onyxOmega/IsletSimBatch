#include "islet-file-handler.h"
#include <iostream>
#include <string>
#include <ostream>
#include <fstream>

void IsletFileHandlerClass::constructPath(int batch, int sim, int rep)
{
	cout << "In path constructor \n";
}

string IsletFileHandlerClass::get_userVarsFile()
{
	return userVarsFile;
}

char const* IsletFileHandlerClass::get_cellPropertiesFile()
{
	return cellPropertiesFile;
}

char const* IsletFileHandlerClass::get_cellPositionFile()
{
	return cellPositionFile;
}

char const* IsletFileHandlerClass::get_nnFile()
{
	return nnFile;
}

char const* IsletFileHandlerClass::get_randomVarsFile()
{
	return randomVarsFile;
}

void IsletFileHandlerClass::set_userVarsFile(string varInputFileName)
{
	ifstream testFile(varInputFileName);
	if (testFile)
	{
		userVarsFile = varInputFileName;
	}
	else
	{
		cout << "User defined variable input file does not exist. Using default variables." << endl;
	}
}

void IsletFileHandlerClass::ObjectiveOutputPurgeFiles()
{
	cout << "Purging old output files from working output directory..." << endl;
	if (remove(obj_potentialOutput)) perror(obj_potentialOutput);
	if (remove(obj_calciumOutput)) perror(obj_calciumOutput);
	if (remove(obj_sodiumOutput)) perror(obj_sodiumOutput);
	if (remove(obj_potassiumOutput)) perror(obj_potassiumOutput);
	if (remove(obj_caerOutput)) perror(obj_caerOutput);
	if (remove(obj_atpOutput)) perror(obj_atpOutput);
	if (remove(obj_adpOutput)) perror(obj_adpOutput);
	if (remove(obj_PPOutput)) perror(obj_PPOutput);
	if (remove(obj_IRPOutput)) perror(obj_IRPOutput);
	if (remove(obj_DPOutput)) perror(obj_DPOutput);
	if (remove(obj_FIPOutput)) perror(obj_FIPOutput);
	if (remove(obj_RIPOutput)) perror(obj_RIPOutput);
	if (remove(obj_capOutput)) perror(obj_capOutput);
	if (remove(obj_noiseOutput)) perror(obj_noiseOutput);
	cout << "Purge complete." << endl;
}

void IsletFileHandlerClass::ObjectiveOutputDataBlock(stringstream * dataOutputStream)
{
	ofstream obj_outfilePotential;
	ofstream obj_outfileCalcium;
	ofstream obj_outfileSodium;
	ofstream obj_outfilePotassium;
	ofstream obj_outfileCaer;
	ofstream obj_outfileATP;
	ofstream obj_outfileADP;
	ofstream obj_outfileIRP;
	ofstream obj_outfilePP;
	ofstream obj_outfileDP;
	ofstream obj_outfileFIP;
	ofstream obj_outfileRIP;
	ofstream obj_outfileCap;
	ofstream obj_outfileNoise;
	
	obj_outfilePotential.open(obj_potentialOutput,ios::app);
	obj_outfileCalcium.open(obj_calciumOutput,ios::app);
	obj_outfileSodium.open(obj_sodiumOutput,ios::app);
	obj_outfilePotassium.open(obj_potassiumOutput,ios::app);
	obj_outfileCaer.open(obj_caerOutput,ios::app);
	obj_outfileATP.open(obj_atpOutput,ios::app);
	obj_outfileADP.open(obj_adpOutput,ios::app);
	obj_outfileIRP.open(obj_IRPOutput,ios::app);
	obj_outfilePP.open(obj_PPOutput,ios::app);
	obj_outfileDP.open(obj_DPOutput,ios::app);
	obj_outfileFIP.open(obj_FIPOutput,ios::app);
	obj_outfileRIP.open(obj_RIPOutput,ios::app);
	obj_outfileCap.open(obj_capOutput,ios::app);
	obj_outfileNoise.open(obj_noiseOutput,ios::app);	
	
	obj_outfilePotential <<  dataOutputStream[0].str();
	obj_outfileSodium <<  dataOutputStream[1].str();
	obj_outfilePotassium <<  dataOutputStream[2].str();
	obj_outfileCalcium <<  dataOutputStream[3].str();
	obj_outfileCaer <<  dataOutputStream[4].str();
	obj_outfileATP <<  dataOutputStream[5].str();
	obj_outfileADP <<  dataOutputStream[7].str();
	obj_outfilePP <<  dataOutputStream[8].str();
	obj_outfileDP <<  dataOutputStream[9].str();
	obj_outfileFIP <<  dataOutputStream[10].str();
	obj_outfileRIP <<  dataOutputStream[11].str();
	obj_outfileCap <<  dataOutputStream[12].str();
	obj_outfileNoise <<  dataOutputStream[13].str();
	
	obj_outfilePotential.close();
	obj_outfileATP.close();
	obj_outfileADP.close();
	obj_outfileCalcium.close();
	obj_outfileSodium.close();
	obj_outfilePotassium.close();
	obj_outfileCaer.close();
	obj_outfileIRP.close();
	obj_outfilePP.close();
	obj_outfileDP.close();
	obj_outfileFIP.close();
	obj_outfileRIP.close();
	obj_outfileCap.close();
	obj_outfileNoise.close();
}
