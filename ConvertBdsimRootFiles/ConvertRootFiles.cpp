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
#include "TH3.h"
#include "TSystem.h"
#include "TTree.h"
#include "TTreeReader.h"


using namespace std;

int main(int argc, char** argv)
{

    if(argc < 3)
    {
        cerr << "Error : no enough arguments" << endl;
        return 0;
    }
    cout << "Load rebdsim librairies " << endl;
    gSystem->Load("librebdsim.so");
    gSystem->Load("libbdsimRootEvent");

    TFile *file = 0;
    TString filename = argv[1];
    cout << "Open file" << endl;
    file = new TFile(filename);

    cout << "Get the tree" << endl;
    TTreeReader reader("Event", file);
    TTreeReaderValue<vector<Float_t>> elossX(reader, "Eloss.X");
    TTreeReaderValue<vector<Float_t>> elossY(reader, "Eloss.Y");
    TTreeReaderValue<vector<Float_t>> elossZ(reader, "Eloss.Z");
    TTreeReaderValue<vector<Float_t>> elossEne(reader, "Eloss.preStepKineticEnergy");
    TTreeReaderValue<vector<Float_t>> elossStepLength(reader, "Eloss.stepLength");
    TTreeReaderValue<vector<int>> elossParentID(reader, "Eloss.partID");

    cout <<"Open the file " << endl;
    ofstream results_file;
    results_file.open(argv[2]);

    // Loop over all entries of the TTree or TChain.
    results_file << "X" << "\t"
                 << "Y" << "\t"
                 << "Z" << "\t"
                 << "Energy" << "\t"
                 << "StepLength" << "\n";

    int current_evt = 0;
    TTree *tree = (TTree*)file->Get("Event"); // initialising the TREE
    int nevents = (Int_t)tree->GetEntries();
    int partId = 2112; // neutron particle

    while (reader.Next()) {
        vector<Float_t> data_elossX = *elossX;
        vector<Float_t> data_elossY = *elossY;
        vector<Float_t> data_elossZ = *elossZ;
        vector<Float_t> data_elossEne = *elossEne;
        vector<Float_t> data_elossStL = *elossStepLength;
        vector<int> data_elossPartID = *elossParentID;

        int size = data_elossX.size();
        for (unsigned int i =0; i < size; i++)
        {
            if(data_elossPartID[i] == partId)
            {
                results_file << data_elossX[i] << "\t"
                             << data_elossY[i] << "\t"
                             << data_elossZ[i] << "\t"
                             << data_elossEne[i] << "\t"
                             << data_elossStL[i] << "\n";
            }

            if(current_evt % (nevents/100) ==0)
            {
                cout <<"Begin treatment of event #" <<current_evt << endl;
            }
            current_evt++;
        }

        results_file.close();
        cout << "Finish" <<endl;
        return 0;
    }
}
