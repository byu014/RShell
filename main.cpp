#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <string.h>
#include <sys/stat.h>
#include <utility>
#include <errno.h>
#include "Cmd.h"
#include "Exit.h"
#include <fcntl.h>
#include <fstream>

using namespace std;

const int stdoutFD = dup(1);
const int stdinFD = dup(0);

void pipe(Op& op, char* left[], char* right[])
{
	int pipeFD[2];
	//char* right[500];
	//op.getcmdArgs(right);
	int pid;

  pipe(pipeFD);

  pid = fork();

  if (pid == 0)
    {
      // child runs right hand

      dup2(pipeFD[0], 0);

      // close unused hald of pipe

      close(pipeFD[1]);

      // execute grep
      execvp(right[0],right);
    }
	else
    {
      
      //parent runs left hand

      dup2(pipeFD[1], 1);

      // close unused unput half of pipe

      close(pipeFD[0]);

      // execute cat

      execvp(left[0], left);
    }
}

bool forkWaitExecvp(Op& op,string filename)
{
	char* cmdArgs[200];
	vector<string> temp1;
	vector<string> temp2;
	temp1 = op.getcmdArgs(cmdArgs);
	for(int i = 0; i < temp1.size();++i)
	{
		cmdArgs[i] = (char*)temp1.at(i).c_str();
	}
	cmdArgs[temp1.size()] = NULL;
	int fileFD;
	string symbol = op.getRedirectSymbol();
	int readFD = 1;
	//int pipeFD[2];
	char* right[200];
	if(op.getPreviousFD() != -1)
	{
		readFD = op.getPreviousFD();
	}
	if(symbol == ">")
	{
		fileFD = open(filename.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0777);
		dup2(fileFD, 1);
		dup2(readFD,0);
		op.setPreviousFD(fileFD);
	}
	else if(symbol == ">>")
	{
		fileFD = open(filename.c_str(), O_RDWR | O_APPEND | O_CREAT, 0777);
		dup2(fileFD, 1);
		dup2(readFD,0);
		op.setPreviousFD(fileFD);
	}
	else if(symbol == "|")
	{
    	//op.getcmdArgs(right);
    	temp2 = op.getcmdArgs(right);
    		for(int i = 0; i < temp2.size();++i)
	{
		right[i] = (char*)temp2.at(i).c_str();
	}
	right[temp2.size()] = NULL;

	}
	pid_t pid = fork();// exec terminates program after call use fork to execvp within child
	        
	if(pid < 0)
	{
		perror("fork() error");
		exit(-1); 
	}
	if(pid == 0)
    {
    	if(symbol == "|")
    	{
    		//char* right[500];
    		//op.tr(right);
    		//op.getcmdArgs(right);


			
    		pipe(op, cmdArgs, right);
    	}
		else if(execvp(cmdArgs[0], cmdArgs) < 0)
		{
			perror("exec");
			exit(errno);
		}
    }

	dup2(stdoutFD, 1);
	dup2(stdinFD,0);
	
	int status = 0;
	wait(&status);
	if(WEXITSTATUS(status) > 0)
	{
		return false;
	}
	else
	{
		return true;
	}
					
	        
}

