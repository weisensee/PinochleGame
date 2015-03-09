/*	AI Trainer -- Card Game AI

	Windows Desktop Application

	Lucas Weisensee

	February 2015

	This program acts as an AI Training Manager for the Card Playing AIs
	
	It runs tests and training of the card game servers:

		"Desktop Server.exe" and "Desktop Client.exe"

*/


#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\query.h"		// Client communication class


#define DEFAULT_PORT 444444
#define DEFAULT_IP localhost

// ::GLOBAL VARIABLES::
query userQuery;		// User querying object


// ::FUNCTION DEFININTIONS::
void euchreTraining();		// launch euchre training
void pinochleTraining();	// launch Pinochle training
void customSetup();			// setup custom training
void runTraining();			// start training


int main(int argc, char **argv) {
	printf("\nWelcome to the AI Training Console.");

	// Get training type from user
	char ans = userQuery.cQuery("\nWhat type of training would you like to run? \n[E]uchre DEFAULT training\n[P]inochle DEFAULT training\n[C]ustom training schema\n[Q]uit", "EPCQ");

	// Launch desired training program
	switch (ans) {
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

	// Run Training
	runTraining();

	// exit
	return 1;
}

void euchreTraining() {		// launch euchre training
	printf("\nRunning Euchre Training");

	// set values to default for euchre game

	// run the program
	runTraining();
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
void runTraining() {			// start training
	printf("\nLaunching Training with specified arguments:");

	// print arguments

	// launch training modules
}
