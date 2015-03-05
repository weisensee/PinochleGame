/*	query -- Lucas Weisensee February 2015

	manages user queries at the terminal

	functions

	see query.h for more information
*/

#include "query.h"

query::query(){}
query::~query(){}

int query::iQuery(char * query) {	// Queries user for integer
	// print query to terminal
	printf(query);
	char buffer[128];

	// get user response
	std::cin.getline(buffer, 128, '\n');
	return atoi(buffer);
}
char query::cQuery(char * query, char * answers) {	// Queries user for char answer
	printf(query);
	char ans;
	do {
		// print what answers are acceptable
		printAcceptableAns(answers);

		// get user input
		std::cin.get(ans);
		std::cin.ignore(100, '\n');

		// check that ans appears somewhere in answers (acceptable answers to query), check the upper and lower case version of user answer 'ans'
		if (strchr(answers, tolower(ans)) != NULL || strchr(answers, toupper(ans)) != NULL)
			return ans;

		// if answer did not match acceptable answers, restart
	} while (true);
}
std::string * query::sQuery(char * query) {		// Queries user for string answer
	printf(query);

	// get answer from user
	char buffer[256];
	std::cin.getline(buffer, 255, '\n');

	// save to string
	std::string * ans = new std::string(buffer);
	return ans;

}

void query::printAcceptableAns(char * answers) {	//prints what answers are acceptable
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
		printf("invalid argument query::printAcceptableAns");
}
