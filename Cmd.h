#ifndef __CMD_H__
#define __CMD_H__
#include <vector>
#include <string>
#include <vector>
using namespace std;
class Op
{
    private:
    bool noError; //by default is true
    //char* cmdArgs[1000];
    vector<string> tempWordVec2;
    //vector<pair<int, string> > redirectionVec;
    vector<string> symbolVec;
    int universalIndex;
    vector<string> filenames;
    int previousFD;
    public:
    Op(vector<string> tempWordVec);
    void setError(bool boolean);
    vector<string> getcmdArgs(char* tempArgs[]);
    bool getError();
    int getVecSize();
    bool hasRedirection();
    string getRedirectSymbol();
    string getFile();
    void resetUniIndex();
    void setPreviousFD(int fd);
    int getPreviousFD();
    string checkNextSymbol();
    void tr(char* tempArgs[]);
};



#endif 