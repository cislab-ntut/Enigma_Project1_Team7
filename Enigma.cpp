// Enigma machine
// gcc 8.1.0
// encoding=utf-8

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

string rotary[6];
string reflector;
vector<int> selectedRol;
int iniNum[6], curNum[3];

class Wire {
    public:
        Wire() {}
        Wire(char p1, char p2) :w1(p1), w2(p2) {}
        int w1, w2;
};
vector<Wire> wire;

void initial();
string encrypt(string);
char encry(char);
void rotate();
int searchRotary(char, int);
char searchPlugBoard(char);

void decrypt();
bool chooseRotary(unsigned int chooseAmount);
bool searchChoosedRotary(int n);
bool rotateRotary();
void plugTest();
void pt(char);

bool searchGuessPlug(Wire&); // search the same
bool checkGuessPlug(Wire&); // check if contradiction, true for no contradiction
bool searchGuessSingle(char);
bool checkGuessSingle(char);
bool searchIsSingle(char);
bool searchWrongPlug(Wire&);
bool searchWrongSingle(char);

void addWrong();

char encryRotary(char);

bool exhaustiveAtk(int, string&); // exhaustive attack recursively, can input the except string of letters
bool ex(int plugAmount, string& exception, int lastPlugNum);
bool checkExcept(int, string&);

bool checkAns(); // call setMachine() then throw "HEILHITLER" into encrypt() and check if equal to "IPQHUGCXZM"
void setMachine(); // set the setting of the test  e.g. rotary, plug    ; call setPlug()
void setPlug(); // set curPlug(vector<int>) into wire[12](Wire)  //can combine into setMachine()


int main(int args, char* argv[]) {
    // decrypt();
    // return 0;

	if (args > 1) {
        if (argv[1][0] == '-' && argv[1][1] == 'd') {
			decrypt();
			return 0;
		}
    }

	string curState, plugState;
    selectedRol.resize(3);
	cout << "Input three rotor numbers which you want to use.(1~5)\n"
		<< "Separate these numbers with space:";
	for (int i = 0; i < 3; ++i)
		cin >> selectedRol[i];
	// WFR XDH
	cout << "Input the three rotor's starting alphabet and current alphabet:(XDH)";
	cin >> curState;

	for (int i = 0; i < 3; ++i) {
		curNum[i] = curState[i] - 'A';
	}
/*
	for(int i = 0; i < 3; ++i)
	cout << rotary[selectedRol[i]][iniNum[i]] << rotary[selectRol[i]][curNum[i]] << endl;
*/
	cout << "Input plugboard:";
	cin.ignore();
	getline(cin, plugState);
	for (unsigned int i = 0; i < plugState.length(); i += 3) {
		Wire tempWire(plugState[i], plugState[i + 1]);
		wire.push_back(tempWire);
	}

	initial();
	string input, output;
	while (true) {
		cout << "Input the plaintext what you want to encrypt:";
		cin >> input;
		output = encrypt(input);
		cout << "Output:";
		cout << output << endl;
	}
}

void initial() {
	rotary[1] = "EKMFLGDQVZNTOWYHXUSPAIBRCJ";
	rotary[2] = "AJDKSIRUXBLHWTMCQGZNPYFVOE";
	rotary[3] = "BDFHJLCPRTXVZNYEIWGAKMUSQO";
	rotary[4] = "ESOVPZJAYQUIRHXLNFTGKDCMWB";
	rotary[5] = "VZBRGITYUPSDNHLXAWMJQOFECK";
	reflector = "YRUHQSLDPXNGOKMIEBFZCWVJAT"; // B
	iniNum[1] = 'R' - 'A';
	iniNum[2] = 'F' - 'A';
	iniNum[3] = 'W' - 'A';
	iniNum[4] = 'K' - 'A';
	iniNum[5] = 'A' - 'A';
}

string encrypt(string plaintext) {
	string ciphertext;
	for (char i : plaintext) {
		char c = encry(i);
		ciphertext.push_back(c);
	}
	return ciphertext;
}

