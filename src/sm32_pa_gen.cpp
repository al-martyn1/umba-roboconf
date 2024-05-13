#include <iostream>
#include <iomanip>

using namespace std;

int main( int argc, char* argv[] )
{

        cout<<"#pragma once\n";
        cout<<"// Automatically generated file, do not edit\n\n";
        
        char ch = 'A';
        
        for(char ch = 'A'; ch!='K'; ++ch)
        {
                cout<<"#ifdef GPIO"<<ch<<"\n";
                for(unsigned i = 0; i != 16; ++i)
                {
                    if (argc<2)
                    {
                        cout<<"    #define P"<<ch      <<setw(2)<<setfill(' ')<<left<<i
                            <<"    umba::periph::traits::GpioPinAddr({ GPIO"   <<ch<<", "<<setw(2)<<setfill(' ')<<left<<i<<" })\n";
                    }
                    else if (argc<3)
                    {
                        cout<<"    #define UMBA_PINADDR_P"<<ch      <<setw(2)<<setfill(' ')<<left<<i
                            <<"    umba::periph::traits::GpioPinAddr({ GPIO"   <<ch<<", "<<setw(2)<<setfill(' ')<<left<<i<<" })\n";
                    }
                    else
                    {
                        cout<<"    #define UMBA_PINADDR_STRUCT_P"<<ch      <<setw(2)<<setfill(' ')<<left<<i
                            <<"    umba::periph::traits::GpioPinAddr({ GPIO"   <<ch<<", "<<setw(2)<<setfill(' ')<<left<<i<<" })\n";
                    }
                }
                cout<<"#endif\n\n";

        }
        
        return 0;
}



