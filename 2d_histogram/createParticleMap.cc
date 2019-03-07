#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */

#include "TFile.h"
#include "TROOT.h"
#include "TH2F.h"
#include "TTree.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        cerr << "Error : no enough arguments" << endl;
        cerr << "1 : input filename" << endl;
        cerr << "2 : output filename" << endl;
        cerr << "2 : particle ID" << endl;
        return 0;
    }

    double ymin = -20;
    double ymax = 20;
    if(argc == 12)
    {
        ymin = atof(argv[10]);
        ymax = atof(argv[11]);
    }

    cout << "Load rebdsim librairies " << endl;
    gSystem->Load("librebdsim.so");
    gSystem->Load("libbdsimRootEvent");

    /// Open the root tree

    TFile *file = 0;
    TString filename = argv[1];
    cout << "Open file" << endl;
    file = new TFile(filename);

    TTree *tree = (TTree*)file->Get("Event"); // initialising the TREE
    int nevents = (Int_t)tree->GetEntries();
    cout <<nevents << endl;
    int partId = atoi(argv[3]);

    TFile mapfile(argv[2],"recreate");

    int nbinsX = atoi(argv[4]);
    double xlow = atoi(argv[5]);
    double xup = atoi(argv[6]);
    int nbinsY = atoi(argv[7]);
    double ylow = atoi(argv[8]);
    double yup = atoi(argv[9]);

    TH2F* histo = new TH2F("2Dmap",
                           "a",
                           nbinsX, xlow, xup,
                           nbinsY, ylow, yup);

    /// Treat the files
    Int_t current_evt=0;

    while (reader.Next())
    {
        vector<Float_t> data_elossX = *elossX;
        vector<Float_t> data_elossY = *elossY;
        vector<Float_t> data_elossZ = *elossZ;
        vector<int> data_elossPartID = *elossParentID;
        vector<Float_t> data_elossWeight = *elossWeight;

        int size = data_elossX.size();
        for (unsigned int i =0; i < size; i++)
        {
            if(data_elossPartID[i] == partId && data_elossY[i] >= ymin && data_elossY[i] <= ymax)
            {
                /// Fill the histogram
                histo->Fill(data_elossZ[i],data_elossX[i],data_elossWeight[i]);
            }
        }
        if(current_evt % (nevents/10) == 0)
        {
            cout <<"Begin treatment of event #" << current_evt << " over " << nevents << endl;
        }
        current_evt++;
    }

    mapfile.Write();
    delete histo;
    cout << "Finish" <<endl;
    return 0;
}