char encry(char input) {
	int c = searchPlugBoard(input) - 'A';

    // rotate rotary
	rotate();
    
    // first 3 rotary
	for (int i = 0; i < 3; ++i) {
		c = (rotary[selectedRol[i]][(c + curNum[i]) % 26] + 26 - curNum[i] - 'A') % 26;
		// cout << c << char(c + 'A') << endl;
	}
    // reflector
	for (int i = 0; i < 26; ++i) {
		if (c == reflector[i] - 'A') {
			c = i;
			// cout << c << char(c + 'A') << endl;
			break;
		}
	}
    // last 3 rotary
	for (int i = 2; i >= 0; --i) {
		c = (searchRotary(char((c + curNum[i]) % 26 + 'A'), i) + 26 - curNum[i]) % 26;
		// cout << c << char(c + 'A') << endl;
	}

	return searchPlugBoard(char(c + 'A'));
}

void rotate() {
	curNum[0] = (curNum[0] + 1) % 26;
	if (curNum[0] == iniNum[selectedRol[0]] || curNum[1] + 1 == iniNum[selectedRol[1]]) {
		curNum[1] = (curNum[1] + 1) % 26;
		if (curNum[1] == iniNum[selectedRol[1]]) {
			curNum[2] = (curNum[2] + 1) % 26;
		}
	}
}

int searchRotary(char c, int rotaryNum) {
	for (int i = 0; i < 26; ++i) {
		if (c == rotary[selectedRol[rotaryNum]][i])
			return i;
	}

    // should not be executed here
    system("pause");
    exit(-1);
}

char searchPlugBoard(char c) {
	for (unsigned int i = 0; i < wire.size(); ++i) {
		if (c == wire[i].w1) {
			return wire[i].w2;
		}
		else if (c == wire[i].w2) {
			return wire[i].w1;
		}
	}
	return c;
}

// initialCurrent
int wireSize = 6;  // max 6
string plain = "HEILHITLER";
string cipher = "IPQHUGCXZM";
// (I,H) (I,Q) (I,G) (H,L) (H,U) (L,X)  //first level 4 or 5 plug --> go down  7 plug = wrong  6 plug --> test encry
// (E,P) (E,Z)  //5 plug --> go down   7 plug = wrong  6 plug --> test
// (T,C) (R,M)  //                     7 plug = wrong  6 plug --> test
Wire correctPair[20] = {
	Wire('I','H'), Wire('I','Q'), Wire('I','G'), Wire('H','L'), Wire('H','U'),
	Wire('L','X'), Wire('E','P'), Wire('E','Z'), Wire('T','C'), Wire('R','M'),
	Wire('H','I'), Wire('Q','I'), Wire('G','I'), Wire('L','H'), Wire('U','H'),
	Wire('X','L'), Wire('P','E'), Wire('Z','E'), Wire('C','T'), Wire('M','R')
};
int pairPosition[20] = { 0, 2, 5, 3, 4, 7, 1, 8, 6, 9,
0, 2, 5, 3, 4, 7, 1, 8, 6, 9 };
char guessChars[14] = { 'I', 'H', 'Q', 'G', 'L', 'U', 'X', 'E', 'P', 'Z', 'T', 'C', 'R', 'M' };
// vector<Wire> testPlug;  //max 6
char testChar[3] = { 'I', 'E', 'C' };
// plug test I, E, C    (I,A) (I,B) ... (I,Z)    (E,A) ... (E,Z)   (C,A) ... (C,Z)
// I, H, Q, G, L, U, X; E, P, Z; T, C, R, M

vector<int> choosedRotary;
vector<int> curPlug; // current choosed plug set
vector<Wire> guessPlug;
vector<char> guessSingle;
vector<char> isSingle;
vector<Wire> wrongPlug;
vector<char> wrongSingle;

// use this to guess 26*26*26 cases
int curNumSave[3] = {20, 9, 21};
int iniNumSave[3] = {};

