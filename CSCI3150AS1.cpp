// Course Title: CSCI 3150 - Operating System
// Author : Tony
// Date   : 01-03-2016


#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <limits.h>
using namespace std;

///*
class shell {
public:

	vector<string> shellInputIntepreter(string input){
		int startPoint = 0;
		vector<string> token;
		for(int i=0; i<input.size(); i++){
			if(input[i] == ' ' || i == input.size() - 1){
				if(startPoint == i) continue;
				else{
					//cout << "testcase3: " << startPoint << "    " << i << endl;
					int cmdLen = i - startPoint;
					string temp = input.substr(startPoint, cmdLen);
					token.push_back(temp);
					startPoint = i+1;
				}
			}
		}
		// for(int j=0; j<token.size(); j++){
		// 	cout << token[j] << endl;
		// }
		if(token[0] == "exit" || token[0] == "cd"){
			//cout << "exit | cd" << endl;
			token.insert(token.begin(), "built-in");
		}
		else{
			//cout << "normal" << endl;
			token.insert(token.begin(), "normalCMD");
		}
		return token;
	}

	string shellWaitForInput(){
		string input;
		string shellLeft = "[TKShell:";
		string shellRight = "]$:";
		char cwd[PATH_MAX + 1];
		int inputLen = 0;
		if(getcwd(cwd, PATH_MAX+1) == NULL){
			string cppcwd = cwd;
			cout << "Current Working Dir: " << cppcwd << endl;
		}
		else{
			string cppcwd = cwd;
			cout << shellLeft << cppcwd << shellRight;
		}
		getline (cin, input);

		//format the input string with one whitespace followed only
		inputLen = input.size();
		while(input[inputLen-1] == ' '){
			input = input.substr(0, inputLen-1);
			inputLen --;
			cout << "special testcase: " << input << endl;
		}
		input = input + " ";
		//cout << "testcase0: " << input << endl;
		return input;
	} 

	bool shellBuiltInChecker(vector<string> command){
		int cmdSize = command.size();
		if(command[0] == "built-in"){
			if(command[1] == "exit"){
				if(cmdSize == 2)  return true;
				else{
					cout << "exit: wrong number of arguments" << endl;
					return false;
				} 
			}
			else if(command[1] == "cd"){
				if(cmdSize == 3) return true;
				else{
					cout << "cd: wrong number of arguments" << endl;
					return false;
				} 
			}
			else return false;
		}
		else return false;
	}

	bool shellInvokeBuiltinCMD(vector<string> command){
		if(command[1] == "exit"){
			return false;
		}
		else if(command[1] == "cd"){
			if(chdir(command[2].c_str()) == -1){
				cout << command[2] << ": cannot change directory" << endl; 
			}
		}
		else{
			cout << "command error!" << endl;
		}
		return true;
	}

	bool shellNormalCommandLine(vector<string> command){

	}
};
//*/

int main(){
	shell test;
	string in;
	vector<string> CMD;
	bool builtinChecker;
	bool builtinInvoke;
	while(1){
		in = test.shellWaitForInput();
		//cout << "testcase1: " << in << endl;
		//in = "cd desktop";
		CMD = test.shellInputIntepreter(in);
		// for(int j=0; j<CMD.size(); j++){
		// 	cout << CMD[j] << endl;
		// }
		if(CMD[0] == "built-in"){
			builtinChecker = test.shellBuiltInChecker(CMD);
			if(builtinChecker == true){
				builtinInvoke = test.shellInvokeBuiltinCMD(CMD);
				if(builtinInvoke == false) return 0;
				else cout << "finish" << endl;
			}
		}
		else if(CMD[0] == "normalCMD"){

		}
	}

	return 0;
}