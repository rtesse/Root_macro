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

int main(int argc, char **argv) {

    /// Extract data of a root file generated by bdsim (can go in rebdsim).
    // 1: input.root
    // 2: output.root
    // 3: element

    /// Output columns
    // x y s E xp yp zp partID

/*
  if(argc < 3)
  {
    cerr << "Error : no enough arguments" << endl;
    return 0;
  }
  */

    TString input_filename = argv[1];
    TString output_filename = argv[2];
    TString element_name = argv[3];

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
    TTreeReaderValue<vector<Float_t>> element_x(reader, element_name + ".x");
    TTreeReaderValue<vector<Float_t>> element_y(reader, element_name + ".y");
    //TODO need to fixe the problem when stored z or s in the TTree
    TTreeReaderValue<Float_t > element_s(reader, element_name+".S");

    TTreeReaderValue<vector<Float_t>> element_E(reader, element_name + ".energy");
    TTreeReaderValue<vector<Float_t>> element_xp(reader, element_name + ".xp");
    TTreeReaderValue<vector<Float_t>> element_yp(reader, element_name + ".yp");
    TTreeReaderValue<vector<Float_t>> element_zp(reader, element_name + ".zp");
    TTreeReaderValue<vector<Int_t>> element_partId(reader, element_name + ".partID");
    TTreeReaderValue<vector<Int_t>> element_parentId(reader, element_name + ".parentID");

    TTree *tree = (TTree *) input_file->Get("Event"); // initialising the TREE
    int nevents = (Int_t) tree->GetEntries();

    /// Create a file for saving ntuples
    TFile *output_file = 0;
    output_file = new TFile(output_filename, "recreate");
    TNtuple *ntuple = new TNtuple("Data", "Data", "X:Y:S:E:PX:PY:PZ:partID:parentID");

    /// Treat the files
    Int_t current_evt = 0;

    while (reader.Next()) {
        vector<Float_t> data_elementx = *element_x;
        vector<Float_t> data_elementy = *element_y;
        Float_t spos = *element_s;
        vector<Float_t> data_elementE = *element_E;
        vector<Float_t> data_elementxp = *element_xp;
        vector<Float_t> data_elementyp = *element_yp;
        vector<Float_t> data_elementzp = *element_zp;
        vector<Int_t> data_elementpartID = *element_partId;
        vector<Int_t> data_elementparentID = *element_parentId;

        int size = data_elementx.size();
        for (unsigned int i = 0; i < size; i++) {
            /// Extract data
            double xpos = data_elementx[i];
            double ypos = data_elementy[i];
            double energy = data_elementE[i] * 1000; // in MeV
            double xp = data_elementxp[i];
            double yp = data_elementyp[i];
            double zp = data_elementzp[i];
            int partID = data_elementpartID[i];
            int parenttID = data_elementparentID[i];
            ntuple->Fill(xpos,ypos,spos,energy,xp,yp,zp,partID,parenttID);
        }
        if (current_evt % (nevents / 10) == 0) {
            cout << "Begin treatment of event #" << current_evt << " over " << nevents << endl;
        }
        current_evt++;
    }

    /// Save the TNuples
    output_file->Write();
    delete ntuple;

    cout << "Finish" << endl;

    return 0;
}