void decrypt() {
	initial();
	choosedRotary.clear();
    selectedRol.clear();
	selectedRol.push_back(4);
	chooseRotary(3);
	// rotateRotary();

	// if (chooseRotary(3)) {
	// 	cout << "correct set" << endl;
	// 	for(int i : selectedRol)
	// 		cout << i;
	// 	for(Wire w : wire)
	// 		cout << w.w1 << w.w2 << ' ';
	// 	cout << endl;
	// 	// print out the correct set and use the set to print out the plain text
	// }
	// else {
	// 	// no correct set, should not be executed here
    //     cout << "\nNo correct rotary set!" << endl;
    //     system("pause");
	// }
}

// encrypt only on rotary
char encryOnRotary(char input) {
	int c = input - 'A';
    // rotate rotary
	rotate();

    // first 3 rotary
	for (int i = 0; i < 3; ++i)
		c = (rotary[selectedRol[i]][(c + curNum[i]) % 26] + 26 - curNum[i] - 'A') % 26;
    // reflector
	for (int i = 0; i < 26; ++i)
		if (c == reflector[i] - 'A') {
			c = i;
			break;
		}
    // last 3 rotary
	for (int i = 2; i >= 0; --i)
		c = (searchRotary(char((c + curNum[i]) % 26 + 'A'), i) + 26 - curNum[i]) % 26;

	return char(c + 'A');
}

void plugTest() {
	curPlug.clear();
	guessPlug.clear();
	guessSingle.clear();
	isSingle.clear();
	wrongPlug.clear();
	wrongSingle.clear();
    
	for (int i = 0; i < 3; ++i)
		curNum[i] = curNumSave[i];

	for (int i = 0; i < 14; ++i) {
		// if (pt(guessChars[i]))
		// 	return true;
		pt(guessChars[i]);
	}
}

void pt(char guessChar) {
	//curPlug.push_back(guessChar-'A');
	Wire guess;
	guess.w1 = guessChar;
	for (int i = 0; i < 25; ++i) {
		bool wrongFlag = false;
		if (i == guessChar - 'A')
            continue;
		guess.w2 = i + 'A';
		if (searchGuessPlug(guess) || searchWrongPlug(guess) || searchIsSingle((char)guess.w2))
            continue;

		// curPlug.push_back(i);
        // first guess
		guessPlug.push_back(guess);

		for (int j = 0; j < 20; ++j) {
			// (I,H) (I,Q) (I,G)
			if (correctPair[j].w1 == guessChar) {
				for (int k = 0; k < 3; ++k)
					curNum[k] = curNumSave[k];
				for (int k = 0; k < pairPosition[j]; ++k)
					rotate();

				char outc = encryOnRotary(i + 'A');
				// Wire(outc, correctPiar[j].w2)
				if (outc != correctPair[j].w2) {
					Wire newGuess(outc, correctPair[j].w2);
					if (searchWrongPlug(newGuess)) {
						guessPlug.push_back(newGuess);
						addWrong();
						wrongFlag = true;
						break;
					}
					// contradiction
					if (checkGuessPlug(newGuess) == false) {
						guessPlug.push_back(newGuess);
						addWrong();
						wrongFlag = true;
						break;
					}
					else {
						// nothing, check next pair
					}
					guessPlug.push_back(newGuess);
				}
				// single
				else {
					// no contradiction
					if (checkGuessSingle(outc)) {
						guessSingle.push_back(outc);
					}
					//contradiction
					else {
						addWrong();
						wrongFlag = true;
						break;
					}
				}


			}
		}
		// call exhaustiveAtk if less than 6 plugs
		if (wrongFlag)
			continue;
		if (guessPlug.size() > 6) {
			addWrong();
			continue;
		}
		else if (guessPlug.size() == 6) {
			if (checkAns()){
				//nothing
			}
			else {
				addWrong();
				continue;
			}
		}
		// less than 6 plugs
		else {
			string exception;
			for (unsigned int j = 0; j < isSingle.size(); ++j)
				exception.push_back(isSingle[j]);
			for (unsigned int j = 0; j < guessSingle.size(); ++j)
				exception.push_back(guessSingle[j]);
			curPlug.clear();
			for (unsigned int j = 0; j < guessPlug.size(); ++j) {
				curPlug.push_back(guessPlug[j].w1);
				curPlug.push_back(guessPlug[j].w2);
			}
			if (exhaustiveAtk(6 - guessPlug.size(), exception)) {
				// return true;
			}
			else {
				addWrong();
				continue;
			}
		}


	}
	// guessChar is single
	isSingle.push_back(guessChar);
	guessPlug.clear();
	guessSingle.clear();
}

