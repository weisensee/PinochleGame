/*	Card.cpp -- Card Game 'Card' class 
	Lucas Weisensee 2015
	
	functions for Card class
		stores and operates on a single Card

	Card values:
	the Card value is the exact Card in the deck that the Card represents
	the values are 1-24 such that
	1,2,...6 = A,10,K,Q,J,9 = value%6
	and
	0,1,2,3 = Spades,Clubs,Diamonds,Hearts = value/6

*/

#pragma once

#include "..\library\Card.h"

Card::Card(char current) {
	value = current;
}
Card::Card() {
	value = 0;
}
Card::~Card() {}
char Card::chr() {		// returns the char value of the current Card
	return value;
}
int Card::suit() {		// returns the suit of the current Card, integer 0 - 3
	return Card::suit(value);
}
int Card::suit(char n) {// returns the suit of Card n, integer 0 - 3
	return n / 6;
}
char * Card::str() {	// returns the string corresponding to the Cards face (Kspads, J diamonds, etc...)
	return Card::str(value);
}
char * Card::str(char n) {	// returns the string corresponding to n's face (Kspads, J diamonds, etc...)
	// establish suit and face value
	int suit = Card::suit();
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
bool Card::operator==(Card &toCompare) {	// returns true if toCompare matches current Card, false otherwise
	return toCompare.value == value;
}
bool Card::operator==(char &toCompare) {	// returns true if toCompare matches current Card, false otherwise
	return toCompare == value;
}
bool Card::operator==(int toCompare) {	// returns true if toCompare matches current Card, false otherwise
	return toCompare == (int)value;
}
bool Card::operator<(Card toCompare) {		// returns true if toCompare is greater than this
	// if they are the same suit, compare relative values
	return (Card::suit() == toCompare.suit()) && (value%6 > toCompare.value%6);
}
bool Card::operator>(Card toCompare) {		// returns true if toCompare is less than this
	// if they are the same suit, compare relative values
	return (Card::suit() == toCompare.suit()) && (value % 6 < toCompare.value % 6);
}
bool Card::operator<(int toCompare) {		// returns true if toCompare is greater than this
	// if they are the same suit, compare relative values
	return (Card::suit() == Card::suit(toCompare)) && (value % 6 > toCompare % 6);
}
bool Card::operator>(int toCompare) {		// returns true if toCompare is less than this
	// if they are the same suit, compare relative values
	return (Card::suit() == Card::suit(toCompare)) && (value % 6 < toCompare % 6);
}
void Card::operator=(int toSet) {			// sets the current Card value to toSet
	value = toSet;
}
