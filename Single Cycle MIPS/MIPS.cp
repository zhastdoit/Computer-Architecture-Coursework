/*
Computer Architecture Lab 1

Authors: Xiaojie Zha xz1776@nyu.edu N11695162
         Mingdi Mao mm8688@nyu.edu N11974036

Version: 1.03
*/
#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include<stdlib.h>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.


class RF
{
    public:
        bitset<32> ReadData1, ReadData2;
     	RF()
    	{
          Registers.resize(32);
          Registers[0] = bitset<32> (0);
        }

        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {
            // implement the funciton by you.
            if (WrtEnable.to_string()=="0"){
            	ReadData1 = Registers[RdReg1.to_ulong()];
            	ReadData2 = Registers[RdReg2.to_ulong()];
			}
			else
				Registers[WrtReg.to_ulong()] = WrtData;
         }

	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {
                        rfout << Registers[j]<<endl;
                      }

                  }
                  else cout<<"Unable to open file";
                  rfout.close();

               }
    private:
    	vector<bitset<32> >Registers;

};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {
                 // implement the ALU operations by you.
                 //R-type
                if (ALUOP.to_ulong()==ADDU)
                    ALUresult=bitset<32>(oprand1.to_ulong()+oprand2.to_ulong());
                if (ALUOP.to_ulong()==SUBU)
                    ALUresult=bitset<32>(oprand1.to_ulong()-oprand2.to_ulong());
                if (ALUOP.to_ulong()==AND)
                    ALUresult=bitset<32>((oprand1&=oprand2).to_ulong());
                if (ALUOP.to_ulong()==OR)
                    ALUresult=bitset<32>((oprand1|=oprand2).to_ulong());
                if (ALUOP.to_ulong()==NOR)
                	ALUresult=bitset<32>((oprand1^=oprand2).to_ulong());
                //I-type
                //addi
                if (ALUOP.to_string()=="000")
                	ALUresult=bitset<32>(oprand1.to_ulong()+oprand2.to_ulong());
                 return ALUresult;
            }
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(MemSize);
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {
                        IMem[i] = bitset<8>(line);
                        i++;
                     }

                  }
                  else cout<<"Unable to open file";
                  imem.close();

                  }

          bitset<32> ReadMemory (bitset<32> ReadAddress)
              {
                // implement by you. (Read the byte at the ReadAddress and the following three byte).
                unsigned long address = ReadAddress.to_ulong();
                bitset<32> ins1 = bitset<32>(IMem[address].to_ulong());
                bitset<32> ins2 = bitset<32>(IMem[address+1].to_ulong());
                bitset<32> ins3 = bitset<32>(IMem[address+2].to_ulong());
                bitset<32> ins4 = bitset<32>(IMem[address+3].to_ulong());
                unsigned long Instruction=bitset<32>(ins1<<24).to_ulong()+(ins2<<16).to_ulong()+(ins3<<8).to_ulong()+(ins4.to_ulong());
                return Instruction;
              }

      private:
           vector<bitset<8> > IMem;


};

class DataMem
{
      public:
          bitset<32> readdata;
          DataMem()
          {
             DMem.resize(MemSize);
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();

          }
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
          {

               // implement by you.
               if (readmem.to_ulong()==1) readdata=bitset<32>((DMem[Address.to_ulong()]).to_ulong());
               if (writemem.to_ulong()==1) DMem[Address.to_ulong()]=bitset<8>(WriteData.to_ulong());
               return readdata;
          }

          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {
                        dmemout << DMem[j]<<endl;
                       }

                  }
                  else cout<<"Unable to open file";
                  dmemout.close();

               }

      private:
           vector<bitset<8> > DMem;

};