bool testCmd(char* cmdArgs[], int size)
{
	string flag = cmdArgs[1]; // could be flag or no flag
	const char* path;
    struct stat buf;
    string startBracket = cmdArgs[0];
    
    
    if(flag.at(0) == '-') //if this exists there is flag
    {
    	path = cmdArgs[2];

    	if(startBracket == "[")
    	{
    		
    		if(size != 4)
    		{
    			return false;
    		}
    		string endBracket = cmdArgs[3];
    		if(endBracket != "]")
    		{
    			return false;
    		}
    	}
    	

    }
    else //there is no flag
    {
    	path = cmdArgs[1];
    	if(startBracket == "[")
    	{
    		if(size != 3) // size of entire cmd should always be three if no flag
    		{
    			return false;
    		}
    		string endBracket = cmdArgs[2];
    		if(endBracket != "]")
    		{
    			return false;
    		}
    	}
    }
    
    if(stat(path, &buf) == 0) // calls stat that runs test
    {
    	if(flag == "-e" || flag.at(0) != '-') // case 1 any
    	{
    		cout << "(TRUE)" << endl;
    		return true;
    	}
        else if(flag == "-d" && S_ISDIR(buf.st_mode)) // case 2 directory
        {
        	cout << "(TRUE)" << endl;
        	return true;
        }
        else if(flag == "-f" && S_ISREG(buf.st_mode))// case 3 file
        {
            cout << "(TRUE)" << endl;
            return true;
        }
        else // nothing matched 
        {
        	cout << "(FALSE)" << endl;
        	return false;
        }

    }
    else // failed
    {
        cout << "(FALSE)" << endl;
        return false;
    }
}



