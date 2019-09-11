#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
//#include <stdio.h>
//#include <stdint.h>
//#include <stdlib.h>
using namespace std;
string filename;
bool Repeat = false;
bool FileNotExist = true;                                              // Boolean for Running Program Again.
// Structure Element for a Polygon.
struct Polygon
{
    int Layer_No, No_Points;
    vector <long int> X,Y;
};

// Convert from Decimal to Binary.
void Dec2Bin(int N, char V[])
{
    int Number = N, i = 7;
    char v[8] = {'0','0','0','0','0','0','0','0'};
    // Calculating.
    while (Number > 0)
    {
        if (Number%2 == 1)
        {
            v[i] = '1';
        }
        else
        {
            v[i] = '0';
        }
        Number = Number/2;
        i--;
    }
    // Writing Values.
    for(i = 0; i < 8; i++)
        V[i] = v[i];
}

// Convert from Binary to Decimal. Type = 0 (Binary to Decimal), Type = 1 (Binary to Decimal 7 bits).
uint64_t Bin2Dec(char V[],int Type, int Bits)
{
    uint64_t N = 0;
    int i = (Type == 0) ? 0:1;
    int P = (Bits - 1);
    while (i < Bits)
    {
        N = N + (V[i] - 48) * pow(2, P-i);
        i++;
    }
    return N;
}

// 4 Bytes Signed Integer
long int Point_Pos(char V2[])
{
    long int N = 0;
    string s = {},c;
    for(unsigned int i = 0; i < 32; i++)
    {
        c = V2[i];
        s.append(c);
    }
    N = stoll(s,nullptr,2);
    return N;
}

// Calculate Units.
void CalcUnits(int U[], double &UserUnit, double &PhysicalUnit)
{
    // Variables Declaration.
    char V[8], M[56];
    int N;
    int Sign, Exponent;
    uint64_t Mantissa;

    // Calculating Sign and Exponent Part for UserUnit.
    N = U[0];
    Dec2Bin(N, V);
    Sign = (V[0] == '0') ? 1 : -1;
    Exponent = Bin2Dec(V,1,8);

    // Calculating Mantissa for UserUnit.
    for (unsigned int i = 1; i < 8; i++)
    {
        N = U[i];
        Dec2Bin(N, V);
        for (unsigned int j = 0; j < 8; j++)
            M[(i-1)*8 + j] = V[j];
    }
    Mantissa = Bin2Dec(M,0,56);
    // Calculating User Unit.
    UserUnit = (Mantissa*1.0 / (pow(2.0,56))) * pow(16.0,(Exponent - 64));

    // Calculating Sign and Exponent Part for PhysicalUnit.
    N = U[8];
    Dec2Bin(N, V);
    Sign = (V[0] == '0') ? 1 : -1;
    Exponent = Bin2Dec(V,1,8);

    // Calculating Mantissa for PhysicalUnit.
    for (unsigned int i = 9; i < 16; i++)
    {
        N = U[i];
        Dec2Bin(N, V);
        for (unsigned int j = 0; j < 8; j++)
            M[(i-9)*8 + j] = V[j];
    }
    Mantissa = Bin2Dec(M,0,56);
    // Calculating PhysicalUnit.
    PhysicalUnit = (Mantissa*1.0 / (pow(2.0,56))) * pow(16.0,(Exponent - 64));
}

// Read Units.
void ReadUnits (ifstream &file, double &UserUnit, double &PhysicalUnit)
{
    bool Searching = true;
    unsigned char C;
    short y, z = 0;
    int U[16];

    while (Searching && !file.eof())
    {
        y = z;
        file >> std::noskipws >> C;
        z = C;
        // Searching for Unit Header.
        if (y == 3 && z == 5)
        {
            for (unsigned int i = 0; i < 16; i++)
            {
                file >> std::noskipws >> C;
                U[i] = C;
                CalcUnits(U, UserUnit, PhysicalUnit);
            }
            Searching = false;
        }
    }
}

// Read NAME.
string Read_NAME(ifstream &file, short x)
{
    char C;
    string s, STRNAME = {};
    // Obtain Name Character by Character.
    for (unsigned int i = 0; i < x; i++)
    {
        file >> std::noskipws >> C;
        if (C != 0)
        {
            s = C;
            STRNAME.append(s);
        }
    }
    return STRNAME;
}

