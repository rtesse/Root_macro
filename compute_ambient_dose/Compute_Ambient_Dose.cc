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
#include "TH2.h"
#include "TProfile2D.h"

using namespace std;

void extract_columns(vector<vector<double> >h10_Table, int flag_isotope, double x[], double y[]);
double GetInterp_h10(double E[], double h10[], double E_x, int size);
vector<vector<double> > LoadTable(string filename);

int main(int argc, char** argv)
{

  //Compute the ambient dose 2D maps in a given world volume
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
  // 12: option

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
 // TString option = atoi(argv[12]);


    /// Open the root tree
    TFile *file = 0;
    cout << "Open file" << endl;
    file = new TFile(input_filename);

    cout << "Get the tree" << endl;
    TTreeReader reader("Data", file);
    TTreeReaderValue<Float_t> elossX(reader, "X");
    TTreeReaderValue<Float_t> elossY(reader, "Y");
    TTreeReaderValue<Float_t> elossZ(reader, "Z");
    TTreeReaderValue<Float_t> elossEne(reader, "E");
    TTreeReaderValue<Float_t> elossStepLength(reader, "L");
    TTreeReaderValue<Float_t> particleID(reader, "PartId");

    TTree *tree = (TTree*)file->Get("Data"); // initialising the TREE
    int nevents = (Int_t)tree->GetEntries();
    cout <<nevents << endl;

    /// Load the h10 coefficients Tables

    vector<vector<double> > h10_protons = LoadTable("../h10_coeffs/h10protons.txt");
    vector<vector<double> > h10_neutrons = LoadTable("../h10_coeffs/h10neutrons.txt");

    int size_h10protons = h10_protons.size();
    int size_h10neutrons = h10_neutrons.size();

    double h10_protons_x[size_h10protons];
    double h10_protons_y[size_h10protons];

    double h10_neutrons_x[size_h10neutrons];
    double h10_neutrons_y[size_h10neutrons];

    extract_columns(h10_protons,1,h10_protons_x,h10_protons_y);
    extract_columns(h10_neutrons,1,h10_neutrons_x,h10_neutrons_y);

    /// Get the wall properties and create the 2D histograms


    TFile* output_file = 0;
    output_file = new TFile(output_filename,"recreate");

    TProfile2D* histo_xy_protons = new TProfile2D("protons_xy",
                                 "H10_xy_protons",
                                 binx, xmin, xmax,
                                 biny, ymin, ymax);

    TProfile2D* histo_yz_protons = new TProfile2D("protons_yz",
                                 "H10_yz_protons",
                                 binx, ymin, ymax,
                                 biny, zmin, zmax);

    TProfile2D* histo_zx_protons = new TProfile2D("protons_zx",
                                 "H10_zx_protons",
                                 binx, zmin, zmax,
                                 biny, xmin, xmax);

    TProfile2D* histo_xy_neutrons = new TProfile2D("neutrons_xy",
                                 "H10_xy_neutrons",
                                 binx, xmin, xmax,
                                 biny, ymin, ymax);

    TProfile2D* histo_yz_neutrons = new TProfile2D("neutrons_yz",
                                 "H10_yz_neutrons",
                                 binx, ymin, ymax,
                                 biny, zmin, zmax);

    TProfile2D* histo_zx_neutrons = new TProfile2D("neutrons_zx",
                                 "H10_zx_neutrons",
                                 binx, zmin, zmax,
                                 biny, xmin, xmax);


    double x_extent = ((xmax - xmin)/binx)*100;
    double y_extent = ((ymax - ymin)/biny)*100;
    double z_extent = ((zmax - zmin)/binz)*100;
    cout << "One cellule has the following dimensions :" << x_extent << "cm in x," << y_extent << "cm3 in y, and" << z_extent << "cm in z" <<endl;

    double volume = (x_extent*y_extent*z_extent); // To calculate the volume en cm3

    /// Treat the files
    Int_t current_evt=0;

    while (reader.Next())
    {

        Float_t xpos = *elossX;
        Float_t ypos = *elossY;
        Float_t zpos = *elossZ;
        Float_t energy = (*elossEne)*0.001; // To convert the energy in GeV
        Float_t stepLength = (*elossStepLength);
	Float_t ID = *particleID;

	if(ID == 2212)
	{
		double value_h10_proton = (GetInterp_h10(h10_protons_x,
						         h10_protons_y,
                                                         energy,
                                                         size_h10protons))*1000000; // To get the h10 value in micro Sievert
                //cout << "Proton encountered. The value of h10 is :" << value_h10_proton << endl;
		histo_xy_protons->Fill(xpos,ypos,stepLength*value_h10_proton);
		histo_yz_protons->Fill(ypos,zpos,stepLength*value_h10_proton);
		histo_zx_protons->Fill(zpos,xpos,stepLength*value_h10_proton);
	}

	else if (ID == 2112)
	{
		double value_h10_neutron = (GetInterp_h10(h10_neutrons_x,
						          h10_neutrons_y,
                                                          energy,
                                                          size_h10neutrons))*1000000; // To get the h10 value in micro Sievert
                //cout << "Neutron encountered. The value of h10 is :" << value_h10_neutron << endl;
		histo_xy_neutrons->Fill(xpos,ypos,stepLength*value_h10_neutron);
		histo_yz_neutrons->Fill(ypos,zpos,stepLength*value_h10_neutron);
		histo_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
	}

        if(current_evt % (nevents/10) == 0)
        {
          cout <<"Begin treatment of event #" << current_evt << " over " << nevents << endl;
        }
        current_evt++;
    }


    cout << "Scaling of the histos" << endl;
    histo_xy_protons->Scale(1/(volume));
    histo_yz_protons->Scale(1/(volume));
    histo_zx_protons->Scale(1/(volume));

    histo_xy_neutrons->Scale(1/(volume));
    histo_yz_neutrons->Scale(1/(volume));
    histo_zx_neutrons->Scale(1/(volume));

    cout << "Write the file" << endl;
    output_file->Write();

    cout << "Finish" <<endl;

    return 0;
}


void extract_columns(vector<vector<double> >h10_Table, int flag_isotope, double x[], double y[])
{
    for (unsigned i =0; i < h10_Table.size(); i++)
    {
        x[i] = h10_Table[i][0];
        y[i] = h10_Table[i][flag_isotope];
    }
}

double GetInterp_h10(double E[], double h10[], double E_x, int size)
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
    double h10_a = h10[index];
    double h10_b = h10[index+1];

    double h10_x;

    double pente = (h10_b-h10_a)/(E_b-E_a);
    h10_x = pente*(E_x-E_a)+h10_a;

    return h10_x;
}


vector<vector<double> > LoadTable(string filename)
{
    vector < vector <double> > Table;

    ifstream fichier(filename.c_str(), ios::in);  // To open the file in read mode
    if(fichier)  // if the file has been successfully opened
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
        cerr << "Cannot open the file "+ filename << endl;
    }
    return Table;
}
