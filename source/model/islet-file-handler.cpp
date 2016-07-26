#include "islet-file-handler.h"
#include <iostream>
#include <string>
#include <ostream>
#include <fstream>
#include <iomanip>

using namespace std;

void IsletFileHandlerClass::constructPath(int batch, int sim, int rep)
{

	stringstream 	inpathSS, outpathSS, timeOutSS, potentialOutSS, calciumOutSS, sodiumOutSS, potassiumOutSS, caerOutSS, 
							atpOutSS, adpOutSS, IRPOutSS, PPOutSS, DPOutSS, FIPOutSS, RIPOutSS, capOutSS, noiseOutSS, infoSS;
	
	inpathSS << "../input/Batch" <<  setfill('0') << setw(4) << batch << "_Sim" <<  setfill('0') << setw(4) << sim << "Vars.txt";
	inpathSS >> inpath;
	
	outpathSS << "../data/SimBatch" <<  setfill('0') << setw(4) << batch <<  "/sim" << setfill('0') << setw(4) << sim;

	if (rep > 0)
	{
		outpathSS << "/rep" << setfill('0') << setw(2) << rep;
	}
		
	outpathSS >> outpath;
	
	timeOutSS << outpath << "/time.txt";
	potentialOutSS << outpath << "/potential.txt";
	calciumOutSS << outpath << "/calcium.txt";
	sodiumOutSS << outpath << "/sodium.txt";
	potassiumOutSS << outpath << "/potassium.txt";
	caerOutSS << outpath << "/caer.txt";
	atpOutSS << outpath << "/atp.txt";
	adpOutSS << outpath << "/adp.txt";
	IRPOutSS << outpath << "/IRP.txt";
	PPOutSS << outpath << "/PP.txt";
	DPOutSS << outpath << "/DP.txt";
	FIPOutSS << outpath << "/FIP.txt";
	RIPOutSS << outpath << "/RIP.txt";
	capOutSS << outpath << "/cap.txt";
	noiseOutSS << outpath << "/noise.txt";
	infoSS << outpath << "/info.txt";
	
	timeOutSS >> timeOut;
	potentialOutSS >> potentialOut; 
	calciumOutSS >> calciumOut; 
	sodiumOutSS  >> sodiumOut;
	potassiumOutSS  >> potassiumOut; 
	caerOutSS >> caerOut;
	atpOutSS >> atpOut;
	adpOutSS >> adpOut;
	IRPOutSS >> IRPOut;
	PPOutSS >> PPOut;
	DPOutSS >> DPOut;
	FIPOutSS >> FIPOut;
	RIPOutSS >> RIPOut;
	capOutSS >> capOut;
	noiseOutSS >> noiseOut;
	infoSS >> infoOut;
	
	set_userVarsFile(inpath);
}

void IsletFileHandlerClass::updateStatus(double pComplete)
{
	ofsream infoFile(infoOut);
	infoFile << pComplete * 100 << "% complete." << endl;
}

void IsletFileHandlerClass::finishedStatus()
{
	
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
	if (remove(potentialOut.c_str())) perror(potentialOut.c_str());
	if (remove(calciumOut.c_str())) perror(calciumOut.c_str());
	if (remove(sodiumOut.c_str())) perror(sodiumOut.c_str());
	if (remove(potassiumOut.c_str())) perror(potassiumOut.c_str());
	if (remove(caerOut.c_str())) perror(caerOut.c_str());
	if (remove(atpOut.c_str())) perror(atpOut.c_str());
	if (remove(adpOut.c_str())) perror(adpOut.c_str());
	if (remove(PPOut.c_str())) perror(PPOut.c_str());
	if (remove(IRPOut.c_str())) perror(IRPOut.c_str());
	if (remove(DPOut.c_str())) perror(DPOut.c_str());
	if (remove(FIPOut.c_str())) perror(FIPOut.c_str());
	if (remove(RIPOut.c_str())) perror(RIPOut.c_str());
	if (remove(capOut.c_str())) perror(capOut.c_str());
	if (remove(noiseOut.c_str())) perror(noiseOut.c_str());
	cout << "Purge complete." << endl;
}

void IsletFileHandlerClass::ObjectiveOutputDataBlock(stringstream * dataOutputStream)
{
	ofstream outfilePotential;
	ofstream outfileCalcium;
	ofstream outfileSodium;
	ofstream outfilePotassium;
	ofstream outfileCaer;
	ofstream outfileATP;
	ofstream outfileADP;
	ofstream outfileIRP;
	ofstream outfilePP;
	ofstream outfileDP;
	ofstream outfileFIP;
	ofstream outfileRIP;
	ofstream outfileCap;
	ofstream outfileNoise;
	
	outfilePotential.open(potentialOut.c_str(),ios::app);
	outfileCalcium.open(calciumOut.c_str(),ios::app);
	outfileSodium.open(sodiumOut.c_str(),ios::app);
	outfilePotassium.open(potassiumOut.c_str(),ios::app);
	outfileCaer.open(caerOut.c_str(),ios::app);
	outfileATP.open(atpOut.c_str(),ios::app);
	outfileADP.open(adpOut.c_str(),ios::app);
	outfileIRP.open(IRPOut.c_str(),ios::app);
	outfilePP.open(PPOut.c_str(),ios::app);
	outfileDP.open(DPOut.c_str(),ios::app);
	outfileFIP.open(FIPOut.c_str(),ios::app);
	outfileRIP.open(RIPOut.c_str(),ios::app);
	outfileCap.open(capOut.c_str(),ios::app);
	outfileNoise.open(noiseOut.c_str(),ios::app);	
	
	outfilePotential <<  dataOutputStream[0].str();
	outfileSodium <<  dataOutputStream[1].str();
	outfilePotassium <<  dataOutputStream[2].str();
	outfileCalcium <<  dataOutputStream[3].str();
	outfileCaer <<  dataOutputStream[4].str();
	outfileATP <<  dataOutputStream[5].str();
	outfileADP <<  dataOutputStream[7].str();
	outfilePP <<  dataOutputStream[8].str();
	outfileDP <<  dataOutputStream[9].str();
	outfileFIP <<  dataOutputStream[10].str();
	outfileRIP <<  dataOutputStream[11].str();
	outfileCap <<  dataOutputStream[12].str();
	outfileNoise <<  dataOutputStream[13].str();
	
	outfilePotential.close();
	outfileATP.close();
	outfileADP.close();
	outfileCalcium.close();
	outfileSodium.close();
	outfilePotassium.close();
	outfileCaer.close();
	outfileIRP.close();
	outfilePP.close();
	outfileDP.close();
	outfileFIP.close();
	outfileRIP.close();
	outfileCap.close();
	outfileNoise.close();
}
