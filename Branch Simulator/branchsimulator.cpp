/*
Computer Architecture Lab 3 

Authors: Xiaojie Zha xz1776@nyu.edu N11695162
         Mingdi Mao mm8688@nyu.edu N11974036

Version: 2.4
Branch Prediction
*/ 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <iomanip>
#include <bitset>
#include <cmath> 
using namespace std;

int main(int argc, char* argv[]){
	long config;
	ifstream configpara;
	configpara.open(argv[1]);
	while(!(configpara).eof())
		configpara>>config;
	config=pow(2,config);
	ifstream traces;
	ofstream tracesout;
	string line;
	string xaddr;
	unsigned int addr;
	bitset<32> branchindex;
	string state;
	int realstate;
	long counter;
	int scounter[config];
	int i;
	for (i=0; i<config;i++) scounter[i]=11;
	traces.open(argv[2]);
	string outname = string(argv[2]) + ".out";
	tracesout.open(outname.c_str());
	if (traces.is_open()&&tracesout.is_open()){
		while (getline(traces,line)){
			istringstream iss(line);
			if (!(iss>>xaddr>>state)) {break;}
			stringstream saddr(xaddr);
			saddr>>std::hex>>addr;
			branchindex = bitset<32>(addr);
			realstate=atoi(state.c_str());
			counter = branchindex.to_ulong()& (long)(config-1)-1;
			switch(scounter[counter]){
				case 11:{
					if (realstate==1) scounter[counter]=11;
					if (realstate==0) scounter[counter]=10;
					tracesout<<"1"<<endl;
					break;
				}
				case 10:{
					if (realstate==1) scounter[counter]=11;
					if (realstate==0) scounter[counter]=0;
					tracesout<<"1"<<endl;
					break;
				}
				case 1:{
					if (realstate==1) scounter[counter]=11;
					if (realstate==0) scounter[counter]=0;
					tracesout<<"0"<<endl;
					break;
				}
				case 0:{
					if (realstate==1) scounter[counter]=1;
					if (realstate==0) scounter[counter]=0;
					tracesout<<"0"<<endl;
					break;
				}
			}
			}
	}else cout<<"Unable to open file";
	configpara.close();
	traces.close();
	tracesout.close();
	return 0;
}
