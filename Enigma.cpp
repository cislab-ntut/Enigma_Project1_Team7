#include<iostream>  //base 1ppl
#include<string>
#include<vector>

using namespace std;

string rotary[6]; // 1~5
string reflector;  //A | B | C
int selectRol[3];
int iniNum[6], curNum[3], plugNum = 0;

class Wire {
public:
	Wire(){}
	Wire(char p1, char p2) :w1(p1), w2(p2) {}
	int w1, w2;
};
Wire wire[12];

void decrypt();

void initial(int[], string);  //rotary index, state  1ppl  //iniNum, curNum, rotary*5, wire*10
string encrypt(string);  //1ppl  call encry()
char encry(char);  //3ppl
void rotate();  //1ppl  rotate curNum
int searchRotary(char, int);  //1ppl
char searchPlugBoard(char);

int main(int args, char* argv[]) {
	if (argv[1][0] == '-' && argv[1][1] == 'd') {
		decrypt();
		return 0;
	}


	string curState, plugState;  //each 3 char
								 // 3 2 1
	cout << "Input three rotor numbers which you want to use.(1~5)\n"
		<< "Separate these numbers with space:";
	for (int i = 0; i < 3; ++i)
		cin >> selectRol[i];
	// WFR XDH
	cout << "Input the three rotor's starting alphabet and current alphabet:(XDH)";
	cin >> curState;

	cout << "Input plugboard:";
	cin.ignore();
	getline(cin, plugState);
	for (int i = 0; i < plugState.length(); i += 3) {
		wire[plugNum].w1 = plugState[i];
		wire[plugNum++].w2 = plugState[i + 1];
	}

	initial(selectRol, curState);
	string input, output;
	while (true) {
		cout << "Input the plaintext what you want to encrypt:";
		cin >> input;
		output = encrypt(input);
		cout << "Output:";
		cout << output << endl;
	}
}

void initial(int selectRol[], string curState) {
	rotary[1] = "EKMFLGDQVZNTOWYHXUSPAIBRCJ";
	rotary[2] = "AJDKSIRUXBLHWTMCQGZNPYFVOE";
	rotary[3] = "BDFHJLCPRTXVZNYEIWGAKMUSQO";
	rotary[4] = "ESOVPZJAYQUIRHXLNFTGKDCMWB";
	rotary[5] = "VZBRGITYUPSDNHLXAWMJQOFECK";
	reflector = "YRUHQSLDPXNGOKMIEBFZCWVJAT";  //B now
	iniNum[1] = 'R' - 'A';
	iniNum[2] = 'F' - 'A';
	iniNum[3] = 'W' - 'A';
	iniNum[4] = 'K' - 'A';
	iniNum[5] = 'A' - 'A';
	for (int i = 0; i < 3; ++i) {
		curNum[i] = curState[i] - 'A';
	}
	/*
	// check it works well
	for(int i = 0; i < 3; ++i)
	cout << rotary[selectRol[i]][iniNum[i]] << rotary[selectRol[i]][curNum[i]] << endl;
	*/
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

	rotate();  //rotate rotary
	for (int i = 0; i < 3; ++i) {  //first 3 rotary
		c = (rotary[selectRol[i]][(c + curNum[i]) % 26] + 26 - curNum[i] - 'A') % 26;
		//cout << c << char(c + 'A') << endl;
	}

	for (int i = 0; i < 26; ++i) {  // reflect
		if (c == reflector[i] - 'A') {
			c = i;
			//cout << c << char(c + 'A') << endl;
			break;
		}
	}

	for (int i = 2; i >= 0; --i) {  //last 3 rotary
		c = (searchRotary(char((c + curNum[i]) % 26 + 'A'), i) + 26 - curNum[i]) % 26;
		//cout << c << char(c + 'A') << endl;
	}

	return searchPlugBoard(char(c + 'A'));
}

void rotate() {
	curNum[0] = (curNum[0] + 1) % 26;
	if (curNum[0] == iniNum[selectRol[0]] || curNum[1] + 1 == iniNum[selectRol[1]]) {		//判斷是否到指標位置
		curNum[1] = (curNum[1] + 1) % 26;
		if (curNum[1] == iniNum[selectRol[1]]) {
			curNum[2] = (curNum[2] + 1) % 26;
		}
	}
}

int searchRotary(char c, int rotaryNum) {
	for (int i = 0; i < 26; ++i) {
		if (c == rotary[selectRol[rotaryNum]][i])
			return i;
	}
}

