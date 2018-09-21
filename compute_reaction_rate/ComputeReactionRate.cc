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
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include "TFile.h"
#include "TH3.h"
#include "TSystem.h"
#include "TTree.h"
#include "TTreeReader.h"


using namespace std;

void extract_columns(vector<vector<double> >CrsTable, int flag_isotope, double x[], double y[]);
double GetInterpCrossSection(double E[], double Crs[], double E_x, int size);
vector<vector<double> > LoadTable(string filename);

int main(int argc, char** argv)
{

  //Compute the reaction rate of a specfic isotope in a cell
  // 1: input.root
  // 2: output.root
  // 3: xmin
  // 4: xmax
  // 5: nbinx
  // 6: ymin
  // 7: ymax
  // 8: nbiny
  // 9: zmin
  // 10: zmax
  // 11: nbinz

  TString input_filename = argv[1];
  TString output_filename = argv[2];
  double xmin = atof(argv[3]);
  double xmax = atof(argv[4]);
  Int_t binx = atoi(argv[5]);
  double ymin = atof(argv[6]);
  double ymax = atof(argv[7]);
  Int_t biny = atoi(argv[8]);
  double zmin = atof(argv[9]);
  double zmax = atof(argv[10]);
  Int_t binz = atoi(argv[11]);

    cout << "Load rebdsim librairies " << endl;
    gSystem->Load("librebdsim.so");
    gSystem->Load("libbdsimRootEvent");


    /// Open the root tree
    TFile *file = 0;
    cout << "Open file" << endl;
    file = new TFile(input_filename);

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

    vector<vector<double> > Crs_Eu151 = LoadTable("CrossSection/CrossSection_Eu151.txt");
    vector<vector<double> > CrsTable_spallation = LoadTable("CrossSection/SpallationCrossSection_Concrete_QGSP_BIC_HP.txt");

    int size_crsNa22 = CrsTable_spallation.size();
    int size_crsEu151 = Crs_Eu151.size();

    double Crs_Na22_x[size_crsNa22];
    double Crs_Na22_y[size_crsNa22];

    double Crs_Eu151_x[size_crsEu151];
    double Crs_Eu151_y[size_crsEu151];

    extract_columns(Crs_Eu151,1,Crs_Eu151_x,Crs_Eu151_y);
    extract_columns(CrsTable_spallation,1,Crs_Na22_x,Crs_Na22_y);

    /// Get the wall properties and create histogram


    TFile* output_file = 0;
    output_file = new TFile(output_filename,"recreate");

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

        double value_Eu152 = (GetInterpCrossSection(Crs_Eu151_x,
                                                            Crs_Eu151_y,
                                                            energy,
                                                            size_crsEu151))*1e-24;


        double value_Na22 = (GetInterpCrossSection(Crs_Na22_x,
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


void extract_columns(vector<vector<double> >CrsTable, int flag_isotope, double x[], double y[])
{
    for (unsigned i =0; i < CrsTable.size(); i++)
    {
        x[i] = CrsTable[i][0];
        y[i] = CrsTable[i][flag_isotope];
    }
}

double GetInterpCrossSection(double E[], double Crs[], double E_x, int size)
{
    int index;
    if(E_x < E[0] || E_x > E[size-1])
    {
        return 0;
    }
    else
    {
        index = static_cast<int>(gsl_interp_bsearch (E, E_x, 0, size)); // index low
    }

    double E_a = E[index];
    double E_b = E[index+1];
    double Crs_a = Crs[index];
    double Crs_b = Crs[index+1];

    double Crs_x;

    double pente = (Crs_b-Crs_a)/(E_b-E_a);
    Crs_x = pente*(E_x-E_a)+Crs_a;

    return Crs_x;
}


vector<vector<double> > LoadTable(string filename)
{
    vector < vector <double> > Table;

    ifstream fichier(filename.c_str(), ios::in);  // on ouvre en lecture
    if(fichier)  // si l'ouverture a fonctionné
    {
        cout << filename << " Succesfuly Open"<< endl;
        vector <double> column;
        int currentRow = 0;
        string contenu;
        string value;

        while(getline(fichier, contenu))
        {
            size_t tabPosition = 0;

            while(tabPosition != std::string::npos)
            {
                tabPosition = contenu.find("\t");
                value = contenu.substr(0,tabPosition);
                column.push_back((strtod(value.c_str(),NULL)));
                contenu = contenu.substr(tabPosition+1);
            }

            Table.push_back(column);

            currentRow ++;
            column.clear();

        }
        fichier.close();
    }
    else
    {
        cerr << "Impossible d'ouvrir le fichier "+ filename << endl;
    }
    return Table;
}
