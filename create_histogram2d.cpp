#include <iostream>
#include "TMath.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TStyle.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdio.h>

using namespace std;

vector<Double_t> readfile(string filename);

void create_histogram2d(string xbin_filename,
                        string ybin_filename,
                        string data_filename,
                        string histo_filename)
{
  vector<Double_t> xbin = readfile(xbin_filename);
  vector<Double_t> ybin = readfile(ybin_filename);

  gROOT->Reset();
  TCanvas *c1 = new TCanvas("c1", "c1",900,900);
  gStyle->SetOptStat(0);

  // Create the histo, open the file and fill it
  TH2F *histo2d = new TH2F("h2","",xbin.size()-1,&xbin[0], ybin.size()-1, &ybin[0]);
  ifstream datafile(data_filename);
  string line;
  // Temporaire
  Float_t x,y;
  Int_t nlines = 0;
  while (1) {
    datafile >> x >> y;
      if (!datafile.good()) break;
      histo2d->Fill(x,y);
     }

  // Scale the histogramdd
  Double_t norm = 1;
  Double_t scale_theta = norm/(histo2d->Integral());
  histo2d->Scale(scale_theta);

  // Set the x axis in log scale_theta
  //c1->SetLogx();
  //c1->SetLogy();

  // Drawing and save
  gStyle->SetPalette(1);
  histo2d->Draw("LEGO");
  string root_filename=histo_filename+".root";
  string eps_filename=histo_filename+".eps";
  string png_filename=histo_filename+".png";
  c1->SaveAs(root_filename.c_str());
  c1->SaveAs(eps_filename.c_str());
  c1->SaveAs(png_filename.c_str());

  // remove file data and bin.txt
  remove(xbin_filename.c_str());
  remove(ybin_filename.c_str());
  remove(data_filename.c_str());
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