int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;
    bitset<32> pcvalue;
    unsigned long PC=0;

    while (1)
	{
        int j=0;
        int index=0;
        bool typedecide=true;
        char mipstype='I';
		bitset<32> instruction=myInsMem.ReadMemory(PC);
		if (instruction.count()==32) break;
		//decode(Read RF)
		for (j=31;j>=26;j--)
			if (instruction[j]) typedecide=false;
		if (typedecide) mipstype='R'; else
			if ((instruction[27]==1)&&(instruction[26]==0)) mipstype='J';
		//Execute
		switch (mipstype){
		case 'I':{
			bitset<6> opcode;
			bitset<5> rsi;
			bitset<5> rti;
			bitset<16> immi;
	        for (index=0; index<=5; index++)
	            opcode[5-index]=instruction[31-index];
	        for (index=0; index<=4; index++)
	            rsi[4-index]=instruction[25-index];
	        for (index=0; index<=4; index++)
	            rti[4-index]=instruction[20-index];
	        for (index=0; index<=15; index++)
	            immi[15-index]=instruction[15-index];
			bitset<3> iop (0);
			if (opcode.to_string()==("001001")){
				myRF.ReadWrite(rsi, bitset<5>(0), bitset<5>(0), bitset<32>(0), bitset<1>(0));
				myRF.ReadWrite(bitset<5>(0), bitset<5>(0), rti, myALU.ALUOperation(iop, myRF.ReadData1, bitset<32>(immi.to_ulong())), bitset<1>(1));
				PC=PC+4;
			}   //addiu
			else if (opcode.to_string()==("100011")){
                myRF.ReadWrite(rsi, bitset<5>(0), bitset<5>(0), bitset<32>(0), bitset<1>(0));
                myRF.ReadWrite(bitset<5>(0), bitset<5>(0), rti, myDataMem.MemoryAccess(bitset<32>(myRF.ReadData1.to_ulong()+immi.to_ulong()), bitset<32>(0), bitset<1>(1), bitset<1>(0)), bitset<1>(1));
                PC=PC+4;
			}	//load word
			else if (opcode.to_string()==("101011")){
                myRF.ReadWrite(rsi, rti, bitset<5>(0), bitset<32> (0), bitset<1>(0));
                myDataMem.MemoryAccess(bitset<32>(myRF.ReadData1.to_ulong()+immi.to_ulong()), myRF.ReadData2, bitset<1>(0), bitset<1>(1));
                PC=PC+4;
			}	//write word
			else
            {
                myRF.ReadWrite(rsi, rti, bitset<5>(0), bitset<32> (0), bitset<1>(0));
                if (myRF.ReadData1.to_ulong()==myRF.ReadData2.to_ulong())
                {
                    bitset<32> iaddress;
                    pcvalue=bitset<32>(PC);
                    for (index=0;index<=13;index++)
                        iaddress[31-index]=immi[15];
                    for (index=0; index<=15; index++)
                        iaddress[17-index] = immi[15-index];
                    iaddress.reset(1);
                    iaddress.reset(0);
                    PC=PC+4+iaddress.to_ulong();
                }
            }    //beq
			break;
			}
		case 'R':{
			bitset<5> rsr;
			bitset<5> rtr;
			bitset<5> rdr;
			bitset<6> funct;
			bitset<3> aluop;
	        for (index=0; index<=4; index++)
	            rsr[4-index]=instruction[25-index];
	        for (index=0; index<=4; index++)
	            rtr[4-index]=instruction[20-index];
	        for (index=0; index<=4; index++)
	            rdr[4-index]=instruction[15-index];
	        for (index=0; index<=5; index++)
	            funct[5-index]=instruction[5-index];

			if (funct.to_string()==("100001")) aluop=bitset<3>(ADDU);
			if (funct.to_string()==("100011")) aluop=bitset<3>(SUBU);
			if (funct.to_string()==("100100")) aluop=bitset<3>(AND);
			if (funct.to_string()==("100101")) aluop=bitset<3>(OR);
			if (funct.to_string()==("100111")) aluop=bitset<3>(NOR);
			myRF.ReadWrite(rsr, rtr, bitset<5>(0), bitset<32>(0), bitset<1>(0));
			myRF.ReadWrite(bitset<5>(0), bitset<5>(0), rdr, myALU.ALUOperation(aluop, myRF.ReadData1, myRF.ReadData2), bitset<1>(1));
			PC=PC+4;
			break;
			}
		case 'J':{
			bitset<32> jaddress;
			bitset<32> pcvalue;
		    PC=PC+4;
		    pcvalue=bitset<32>(PC);
		    for (index=0;index<=3;index++)
                jaddress[31-index]=pcvalue[31-index];
		    for (index=0; index<=25; index++)
                jaddress[27-index] = instruction[25-index];
            jaddress.reset(1);
            jaddress.reset(0);
            PC=jaddress.to_ulong();
			break;
			}
		}
        myRF.OutputRF(); // dump RF;
    }
        myDataMem.OutputDataMem(); // dump data mem

        return 0;

}
