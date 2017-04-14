#include<iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>

using namespace std;

///global variables
char key[5000];
char data[5000];
int main()
{
    fstream ifile1;
    ifile1.open("data.txt",ios::in);
    fstream ofile;
    ofile.open("new_data.txt",ios::out);
    if(!ifile1){
        cout << "Cannot open input file!" << endl;
    }
    else{
        do{
            ifile1.getline(key,sizeof(key));
            for(int i=0;i<4;i++){
                if(key[0]=='\0') break;
                ofile << key[i];
            }
        }while(!ifile1.eof());
        ifile1.close();
    }
    ofile << endl;

    fstream ifile2;
    ifile2.open("data.txt",ios::in);
    if(!ifile2){
        cout << "Cannot open input file!" << endl;
    }
    else{
        do{
            ifile2.getline(data,sizeof(data));
            if(data[0]!='\0'){
                for(int i=4;i<4095;i++){
                ofile << data[i];
                }
                ofile << endl;
            }
        }while(!ifile2.eof());
    }
     ofile.close();

}