// Read Layer.
void Assign_Layer(ifstream &file, Polygon &Poly)
{
    // Variables Declaration.
    bool NotFound = true;
    unsigned char C;
    short y, z;

    // Random Assignment.
    y = 0;
    z = 0;
    // Searching for Layer Number.
    while (NotFound)
    {
        y = z;
        file >> std::noskipws >> C;
        z = C;
        if (y == 13 && z == 2)
        {
            NotFound = false;
            file >> std::noskipws >> C;
            file >> std::noskipws >> C;
        }
    }
    // Assign Layer Number.
    Poly.Layer_No = int(C) + 1;
}

// Read Boundary.
void Read_Boundary(ifstream &file, Polygon &Poly)
{
    // Variables Declaration.
    unsigned char C;
    char V1[8], V2[32];
    short x, y, z;

    // Random Assignment.
    x = 0;
    y = 0;
    z = 0;
    // Searching for Number of XY Points.
    for(unsigned int i = 0; i < 8; i++)
        file >> std::noskipws >> C;
    // Number of Bytes To Read.
    x = C - 4;
    // Skip These Values. (Read to Overwrite on it).
    file >> std::noskipws >> C;
    file >> std::noskipws >> C;
    // Assign Number of Points.
    Poly.No_Points = x / 8;
    // Reading XY Points.
    while (y < x)
    {
        // Reading Point X.
        for (unsigned int i = 0; i < 4; i++)
        {
            file >> std::noskipws >> C;
            z = C;
            Dec2Bin(z, V1);
            for (unsigned int j = 0; j < 8; j++)
                V2[j + i*8] = V1[j];
            y++;
        }
        Poly.X.push_back(Point_Pos(V2));
        // Reading Point Y.
        for (unsigned int i = 0; i < 4; i++)
        {
            file >> std::noskipws >> C;
            z = C;
            Dec2Bin(z, V1);
            for (unsigned int j = 0; j < 8; j++)
                V2[j + i*8] = V1[j];
            y++;
        }
        Poly.Y.push_back(Point_Pos(V2));
    }
}

// Write Polygon.
void Write_Polygon(ofstream &Structure, Polygon Poly)
{
    for (unsigned int i = 0; i < Poly.No_Points; i++)
    {
        Structure.width(10);
        Structure << std::right << Poly.X[i] <<  " ";
        Structure.width(10);
        Structure << std::right << Poly.Y[i] << "    ";
        Structure.width(5);
        if(i == 0)
            Structure << std::right << Poly.Layer_No;
        else if(i == 1)
            Structure << std::right << -Poly.No_Points;
        else
            Structure << std::right << 0;
        Structure << endl;
    }
}

// Extract Polygons From Path.
void Extract_Poly_Path(ofstream &Structure, long int Width, int NoOfPoints, vector <long int> X, vector <long int> Y, Polygon Poly)
{
    long int Half_Width = Width / 2, A, B;                                  // This Case Should be Even Anytime.

    for (unsigned int i = 0; i < NoOfPoints-1; i++)
    {
        // Inherit Data.
        Polygon PPoly;                                                       // Path Polygons.
        PPoly.Layer_No = Poly.Layer_No;
        PPoly.No_Points = 5;
        if (X[i] == X[i+1])
        {
            A = X[i] - Half_Width;                                          // Negative Edge.
            B = X[i] + Half_Width;                                          // Positive Edge.
            // Assign Point 1.
            PPoly.X.push_back(A);
            PPoly.Y.push_back(Y[i]+Half_Width);
            // Assign Point 2.
            PPoly.X.push_back(A);
            PPoly.Y.push_back(Y[i+1]-Half_Width);
            // Assign Point 3.
            PPoly.X.push_back(B);
            PPoly.Y.push_back(Y[i+1]-Half_Width);
            // Assign Point 4.
            PPoly.X.push_back(B);
            PPoly.Y.push_back(Y[i]+Half_Width);
            // Assign Point 5.
            PPoly.X.push_back(A);
            PPoly.Y.push_back(Y[i]+Half_Width);
        }
        else                                                                // Else Only is Used Since Every Point is Linked to The One After.
        {
            A = Y[i] - Half_Width;                                          // Negative Edge.
            B = Y[i] + Half_Width;                                          // Positive Edge.
            // Assign Point 1.
            PPoly.X.push_back(X[i]+Half_Width);
            PPoly.Y.push_back(A);
            // Assign Point 2.
            PPoly.X.push_back(X[i]+Half_Width);
            PPoly.Y.push_back(B);
            // Assign Point 3.
            PPoly.X.push_back(X[i+1]-Half_Width);
            PPoly.Y.push_back(B);
            // Assign Point 4.
            PPoly.X.push_back(X[i+1]-Half_Width);
            PPoly.Y.push_back(A);
            // Assign Point 5.
            PPoly.X.push_back(X[i]+Half_Width);
            PPoly.Y.push_back(A);
        }
        Write_Polygon(Structure, PPoly);
    }
}

