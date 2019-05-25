#include <iostream>
#include <stdio.h>     
#include <stdlib.h>
using namespace std;

#include "Exit.h"

void Exit::endShell()
{
    cout << "Exiting rshell" << endl;
    exit(0);
}
