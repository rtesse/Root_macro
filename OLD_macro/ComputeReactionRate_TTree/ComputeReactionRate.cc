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
#include "Data.hh"
#include "TNtuple.h"

using namespace std;
void histo_to_txtfile(TH3F* histo, string filename);
//double GetInterpCrossSection2(double E[], double Crs[], double E_x, int size);

int main(int argc, char** argv)
{

  //Compute the reaction rate of a specfic isotope.
  // 1: filename root file
  // 2: Wall name
  // 3: filename txt file

/*
  if(argc < 3)
  {
    cerr << "Error : no enough arguments" << endl;
    return 0;
  }
  */


    cout << "Load rebdsim librairies " << endl;
    gSystem->Load("librebdsim.so");
    gSystem->Load("libbdsimRootEvent");

    /// Open the root tree
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

    TTree *tree = (TTree*)file->Get("Event"); // initialising the TREE
    int nevents = (Int_t)tree->GetEntries();
    cout <<nevents << endl;
    int neutron_partId = 2112; // neutron particle

    /// Load Table for the cross section

    vector<vector<double> > Crs_Eu151 = Data::Instance()->LoadTable("CrossSection/CrossSection_Eu151.txt");
    vector<vector<double> > CrsTable_spallation = Data::Instance()->LoadTable("CrossSection/SpallationCrossSection_Concrete_QGSP_BIC_HP.txt");

    int size_crsNa22 = CrsTable_spallation.size();
    int size_crsEu151 = Crs_Eu151.size();

    double Crs_Na22_x[size_crsNa22];
    double Crs_Na22_y[size_crsNa22];

    double Crs_Eu151_x[size_crsEu151];
    double Crs_Eu151_y[size_crsEu151];

    Data::Instance()->extract_columns(Crs_Eu151,1,Crs_Eu151_x,Crs_Eu151_y);
    Data::Instance()->extract_columns(CrsTable_spallation,1,Crs_Na22_x,Crs_Na22_y);

        /// Create a file for saving ntuples
    TFile* f = 0;
    TString newfilename = argv[2];
    f = new TFile(newfilename,"recreate");
    TNtuple *ntuple_reactionrate = new TNtuple("eu152","eu152","x:y:z:r_eu152:r_na22");

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
            if(data_elossPartID[i] == neutron_partId)
            {

                /// Compute the reaction rate for Eu and Na
                double xpos = data_elossX[i];
                double ypos = data_elossY[i];
                double zpos = data_elossZ[i];
                double energy = data_elossEne[i]*1000;
                double steplength = data_elossStL[i]*100;

                double value_Eu152 = (Data::Instance()->GetInterpCrossSection(Crs_Eu151_x,
                                                            Crs_Eu151_y,
                                                            energy,
                                                            size_crsEu151))*1e-24;


                double value_Na22 = (Data::Instance()->GetInterpCrossSection(Crs_Na22_x,
                                                            Crs_Na22_y,
                                                            energy,
                                                            size_crsNa22))*1e-24;

                /// Fill the histogram
                ntuple_reactionrate->Fill(xpos,ypos,zpos,steplength*value_Eu152,steplength*value_Na22);
            }

        }
        if(current_evt % (nevents/10) == 0)
        {
            cout <<"Begin treatment of event #" << current_evt << " over " << nevents << endl;
        }
        current_evt++;
    }

    /// Save the TNuples

    f->Write();

    delete ntuple_reactionrate;
    cout << "Finish" <<endl;

    return 0;
}
