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

#include <boost/algorithm/string.hpp>

using namespace std;

vector<Double_t> logspace(Double_t xmin, Double_t xmax, Int_t nbins);
vector<Double_t> linspace(Double_t xmin, Double_t xmax, Int_t nbins);
void Create2D_histogram(string filename, vector<Double_t> binx, vector<Double_t> biny, int xcolumnId, int ycolumnId, string histo_filename);
vector<Double_t> ConvertData(vector<string> old_data);

int main(int argc, char *argv[])
{
    Int_t nEnergybins=100;
    Int_t nAnglebins=100;

    vector<Double_t> Energybins=logspace(1e-5,230,nEnergybins);
    vector<Double_t> Anglebins=linspace(0,180,nAnglebins);

    string filename = "Distribution_QGSP_BIC_HP_Degrador_210.00MeV.txt";
    string histo_filename="Histo_theta";
    Create2D_histogram(filename,Energybins,Anglebins,0,4,histo_filename);

    return 0;
}

vector<Double_t> ConvertData(vector<string> old_data)
{
    vector<Double_t> data;
    for(int i =0; i< old_data.size(); i++)
    {
        data.push_back(stod(old_data[i]));
    }
    return data;
}

void Create2D_histogram(string filename, vector<Double_t> binx, vector<Double_t> biny, int columnId_x, int columnId_y, string histo_filename)
{

    gROOT->Reset();
    TCanvas *c1 = new TCanvas("c1", "c1",900,900);
    gStyle->SetOptStat(0);

    // Create the histo, open the file and fill it

    TH2F *histo2d = new TH2F("h2","",binx.size()-1,&binx[0], biny.size()-1, &biny[0]);
    ifstream datafile(filename);
    string line;
    vector<string> data_line;
    std::string delims = "\t,|";

    // Temporaire
    Float_t Energy,Px,Py,Pz,Theta,Phi;
       Int_t nlines = 0;
       while (1) {
          datafile >> Energy >> Px >> Py >> Pz >> Theta >> Phi;
          if (!datafile.good()) break;
          histo2d->Fill(Energy,Theta);

          nlines++;
       }

    // Scale the histogramdd
    Double_t norm = 1;
    Double_t scale_theta = norm/(histo2d->Integral());
    histo2d->Scale(scale_theta);

    // Drawing
    gStyle->SetPalette(1);
    histo2d->Draw("LEGO");
    string root_filename=histo_filename+".root";
    string eps_filename=histo_filename+".eps";
    c1->SaveAs(root_filename.c_str());
    c1->SaveAs(eps_filename.c_str());

}

std::vector<Double_t> logspace(Double_t xmin, Double_t xmax, Int_t nbins)
{
    /// Function creating a logspace vector
    Double_t logxmin = TMath::Log10(xmin);
    Double_t logxmax = TMath::Log10(xmax);
    Double_t binwidth = (logxmax-logxmin)/nbins;
    std::vector<Double_t> xbins;
    xbins.push_back(xmin);
    for (Int_t i=1;i<=nbins;i++)
    {
        xbins.push_back(xmin + TMath::Power(10,logxmin+i*binwidth));
    }
    return xbins;
 }

 std::vector<Double_t> linspace(Double_t xmin, Double_t xmax, Int_t nbins)
 {
    /// Function creating a linspace vector
    Double_t binwidth = (xmax-xmin)/nbins;
    std::vector<Double_t> xbins;
    xbins.push_back(xmin);
    for (Int_t i=1;i<=nbins;i++)
    {
       xbins.push_back(xmin + (xmin+i*binwidth));
    }
    return xbins;
  }