// Read Path.
void Read_Path(ifstream &file, ofstream &Structure, Polygon Poly)
{
    // Variables Declaration.
    unsigned char C;
    char V1[8], V2[32];
    short x, y, z;
    int NoOfPoints;
    long int Width;
    vector <long int> X,Y;
    // Random Assignment.
    x = 0;
    y = 0;
    z = 0;
    // Searching For Width.
    while (!file.eof())
    {
        x = y;
        y = z;
        file >> std::noskipws >> C;
        z = C;
        if(y == 15 && z == 3)
            break;
    }
    // Reading Number of Bytes Contains Width .
    x -= 4;
    // Reading Width.
    for (unsigned int i = 0; i < x; i++)
    {
        file >> std::noskipws >> C;
        z = C;
        Dec2Bin(z, V1);
        for (unsigned int j = 0; j < 8; j++)
            V2[j + i*8] = V1[j];
    }
    Width = Point_Pos(V2);
    // Skip a Value to read the one after it. (Number of Bytes of XY Points).
    file >> std::noskipws >> C;
    file >> std::noskipws >> C;
    // Assign Number of Points and Number of Bytes of XY Points.
    x = C;
    x -= 4;
    NoOfPoints = x / 8;
    // Skip These Values. (Read to Overwrite on it).
    file >> std::noskipws >> C;
    file >> std::noskipws >> C;
    // Reset y To Use It As A Counter.
    y = 0;
    // Reading XY Points.
    while (y < x)
    {
        // Reading Point X.
        for (unsigned int i = 0; i < 4; i++)
        {
            file >> std::noskipws >> C;
            z = C;
            Dec2Bin(z, V1);
            for (unsigned int j = 0; j < 8; j++)
                V2[j + i*8] = V1[j];
            y++;
        }
        X.push_back(Point_Pos(V2));
        // Reading Point Y.
        for (unsigned int i = 0; i < 4; i++)
        {
            file >> std::noskipws >> C;
            z = C;
            Dec2Bin(z, V1);
            for (unsigned int j = 0; j < 8; j++)
                V2[j + i*8] = V1[j];
            y++;
        }
        Y.push_back(Point_Pos(V2));
    }
    Extract_Poly_Path(Structure, Width, NoOfPoints, X, Y, Poly);                // Create Polygons From Every 2 Points And Write In The Output File.
}

// Write Units.
void Write_Units(ofstream &Output, double PhysicalUnit)
{
    Output.width(10);
    Output << std::right << 0 << " ";
    Output.width(10);
    Output << std::right << 0 << "    ";
    Output.width(5);
    Output << std::right <<  PhysicalUnit;
}

// Reading SREF (Name, XY Points).
void Read_SREF_Name_XY(ifstream &file, string &SNAME, long int &X, long int &Y)
{
    // Variable Declaration.
    unsigned char C;
    char V1[8], V2[32];
    short x, y, NoOfBytes;
    // Read Number of Bytes For SNAME.
    file >> std::noskipws >> C;
    file >> std::noskipws >> C;
    // Setting Number of Bytes.
    NoOfBytes = int(C) - 4;
    // Reading Bytes.
    file >> std::noskipws >> C;
    x = C;
    file >> std::noskipws >> C;
    y = C;
    // Checking That the Data Read in x,y are SNAME. (This Should Always Be True).
    if (x == 18 && y == 6 )
        SNAME = Read_NAME(file, NoOfBytes);
    // Read Number of Bytes For XY Points.
    file >> std::noskipws >> C;
    file >> std::noskipws >> C;
    // Setting Number of Bytes.
    NoOfBytes = int(C) - 4;
    while (x != 16 && y != 3)
    {
        // Reading Bytes.
        file >> std::noskipws >> C;
        x = C;
        file >> std::noskipws >> C;
        y = C;
    }
    // Checking That the Data Read in x,y are SNAME. (This Should Always Be True).
    if (x == 16 && y == 3)
    {
        // Reading Point X.
        for (unsigned int i = 0; i < 4; i++)
        {
            file >> std::noskipws >> C;
            x = C;
            Dec2Bin(x, V1);
            for (unsigned int j = 0; j < 8; j++)
                V2[j + i*8] = V1[j];
        }
        X = Point_Pos(V2);
        // Reading Point Y.
        for (unsigned int i = 0; i < 4; i++)
        {
            file >> std::noskipws >> C;
            x = C;
            Dec2Bin(x, V1);
            for (unsigned int j = 0; j < 8; j++)
                V2[j + i*8] = V1[j];
        }
        Y = Point_Pos(V2);
    }
}

