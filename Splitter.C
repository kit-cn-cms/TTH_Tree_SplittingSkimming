#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "TGraph.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TRandom3.h"
#include "TTreeFormula.h"


class Splitter{
public:
  Splitter();
  ~Splitter();
  void PrepareTrees();

private:
  TString OutputDir;
  TString InName;
  TString OutName;
  TString SplitMode;
  TString Systematic;
  TString FlavorSplitting;
  
  bool useFlavorSplitting=false;

  std::vector<TString> CategoryNames;
  std::vector<TString> CategorySelections;

  std::vector<double> EvenYields;
  std::vector<double> OddYields;
  std::vector<double> lf_EvenYields;
  std::vector<double> lf_OddYields;
  std::vector<double> bb_EvenYields;
  std::vector<double> bb_OddYields;
  std::vector<double> b_EvenYields;
  std::vector<double> b_OddYields;
  std::vector<double> b2_EvenYields;
  std::vector<double> b2_OddYields;
  std::vector<double> cc_EvenYields;
  std::vector<double> cc_OddYields;
  int nCats=0;
  
  std::vector<TTreeFormula*> CatFormulas;
  TTreeFormula* EvenFormula;
  TTreeFormula* OddFormula;
  TTreeFormula* bb_FlavorFormula;
  TTreeFormula* b2_FlavorFormula;
  TTreeFormula* b_FlavorFormula;
  TTreeFormula* cc_FlavorFormula;
  TTreeFormula* lf_FlavorFormula;
  
  TTreeFormula* weightFormula;
  
  TTree* inTree;
  std::vector<TTree*> evenTrees;
  std::vector<TTree*> oddTrees;
  std::vector<TTree*> lf_evenTrees;
  std::vector<TTree*> lf_oddTrees;
  std::vector<TTree*> bb_evenTrees;
  std::vector<TTree*> bb_oddTrees;
  std::vector<TTree*> cc_evenTrees;
  std::vector<TTree*> cc_oddTrees;
  std::vector<TTree*> b2_evenTrees;
  std::vector<TTree*> b2_oddTrees;
  std::vector<TTree*> b_evenTrees;
  std::vector<TTree*> b_oddTrees;
  
  std::vector<TFile*> outfiles;
    
   TString branchListFile="/nfs/dust/cms/user/kelmorab/splittingSpring17/TTH_Analysis_Chain/ParallelChain/branchlist.txt";
//  TString branchListFile="";
  std::vector<TString> relevantbranches;
  
};

