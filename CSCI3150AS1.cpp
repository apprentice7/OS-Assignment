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
#include <glob.h>
#include <signal.h>
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

			signal(SIGINT,SIG_DFL); 
			signal(SIGQUIT,SIG_DFL); 
			signal(SIGTERM,SIG_DFL); 
			signal(SIGTSTP,SIG_DFL);

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
			//wait(NULL);
			waitpid(child_pid, NULL, WUNTRACED);
		}
		return;
	}

	void shellWildCardCommandLine(vector<string> command){
		pid_t child_pid;

		if(!(child_pid = fork())){
			vector<string> cmdWithoutStar;
			vector<string> cmdWithStar;
			int offCount = 0;
			int starCount = 0;
			glob_t globbuf;

			signal(SIGINT,SIG_DFL); 
			signal(SIGQUIT,SIG_DFL); 
			signal(SIGTERM,SIG_DFL); 
			signal(SIGTSTP,SIG_DFL);

			cmdWithoutStar.push_back(command[1]);
			offCount++;
			for(int i=2; i<command.size(); i++){
				bool star = false;
				for(int j=0; j<command[i].size(); j++){
					if(command[i][j] == '*') star = true;
				}
				if(star == true){
					cmdWithStar.push_back(command[i]);
				}
				else{
					cmdWithoutStar.push_back(command[i]);
					offCount++;
				}
			}

			globbuf.gl_offs = offCount;
			const char * firstWildCard = cmdWithStar[0].c_str();
			glob(firstWildCard, GLOB_DOOFFS | GLOB_NOCHECK, NULL, &globbuf);
			starCount = cmdWithStar.size();
			for(int k=1; k<starCount; k++){
				const char * tempWildCard = cmdWithStar[k].c_str();
				glob(tempWildCard, GLOB_DOOFFS | GLOB_NOCHECK | GLOB_APPEND, NULL, &globbuf);
			}
			for(int w=0; w<cmdWithoutStar.size(); w++){
				char * tempCMD = new char[cmdWithoutStar[w].size()+1];
				strcpy(tempCMD, cmdWithoutStar[w].c_str());
				globbuf.gl_pathv[w] = tempCMD;
			}
			execvp(globbuf.gl_pathv[0],globbuf.gl_pathv);
			if(errno == ENOENT){
				cout << "No Command found...(star)" << endl;
			}
			else{
				cout << "erron: (star)" << errno << endl;
			}
		}
		else{
			//wait(NULL);
			waitpid(child_pid, NULL, WUNTRACED);
		}
		return;
	}

};
//*/

int main(){
	shell test;
	string in;
	vector<string> CMD;
	bool builtinChecker = false;
	bool builtinInvoke = false;
	bool wildCard = false;

	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN); 
	signal(SIGTERM,SIG_IGN); 
	signal(SIGTSTP,SIG_IGN);

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
			}
		}
		else if(CMD[0] == "normalCMD"){
			for(int i=0; i<in.size(); i++){
				if(in[i] == '*') wildCard = true;
			}
			if(wildCard == false){
				test.shellNormalCommandLine(CMD);
			}
			else{
				test.shellWildCardCommandLine(CMD);
			}	
		}
	}

	return 0;
}