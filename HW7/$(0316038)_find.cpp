//
//  main.cpp
//  OS_HW7
//
//  Created by 石瑾旋 on 2016/12/27.
//  Copyright © 2016年 石瑾旋. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

enum
{
    NAME,
    INODE,
    SIZE_MINIMUM,
    SIZE_MAXIMUM,
};

struct expression{
    int option;
    struct
    {
        long int inode_num;
        double lower_bound;
        double upper_bound;
        string filename;
    }operand;
}expr;

void search();
// The fullpath character array is initiallized to store the path and the
// pathsize is initialized to store the length the fullpath array.
char *fullpath = NULL;
int pathsize = 0;
int fail=0;
int d=32767;
int flag[4]={0};

int main(int argc,char **argv)
{
    //expression expr;
    int parameter_num=1;
    int definedExpression = 0;
    
    while(argv[parameter_num][0]!='-'){
        fullpath=argv[parameter_num];
        parameter_num++;
    }
    
    while(parameter_num<argc){
        if(argv[parameter_num][0]=='-' && definedExpression==0){
            if(strcmp(argv[parameter_num]+1, "inode") == 0){
                flag[INODE]=1;
                expr.option = INODE;
            }
            else if(strcmp(argv[parameter_num]+1, "name") == 0){
                flag[NAME]=1;
                expr.option = NAME;
            }
            else if(strcmp(argv[parameter_num]+1, "size_min") == 0){
                flag[SIZE_MINIMUM]=1;
                expr.option = SIZE_MINIMUM;
            }
            else if(strcmp(argv[parameter_num]+1, "size_max") == 0){
                flag[SIZE_MAXIMUM]=1;
                expr.option = SIZE_MAXIMUM;
            }
            definedExpression=1;
        }
        
        else if(definedExpression==1){
            switch(expr.option){
                case INODE:
                    expr.operand.inode_num=atoi(argv[parameter_num]);
                    break;
                case NAME:
                    expr.operand.filename=argv[parameter_num];
                    break;
                case SIZE_MINIMUM:
                    expr.operand.lower_bound=atof(argv[parameter_num]);
                    for(int i=0;i<20;i++){
                        expr.operand.lower_bound*=2;
                    }
                    break;
                case SIZE_MAXIMUM:
                    expr.operand.upper_bound=atof(argv[parameter_num]);
                    for(int i=0;i<20;i++){
                        expr.operand.upper_bound*=2;
                    }
                    break;
            }
            definedExpression=0;
        }
        parameter_num++;
    }
    search();

}

void search()
{
    DIR *dp;
    dp=opendir(fullpath);
    int isdir=0;
    int printout[4]={0};
    int shouldprint;
    if(dp==NULL){
        fail=1;
        perror(fullpath);
        return;
    }
    
    struct dirent * currentdir;
    //readdir() returns a pointer to a dirent structure
    
    while((currentdir=readdir(dp)) && currentdir){
        //ignore "." & ".."
        
        if(strcmp(currentdir->d_name,".")==0 || strcmp(currentdir->d_name,"..")==0){
            continue;
        }

        isdir=currentdir->d_type & DT_DIR;  //This is a directory
        for(int i=0;i<4;i++){
            printout[i]=0;
        }
        shouldprint=1;
        
        strcat(fullpath, "/");
        strcat(fullpath, currentdir->d_name);

        if(!isdir){
            struct stat sb;
            if(lstat(fullpath, &sb) >= 0){
                if(flag[SIZE_MINIMUM]==1 && sb.st_size >= expr.operand.lower_bound){
                    printout[SIZE_MINIMUM]=1;
                }
                if(flag[SIZE_MAXIMUM]==1 && sb.st_size <= expr.operand.upper_bound){
                    printout[SIZE_MAXIMUM]=1;
                }
                if(flag[INODE]==1 && sb.st_ino == expr.operand.inode_num){
                    printout[INODE]=1;
                }
                
                string d_name(currentdir->d_name);
                if(flag[NAME]==1 && d_name==expr.operand.filename){
                    printout[NAME]=1;
                }
                
            }
        }
        
        for(int i=0;i<4;i++){
            if(flag[i]==1){
                shouldprint*=printout[i];
            }
        }

        if(shouldprint==1)
        {
            struct stat sb;
            cout << fullpath << " ";
            cout << sb.st_ino << " ";
            cout << sb.st_size << "MB" << endl;
            
        }
        
        if(isdir && d>0){
            d--;
            search();   //recursive search directory
            d++;
        }
        fullpath[strlen(fullpath)-strlen(currentdir->d_name) - 1] = '\0';
    }
    closedir(dp);
}
