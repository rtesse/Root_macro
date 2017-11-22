#include <iostream>

#include <TH1D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TVectorD.h>
#include "string.h"

#include <fstream>


using namespace std;

void FillGraph(string filename, TGraph*);
void OpenData(string filename, vector<double> &x, vector<double> &y);

int main(int argc, char *argv[])
{


    vector<double> x;
    vector<double> y;

    OpenData("data.dat",x,y);


    TVectorD data_x(x.size(),&x[0]);
    TVectorD data_y(y.size(),&y[0]);

    //for(unsigned i=0; i< x.size(); i++)
    //{
    //    cout << x[i] << " " << y[i] << endl;
    //}

    //Draw a simple graph
    TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);

    //c1->SetFillColor(42);
    c1->SetGrid();


    TGraph *gr = new TGraph(data_x,data_y);
    gr->SetLineColor(1);
    gr->SetLineWidth(1);
    //gr->SetMarkerColor(4);
    //gr->SetMarkerStyle(21);
    gr->SetTitle("a simple graph");
    gr->GetXaxis()->SetTitle("Energy (MeV)");
    gr->GetYaxis()->SetTitle("Stopping Power");
    gr->Draw("ACP");

    // TCanvas::Update() draws the frame, after which one can change it
    c1->Update();
    //c1->GetFrame()->SetFillColor(21);
    //c1->GetFrame()->SetBorderSize(12);
    c1->SetLogx();
    c1->SetLogy();
    c1->Modified();
    string filename = "test.root";
    c1->SaveAs(filename.c_str());
}

void OpenData(string filename, vector<double> &x, vector<double> &y)
{
    ifstream file(filename);

    if (file)
    {
        double data_x;
        double data_y;
        // read the elements in the file into a vector
        while (file >> data_x >> data_y )
        {
            x.push_back(data_x);
            y.push_back(data_y);
        }
    }
    else
    {
        cout << "ERROR WHEN OPENING FILE "<< endl;
    }
    file.close();

}

/*
void FillGraph(string filename, TGraph *)
{
    ifstream file(filename);

    if (inputFile) {
        int count = 0;      // count number of items in the file

        // read the elements in the file into a vector
        while ( inputFile >> rainfall ) {
            rainfall.push_back(count);
            ++count;
        }

        file.close()
    }
*/