void addWrong() {
	for (unsigned int i = 0; i < guessPlug.size(); ++i) {
		if (!searchWrongPlug(guessPlug[i]))
			wrongPlug.push_back(guessPlug[i]);
	}
	guessPlug.clear();
	guessSingle.clear();
}

bool exhaustiveAtk(int plugAmount, string& exception) {
	// int p[12] = { 0 };
	for (int i = 0; i < 26; ++i) {
		if (checkExcept(i, exception))
			continue;
		curPlug.push_back(i + 'A');
		// if (ex(plugAmount, exception, i))
		// 	return true;
		// else
		// 	curPlug.pop_back();
		ex(plugAmount, exception, i);
		curPlug.pop_back();
	}
	return false;
}

bool ex(int plugAmount, string& exception, int lastPlugNum) {
	bool second = curPlug.size() % 2 ? true : false;
	for (int i = lastPlugNum + 1; i < 26; ++i) {
		if (checkExcept(i, exception))
			continue;
		curPlug.push_back(i + 'A');
		if (second) {
            // last plug choosed
			if (plugAmount == 1) {
				// if (checkAns())
				// 	return true;
				// else
				// 	curPlug.pop_back();
				checkAns();
				curPlug.pop_back();
			}
			else {
				// if (ex(plugAmount - 1, exception, i))
				// 	return true;
				// else
				// 	curPlug.pop_back();
				ex(plugAmount - 1, exception, i);
				curPlug.pop_back();
			}
		}
		else {
			// if (ex(plugAmount, exception, i))
			// 	return true;
			// else
			// 	curPlug.pop_back();
			ex(plugAmount, exception, i);
			curPlug.pop_back();
		}
	}


	return false;
}

bool checkExcept(int letter, string& exception) {
	vector<int>::iterator it;
	it = find(curPlug.begin(),curPlug.end(), letter + 'A');
	if(it != curPlug.end())
		return true;
	char c = letter + 'A';
    // c is in exception
	if (exception.find(c) != exception.npos)
		return true;
	return false;
}

void printAns() {
	ofstream outFile("ans.txt", ios::app);
	if(!outFile) {
		cout << "File could not be opened" << endl;
		system("pause");
		exit(1);
	}

	outFile << "correct set" << endl;
	for(int i : selectedRol)
		outFile << i << ' ';
	outFile << endl;
	for(int i = 0; i < 3; ++i) {
		outFile << char(curNum[i] + 'A') << ' ';
	}
	outFile << endl;
	for(Wire w : wire)
		outFile << char(w.w1) << char(w.w2) << ' ';
	outFile << endl;
	outFile.close();
}

bool checkAns() {
	setMachine();
	string output = encrypt(plain);
	if(output != cipher)
		return false;
	setMachine();
	printAns();
	return true;
}

void setMachine() {
	setPlug();
	for(int i = 0; i < 3; ++i) {
		curNum[i] = curNumSave[i];
	}
}

void setPlug() {
	wire.clear();
	for(int i = 0; i < 12; i+=2) {
		Wire w(curPlug[i], curPlug[i + 1]);
		wire.push_back(w);
	}
}

