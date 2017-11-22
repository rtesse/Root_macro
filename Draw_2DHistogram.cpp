#include <iostream>
#include "TMath.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TStyle.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdio.h>

using namespace std;

vector<Double_t> readfile(string filename);

void Draw_2DHistogram(double xa, double xb, double nx,
                      double ya, double yb, double ny,
                      double zmin, double zmax,
                      string data_filename,
                      string histo_filename)
{

  gROOT->Reset();
  TCanvas *c1 = new TCanvas("c1", "c1",200,10,700,500);
  gStyle->SetOptStat(0);

  // Create the histo, open the file and fill it
  TH2F *profile2d = new TH2F("h2","",
                                      nx, xa, xb,
                                      ny ,ya, yb);
  ifstream datafile(data_filename);
  string line;
  // Temporaire
  Float_t x,y,z;
  Int_t nlines = 0;

  while (!datafile.eof()) {

    datafile >> x >> y >> z;
    if (!datafile.good()) break;
    cout << "FILL DATA" << endl;
    int a = profile2d->Fill(x,y,z);
    cout << a << endl;
 }

  // Scale the histogramdd
  //Double_t norm = 1;
  //Double_t scale_theta = norm/(profile->Integral());
  //profile->Scale(scale_theta);

  // Set the x, y and z axis in log scale
  //c1->SetLogx();
  //c1->SetLogy();
  //c1->SetLogz();

  // Drawing and save
  // see drawing option at https://root.cern.ch/root/htmldoc/guides/users-guide/ROOTUsersGuide.html

  profile2d->Draw();//"E1X0 SAME HIST");
  string root_filename=histo_filename+".root";
  string eps_filename=histo_filename+".eps";
  //string png_filename=histo_filename+".png";
  c1->SaveAs(root_filename.c_str());
  c1->SaveAs(eps_filename.c_str());
  //c1->SaveAs(png_filename.c_str());

  // remove file data and bin.txt
  //remove(xbin_filename.c_str());
  //remove(data_filename.c_str());
}

vector<Double_t> readfile(string filename)
{
    ifstream inFile;
    inFile.open(filename);
    if (inFile.fail())
    {
        cerr << "Could not find file : " << filename << endl;
    }
    vector<Double_t> data;
    string S;
    while (inFile >> S)
    {
        data.push_back(stod(S));
    }
    return data;
}
