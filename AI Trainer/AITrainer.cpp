/*	AI Trainer -- Card Game AI

	Windows Desktop Application

	Lucas Weisensee

	February 2015

	This program acts as an AI Training Manager for the Card Playing AIs
	
	It runs tests and training of the card game servers:

		"Desktop Server.exe" and "Desktop Client.exe"

*/
#define WIN32_LEAN_AND_MEAN


#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <time.h>

#include "..\Library\Query.h"			// Client communication class
#include "..\Library\GameSettings.h"	// Client communication class


#define DEFAULT_PORT 444444
#define DEFAULT_IP localhost
#define WINDOWED true

// ::EXECUTABLE LOCATIONS::
std::string cmdPath = "C:\\Windows\\System32\\cmd.exe";
std::string cmdStart = "/C start cmd.exe /C ";		// cmd start command for new window with current executable
std::string SERVER_DIR = "\\Desktop Server\\Debug\\PinochleServer.exe";
std::string Client_DIR = "\\Desktop Client\\Debug\\Desktop Client.exe";
std::string serverArguments = "444444";	// SERVER: command line arguments for  executable
std::string ClientArguments = "localhost 444444";	// Client: command line arguments for executable
std::string projectDir;	// main project directory
std::string serverArgv, servername;	// SERVER: the argv[] and file path
std::string playerArgv, Clientname;	// Client: the argv[] and file path 
std::string localDirectory, executableName;	// where the program is located

// ::LOCAL SETTINGS::  
int ROUNDS = 1;
int AIs = 3;
int Humans = 1;
char TYPE = 'E';
GameSettings SETTINGS;


// ::GLOBAL VARIABLES::
Client server;
Query userQuery;		// User Querying object
PROCESS_INFORMATION * processes;	// array of active processes

// ::FUNCTION DEFININTIONS::
void euchreTraining();		// launch euchre training
void pinochleTraining();	// launch Pinochle training
void customSetup();			// setup custom training
void runTraining();			// start training
void setup(int argc, char **argv);				// initialize training program
void setupWithDefaults();	// setup training mode with default attributes
void startPlayers();		// launches a process for each player and one for the server
void launch(std::string * exeDir, std::string * curArgs, int n);		// launches the processes with the attributes as set
void cleanup();				// deallocate memory and cleanup processes
void printError(int n);		// formats and prints the error 'n' from GetLastError
int getGameID();			// returns the game ID for the current game

