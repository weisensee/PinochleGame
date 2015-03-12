/*	Query -- Lucas Weisensee February 2015

	manages user queries at the terminal

	functions

	see Query.h for more information
*/

#include "Query.h"

Query::Query(){}
Query::~Query(){}

int Query::iQuery(char * Query) {	// Queries user for integer
	//// print Query to terminal
	//printf(Query);
	//char buffer[128];

	//// get user response
	//int ans;
	//std::cin >> ans;
	//return ans;	
	std::string input;
	int ans;
	while (true) {
		printf(Query);
		std::getline(std::cin, input);

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (!(myStream >> ans))
			printf("\nInvalid number, please try again:  ");
		else
			break;
	}
	return ans;
}
char Query::cQuery(char * Query, char * answers) {	// Queries user for char answer
	printf(Query);
	char ans;
	do {
		// print what answers are acceptable
		printAcceptableAns(answers);

		// get user input
		std::cin.get(ans);
		std::cin.ignore(100, '\n');

		// check that ans appears somewhere in answers (acceptable answers to Query), check the upper and lower case version of user answer 'ans'
		if (strchr(answers, tolower(ans)) != NULL || strchr(answers, toupper(ans)) != NULL)
			return ans;

		// if answer did not match acceptable answers, restart
	} while (true);
}
bool Query::bQuery(char * Query) {					// Queries user for true/false answer
	// get answer
	char ans = Query::cQuery(Query, "TFYN");
	ans = toupper(ans);

	// return true if an affirming answer was entered
	return (ans == 'T' || ans == 'Y');
}
std::string * Query::sQuery(char * Query) {		// Queries user for string answer
	printf(Query);

	// get answer from user
	char buffer[256];
	std::cin.getline(buffer, 255, '\n');

	// save to string
	std::string * ans = new std::string(buffer);
	return ans;

}

void Query::printAcceptableAns(char * answers) {	//prints what answers are acceptable
	if (answers) {
		std::cout << "\nPlease enter: " << answers[0];	// print first possible answer
		int len = strlen(answers);
		for (int i = 1; i < len; i++) {
			if (answers[1] != '\0')
				std::cout << " or " << answers[i];
		}
		std::cout << ".\n";
	}
	// if the string wasnt valid:
	else
		printf("invalid argument Query::printAcceptableAns");
}
