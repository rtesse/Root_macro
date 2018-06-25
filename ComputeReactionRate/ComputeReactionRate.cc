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
void histo_to_txtfile(TH3F* histo, string filename);
//double GetInterpCrossSection2(double E[], double Crs[], double E_x, int size);

int main(int argc, char** argv)
{
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

    TH3F* histo_eu152 = new TH3F("eu152",
                                 "a",
                                 binx, xmin, xmax,
                                 biny, ymin, ymax,
                                 binz, zmin, zmax);

    TH3F* histo_na22 = new TH3F("na22",
                                "a",
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
                double xpos = data_elossX[i]*100;
                double ypos = data_elossY[i]*100;
                double zpos = data_elossZ[i]*100;
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
                histo_eu152->Fill(xpos,ypos,zpos,steplength*value_Eu152);
                histo_na22->Fill(xpos,ypos,zpos,steplength*value_Na22);
            }

        }
        if(current_evt % (nevents/10) == 0)
        {
            cout <<"Begin treatment of event #" << current_evt << " over " << nevents << endl;
        }
        current_evt++;
    }
    cout << "Scale the histo" << endl;
    histo_eu152->Scale(1/(nevents*volume));
    histo_na22->Scale(1/(nevents*volume));

    /// Convert the histogram in a txt file and save it
    cout << "Convert in a txt file" << endl;
    string txtFilename = argv[3];
    char filename_eu152[500];
    char filename_na22[500];
    sprintf(filename_eu152,"%s_%s_eu152.txt",txtFilename.c_str(),wallname.c_str());
    sprintf(filename_na22, "%s_%s_na22.txt",txtFilename.c_str(),wallname.c_str());

    histo_to_txtfile(histo_eu152, filename_eu152);
    histo_to_txtfile(histo_na22, filename_na22);

    delete histo_eu152;
    delete histo_na22;
    cout << "Finish" <<endl;

    return 0;
}

void histo_to_txtfile(TH3F* histo, string filename)
{
    ofstream results_file;
    results_file.open(filename.c_str());

    results_file << "X" << "\t"
                 << "Y" << "\t"
                 << "Z" << "\t"
                 << "ReactionRate" << "\n";

    int NbinX = histo->GetNbinsX();
    int NbinY = histo->GetNbinsY();
    int NbinZ = histo->GetNbinsZ();

    double posX, posY, posZ, value;
    for(int i=1; i<NbinX+1; i++)
    {
        for(int j=1; j<NbinY+1; j++)
        {
            for(int k=1; k<NbinZ+1; k++)
            {
                posX = histo->GetXaxis()->GetBinCenter(i);
                posY = histo->GetYaxis()->GetBinCenter(j);
                posZ = histo->GetZaxis()->GetBinCenter(k);
                value = histo->GetBinContent(i,j,k);

                results_file << posX << "\t"
                             << posY << "\t"
                             << posZ << "\t"
                             << value << "\n";
            }
        }
    }
    results_file.close();
}
