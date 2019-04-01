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
#include "TProfile3D.h"

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

    vector<vector<double> > h10_protons = LoadTable("../h10_coeffs/new_h10protons.txt");
    vector<vector<double> > h10_neutrons = LoadTable("../h10_coeffs/new_h10neutrons.txt");
    vector<vector<double> > h10_electrons = LoadTable("../h10_coeffs/new_h10electrons.txt");
    vector<vector<double> > h10_photons = LoadTable("../h10_coeffs/new_h10photons.txt");


    int size_h10protons = h10_protons.size();
    int size_h10neutrons = h10_neutrons.size();
    int size_h10electrons = h10_electrons.size();
    int size_h10photons = h10_photons.size();

    double h10_protons_x[size_h10protons];
    double h10_protons_y[size_h10protons];

    double h10_neutrons_x[size_h10neutrons];
    double h10_neutrons_y[size_h10neutrons];

    double h10_electrons_x[size_h10electrons];
    double h10_electrons_y[size_h10electrons];

    double h10_photons_x[size_h10photons];
    double h10_photons_y[size_h10photons];

    extract_columns(h10_protons,1,h10_protons_x,h10_protons_y);
    extract_columns(h10_neutrons,1,h10_neutrons_x,h10_neutrons_y);
    extract_columns(h10_electrons,1,h10_electrons_x,h10_electrons_y);
    extract_columns(h10_photons,1,h10_photons_x,h10_photons_y);

    /// Get the wall properties and create the 2D histograms


    TFile* output_file = 0;
    output_file = new TFile(output_filename,"recreate");

    TProfile3D* histo_3D_protons = new TProfile3D("protons_3D",
				 "H1O_3D_protons",
				 binx, xmin, xmax,
				 biny, ymin, ymax,
				 binz, zmin, zmax);

    TProfile3D* histo_3D_neutrons = new TProfile3D("neutrons_3D",
				 "H1O_3D_neutrons",
				 binx, xmin, xmax,
				 biny, ymin, ymax,
				 binz, zmin, zmax);

    TProfile3D* histo_3D_electrons = new TProfile3D("electrons_3D",
				 "H1O_3D_electrons",
				 binx, xmin, xmax,
				 biny, ymin, ymax,
				 binz, zmin, zmax);

    TProfile3D* histo_3D_photons = new TProfile3D("photons_3D",
				 "H1O_3D_photons",
				 binx, xmin, xmax,
				 biny, ymin, ymax,
				 binz, zmin, zmax);

    TProfile2D* histo_xy_protons = new TProfile2D("protons_xy",
                                 "H10_xy_protons",
                                 binx, xmin, xmax,
                                 biny, ymin, ymax);

    TProfile2D* histo_yz_protons = new TProfile2D("protons_yz",
                                 "H10_yz_protons",
                                 biny, ymin, ymax,
                                 binz, zmin, zmax);

    TProfile2D* histo_zx_protons = new TProfile2D("protons_zx",
                                 "H10_zx_protons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo_xy_neutrons = new TProfile2D("neutrons_xy",
                                 "H10_xy_neutrons",
                                 binx, xmin, xmax,
                                 biny, ymin, ymax);

    TProfile2D* histo_yz_neutrons = new TProfile2D("neutrons_yz",
                                 "H10_yz_neutrons",
                                 biny, ymin, ymax,
                                 binz, zmin, zmax);

    TProfile2D* histo_zx_neutrons = new TProfile2D("neutrons_zx",
                                 "H10_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo1_zx_neutrons = new TProfile2D("h1_neutrons_zx",
                                 "h1_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo2_zx_neutrons = new TProfile2D("h2_neutrons_zx",
                                 "h2_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo3_zx_neutrons = new TProfile2D("h3_neutrons_zx",
                                 "h3_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo4_zx_neutrons = new TProfile2D("h4_neutrons_zx",
                                 "h4_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo5_zx_neutrons = new TProfile2D("h5_neutrons_zx",
                                 "h5_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo6_zx_neutrons = new TProfile2D("h6_neutrons_zx",
                                 "h6_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo7_zx_neutrons = new TProfile2D("h7_neutrons_zx",
                                 "h7_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo8_zx_neutrons = new TProfile2D("h8_neutrons_zx",
                                 "h8_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    TProfile2D* histo9_zx_neutrons = new TProfile2D("h9_neutrons_zx",
                                 "h9_zx_neutrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);
		
    TProfile2D* histo_xy_electrons = new TProfile2D("electrons_xy",
                                 "H10_xy_electrons",
                                 binx, xmin, xmax,
                                 biny, ymin, ymax);

    TProfile2D* histo_yz_electrons = new TProfile2D("electrons_yz",
                                 "H10_yz_electrons",
                                 biny, ymin, ymax,
                                 binz, zmin, zmax);

    TProfile2D* histo_zx_electrons = new TProfile2D("electrons_zx",
                                 "H10_zx_electrons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);
		
    TProfile2D* histo_xy_photons = new TProfile2D("photons_xy",
                                 "H10_xy_photons",
                                 binx, xmin, xmax,
                                 biny, ymin, ymax);

    TProfile2D* histo_yz_photons = new TProfile2D("photons_yz",
                                 "H10_yz_photons",
                                 biny, ymin, ymax,
                                 binz, zmin, zmax);

    TProfile2D* histo_zx_photons = new TProfile2D("photons_zx",
                                 "H10_zx_photons",
                                 binz, zmin, zmax,
                                 binx, xmin, xmax);

    double x_extent = ((xmax - xmin)/binx)*100;
    double y_extent = ((ymax - ymin)/biny)*100;
    double z_extent = ((zmax - zmin)/binz)*100;
    cout << "One cellule has the following dimensions : " << x_extent << " cm in x, " << y_extent << " cm in y and " << z_extent << " cm in z." <<endl;

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
		histo_3D_protons->Fill(xpos,ypos,zpos,stepLength*value_h10_proton);
	}

	else if(ID == 2112)
	{
		double value_h10_neutron = (GetInterp_h10(h10_neutrons_x,
						          h10_neutrons_y,
                                                          energy,
                                                          size_h10neutrons))*1000000; // To get the h10 value in micro Sievert
                //cout << "Neutron encountered. The value of h10 is :" << value_h10_neutron << endl;
		histo_xy_neutrons->Fill(xpos,ypos,stepLength*value_h10_neutron);
		histo_yz_neutrons->Fill(ypos,zpos,stepLength*value_h10_neutron);
		histo_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		histo_3D_neutrons->Fill(xpos,ypos,zpos,stepLength*value_h10_neutron);

		if (ypos > -4.5 && ypos < -3.5)
		{
			histo1_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		}
		else if (ypos > -3.5 && ypos < -2.5)
		{
			histo2_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		}
		else if (ypos > -2.5 && ypos < -1.5)
		{
			histo3_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		}
		else if (ypos > -1.5 && ypos < -0.5)
		{
			histo4_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		}
		else if (ypos > -0.5 && ypos < 0.5)
		{
			histo5_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		}
		else if (ypos > 0.5 && ypos < 1.5)
		{
			histo6_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		}
		else if (ypos > 1.5 && ypos < 2.5)
		{
			histo7_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		}
		else if (ypos > 2.5 && ypos < 3.5)
		{
			histo8_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		}
		else if (ypos > 3.5 && ypos < 4.5)
		{
			histo9_zx_neutrons->Fill(zpos,xpos,stepLength*value_h10_neutron);
		}
	}

	else if(ID == 11)
	{
		double value_h10_electron = (GetInterp_h10(h10_electrons_x,
						           h10_electrons_y,
                                                           energy,
                                                           size_h10electrons))*1000000; // To get the h10 value in micro Sievert
                //cout << "Electron encountered. The value of h10 is :" << value_h10_electron << endl;
		histo_xy_electrons->Fill(xpos,ypos,stepLength*value_h10_electron);
		histo_yz_electrons->Fill(ypos,zpos,stepLength*value_h10_electron);
		histo_zx_electrons->Fill(zpos,xpos,stepLength*value_h10_electron);
		histo_3D_electrons->Fill(xpos,ypos,zpos,stepLength*value_h10_electron);
	}

	else if(ID == 22)
	{
		double value_h10_photon = (GetInterp_h10(h10_photons_x,
						           h10_photons_y,
                                                           energy,
                                                           size_h10photons))*1000000; // To get the h10 value in micro Sievert
                //cout << "Photon encountered. The value of h10 is :" << value_h10_photon << endl;
		histo_xy_photons->Fill(xpos,ypos,stepLength*value_h10_photon);
		histo_yz_photons->Fill(ypos,zpos,stepLength*value_h10_photon);
		histo_zx_photons->Fill(zpos,xpos,stepLength*value_h10_photon);
		histo_3D_photons->Fill(xpos,ypos,zpos,stepLength*value_h10_photon);
	}

        if(current_evt % (nevents/10) == 0)
        {
          cout <<"Begin treatment of event #" << current_evt << " over " << nevents << endl;
        }
        current_evt++;
    }

    cout << "Scaling the histograms" << endl;

    histo_3D_protons->Scale(1/(volume));
    histo_3D_neutrons->Scale(1/(volume));
    histo_3D_electrons->Scale(1/(volume));
    histo_3D_photons->Scale(1/(volume));

    histo_xy_protons->Scale(1/(volume));
    histo_yz_protons->Scale(1/(volume));
    histo_zx_protons->Scale(1/(volume));

    histo_xy_neutrons->Scale(1/(volume));
    histo_yz_neutrons->Scale(1/(volume));
    histo_zx_neutrons->Scale(1/(volume));

    histo_xy_electrons->Scale(1/(volume));
    histo_yz_electrons->Scale(1/(volume));
    histo_zx_electrons->Scale(1/(volume));

    histo_xy_photons->Scale(1/(volume));
    histo_yz_photons->Scale(1/(volume));
    histo_zx_photons->Scale(1/(volume));

    histo1_zx_neutrons->Scale(1/(volume));
    histo2_zx_neutrons->Scale(1/(volume));
    histo3_zx_neutrons->Scale(1/(volume));
    histo4_zx_neutrons->Scale(1/(volume));
    histo5_zx_neutrons->Scale(1/(volume));
    histo6_zx_neutrons->Scale(1/(volume));
    histo7_zx_neutrons->Scale(1/(volume));
    histo8_zx_neutrons->Scale(1/(volume));
    histo9_zx_neutrons->Scale(1/(volume));

    cout << "Writing the file" << endl;
    output_file->Write();

    cout << "Game over" <<endl;

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
        cout << filename << " Succesfully opened the file" + filename << endl;
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
