#include <string.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "include\UniversalCode.h"
#include "include\WordBasedText.h"
#include "include\FibMCoder.hpp"

using namespace std;

int main(int argc, char** argv) {
    if(argc<2)
        cout<<"Incorrect number of command line arguments.";
    else {
        string ifname(argv[1]);
        WordBasedText *wa;
        wa = new WordBasedText(ifname,1);   // Pre-process the text

        cout<<endl<<"================ Encoding. =================="<<endl;
        // Fibonacci mixed d-ary code
        wa->precalcBestBlocks();
        FibMixTCode* umixt=new FibMixTCode(wa);
        FibMCoderS FM(wa,umixt);
        FM.emulate();
        // Fibonacci Higher Order code
        FibUCode* u=new FibUCode(3);
        FibMCoderS F(wa,u);
        F.emulate();
        // Fibonacci code Fib3
        Fib3Code* u3=new Fib3Code();
        FibMCoderS F3(wa,u3);
        F3.emulate();
        // BCMix code
        wa->precalcBestBlocks_bmix();
        BCMix* bmix=new BCMix(wa);
        FibMCoderS Fbc(wa,bmix);
        Fbc.emulate();
        cout<<endl;
    }
	system("pause");
}

