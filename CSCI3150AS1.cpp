// Course Title: CSCI 3150 - Operating System
// Author : Tony
// Date   : 01-03-2016


#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>
using namespace std;

///*
class shell {
public:

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

		//get input from console
		int a = getline (cin, input);
		if(a == 0) input = "ctrl-d";
		inputLen = input.size();
		if(inputLen <= 0) {
			input = "\n ";
			return input;
		}

		//format the input string with one whitespace followed only
		while(input[inputLen-1] == ' '){
			input = input.substr(0, inputLen-1);
			inputLen --;
			//cout << "eliminate trailing whitespace"<< endl;
		}
		input = input + " ";
		//cout << "testcase0: " << input << endl;
		return input;
	} 

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

		if(token[0] == "exit" || token[0] == "cd"){
			token.insert(token.begin(), "built-in");
		}
		else if(token[0] == "ctrl-d"){
			return token;
		}
		else if(token[0] == "\n"){
			token.insert(token.begin(), "continue");
		}
		else{
			token.insert(token.begin(), "normalCMD");
		}
		return token;
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

	void shellNormalCommandLine(vector<string> command){
		pid_t child_pid;
		if(!(child_pid = fork())) {
			vector<char *> commandVector;
			int cmdLenv = command.size();
			for(int i=1; i<cmdLenv; i++){
				char * tmp = new char[command[i].size()+1];
				strcpy(tmp, command[i].c_str());
				commandVector.push_back(tmp);
			}
			commandVector.push_back(NULL);
			setenv("PATH","/bin:/usr/bin:.",1);
			char **argList = &commandVector[0];
			execvp(*argList,argList);
			if(errno == ENOENT){
				cout << "No Command found..." << endl;
			}
			else{
				cout << "erron: " << errno << endl;
			}
		}
		else{
			wait(NULL);
		}
		return;
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
		CMD = test.shellInputIntepreter(in);

		if(CMD[0] == "ctrl-d"){
			cout << endl;
			break;
		}
		else if(CMD[0] == "continue"){
			continue;
		}
		else if(CMD[0] == "built-in"){
			builtinChecker = test.shellBuiltInChecker(CMD);
			if(builtinChecker == true){
				builtinInvoke = test.shellInvokeBuiltinCMD(CMD);
				if(builtinInvoke == false) return 0;
				//else cout << "finish" << endl;
			}
		}
		else if(CMD[0] == "normalCMD"){
			test.shellNormalCommandLine(CMD);
		}
	}

	return 0;
}