int main(){
	while(1)
	{
		vector<string> command;
		vector<string> connector; 
		char* point;
		char input[1000];  
		cout << "$";
		cin.getline(input, 1000);
		string stringTemp = "";
		bool commentFlag = true;
		point = strtok(input," ");		//isolates everything before the space and adds a null character at the whitespace 
		vector<pair<int,int> > paraVec;
		//parse
		char hash = input[0];			// checks if the first command is a # or not this will help us within the while loop so that we don't get an execvp error
		
		while(point != NULL && commentFlag == true)			// at the end of the input there is a null character and that's where we will stop
		{
		    string currentWord = static_cast<string>(point); // cast point into a string to do comparisons 
		    if(currentWord == "&&") //check for &&
		    {
		        connector.push_back(currentWord);
		        command.push_back(stringTemp);
		        stringTemp.clear();
		    }
		    else if(currentWord == "||") // check for ||
		    {
		    	connector.push_back(currentWord);
		    	command.push_back(stringTemp);
		    	stringTemp.clear();
		    }
		    else if(currentWord.at(currentWord.size() - 1) == ';') // check for ;
		    {
		        connector.push_back(";");
		        currentWord.erase(currentWord.size() - 1);
		        if(stringTemp.empty() == false)
		        {
		            stringTemp += " " + currentWord;
		        }
		        else
		        {
		            stringTemp += currentWord;
		        }
		        command.push_back(stringTemp);
		        stringTemp.clear();
		        	
		    }
			
		    else if(currentWord == "#")// checks if string is only this 
		    {
		    	if(hash != '#')
		    	{
		    		command.push_back(stringTemp);
		    	}
		    	stringTemp = "";
		    	commentFlag = false;
		    }
		   
		    else if(currentWord.find('#') != string::npos && currentWord.at(0) == '#') // checks for within middle of string/ accounts for possibilities of # placements within the line
		    {
		    	commentFlag = false;
		    	if(hash != '#')
		    	{
		    		command.push_back(stringTemp);
		    	}
		    	stringTemp = "";
		    	//out << stringTemp << endl;
		    }
		    else // didn't find a connector 
		    {
		    	if(stringTemp.empty() == false)
		        {
		            stringTemp += " " + currentWord;
		        }
		        else
		        {
		            stringTemp += currentWord;
		        }
		    }
		    point = strtok(NULL, " "); // continue with strtok
		    
		    if(point == NULL && hash != '#' ) //in the case strtok has reached the last word
			{
				if(stringTemp.size() != 0)
				{
					command.push_back(stringTemp);
				}
			}
		}
		string indexCommand = "";
		string tempWord = "";
		vector<string> tempWordVec;
		vector<bool> paraBool;
		vector<Op> opVector;
		for(unsigned int i = 0; i < command.size(); ++i)
		{
			indexCommand = command.at(i);
			while(indexCommand.find('(') != string::npos) // checks for '('
			{
					pair<int,int> indexPair (i,-1);
					paraVec.push_back(indexPair);
					indexCommand.erase(0,1);
			}
			while(indexCommand.find(')') != string::npos) //checks for ')'
			{
				for(int j = paraVec.size()-1; j >= 0; --j)
				{
					if(paraVec.at(j).second == -1)
					{
						paraVec.at(j).second = i;
						j = -1;
					}
				}
				indexCommand.erase(indexCommand.size()-1,1);
			}
			for(unsigned int j = 0; j < indexCommand.size(); ++j) //parse each letter in command vector
			{
	
				if(indexCommand.at(j) != ' ')
				{
					tempWord += indexCommand.at(j);
				}
				else
				{
					tempWordVec.push_back(tempWord);
					tempWord = "";
				}
			}
			
			tempWordVec.push_back(tempWord); // take this vector in as argument
			tempWord = "";
			Op op(tempWordVec);
			opVector.push_back(op);
			tempWordVec.clear();
			
		}
		// add a forloop
		bool noParaError = true;
		for(int i = 0; i < paraVec.size(); ++i)
		{
			if(paraVec.at(i).second == -1)
			{
				i = 100; // any number bigger than size
				noParaError = false;
				cout << "missing paranthesis" << endl;
				
			}
		}
		bool shouldRun = true;
		if(noParaError == true)
		{
			for(int i = 0; i < opVector.size(); ++i)
			{	
				char* cmdArgs[sizeof(opVector.at(i))+1];
				opVector.at(i).getcmdArgs(cmdArgs);
				opVector.at(i).resetUniIndex();
				if (shouldRun == true)
				{
					string isExit = static_cast<string>(cmdArgs[0]);
		            if(isExit == "exit") //exits shell
		            {
		            	 Exit leave = Exit();
		            	 leave.endShell();
		            }
		            string statTest = cmdArgs[0];
					if(statTest == "[" || statTest == "test") // if cmd is test
					{
						bool tempBool = testCmd(cmdArgs, opVector.at(i).getVecSize());
						opVector.at(i).setError(tempBool);
					}
					else // anything else is called with execvp
					{
						do
						{
						bool tempBool = forkWaitExecvp(opVector.at(i), opVector.at(i).getFile());
						opVector.at(i).setError(tempBool);
						} while(opVector.at(i).hasRedirection() == true);
					}
				}
				shouldRun = opVector.at(i).getError(); // whether the run was successful/didn't fail or failed
				if(i < connector.size()) //determines whether right hand side should run; prevent out of bounds
				{
					if(connector.at(i) == "||") // if connector is ||
					{
						if(shouldRun == true) //left side ran so right side shouldn't; accounts for statements in paranthesis on right
						{
							shouldRun = false;
							for(int j = paraVec.size()-1; j >= 0; --j) // finds whether next command(s) is wrapped within paranthesis to skip
							{
								if(paraVec.at(j).first == i+1)
								{
									i = paraVec.at(j).second;
									paraVec.erase(paraVec.begin() + j);
									j = -1;
									if(i < connector.size() && connector.at(i) == "&&") // prevent out of bounds
									{
										shouldRun = opVector.at(i).getError();
									}
								}
							}
						}
						else if(shouldRun == false) // left side failed so run right side
						{
							shouldRun = true;
						}
					}
					else if(connector.at(i) == "&&") // if connector is &&
					{
						if(shouldRun == false) // left side failed so right side shouldn't run; accounts for statements in paranthesis on right
						{
							for(int j = paraVec.size()-1; j >= 0; --j)
							{
								if(paraVec.at(j).first == i+1)
								{
									i = paraVec.at(j).second;
									paraVec.erase(paraVec.begin() + j);
									j = -1;
								}
							}
							if(i < opVector.size()-1) // prevent out of bounds
							{
								opVector.at(i+1).setError(false); // because shouldRun is based off of the noError boolean we set this for && only
							}
						}
					}
					else if(connector.at(i) == ";") // if connector is ;
					{
						shouldRun = true; // result of this does not affect right side so shouldRun set to true 
					}

				}
			}
		}
			
		

	}


	return 0;
}