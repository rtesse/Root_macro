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

void Draw_2DProfile(string xbin_filename,
                    string ybin_filename,
                    double zmin, double zmax,
                    string data_filename,
                    string histo_filename)

{

  vector<Double_t> xbin = readfile(xbin_filename);
  vector<Double_t> ybin = readfile(ybin_filename);

  gROOT->Reset();
  TCanvas * c1 = new TCanvas("c", "c");
  c1->SetCanvasSize(1500,1500);
  c1->SetWindowSize(15000,600);


  gStyle->SetOptStat(0);
  gStyle->SetPalette(104);
  //gStyle->SetPalette(56,0);
  gStyle->SetNumberContours(99);
  gStyle->SetLabelFont(132, "X");
  gStyle->SetLabelFont(132, "Y");
  gStyle->SetLabelFont(132, "Z");
  //TColor::InvertPalette();

  // Create the histo, open the file and fill it
  TProfile2D *profile2d = new TProfile2D("h2","",
                                          xbin.size()-1,&xbin[0],
                                          ybin.size()-1,&ybin[0]);
  ifstream datafile(data_filename);
  string line;
  // Temporaire
  Float_t x,y,z;
  Int_t nlines = 0;

  while (!datafile.eof()) {
    cout << x << " "<< y << " "<< z << endl;
    datafile >> x >> y >> z;
    if (!datafile.good()) break;
    profile2d->Fill(x,y,z);
 }

  // Drawing profile2d
  // see drawing option at https://root.cern.ch/root/htmldoc/guides/users-guide/ROOTUsersGuide.html

  profile2d->GetZaxis()->SetRangeUser(zmin, zmax);
  profile2d->Draw("COLZ");//"E1X0 SAME HIST");

  //superimpose lines at the xbins positions
  TLine l;

  c1->Update();

  Double_t ymin = c1->GetUymin();
  Double_t ymax = c1->GetUymax();
  Double_t xmin = c1->GetUxmin();
  Double_t xmax = c1->GetUxmax();

  l.SetLineStyle(1);
  for (Int_t bin=1; bin < xbin.size();bin++)
  {
    l.DrawLine(xbin[bin],ymin,xbin[bin],ymax);
  }
  for (Int_t bin=1; bin < ybin.size();bin++)
  {
    l.DrawLine(xmin,ybin[bin],xmax,ybin[bin]);
  }



    TEllipse el1(0.,0.,5.,5.);
    el1.SetFillColor(3);
    el1.SetFillStyle(0);
    el1.SetLineColor(1);
    el1.SetLineStyle(1);
    el1.SetLineWidth(2);
    el1.Draw();

  // Save file
  string root_filename=histo_filename+".root";
  string pdf_filename=histo_filename+".pdf";
  string png_filename=histo_filename+".png";
  string tex_filename=histo_filename+".tex";
  c1->SaveAs(root_filename.c_str());
  c1->SaveAs(pdf_filename.c_str());
  c1->SaveAs(tex_filename.c_str());
  c1->SaveAs(pdf_filename.c_str());

  // remove file data and bin.txt
  remove(data_filename.c_str());
  remove(xbin_filename.c_str());
  remove(ybin_filename.c_str());
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