// Write SREF Data.
void Write_SREF_Data(ifstream &SREF, ofstream &Structure, long int X, long int Y)
{
    // Declaring Variables.
    long int XX, YY, Z;
    // Writing Data.
    while (!SREF.eof())
    {
        // Reading Line.
        SREF >> XX;
        if (!SREF.eof())
        {
            SREF >> YY;
            SREF >> Z;
            // Write Data.
            Structure.width(10);
            Structure << std::right << XX + X <<  " ";
            Structure.width(10);
            Structure << std::right << YY + Y << "    ";
            Structure.width(5);
            Structure << Z << endl;
        }
        else
            break;
    }
}

// Rename File.
void Rename_File(vector <string> STRNAME, string ofilepath, string filename)
{
    // Obtaining the Name Of Last STRNAME.
    string Sfilename = {}, Untagged = {};                                   // Untagged: Filename Without Tag
    unsigned int x = -1;                                                    // Position of Main Structure.
    // Write File Name Without Tagging.
    for (unsigned int i = 0; i < filename.length(); i++)
    {
        if (filename[i] == '.')                                             // If '.' Found Break.
        {
            break;
        }
        else                                                                // Else Append.
            Untagged = Untagged + filename[i];
    }
    // Searching For Untagged Name To Get x.
    for (unsigned int i = 0; i < STRNAME.size(); i++)
    {
        unsigned check = 0;                                                 // Counter to Count Similarities.
        string Name = STRNAME[i];                                           // Get Structure Name.
        for (unsigned int j = 0; j < Untagged.length(); j++)
        {
            if (Name[j] == Untagged[j])                                     // If Similarities Increase Check.
                check++;
        }
        // If Matching Break and Save x Position.
        if (check == Untagged.length())
        {
            x = i;
            break;
        }
    }
    if (x == -1)
        Sfilename = "Output\\\\" + STRNAME[STRNAME.size()-1];               // Obtain Last Output Structure File Name.
    else
        Sfilename = "Output\\\\" + STRNAME[x];                              // Obtain Last Output Structure File Name.
    char file1[Sfilename.size() + 1];                                       // Create Array of Characters.
    strcpy(file1, Sfilename.c_str());                                       // Copy String To Char.
    // Obtaining The Name Of The File.
    char file2[ofilepath.size() + 1];                                       // Create Array of Characters.
    strcpy(file2, ofilepath.c_str());                                       // Copy String To Char.
    // Rename File.
    rename(file1, file2);                                                   // Rename File.
}

// Delete Files.
void Delete_Files(vector <string> STRNAME)
{
    for (unsigned int i = 0; i < STRNAME.size(); i++)
    {
        string Sfilename = "Output\\\\" + STRNAME[i];                       // Read STRNAME File.
        char delfile[Sfilename.size() + 1];                                 // Create Array of Characters.
        strcpy(delfile, Sfilename.c_str());                                 // Copy String To Char.
        remove(delfile);                                                    // Remove File.
    }
}

// Delete Output File If Exist.
void Delete_Output_If_Exists(string ofilepath)
{
    char delfile[ofilepath.size() + 1];                                 // Create Array of Characters.
    strcpy(delfile, ofilepath.c_str());                                 // Copy String To Char.
    remove(delfile);                                                    // Remove File.
}

// Check If File Exists or Not.
bool FileExist(ifstream &SREF)
{
    if(!SREF)            // If The File Was Not Found.
        return false;    // Return False.
    else                 // If The File Was Found.
        return true;     // Return False.
}

