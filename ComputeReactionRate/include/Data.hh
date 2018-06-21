#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>
/* stdlib pour exit(), et stdio pour puts() */

#include <dirent.h>
/* Pour l'utilisation des dossiers */

using namespace std;


#ifndef DATA_H
#define DATA_H

class Data
{
public:
    Data();
    ~Data();
    static Data* Instance();

    void AfficheTable(const vector<vector <double> > Table);
    vector<vector <double> > LoadTable(string filename);
    vector <double> GetWallProperties(string wallName);

private:

    static Data* fgInstance;

};

#endif // DATA_H