Splitter::Splitter(){

  char* configfilename = getenv ("myCONFIGFILE");
  std::cout<<"Reading "<<configfilename<<std::endl;
  std::ifstream config(configfilename);
  std::string dump;
  std::string subdump;
  int posspace1;
  int posspace2;
    bool readline=true;
  int count=0;
  std::cout<<"reading Config"<<std::endl;
  while(readline==true && count<400){
    TString thisCatName;
    TString thisCatSelection;
    count++;
    std::getline(config,dump);
    std::cout<<dump<<std::endl;
    if(dump.find("INNAME")!=std::string::npos){
      posspace1=dump.find(" ");
      subdump=dump.substr(posspace1+1);
      InName=subdump;
    }
    if(dump.find("OUTNAME")!=std::string::npos){
      posspace1=dump.find(" ");
      subdump=dump.substr(posspace1+1);
      OutName=subdump;
    }
    if(dump.find("OUTDIR")!=std::string::npos){
      posspace1=dump.find(" ");
      subdump=dump.substr(posspace1+1);
      OutputDir=subdump;
    }
    if(dump.find("SPLITMODE")!=std::string::npos){
      posspace1=dump.find(" ");
      subdump=dump.substr(posspace1+1);
      SplitMode=subdump;
    }
    if(dump.find("FLAVORSPLITTING")!=std::string::npos){
      posspace1=dump.find(" ");
      subdump=dump.substr(posspace1+1);
      FlavorSplitting=subdump;
    }
    if(dump.find("SYSTEMATIC")!=std::string::npos){
      posspace1=dump.find(" ");
      subdump=dump.substr(posspace1+1);
      Systematic=subdump;
    }
    if(dump.find("CATEGORY")!=std::string::npos){
      posspace1=dump.find(" ");
      posspace2=dump.find(" ",posspace1+1);
      subdump=dump.substr(posspace1+1,posspace2-posspace1-1);
      std::cout<<posspace1<<" "<<posspace2<<" "<<posspace2-posspace1<<std::endl;
      thisCatName=subdump;
      subdump=dump.substr(posspace2+1);
      thisCatSelection=subdump;
      CategoryNames.push_back(thisCatName);
      CategorySelections.push_back(thisCatSelection);
    }
    
    if(config.eof())readline=false;

  }
  config.close();
  
  //setup up relevant branch
  count=0;
  readline=true;
  if(branchListFile!=""){
  std::ifstream branchff(branchListFile);
  while(readline==true && count<5000){
    count++;
    branchff>>dump;
    std::cout<<dump<<std::endl;
    relevantbranches.push_back(dump);
    
    if(branchff.eof())readline=false;
  }
  branchff.close();
  }
  else{
    std::cout<<"copying all branches"<<std::endl;
  }
  
  std::cout<<std::endl;
  std::cout<<InName<<std::endl;
  std::cout<<OutName<<std::endl;
  std::cout<<OutputDir<<std::endl;
  for(int i=0;i<CategoryNames.size();i++){ std::cout<<CategoryNames.at(i)<<" sss "<<CategorySelections.at(i)<<std::endl;}
  nCats=CategoryNames.size();
  for(int icat=0;icat<nCats;icat++){
    EvenYields.push_back(0.0);
    OddYields.push_back(0.0);
    lf_EvenYields.push_back(0.0);
    lf_OddYields.push_back(0.0);
    bb_EvenYields.push_back(0.0);
    bb_OddYields.push_back(0.0);
    b2_EvenYields.push_back(0.0);
    b2_OddYields.push_back(0.0);
    b_EvenYields.push_back(0.0);
    b_OddYields.push_back(0.0);
    cc_EvenYields.push_back(0.0);
    cc_OddYields.push_back(0.0);
  }
  
  if(FlavorSplitting=="True")useFlavorSplitting=true;
    
  TFile* inTreeFile=new TFile(InName);
  inTree = (TTree*) inTreeFile->Get("MVATree");
  std::cout<<inTree<<std::endl;
  inTree->SetBranchStatus("*",0);
  if(relevantbranches.size()>0){
  for(int ib=0;ib<relevantbranches.size();ib++){
    std::cout<<relevantbranches.at(ib)<<std::endl;
    inTree->SetBranchStatus(relevantbranches.at(ib),1);
  }
  }
  else{
    inTree->SetBranchStatus("*",1);
  }
  std::cout<<"setting up Categories"<<std::endl;
  for(int icat=0;icat<nCats;icat++){
    TString thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_"+Systematic+".root";
    TFile* thisevenfile = new TFile(thisoutname,"RECREATE");
    thisevenfile->cd();
//     TTree* outevenTree = inTree->CloneTree(0);
    evenTrees.push_back(inTree->CloneTree(0));
    outfiles.push_back(thisevenfile);
    evenTrees.back()->SetAutoFlush(1);

    std::cout<<thisevenfile<<std::endl;
    std::cout<<evenTrees.back()->GetDirectory()<<" "<<evenTrees.back()->GetCurrentFile()<<std::endl;
//     evenTrees.back()->GetDirectory()->Print();
    
    thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_"+Systematic+".root";
    TFile* thisoddfile = new TFile(thisoutname,"RECREATE");
    thisoddfile->cd();
    oddTrees.push_back(inTree->CloneTree(0));
    outfiles.push_back(thisoddfile);
    std::cout<<thisoddfile<<std::endl;
    std::cout<<oddTrees.back()->GetDirectory()<<" "<<oddTrees.back()->GetCurrentFile()<<std::endl;
//     evenTrees.back()->GetDirectory()->Print();
    oddTrees.back()->SetAutoFlush(1);

	
    if(useFlavorSplitting){
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_lf_"+Systematic+".root";
      TFile* lf_thisevenfile = new TFile(thisoutname,"RECREATE");
      lf_thisevenfile->cd();
      lf_evenTrees.push_back(inTree->CloneTree(0));
      outfiles.push_back(lf_thisevenfile);
      lf_evenTrees.back()->SetAutoFlush(1);
      
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_lf_"+Systematic+".root";
      TFile* lf_thisoddfile = new TFile(thisoutname,"RECREATE");
      lf_thisoddfile->cd();
      lf_oddTrees.push_back(inTree->CloneTree(0));
      outfiles.push_back(lf_thisoddfile);
      lf_oddTrees.back()->SetAutoFlush(1);

      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_bb_"+Systematic+".root";
      TFile* bb_thisevenfile = new TFile(thisoutname,"RECREATE");
      bb_thisevenfile->cd();
      bb_evenTrees.push_back(inTree->CloneTree(0));
       outfiles.push_back(bb_thisevenfile);
            bb_evenTrees.back()->SetAutoFlush(1);

      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_bb_"+Systematic+".root";
      TFile* bb_thisoddfile = new TFile(thisoutname,"RECREATE");
      bb_thisoddfile->cd();
      bb_oddTrees.push_back(inTree->CloneTree(0));
      outfiles.push_back(bb_thisoddfile);
            bb_oddTrees.back()->SetAutoFlush(1);

      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_b_"+Systematic+".root";
      TFile* b_thisevenfile = new TFile(thisoutname,"RECREATE");
      b_thisevenfile->cd();
      b_evenTrees.push_back(inTree->CloneTree(0));
       outfiles.push_back(b_thisevenfile);
            b_evenTrees.back()->SetAutoFlush(1);

      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_b_"+Systematic+".root";
      TFile* b_thisoddfile = new TFile(thisoutname,"RECREATE");
      b_thisoddfile->cd();
      b_oddTrees.push_back(inTree->CloneTree(0));
      outfiles.push_back(b_thisoddfile);
       std::cout<<b_thisoddfile<<std::endl;
    std::cout<< b_oddTrees.back()->GetDirectory()<<" "<< b_oddTrees.back()->GetCurrentFile()<<std::endl;
            b_oddTrees.back()->SetAutoFlush(1);

      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_2b_"+Systematic+".root";
      TFile* b2_thisevenfile = new TFile(thisoutname,"RECREATE");
      b2_thisevenfile->cd();
      b2_evenTrees.push_back(inTree->CloneTree(0));
      outfiles.push_back(b2_thisevenfile);
            b2_evenTrees.back()->SetAutoFlush(1);

      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_2b_"+Systematic+".root";
      TFile* b2_thisoddfile = new TFile(thisoutname,"RECREATE");
      b2_thisoddfile->cd();
      b2_oddTrees.push_back(inTree->CloneTree(0));
      outfiles.push_back(b2_thisoddfile);
            b2_oddTrees.back()->SetAutoFlush(1);

      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_cc_"+Systematic+".root";
      TFile* cc_thisevenfile = new TFile(thisoutname,"RECREATE");
      cc_thisevenfile->cd();
      cc_evenTrees.push_back(inTree->CloneTree(0));
       outfiles.push_back(cc_thisevenfile);
            cc_evenTrees.back()->SetAutoFlush(1);

      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_cc_"+Systematic+".root";
      TFile* cc_thisoddfile = new TFile(thisoutname,"RECREATE");
      cc_thisoddfile->cd();
      cc_oddTrees.push_back(inTree->CloneTree(0));
      outfiles.push_back(cc_thisoddfile);
            cc_oddTrees.back()->SetAutoFlush(1);

    }
  }
  std::cout<<"done setting up Categories"<<std::endl;
  std::cout<<"setting up Forumulas"<<std::endl;

  // set up cat formulas
  for(int icat=0;icat<nCats;icat++){
    CatFormulas.push_back(new TTreeFormula(CategoryNames.at(icat),CategorySelections.at(icat),inTree));
  }
  EvenFormula = new TTreeFormula("evenForm","Evt_Odd==0",inTree);
  OddFormula = new TTreeFormula("oddForm","Evt_Odd==1",inTree);
  bb_FlavorFormula = new TTreeFormula("bbForm","GenEvt_I_TTPlusBB==3",inTree);
  b_FlavorFormula = new TTreeFormula("bForm","GenEvt_I_TTPlusBB==1",inTree);
  b2_FlavorFormula = new TTreeFormula("b2Form","GenEvt_I_TTPlusBB==2",inTree);
  cc_FlavorFormula = new TTreeFormula("ccForm","GenEvt_I_TTPlusCC==1",inTree);
  lf_FlavorFormula = new TTreeFormula("lfForm","GenEvt_I_TTPlusCC==0&&GenEvt_I_TTPlusBB==0",inTree);
  weightFormula = new TTreeFormula("weightForm","Weight",inTree);
  std::cout<<"done setting up Forumulas"<<std::endl;
  
}

