#include "Data.hh"
#include <fstream>


#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

using namespace std;

Data::Data()
{
    fgInstance = this;
}

Data::~Data()
{}

//Singleton pattern
Data* Data::fgInstance = 0;
Data* Data::Instance()
{
    return fgInstance;
}

void Data::AfficheTable(const vector<vector<double> > Table)
{
    cout << "---------------------------------------" << endl;
    cout << "Affichage de la table xxx " << endl;
    unsigned i,j;
    for(i = 0; i< Table.size(); i++)
    {
        for (j = 0; j< Table[i].size(); j++)
        {
            cout  << Table[i][j] << "\t";
        }
        cout << endl;

    }
    cout << "---------------------------------------" << endl;
}

vector<vector<double> > Data::LoadTable(string filename)
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


vector <double> Data::GetWallProperties(string wallName)
{

    vector <double> wallproperties;

    string studyWallName;
    double X_min, X_max;
    double Y_min, Y_max;
    double Z_min, Z_max;
    int Nx, Ny, Nz;
    double center_x, center_y, center_z;
    double size_x, size_y, size_z;

    /// Selon les wall, l'extension en XYZ et le nombre de cellules qu'on veut
    if(wallName == "BeamLine1")
    {
        center_x = 0;
        center_y = -0.875;
        center_z = 3.825;
        size_x = 1;
        size_y = 0.75;
        size_z = 1.6;
        Nx = 2;
        Ny = 2;
        Nz = 16;
    }

    if(wallName == "North")
    {
        center_x = -0.13;
        center_y = 0.65;
        center_z = -4.525;
        size_x = 7.34;
        size_y = 3.8;
        size_z = 3;
        Nx = 10;
        Ny = 8;
        Nz = 30;
    }

    if(wallName == "Internal")
    {
        center_x = 2.02;
        center_y = 0.65;
        center_z = 3.825;
        size_x = 3.04;
        size_y = 3.8;
        size_z = 1.6;
        Nx = 8;
        Ny = 8;
        Nz = 16;
    }

    if(wallName == "East")
    {
        center_x = 5.04;
        center_y = 0.65;
        center_z = -0.7;
        size_x = 3;
        size_y = 3.8;
        size_z = 10.65;
        Nx = 30;
        Ny = 8;
        Nz = 20;
    }

    X_min = center_x-0.5*size_x;
    X_max = center_x+0.5*size_x;

    Y_min = center_y-0.5*size_y;
    Y_max = center_y+0.5*size_y;

    Z_min = center_z-0.5*size_z;
    Z_max = center_z+0.5*size_z;

    wallproperties.push_back(Nx);
    wallproperties.push_back(X_min);
    wallproperties.push_back(X_max);

    wallproperties.push_back(Ny);
    wallproperties.push_back(Y_min);
    wallproperties.push_back(Y_max);

    wallproperties.push_back(Nz);
    wallproperties.push_back(Z_min);
    wallproperties.push_back(Z_max);

    return wallproperties;
}


void Data::extract_columns(vector<vector<double> >CrsTable, int flag_isotope, double x[], double y[])
{
    for (unsigned i =0; i < CrsTable.size(); i++)
    {
        x[i] = CrsTable[i][0];
        y[i] = CrsTable[i][flag_isotope];
    }
}

double Data::GetInterpCrossSection(double E[], double Crs[], double E_x, int size)
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
