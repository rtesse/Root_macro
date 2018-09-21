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
#include "TNtuple.h"

using namespace std;

int main(int argc, char** argv)
{

  //Compute the reaction rate of a specfic isotope.
  // 1: input.root
  // 2: output.root
  // 3: xmin
  // 4: xmax
  // 5: ymin
  // 6: ymax
  // 7: zmin
  // 8: zmax
  // 9: partID

/*
  if(argc < 3)
  {
    cerr << "Error : no enough arguments" << endl;
    return 0;
  }
  */

    TString input_filename = argv[1];
    TString output_filename = argv[2];
    double xmin = atof(argv[3]);
    double xmax = atof(argv[4]);
    double ymin = atof(argv[5]);
    double ymax = atof(argv[6]);
    double zmin = atof(argv[7]);
    double zmax = atof(argv[8]);
    Int_t  partID = atoi(argv[9]);

    cout << "Load rebdsim librairies " << endl;
    gSystem->Load("librebdsim.so");
    gSystem->Load("libbdsimRootEvent");

    /// Open the root tree
    TFile *input_file = 0;
    cout << "Open file" << endl;
    input_file = new TFile(input_filename);

    cout << "Get the tree" << endl;
    TTreeReader reader("Event", input_file);

    // Get the desired data (correspond to selection in analysisConfig.txt)
    TTreeReaderValue<vector<Float_t>> elossX(reader, "Eloss.X");
    TTreeReaderValue<vector<Float_t>> elossY(reader, "Eloss.Y");
    TTreeReaderValue<vector<Float_t>> elossZ(reader, "Eloss.Z");
    TTreeReaderValue<vector<Float_t>> elossEne(reader, "Eloss.preStepKineticEnergy");
    TTreeReaderValue<vector<Float_t>> elossStepLength(reader, "Eloss.stepLength");
    TTreeReaderValue<vector<int>> elossParentID(reader, "Eloss.partID");

    TTree *tree = (TTree*)input_file->Get("Event"); // initialising the TREE
    int nevents = (Int_t)tree->GetEntries();

    /// Create a file for saving ntuples
    TFile* output_file = 0;
    output_file = new TFile(output_filename,"recreate");
    TNtuple *ntuple = new TNtuple("","","X:Y:Z:E:L");

    /// Treat the files
    Int_t current_evt=0;

    while (reader.Next())
    {
        vector<Float_t> data_elossX = *elossX;
        vector<Float_t> data_elossY = *elossY;
        vector<Float_t> data_elossZ = *elossZ;
        vector<Float_t> data_elossEne = *elossEne;
        vector<Float_t> data_elossStL = *elossStepLength;
        vector<int> data_elossPartID = *elossParentID;

        int size = data_elossX.size();
        for (unsigned int i =0; i < size; i++)
        {
            if(data_elossPartID[i] == partID)
            {

                /// Extract data
                double xpos = data_elossX[i];
                double ypos = data_elossY[i];
                double zpos = data_elossZ[i];
                double energy = data_elossEne[i]*1000; // in MeV
                double steplength = data_elossStL[i]*100; // in cm

                /// Fill the ntuple
                ntuple->Fill(xpos,ypos,zpos,energy,steplength);
            }
        }
        if(current_evt % (nevents/10) == 0)
        {
            cout <<"Begin treatment of event #" << current_evt << " over " << nevents << endl;
        }
        current_evt++;
    }

    /// Save the TNuples

    output_file->Write();

    delete ntuple;
    cout << "Finish" <<endl;

    return 0;
}