bool searchGuessPlug(Wire& plug) {
	for (unsigned int i = 0; i < guessPlug.size(); ++i)
		if ((plug.w1 == guessPlug[i].w1 && plug.w2 == guessPlug[i].w2)
            || (plug.w1 == guessPlug[i].w2 && plug.w2 == guessPlug[i].w1))
			return true;
	return false;
}

// check if no contradiction of plug, true for no contradiction
bool checkGuessPlug(Wire& plug) {
	// same
    if (searchGuessPlug(plug))
		return false;
	for (unsigned int i = 0; i < guessPlug.size(); ++i) {
        // contradiction
		if (plug.w1 == guessPlug[i].w1 || plug.w1 == guessPlug[i].w2
            || plug.w2 == guessPlug[i].w1 || plug.w2 == guessPlug[i].w2)
			return false;
	}
	for (unsigned int i = 0; i < guessSingle.size(); ++i) {
        // contradiction
		if (plug.w1 == guessSingle[i] || plug.w2 == guessSingle[i])
			return false;
	}
	for (unsigned int i = 0; i < isSingle.size(); ++i) {
        // contradiction
		if (plug.w1 == isSingle[i] || plug.w2 == isSingle[i])
			return false;
	}
	return true;
}

bool searchGuessSingle(char c) {
	for (unsigned int i = 0; i < guessSingle.size(); ++i) {
		if (c == guessSingle[i])
			return true;
	}
	return false;
}

// check if single
bool checkGuessSingle(char c) {
	if (searchIsSingle(c) || searchGuessSingle(c))
		return true;
	for (unsigned int i = 0; i < guessPlug.size(); ++i) {
        // contradiction
		if (c == guessPlug[i].w1 || c == guessPlug[i].w2)
			return false;
	}
    // contradiction
	if (searchWrongSingle(c))
		return false;
	return true;
}

bool searchIsSingle(char c) {
	for (unsigned int i = 0; i < isSingle.size(); ++i) {
		if (c == isSingle[i])
			return true;
	}
	return false;
}

bool searchWrongPlug(Wire& plug) {
	for (unsigned int i = 0; i < wrongPlug.size(); ++i)
		if ((plug.w1 == wrongPlug[i].w1 && plug.w2 == wrongPlug[i].w2)
            || (plug.w1 == wrongPlug[i].w2 && plug.w2 == wrongPlug[i].w1))
			return true;
	return false;
}

bool searchWrongSingle(char c) {
	for (unsigned int i = 0; i < wrongSingle.size(); ++i) {
		if (c == wrongSingle[i])
			return true;
	}
	return false;
}

bool chooseRotary(unsigned int chooseAmount) {
	if (selectedRol.size() <= chooseAmount) {
		for (int i = 1; i <= 5; ++i) {
            // already been chosen
			if (searchChoosedRotary(i))
                continue;

			selectedRol.push_back(i);
            // ready to rotate and attack
			if (selectedRol.size() == chooseAmount) {
				cout << "\rRol:\t";
				for(int i : selectedRol)
					cout << i << '\t';
				cout << endl;
				// if (rotateRotary())
				// 	return true;
				rotateRotary();
			}
			else if (chooseRotary(chooseAmount)) {
				// return true;
			}
			selectedRol.pop_back();
		}
	}
    return false;
}

bool searchChoosedRotary(int n) {
	for(int i : selectedRol)
		if(n == i)
			return true;
	return false;
}

bool rotateRotary() {
	do {
		cout << "\rTest:\t";
		for(int i : curNumSave)
			cout << i << '\t';
			cout << endl;
		// if (plugTest()) {
		// 	return true;
		// }
		plugTest();
		for (int i = 0; i < 3; ++i)
			curNum[i] = curNumSave[i];
		rotate();
		for (int i = 0; i < 3; ++i)
			curNumSave[i] = curNum[i];
	} while (!(curNumSave[0] == iniNumSave[0] && curNumSave[1] == iniNumSave[1] && curNumSave[2] == iniNumSave[2]));

    memset(iniNumSave, 0, sizeof(iniNumSave));
	return false;
}