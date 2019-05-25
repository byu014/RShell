#include <iostream>
#include "Cmd.h"
using namespace std;

Op::Op(vector<string> tempWordVec)
{
    noError = true;
    universalIndex = 0;
    previousFD = -1;
    for(int i = 0; i < tempWordVec.size(); ++i)
    {
        if(tempWordVec.at(i) == ">" || tempWordVec.at(i) == ">>")
        {
            symbolVec.push_back(tempWordVec.at(i));
            filenames.push_back(tempWordVec.at(i+1));
            i += 1;
        }
        else if(tempWordVec.at(i) == "|")
        {
            symbolVec.push_back("|");
            tempWordVec2.push_back(tempWordVec.at(i));
        }
        else if(tempWordVec.at(i) == "<")
        {
            
        }
        else
        {
            tempWordVec2.push_back(tempWordVec.at(i));
        }
    }
}

vector<string> Op::getcmdArgs(char* tempArgs[])
{
    int index = universalIndex;
    int sizecounter = 1;
    vector<string>temp;
    for(int i = index; i < tempWordVec2.size(); ++i)
    {
        sizecounter += 1;
        if(tempWordVec2.at(i) == "|")
        {
            sizecounter -= 1;
            universalIndex = i+1;
            break;
        }
        else
        {
            temp.push_back(tempWordVec2.at(i));
            tempArgs[i] = (char*) tempWordVec2.at(i).c_str();

            
        }
        // if(tempWordVec2.at(i) == "tr")
        // {
        //     tr(tempArgs);
        // }
    }
    //tempArgs[tempWordVec2.size()] = NULL;
    tempArgs[sizecounter] = NULL;
        return temp;
}


void Op::tr(char* tempArgs[])
{
    // tempArgs[0] = (char*)"tr";
    // tempArgs[1] = (char*)"A-Z";
    // tempArgs[2] = (char*)"a-z";
    // tempArgs[3] = NULL;
}

bool Op::getError()
{
    return noError;
}

void Op::setError(bool boolean)
{
    noError = boolean;
}

int Op::getVecSize()
{
    return tempWordVec2.size();
}

bool Op::hasRedirection()
{
    if(symbolVec.size() > 0)
    {
        return true;
    }
    return false;
}

string Op::getRedirectSymbol()
{
    if(symbolVec.size() != 0)
    {
        string returnstuff = symbolVec.at(0);
        //redirectionVec.erase(0);
        symbolVec.erase(symbolVec.begin() + 0);
        return returnstuff;
    }
    return "";
}

string Op::getFile()
{
    if(filenames.size() != 0)
    {
        string returnval = filenames.at(0);
        filenames.erase(filenames.begin() + 0);
        return returnval;
    }
    return "";
}

void Op::resetUniIndex()
{
    universalIndex = 0;
}

void Op::setPreviousFD(int fd)
{
    previousFD = fd;
}

int Op::getPreviousFD()
{
    return previousFD;
}

string Op::checkNextSymbol()
{
    if(symbolVec.size() > 1)
    {
        return symbolVec.at(1);
    }
    else
    {
        return "";
    }
}
