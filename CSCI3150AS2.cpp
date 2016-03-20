//Course Title: CSCI 3150 - Operating System
//Author : Tony
//Date   : 17-03-2016
//Assignment 2

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/types.h>
#include <signal.h>
using namespace std;

//Child process id for FIFO mode
pid_t child_pid= (pid_t)-1;

//Child process id for PARA mode
vector<vector<pid_t> > childrenPid;

///*
class scheduler {
public:
    
    /*
    Open file for input string
    Split string by \t into cmd & time
    [cmd, time] pairs will appear in the outString vector for further use
    */
	vector<vector<string> > inputSplitter(string filename){
		string inputString;
		vector<vector<string> > outString;
  		ifstream cmdFile (filename);

  		if (cmdFile.is_open()) {
    		while (getline(cmdFile,inputString)) {
    			int strSize = inputString.size();
    			for(int i=0; i<strSize-1; i++){
    				if(inputString[i] == '\t'){
    					vector<string> cmdString;
    					string tempCmd = inputString.substr(0, i);
    					string tempTime = inputString.substr(i+1, (strSize-i-1));
    					cmdString.push_back(tempCmd);
    					cmdString.push_back(tempTime);
    					outString.push_back(cmdString);
    				}
    			}
    		}
  		}
  		cmdFile.close();
  		return outString;
	}

    /*
    Tokenize string into c string of vector
    results will be input parameter for exec() function
    */
    vector<char *> strTokenize(string strToToken){
        int startPoint = 0;
        vector<char *> outCmd;
        strToToken = strToToken + ' ';
        for(int i=0; i<strToToken.size(); i++){
            if(strToToken[i] == ' ' || i == strToToken.size()-1){
                if(startPoint==i) continue;
                else{
                    int cmdLen = i - startPoint;
                    string temp = strToToken.substr(startPoint, cmdLen);
                    char * copyString = new char[temp.size()+1];
                    strcpy(copyString, temp.c_str());
                    startPoint = i + 1;
                    outCmd.push_back(copyString);
                }
            }
        }
        outCmd.push_back(NULL);
        return outCmd;
    }

    /*
    First in First out scheduler mode
    Jobs are conducted one by one in child processes
    */
	void FIFOScheduler(vector<vector<string> > cmdString){
        //get the system tick per second
        
        for(int i=0; i<cmdString.size(); i++){
            //Initialize and start the timer
            clock_t startTime,endTime;
            struct tms stTime;
            struct tms enTime;
            startTime = times(&stTime);

            if(!(child_pid = fork())){
                vector<char *> cmdArglist;
                cmdArglist = strTokenize(cmdString[i][0]);
                setenv("PATH","/bin:/usr/bin:.",1);
                char **argList = &cmdArglist[0];

                //for testing
                for(int i=0; i<1000000000; i++);

                //execute the normal command with execvp() function
                execvp(*argList,argList);
                if(errno == ENOENT) cout << "No Command found..." << endl;
                else cout << "erron: " << errno << endl;           
            }
            else{
                int alarmTime = stoi(cmdString[i][1]);
                if(alarmTime > -1){
                    alarm(alarmTime);
                }
                wait(NULL);
                endTime = times(&enTime);
            
                //Print out the times usage
                schedulingReport(startTime, endTime, stTime, enTime, child_pid);
            }
        }
        return;
	}

    /*
    Parallel scheduler mode
    All jobs are conducted at the same time
    Monitor the time for each process
    */
	void ParaScheduler(vector<vector<string> > cmdString){
        //get the system tick per second

        for(int i=0; i<cmdString.size(); i++){
            pid_t monitorChild;
            pid_t jobChild;
            vector<pid_t> tempPid;
            tempPid.push_back(monitorChild);
            tempPid.push_back(jobChild);
            childrenPid.push_back(tempPid);
            
            //Monitor process counting the system time
            if(!(childrenPid[i][0] = fork())){
                childrenPid[i][0] = getpid();

                //Initialize and start the timer
                clock_t startTime,endTime;
                struct tms stTime;
                struct tms enTime;
                startTime = times(&stTime);
                
                //child process implementing input command
                if(!(childrenPid[i][1] = fork())){
                    vector<char *> cmdArglist;
                    cmdArglist = strTokenize(cmdString[i][0]);
                    setenv("PATH","/bin:/usr/bin:.",1);
                    char **argList = &cmdArglist[0];

                    childrenPid[i][1] = getpid();

                    //for time testing
                    //for(int i=0; i<100000000; i++);

                    //execute the normal command with execvp() function
                    execvp(*argList,argList);
                    if(errno == ENOENT) cout << "No Command found..." << endl;
                    else cout << "erron: " << errno << endl;
                }
                else{
                    int alarmTime = stoi(cmdString[i][1]);
                    if(alarmTime > -1){
                        alarm(alarmTime);
                    }
                    wait(NULL);
                    endTime = times(&enTime);

                    // print out the times usage
                    schedulingReport(startTime, endTime, stTime, enTime, childrenPid[i][1]);
                }
                exit(0);
            }
        }
        for(int j=0; j<cmdString.size(); j++){
            waitpid(childrenPid[j][0], NULL, 0);
        }
	}

    /* 
    Produce scheduler report with sample format:
    ----------------------
    <<Process 1234>>
    time elapsed: 0.1900
    user time   : 0.1000
    system time : 0.0100
    ----------------------
    */
    void schedulingReport(clock_t startTime, clock_t endTime, struct tms stTime, struct tms enTime, pid_t ProcessID){
        double tickPerSec = (double)sysconf(_SC_CLK_TCK);
        cout.precision(4);
        cout << fixed; 
        cout << "<<Process: " << ProcessID << " >>" << endl;
        cout << "Time Elapsed: " 
            << (endTime - startTime)/tickPerSec << endl;
        cout << "User Time: " 
        << (enTime.tms_cutime - stTime.tms_cutime)/tickPerSec << endl;
        cout << "System Time: " 
            << (enTime.tms_cstime - stTime.tms_cstime)/tickPerSec << endl;
        cout << endl;
    }
    
};

    /*
    Alarm signal handler
    When the time in alarm() runs out, this handler will be invoked
    Handler will try to kill the process and print a timeout message
    */
    void alarmHandler(int signal){
        cout << "Timeout, automatically kill the child process" << endl;
        pid_t currentPid = getpid();
        pid_t toKillPid = (pid_t) -1;
        for(int i=0; i<childrenPid.size(); i++){
            if(currentPid == childrenPid[i][0]){
                toKillPid = childrenPid[i][1];
                break;
            }          
        }
        if(child_pid != -1) kill(child_pid, SIGTERM);
        else if(toKillPid != -1) kill(toKillPid, SIGTERM);
        else{
            cout << "No child processed found to be killed";
        }
    }

//*/

int main(){
    signal(SIGALRM, alarmHandler);
	scheduler schedulertest;
	vector<vector<string> > test;
    string filename = "read.txt";
	test = schedulertest.inputSplitter(filename);
    
    //test case for FIFO mode
    schedulertest.FIFOScheduler(test);

    //test case for PARA mode
    //schedulertest.ParaScheduler(test);
	
	return 0;
}




