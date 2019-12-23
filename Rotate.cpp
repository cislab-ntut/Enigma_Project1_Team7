// Enigma machine
// gcc 8.1.0
// encoding=utf-8

#include <iostream>
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
void unrotate();
int searchRotary(char, int);
char searchPlugBoard(char);

string plain = "HEILHITLER";
string cipher = "IPQHUGCXZM";

int main() {
	string curState, plugState;
    selectedRol.resize(3);
	cout << "Input three rotor numbers which you want to use.(1~5)\n"
		<< "Separate these numbers with space:";
	for (int i = 0; i < 3; ++i)
		cin >> selectedRol[i];
	cout << "Input the three rotor's starting alphabet and current alphabet:";
	cin >> curState;

	for (int i = 0; i < 3; ++i) {
		curNum[i] = curState[i] - 'A';
	}
    initial();
    string input = "ZATVAWORBRQGRJSXZVNORWZBLORMEGRASLQLAFWXZYODVVTD\
HCIRDMNWOPNIXVKASIIIALOOSZXAMSYCQHYGPRLMSACGAWPCPAVZTMUUZCTJDVBU\
ZAGFWMIVEZGBTLFIQDPPRZHDNKIPQHUGCXZM";
    // string input = "XFUHMVRMJVSSZHDMDXULSHEILHITLER";

    cout << "Counterclockwise" << endl;
    for(unsigned int i = 0; i < input.size() - 10; ++i) {
        unrotate();
    }
	// for(unsigned int i = 0; i < input.size() - 10; ++i) {
    //     rotate();
    // }
    for (int i = 0; i < 3; ++i) {
        cout << char(curNum[i] + 'A') << ' ';
    }
    cout << endl;

    cout << "Input plugboard:";
	cin.ignore();
	// getline(cin, plugState);
    plugState = "HS NL KU AE MO VZ";
	cout << endl;

	for (unsigned int i = 0; i < plugState.length(); i += 3) {
		Wire tempWire(plugState[i], plugState[i + 1]);
		wire.push_back(tempWire);
	}

    string output = encrypt(input);
    cout << output << endl;
    system("pause");
    return 0;
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

void unrotate() {
    if (curNum[1] == iniNum[selectedRol[1]]) {
        curNum[2] = (curNum[2] + 26 - 1) % 26;
    }
    if (curNum[0] == iniNum[selectedRol[0]] || curNum[1] == iniNum[selectedRol[1]]) {
        curNum[1] = (curNum[1] + 26 - 1) % 26;
    }
    curNum[0] = (curNum[0] + 26 - 1) % 26;
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