Splitter::~Splitter(){
 delete EvenFormula;
  delete OddFormula;
  delete bb_FlavorFormula;
  delete b2_FlavorFormula;
  delete b_FlavorFormula;
  delete cc_FlavorFormula;
  delete lf_FlavorFormula;
  
  delete weightFormula;
  
  delete inTree;  
  /*for(int iTree=0;iTree<evenTrees.size();iTree++){
    delete evenTrees.at(iTree);
    delete oddTrees.at(iTree);
  }
  if(lf_evenTrees.size()>0){
  for(int iTree=0;iTree<lf_evenTrees.size();iTree++){
    delete lf_evenTrees.at(iTree);
    delete lf_oddTrees.at(iTree);
        delete bb_evenTrees.at(iTree);
    delete bb_oddTrees.at(iTree);
        delete b_evenTrees.at(iTree);
    delete b_oddTrees.at(iTree);
        delete b2_evenTrees.at(iTree);
    delete b2_oddTrees.at(iTree);
        delete cc_evenTrees.at(iTree);
    delete cc_oddTrees.at(iTree);
  }
  }
  */
  for(int iform=0; iform<CatFormulas.size();iform++){
    delete CatFormulas.at(iform);
  }
 
}

void Splitter::PrepareTrees(){
  int nEventsTotal=inTree->GetEntries();
  for(long ievt=0; ievt<nEventsTotal; ievt++){
//     useFlavorSplitting=false;
    if(ievt%100==0)std::cout<<"at event "<<ievt<<std::endl;
    inTree->GetEntry(ievt);
    bool isEven=false;
    Double_t thisWeight=weightFormula->EvalInstance();
    for(int icat=0;icat<nCats;icat++){
      if(CatFormulas.at(icat)->EvalInstance()!=0){
        if(EvenFormula->EvalInstance()!=0)isEven=true;
	if(isEven==true or SplitMode=="None"){
	  evenTrees.at(icat)->Fill();
	  EvenYields.at(icat)+=thisWeight;
	  if(useFlavorSplitting){
	    if(lf_FlavorFormula->EvalInstance()!=0){ lf_evenTrees.at(icat)->Fill(); lf_EvenYields.at(icat)+=thisWeight;}
  	    if(bb_FlavorFormula->EvalInstance()!=0){ bb_evenTrees.at(icat)->Fill(); bb_EvenYields.at(icat)+=thisWeight;}
	    if(b_FlavorFormula->EvalInstance()!=0){ b_evenTrees.at(icat)->Fill(); b_EvenYields.at(icat)+=thisWeight;}
	    if(b2_FlavorFormula->EvalInstance()!=0){ b2_evenTrees.at(icat)->Fill(); b2_EvenYields.at(icat)+=thisWeight;}
	    if(cc_FlavorFormula->EvalInstance()!=0){ cc_evenTrees.at(icat)->Fill(); cc_EvenYields.at(icat)+=thisWeight;}
	  }
        }
        if(isEven==false or SplitMode=="None"){
	  oddTrees.at(icat)->Fill();
	  OddYields.at(icat)+=thisWeight;
	  if(useFlavorSplitting){
	    if(lf_FlavorFormula->EvalInstance()!=0){ lf_oddTrees.at(icat)->Fill(); lf_OddYields.at(icat)+=thisWeight;}
  	    if(bb_FlavorFormula->EvalInstance()!=0){ bb_oddTrees.at(icat)->Fill(); bb_OddYields.at(icat)+=thisWeight;}
	    if(b_FlavorFormula->EvalInstance()!=0){ b_oddTrees.at(icat)->Fill(); b_OddYields.at(icat)+=thisWeight;}
	    if(b2_FlavorFormula->EvalInstance()!=0){ b2_oddTrees.at(icat)->Fill(); b2_OddYields.at(icat)+=thisWeight;}
	    if(cc_FlavorFormula->EvalInstance()!=0){ cc_oddTrees.at(icat)->Fill(); cc_OddYields.at(icat)+=thisWeight;}
	  }
        }
      }//end cat eval
    }//end cat loop
  }//end event loop
  
  std::cout<<"saving Trees"<<std::endl;
  // save trees 
  /*for(int icat=0;icat<nCats;icat++){
    TString thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_"+Systematic+".root";
    TFile* thisevenfile = new TFile(thisoutname,"RECREATE");
    thisevenfile->cd();
    evenTrees.at(icat)->Write();
    thisevenfile->Close();
    if(useFlavorSplitting){
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_lf_"+Systematic+".root";
      TFile* lf_thisevenfile = new TFile(thisoutname,"RECREATE");
      lf_thisevenfile->cd();
      lf_evenTrees.at(icat)->Write();
      lf_thisevenfile->Close();
      
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_bb_"+Systematic+".root";
      TFile* bb_thisevenfile = new TFile(thisoutname,"RECREATE");
      bb_thisevenfile->cd();
      bb_evenTrees.at(icat)->Write();
      bb_thisevenfile->Close();
   
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_b2_"+Systematic+".root";
      TFile* b2_thisevenfile = new TFile(thisoutname,"RECREATE");
      b2_thisevenfile->cd();
      b2_evenTrees.at(icat)->Write();
      b2_thisevenfile->Close();
      
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_b_"+Systematic+".root";
      TFile* b_thisevenfile = new TFile(thisoutname,"RECREATE");
      b_thisevenfile->cd();
      b_evenTrees.at(icat)->Write();
      b_thisevenfile->Close();
      
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Even/"+OutName+"_cc_"+Systematic+".root";
      TFile* cc_thisevenfile = new TFile(thisoutname,"RECREATE");
      cc_thisevenfile->cd();
      cc_evenTrees.at(icat)->Write();
      cc_thisevenfile->Close();
      
       delete lf_thisevenfile;
  delete bb_thisevenfile;
  delete b_thisevenfile;
  delete b2_thisevenfile;
  delete cc_thisevenfile;
    }
    
    thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_"+Systematic+".root";
    TFile* thisoddfile = new TFile(thisoutname,"RECREATE");
    thisoddfile->cd();
    oddTrees.at(icat)->Write();
    thisoddfile->Close();
    if(useFlavorSplitting){
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_lf_"+Systematic+".root";
      TFile* lf_thisoddfile = new TFile(thisoutname,"RECREATE");
      lf_thisoddfile->cd();
      lf_oddTrees.at(icat)->Write();
      lf_thisoddfile->Close();
      
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_bb_"+Systematic+".root";
      TFile* bb_thisoddfile = new TFile(thisoutname,"RECREATE");
      bb_thisoddfile->cd();
      bb_oddTrees.at(icat)->Write();
      bb_thisoddfile->Close();
   
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_b2_"+Systematic+".root";
      TFile* b2_thisoddfile = new TFile(thisoutname,"RECREATE");
      b2_thisoddfile->cd();
      b2_oddTrees.at(icat)->Write();
      b2_thisoddfile->Close();
      
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_b_"+Systematic+".root";
      TFile* b_thisoddfile = new TFile(thisoutname,"RECREATE");
      b_thisoddfile->cd();
      b_oddTrees.at(icat)->Write();
      b_thisoddfile->Close();
      
      thisoutname=OutputDir+"/Category_"+CategoryNames.at(icat)+"/Odd/"+OutName+"_cc_"+Systematic+".root";
      TFile* cc_thisoddfile = new TFile(thisoutname,"RECREATE");
      cc_thisoddfile->cd();
      cc_oddTrees.at(icat)->Write();
      cc_thisoddfile->Close();
      
      
  delete lf_thisoddfile;
  delete bb_thisoddfile;
  delete b_thisoddfile;
  delete b2_thisoddfile;
  delete cc_thisoddfile;
    }
  delete thisevenfile;
  delete thisoddfile;

  }*/
  
  for(int icat=0;icat<nCats;icat++){
    evenTrees.at(icat)->AutoSave();
    if(useFlavorSplitting){
      lf_evenTrees.at(icat)->AutoSave();
      
      bb_evenTrees.at(icat)->AutoSave();
      
      b2_evenTrees.at(icat)->AutoSave();
      
      b_evenTrees.at(icat)->AutoSave();
      
      cc_evenTrees.at(icat)->AutoSave();
      
    }
    
    
    oddTrees.at(icat)->AutoSave();
    
    if(useFlavorSplitting){
      
      lf_oddTrees.at(icat)->AutoSave();
      
      bb_oddTrees.at(icat)->AutoSave();
      
      b2_oddTrees.at(icat)->AutoSave();
      
      b_oddTrees.at(icat)->AutoSave();
      
      cc_oddTrees.at(icat)->AutoSave();

    }
  }
  
  for(int ifile=0; ifile<outfiles.size();ifile++){
    outfiles.at(ifile)->Close();
  }
    
  
  std::cout<<"done saving Trees"<<std::endl;
}



# ifndef __CINT__
int main()
{
  Splitter mySplitter=Splitter();
  std::cout<<"done constructor"<<std::endl;

  mySplitter.PrepareTrees();
  std::cout<<"done preparing Trees"<<std::endl;

  return 0;
}
# endif