char searchPlugBoard(char c) {
	for (int i = 0; i < 10; ++i) {  //plug
		if (c == wire[i].w1) {
			return wire[i].w2;
		}
		else if (c == wire[i].w2) {
			return wire[i].w1;
		}
	}
}



//int wireSize = 0;  //max 6
string plain = "HEILHITLER";
string cipher = "IPQHUGCXZM";
//(I,H) (I,Q) (I,G) (H,L) (H,U) (L,X)  //first level 4 or 5 plug --> go down  7 plug = wrong  6 plug --> test encry
//(E,P) (E,Z)  //5 plug --> go down   7 plug = wrong  6 plug --> test
//(T,C) (R,M)  //                     7 plug = wrong  6 plug --> test
Wire pair[10] = { Wire('I','H'), Wire('I','Q'), Wire('I','G'), Wire('H','L'), Wire('H','U'), Wire('L','X'), Wire('E','P'), Wire('E','Z'), Wire('T','C'), Wire('R','M') };
vector<Wire> testPlug;  //max 6
char testChar[3] = { 'I', 'E', 'C' };
//plug test I, E, C    (I,A) (I,B) ... (I,Z)    (E,A) ... (E,Z)   (C,A) ... (C,Z)
//I, H, Q, G, L, U, X, E, P, Z, T, C, R, M

vector<int> curPlug;

void changeRotary();
//void decryptInitial();
bool rotaryChoose();  //first rotary, call rotaryChoose2() recursively
bool rotaryChoose2();  //second rotary, call rotaryChoose3() recursively
bool rotaryChoose3();  //third rotary, call plugTest()

bool plugTest();  //first level plug set, call plugTest2() recursively
bool plugTest2();  //second level, call plugTest3()
bool plugTest3();  //third level, call exhaustiveAtk()

bool exhaustiveAtk(int, string&);  //exhaustive attack recursively, can input the except string of letters
bool ex(int plugAmount, string& exception, int lastPlugNum);
bool checkExcept(int, string&);
bool checkAns();  //call setMachine() then throw "HEILHITLER" into encrypt() and check if equal to "IPQHUGCXZM"

void setMachine();  //set the setting of the test  e.g. rotary, plug    ; call setPlug()
void setPlug();  //set curPlug(vector<int>) into wire[12](Wire)  //can combine into setMachine()

void decrypt() {




}

bool exhaustiveAtk(int plugAmount, string& exception) {
	int p[12] = { 0 };
	for (int i = 0; i < 25 - 2 * plugAmount; ++i) {
		if (checkExcept(i, exception))
			continue;
		curPlug.push_back(i);
		if (ex(plugAmount, exception, i))
			return true;
		else
			curPlug.pop_back();
	}

	/*for (p[0] = 0; p[0] < 25 - 2*plugAmount; ++p[0]) {  //plug 1
		if (checkExcept(p[0], exception))
			continue;
		for (p[1] = p[1] + 1; p[1] < 26 - 2*plugAmount; ++p[1]) {
			if (checkExcept(p[1], exception))
				continue;

			if (plugAmount > 1) {  // plug 2
				for (p[2] = p[2] + 1; p[2] < 27 - 2*plugAmount; ++p[2]) {
					if (checkExcept(p[2], exception))
						continue;
					for (p[3] = p[2] + 1; p[3] < 28 - 2 * plugAmount; ++p[3]) {
						if (checkExcept(p[3], exception))
							continue;

					}

				}
			}

		}
	}*/

	return false;
}

bool ex(int plugAmount, string& exception, int lastPlugNum) {
	bool second = curPlug.size() % 2 ? true : false;
	for (int i = lastPlugNum + 1; i < (second ? 26 : 25) - 2 * plugAmount; ++i) {
		if (checkExcept(i, exception))
			continue;
		curPlug.push_back(i);
		if (second) {
			if (plugAmount == 1) {  //last plug choosed
				if (checkAns())
					return true;
				else
					curPlug.pop_back();
			}
			else {
				if (ex(plugAmount - 1, exception, i))
					return true;
				else
					curPlug.pop_back();
			}
		}
		else {
			if (ex(plugAmount, exception, i))
				return true;
			else
				curPlug.pop_back();
		}
	}


	return false;
}

bool checkExcept(int letter, string& exception) {
	char c = letter + 'A';
	if (exception.find(c) != exception.npos)  //c is in exception
		return true;
	return false;
}