int main(int argc, char **argv) {
	printf("\nWelcome to the AI Training Console.");

	// initialize the training program
	setup(argc, argv);

	//// Get training type from user
	//char ans = userQuery.cQuery("\nWhat type of training would you like to run? \n[E]uchre DEFAULT training\n[P]inochle DEFAULT training\n[C]ustom training schema\n[Q]uit", "EPCQ");

	// Launch desired training program
	switch (TYPE) {
	case 'E':
		euchreTraining();	// launch euchre training
		break;
	case 'P':
		pinochleTraining();	// launch Pinochle training
		break;
	case 'C':
		customSetup();		// setup custom training
		break;
	case 'Q':
		break;
	default:
		printf("\nSwitch error: in default...");
		break;
	}

	//// Run Training
	//runTraining();

	// exit
	cleanup();
	return 1;
}
void setup(int argc, char **argv) {				// initialize training program
	printf("\nSetting up the AI Training program");

	// get current directory
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	
	// set local directory
	int pos = std::string(buffer).find_last_of("\\/");
	localDirectory = std::string(buffer).substr(0, pos);

	// set executable name
	executableName = std::string(buffer).substr(pos+1, strlen(buffer));
	printf("\nRunning: %s", executableName.c_str());
	printf("\nIn Local Directory: %s", localDirectory.c_str());

	// set project:PinochleGame 'main' directory
	pos = localDirectory.find_last_of("\\/");	// up one directory to parent of local
	pos = localDirectory.substr(0, pos).find_last_of("\\/");	// up another directory to end point of 'main' PinochleGame path in starting filepath
	projectDir = localDirectory.substr(0, pos);				// project main

	printf("\nMain project directory ('PinochleGame'): %s", projectDir.c_str());

	// get running mode
	// if in default mode, load defaults
	if (strcmp(argv[1], "-D") == 0 || strcmp(argv[1], "-d") == 0) {		// default mode
		setupWithDefaults();
	}
	// check if running in custom mode [-C | -c]
	else if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0) {
		// setup custom mode
		printf("\nCUSTOM MODE: loading in from file settings.txt");
	}
}
void setupWithDefaults() {	// setup training mode with default attributes
	// initiate settings object with TYPE's settings
	SETTINGS.setup(TYPE);
	SETTINGS.GAMEID = getGameID();
}
void euchreTraining() {		// launch euchre training
	printf("\nRunning Euchre Training");

	// send game info string
	char temp[32];
	itoa(SETTINGS.GAMEID, temp, 10);
	ClientArguments += " ";
	ClientArguments += temp;
	serverArguments += " ";
	serverArguments += temp;

	// send game type to server
	serverArguments += " E ";

	// run the training the specified number of times
	for (int i = 0; i < ROUNDS; ++i) {
		// set game IDs?


		// run the program
		runTraining();
	}


}
void runTraining() {			// start training
	printf("\nLaunching Training with specified arguments:");

	// array of process info
	processes = new PROCESS_INFORMATION[1 + AIs + ROUNDS];
	// print arguments

	// launch players
	startPlayers();

	// connect to server
	
}
void startPlayers() {		// launches the players with the attributes as set
	printf("\nStarting 'PinochleServer' with: %s", serverArgv.c_str());
	int pos;
	char buf[6];
	// signal AIs to perform as AIs and human tests as humans
	std::string humanArgs = ClientArguments + " HU ";
	ClientArguments += " AI ";

	// Initiate each AI
	for (pos = 0; pos < AIs; ++pos) {
		// give each player a player number
		//std::string temp = ClientArguments + itoa(pos, buf, 10);
		std::string temp = ClientArguments + " " + itoa(pos, buf, 10);

		// create each player
		launch(&(Clientname + Client_DIR), &temp, pos);
	}

	// Initiate any testing human players
	for (pos; pos < AIs + Humans; ++pos) {
		// give each player a player number
		std::string temp = ClientArguments + " " + itoa(pos, buf, 10);

		// create each player
		launch(&(Clientname + Client_DIR), &temp, pos);
	}

	// initiate server
	launch(&(SERVER_DIR), &serverArguments, pos+1);

}
void launch(std::string * exeDir, std::string * curArgs, int n) {		// launches the server with the attributes as set
	printf("\n*%i*\nStarting: %s \nwith args: %s", n, exeDir->c_str(), curArgs->c_str());

	// set server executable location
	std::string name = "\"" + projectDir + *exeDir + "\"";

	// set server argv
	std::string args = " \"" + name + " " + *curArgs + "\"";

	// additional information
	STARTUPINFO si;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&processes[n], sizeof(processes[n]));

	//args = name + " 444444";//+ args;

	// if the program is set to run in windowed mode
	if (WINDOWED) {
		args.insert(0, cmdStart.c_str());		// cmd start command for new window with current executable
		name = cmdPath;
	}

	// create non const version of argv string
	char* servArgvCp = strdup(args.c_str());

	// start the program up
	printf("\n*%i*file path: %s\nArgv: %s", n, name.c_str(), servArgvCp);
	if (!CreateProcess(name.c_str(),   // the path
		servArgvCp,		// Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&processes[n]))	// Pointer to server PROCESS_INFORMATION 
		printError(GetLastError());
	else
		printf("\n Process started successfully");
}
void printError(int n) {	// formats and prints the error 'n' from GetLastError
	printf("\nCreateProcess failure : %d", GetLastError());

	//if (!FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
	//	pMessage,
	//	0,
	//	0,
	//	buffer,
	//	size,
	//	(va_list*)pArgs))
	//{
	//	wprintf(L"Format message failed with 0x%x\n", GetLastError());
	//	return;
	//}

	//wprintf(L"Formatted message: %s\n", buffer);
}
void pinochleTraining() {	// launch Pinochle training
	printf("\nRunning Pinochle Training");

	// set values o default for euchre game

	// run the program
	runTraining();
}
void customSetup() {			// setup custom training
	printf("\nGetting custom configuration from user");

	// get custom specifications from user

	// run the program
	runTraining();
}
void cleanup() {		// deallocate memory and cleanup processes
	// wait for processes to finish, then close them
	int result = WaitForMultipleObjects(
		1 + AIs + Humans,
		(HANDLE*)&processes->hProcess,
		true,
		INFINITE
		);

	// if result was successful
	if (result)
		// Close process and thread handles. 
		for (int i = 0; i < Humans + AIs + 1; ++i) {
			//CloseHandle(processes[i].hThread);
			CloseHandle(processes[i].hProcess);
		}
	else
		printf("\nError closing process handles, %d", GetLastError());

}
int getGameID() {			// returns the game ID for the current game
	srand(time(NULL));
	return rand() % 100;

}
