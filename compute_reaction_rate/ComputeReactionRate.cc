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


using namespace std;

int main(int argc, char** argv)
{

  //Compute the reaction rate of a specfic isotope in a cell
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


    cout << "Load rebdsim librairies " << endl;
    gSystem->Load("librebdsim.so");
    gSystem->Load("libbdsimRootEvent");


    /// Open the root tree
    TFile *file = 0;
    TString filename = argv[1];
    cout << "Open file" << endl;
    file = new TFile(filename);

    cout << "Get the tree" << endl;
    TTreeReader reader("", file);
    TTreeReaderValue<Float_t> elossX(reader, "X");
    TTreeReaderValue<Float_t> elossY(reader, "Y");
    TTreeReaderValue<Float_t> elossZ(reader, "Z");
    TTreeReaderValue<Float_t> elossEne(reader, "E");
    TTreeReaderValue<Float_t> elossStepLength(reader, "L");

    TTree *tree = (TTree*)file->Get(""); // initialising the TREE
    int nevents = (Int_t)tree->GetEntries();
    cout <<nevents << endl;

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

    /// Get the wall properties and create histogram

    string wallname = argv[2];
    vector<double> wallProperties = Data::Instance()->GetWallProperties(wallname);

    double binx = wallProperties[0];
    double xmin = wallProperties[1];
    double xmax = wallProperties[2];
    double biny = wallProperties[3];
    double ymin = wallProperties[4];
    double ymax = wallProperties[5];
    double binz = wallProperties[6];
    double zmin = wallProperties[7];
    double zmax = wallProperties[8];

    TFile* output_file = 0;
    output_file = new TFile("output_histo.root","recreate");

    TH3F* histo_eu152 = new TH3F("2DMap_eu152",
                                 "eu152",
                                 binx, xmin, xmax,
                                 biny, ymin, ymax,
                                 binz, zmin, zmax);

    TH3F* histo_na22 = new TH3F("2DMap_na22",
                                "na22",
                                binx, xmin, xmax,
                                biny, ymin, ymax,
                                binz, zmin, zmax);


    double x_extent = (xmax - xmin)/binx;
    double y_extent = (ymax - ymin)/biny;
    double z_extent = (zmax - zmin)/binz;

    double volume = (x_extent*y_extent*z_extent); // Volume en cm3

    /// Treat the files
    Int_t current_evt=0;

    while (reader.Next())
    {

        Float_t xpos = *elossX;
        Float_t ypos = *elossY;
        Float_t zpos = *elossZ;
        Float_t energy = (*elossEne)*1000;
        Float_t stepLength = (*elossStepLength)*100;

        double value_Eu152 = (Data::Instance()->GetInterpCrossSection(Crs_Eu151_x,
                                                            Crs_Eu151_y,
                                                            energy,
                                                            size_crsEu151))*1e-24;


        double value_Na22 = (Data::Instance()->GetInterpCrossSection(Crs_Na22_x,
                                                            Crs_Na22_y,
                                                            energy,
                                                            size_crsNa22))*1e-24;

        /// Fill the histogram
        histo_eu152->Fill(xpos,ypos,zpos,stepLength*value_Eu152);
        histo_na22->Fill(xpos,ypos,zpos,stepLength*value_Na22);

        if(current_evt % (nevents/10) == 0)
        {
          cout <<"Begin treatment of event #" << current_evt << " over " << nevents << endl;
        }
        current_evt++;

    }

    cout << "Scale the histo" << endl;
    histo_eu152->Scale(1/(volume));
    histo_na22->Scale(1/(volume));

    cout << "Write the file" << endl;
    output_file->Write();

    delete histo_eu152;
    delete histo_na22;

    cout << "Finish" <<endl;

    return 0;
}
