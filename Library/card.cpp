/*	card.cpp -- Card Game 'card' class 
	Lucas Weisensee 2015
	
	functions for card class
		stores and operates on a single card

	Card values:
	the card value is the exact card in the deck that the card represents
	the values are 1-24 such that
	1,2,...6 = A,10,K,Q,J,9 = value%6
	and
	0,1,2,3 = Spades,Clubs,Diamonds,Hearts = value/6

*/

#pragma once

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\library\card.h"

card::card(char current) {
	value = current;
}
card::card() {
	value = 0;
}
card::~card() {}
char card::chr() {		// returns the char value of the current card
	return value;
}
int card::suit() {		// returns the suit of the current card, integer 0 - 3
	return card::suit(value);
}
int card::suit(char n) {// returns the suit of card n, integer 0 - 3
	return n / 6;
}
char * card::str() {	// returns the string corresponding to the cards face (Kspads, J diamonds, etc...)
	return card::str(value);
}
char * card::str(char n) {	// returns the string corresponding to n's face (Kspads, J diamonds, etc...)
	// establish suit and face value
	int suit = card::suit();
	int face = n % 6;

	// build face string
	char * temp = new char[3];
	temp[2] = '\0';

	//	0, 1, 2, 3 = Spades, Clubs, Diamonds, Hears = value / 6
	// set suit
	switch (suit) {
	case 0:
		temp[1] = 'S';
		break;
	case 1:
		temp[1] = 'C';
		break;
	case 2:
		temp[1] = 'D';
		break;
	case 3:
		temp[1] = 'H';
		break;
	}

	//	1, 2, ...6 = A, 10, K, Q, J, 9 = value % 6
	switch (face) {
	case 0:
		temp[0] = 'A';
		break;
	case 1:
		temp[0] = 'K';
		break;
	case 2:
		temp[0] = 'Q';
		break;
	case 3:
		temp[0] = 'J';
		break;
	case 4:
		temp[0] = 'T';
		break;
	case 5:
		temp[0] = '9';
		break;
	}

	// return string
	return temp;

}
bool card::operator==(card &toCompare) {	// returns true if toCompare matches current card, false otherwise
	return toCompare.value == value;
}
bool card::operator==(char &toCompare) {	// returns true if toCompare matches current card, false otherwise
	return toCompare == value;
}
bool card::operator==(int toCompare) {	// returns true if toCompare matches current card, false otherwise
	return toCompare == (int)value;
}
bool card::operator<(card toCompare) {		// returns true if toCompare is greater than this
	// if they are the same suit, compare relative values
	return (card::suit() == toCompare.suit()) && (value%6 > toCompare.value%6);
}
bool card::operator>(card toCompare) {		// returns true if toCompare is less than this
	// if they are the same suit, compare relative values
	return (card::suit() == toCompare.suit()) && (value % 6 < toCompare.value % 6);
}
bool card::operator<(int toCompare) {		// returns true if toCompare is greater than this
	// if they are the same suit, compare relative values
	return (card::suit() == card::suit(toCompare)) && (value % 6 > toCompare % 6);
}
bool card::operator>(int toCompare) {		// returns true if toCompare is less than this
	// if they are the same suit, compare relative values
	return (card::suit() == card::suit(toCompare)) && (value % 6 < toCompare % 6);
}
void card::operator=(int toSet) {			// sets the current card value to toSet
	value = toSet;
}