int main()
{
    // Variables.
    unsigned char C;                                                        // To Read Stream File.
    bool NewSTR = true;                                                     // Flag For New Structure.
    short x, y, z;                                                          // Variables for Converting Chars to Int.
    double UserUnit, PhysicalUnit;                                          // Physical and User Units.
    string ifilepath = "Input\\\\", ofilepath = "Output\\\\";               // File Names.
    string Sfilename = {};                                                  // Initiate Filename of Structure.
    vector <string> STRNAME;                                                // STRNAME Vector.
    // Read File Name.
    if (!Repeat)
    {
        cout << "Enter GDSII File Name: ";                                 	// Output Asking User to Input Filename.
        cin >> filename;                                                    // User Enter Filename including Type (filename.type).
    }
    ifilepath.append(filename);                                             // Add filename to search in Input Directory. (Input/filename.type).
    ofilepath.append(filename + ".txt");                                    // Add filename to output Directory. (Output/filename.type.txt).
    // Open and Read File.
    ifstream file;                                                          // Initiate Input Stream.
    file.open(ifilepath, ios::binary);                                      // Open File (Binary).
    // Read Units.
    ReadUnits(file, UserUnit, PhysicalUnit);                                // Read Units Function. (Physical and User Units).
    // Searching for STRNAME Then Structures (Boundaries, Paths, SREF).
    x = 0;                                                                  // Setting Initial Value.
    y = 0;                                                                  // Setting Initial Value.
    z = 0;                                                                  // Setting Initial Value.
    // Create Structure Output Stream File.
    ofstream Structure;                                                     // Initiate Output Stream.
    while (!file.eof())                                                     // True Until Reaching File End.
    {
        Polygon Poly;                                                       // Initiate Polygon Structure Every Loop. (To Facilitate Deleting Inner Variables).
        x = y;                                                              // x Saves the Value Read From Two Reads.
        y = z;                                                              // y Saves the Old Value From the Last Read.
        file >> std::noskipws >> C;                                         // Read Without Skipping White Spaces in C (Unsigned Char).
        z = C;                                                              // Convert Char to Int.

        if (y == 6 && z == 6 && NewSTR)                                     // For STRNAME.
        {
            x -= 4;                                                         // Store the Number of Bytes of STRNAME.
            STRNAME.push_back(Read_NAME(file, x));                          // Function Read STRNAME.
            Sfilename = "Output\\\\" + STRNAME[STRNAME.size()-1];           // Setting Output Directory.
            Structure.open(Sfilename, ios::out);                            // Open New File Name To Write The New Structure.
            NewSTR = false;
        }
        if (x == 4 && y == 8 && z == 0)                                     // For BOUNDARY.
        {
            Assign_Layer(file, Poly);                                       // Function Assign STRNAME, Layer Number, and STRIndex to Poly.
            // Read Polygon Points.
            Read_Boundary(file, Poly);                                      // Function Reads XY Points and Store Them in Poly.
            // Write Polygon.
            Write_Polygon(Structure, Poly);                                 // Function Writes The Points, Layer Number, Number of Points, and STRIndex to Ofstream File.
        }
        if (x == 4 && y == 9 && z == 0)                               		// For PATH.
        {
            Assign_Layer(file, Poly);                                       // Function Assign STRNAME, Layer Number, and STRIndex to Poly.
            // Read Points To Generate Polygons and Write It.
            Read_Path(file, Structure, Poly);                               // Function Reads Paths, Generate Polygons, and Write Polgons in Ostream File.
        }
        if ((x == 4 && y == 10 && z == 0) && Repeat)                                    // For SREF.
        {
            FileNotExist = false;
            // Variable Declaration.
            long int X, Y;                                                  // SREF XY Position.
            string SNAME, SREF_File;                                        // SNAME And SREF File Location Variables.
            // Read SNAME, X, Y.
            Read_SREF_Name_XY(file, SNAME, X, Y);                           // Function Reads SNAME of SREF And X, Y Position Of The SREF.
            SREF_File = "Output\\\\" + SNAME;                               // Setting The Location Of The SREF Data File.
            // Open File To Read.
            ifstream SREF;                                                  // Declare Ifstream File.
            SREF.open(SREF_File);                                           // Open File.
            if (FileExist(SREF))                                            // Function Check the Existence of file.
            {
                // Write SREF Data.
                Write_SREF_Data(SREF, Structure, X, Y);                     // Write SREF Data.
            }
            // Close File.
            SREF.close();                                                   // Close SREF File.
        }
        if (x == 4 && y == 7 && z == 0)                                     // For ENDSTR.
		{
            Structure.close();                                              // Close Structure File.
            NewSTR = true;
		}
	}
    // Close File.
    file.close();                                                           // Close File.
    if (!FileNotExist)
    {
        // Delete Output File If Exists.
        Delete_Output_If_Exists(ofilepath);                                 // Function To Delete Output File If Exists From Previous Runs.
        // Rename Files.
        Rename_File(STRNAME, ofilepath, filename);                          // Function Rename File.
        // Delete Work Files.
        Delete_Files(STRNAME);                                              // Function Delete Work Files.
        // Create and Open Output File.
        ofstream Output;                                                    // Initiate Output Stream.
        Output.open(ofilepath, std::ofstream::out | std::ofstream::app);    // Open File.
        // Write Physical Unit.
        Write_Units(Output, PhysicalUnit);                                  // Function Writes Physical Unit Number At The End of The Ofstream File.
        // Close Output.
        Output.close();                                                     // Close File.
    }
    else
    {
        Repeat = true;
        main();
    }
    return 0;
}
