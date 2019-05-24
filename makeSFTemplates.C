#include "setTDRStyle.h"
#include <sstream>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include <vector>
#include "Rtypes.h"
#include "TColor.h"
#include "TVectorF.h"
#include <cstdlib>

TH1D *create1Dhisto(TString sample, TTree *tree,TString intLumi,TString cuts,TString branch,int bins,float xmin,float xmax,
		    bool useLog,int color, int style,TString name,bool norm,bool data);
void setTDRStyle();

void makeSFTemplates(TString object, TString algo, TString wp, TString ptrange,TString whichbit, bool pass) {

  TString passstr = "pass"; if (pass) { passstr = "pass"; } else { passstr = "fail"; }
  TString name = object+"_"+algo+"_"+wp+"_"+whichbit+"_"+ptrange+"_"+passstr;


  setTDRStyle();
  gROOT->SetBatch(true);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  gStyle->SetPalette(1);
  TH1::SetDefaultSumw2(kTRUE);


  //TFile *f_mc   = TFile::Open("/afs/cern.ch/user/c/cmantill/public/forSangEon/sklimWtagAK8v12017WP026/PseudoData_AK8v12017WP026.root" , "READONLY" );
  //TFile *f_data = TFile::Open("/afs/cern.ch/user/c/cmantill/public/forSangEon/sklimWtagAK8v12017WP026/Data_AK8v12017WP026.root" , "READONLY" );
  if (whichbit == "04"){
    TFile *f_mc   = TFile::Open("/afs/cern.ch/user/c/cmantill/public/forSangEon/bits04-vbf/PseudoData.root" , "READONLY" );
    TFile *f_data = TFile::Open("/afs/cern.ch/user/c/cmantill/public/forSangEon/bits04-vbf/Data.root" , "READONLY" );
  }
  else if (whichbit == "05"){
    TFile *f_mc   = TFile::Open("/afs/cern.ch/user/c/cmantill/public/forSangEon/bits05-vbf/PseudoData.root" , "READONLY" );
    TFile *f_data = TFile::Open("/afs/cern.ch/user/c/cmantill/public/forSangEon/bits05-vbf/Data.root" , "READONLY" );
  }
  else{
    perror("enter either 04 or 05 for bit version");
  }

  TTree *t_mc   = (TTree*)f_mc->Get("otree2");
  TTree *t_data = (TTree*)f_data->Get("otree2");
  
  std::vector<TTree *> samples; samples.clear();
  samples.push_back(t_mc);
  samples.push_back(t_data);

  //float intLumi     = 41.1;
  float intLumi     = 59.0;
  ostringstream tmpLumi;
  tmpLumi << intLumi;
  TString lumi = tmpLumi.str();

  // selection on algo
  TString c_algo_wp;
  /*
  if (algo == "lsf") { 
    if (wp == "test") { c_algo_wp = "(AK8Puppijet1_lsfC_3>0.7)"; } // pass is > 0.7
  } */
  if (algo == "n2") {
    if (wp == "hbb") { c_algo_wp = "(Puppijet0_N2DDT<0)"; } // pass is < 0. 
  }

  // mass selection
  TString c_mass = "(Puppijet0_msd>50. && Puppijet0_msd<110.)";
  
  // pt range
  TString c_ptrange;
  //if ( ((TString)object=="T") && (ptrange == "incl") )   { c_ptrange = "(AK8Puppijet0_pt>300.)"; }
  //if ( ((TString)object=="T") && (ptrange == "low") )    { c_ptrange = "(AK8Puppijet0_pt>300. && AK8Puppijet0_pt<=400.)";  }
  //if ( ((TString)object=="T") && (ptrange == "med") )    { c_ptrange = "(AK8Puppijet0_pt>400. && AK8Puppijet0_pt<=600.)";  }
  c_ptrange = "(Puppijet0_pt>200.)";



  // pass or fail tagging score
  if (pass) { c_algo_wp = "("+c_algo_wp+")";  }
  else      { c_algo_wp = "!("+c_algo_wp+")"; }

  // matching definition: modify!!
  //TString c_p2 = "( Puppijet0_vMatching < 0.8 && Puppijet0_isHadronicV==1 && Puppijet0_vMatching > 0.)";
  TString c_p2 = "( Puppijet0_vMatching < 0.8 && (Puppijet0_isHadronicV==1 ||Puppijet0_isHadronicV==2) && Puppijet0_vMatching > 0.)";
  TString c_p1 = "(!("+c_p2+"))";

  // final set of cuts
  //TString cut = c_base+" && "+c_algo_wp+" && "+c_mass+" && "+c_ptrange; std::cout << cut << "\n";
  TString cut = c_algo_wp+" && "+c_mass+" && "+c_ptrange; std::cout << cut << "\n";

  std::vector<TString> cuts; cuts.clear();
  cuts.push_back(cut);
  cuts.push_back(cut+" && "+c_p2);
  cuts.push_back(cut+" && "+c_p1);

  std::vector<TString> leg_sample; leg_sample.clear();
  leg_sample.push_back("matched");
  leg_sample.push_back("unmatched");
  leg_sample.push_back("Data");

  // create histos
  TH1D *h_incl = create1Dhisto(name,samples[0],lumi,cuts[0],"Puppijet0_msd",20,50.,110.,false,1,1,"h_"+name+"_incl",false,false);      h_incl->SetFillColor(0);
  TH1D *h_p2   = create1Dhisto(name,samples[0],lumi,cuts[1],"Puppijet0_msd",20,50.,110.,false,kRed+1,1,"catp2",false,false);   h_p2->SetFillColor(0);
  TH1D *h_p1   = create1Dhisto(name,samples[0],lumi,cuts[2],"Puppijet0_msd",20,50.,110.,false,kGreen-1,1,"catp1",false,false); h_p1->SetFillColor(0);

  TH1D *h_data = create1Dhisto(name,samples[1],lumi,cuts[0],"Puppijet0_msd",20,50.,110.,false,1,1,"data_obs",false,true); h_data->SetFillColor(0);


  h_data->SetMarkerColor(1); h_data->SetMarkerSize(1.2); h_data->SetMarkerStyle(20);
  h_data->SetLineWidth(1);

  //pileup systematics
  TH1D *h_p2_puup = create1Dhisto(name,samples[0],lumi,cuts[1],"Puppijet0_msd",20,50.,110.,false,kRed+1,1,"catp2_puUp",false,false);   h_p2_puup->SetFillColor(0);
  TH1D *h_p1_puup = create1Dhisto(name,samples[0],lumi,cuts[2],"Puppijet0_msd",20,50.,110.,false,kGreen-1,1,"catp1_puUp",false,false); h_p1_puup->SetFillColor(0);
  TH1D *h_p2_pudn = create1Dhisto(name,samples[0],lumi,cuts[1],"Puppijet0_msd",20,50.,110.,false,kRed+1,1,"catp2_puDown",false,false);   h_p2_pudn->SetFillColor(0);
  TH1D *h_p1_pudn = create1Dhisto(name,samples[0],lumi,cuts[2],"Puppijet0_msd",20,50.,110.,false,kGreen-1,1,"catp1_puDown",false,false); h_p1_pudn->SetFillColor(0);
  // avoid zero bins in mc
  for (unsigned int ii=0; ii<h_incl->GetNbinsX(); ++ii) {
    if (h_p2->GetBinContent(ii)<=0) { h_p2->SetBinContent(ii,0.001); h_p2->SetBinError(ii,0.001); }
    if (h_p1->GetBinContent(ii)<=0) { h_p1->SetBinContent(ii,0.001); h_p1->SetBinError(ii,0.001); }
    if (h_p2_puup->GetBinContent(ii)<=0) { h_p2_puup->SetBinContent(ii,0.001); h_p2_puup->SetBinError(ii,0.001); }
    if (h_p1_puup->GetBinContent(ii)<=0) { h_p1_puup->SetBinContent(ii,0.001); h_p1_puup->SetBinError(ii,0.001); }
    if (h_p2_pudn->GetBinContent(ii)<=0) { h_p2_pudn->SetBinContent(ii,0.001); h_p2_pudn->SetBinError(ii,0.001); }
    if (h_p1_pudn->GetBinContent(ii)<=0) { h_p1_pudn->SetBinContent(ii,0.001); h_p1_pudn->SetBinError(ii,0.001); }

  }

  TString xname = "m_{SD} [GeV]";
  h_p2->GetXaxis()->SetTitle(xname);
  h_p1->GetXaxis()->SetTitle(xname);
  h_data->GetXaxis()->SetTitle(xname);
 
  h_p2_puup->GetXaxis()->SetTitle((TString)xname);
  h_p1_puup->GetXaxis()->SetTitle((TString)xname);
  h_p2_pudn->GetXaxis()->SetTitle((TString)xname);
  h_p1_pudn->GetXaxis()->SetTitle((TString)xname);
 
  // make dir
  TString dirname1 = object+"_"+algo;
  const int dir_err = system("mkdir -p ./"+dirname1);
  if (-1 == dir_err) { printf("Error creating directory!n"); exit(1); }

  TString nameoutfile = name+"_pre.root"; 
  //TString nameoutfile = name+".root";
  TFile *fout = new TFile("./"+dirname1+"/"+nameoutfile,"RECREATE");

  h_p2->Write();
  h_p1->Write();
  h_data->Write();

  h_p2_puup->Write();
  h_p1_puup->Write();
  h_p2_pudn->Write();
  h_p1_pudn->Write();

  fout->Close();
  std::cout << "\n\n";
}


TH1D *create1Dhisto(TString sample,TTree *tree,TString intLumi,TString cuts,TString branch,int bins,float xmin,float xmax,
		    bool useLog,int color, int style,TString name,bool norm,bool data) {
  TH1::SetDefaultSumw2(kTRUE);

  TString cut;
  if (data) { cut ="("+cuts+")"; } 
  else {
    //cut ="(xsecWeight*puWeight*"+intLumi+")*("+cuts+")";
    cut ="(weight*"+intLumi+")*("+cuts+")";
  }
  
  std::cout << "cut = " << cut << "\n";

  TH1D *hTemp = new TH1D(name,name,bins,xmin,xmax); //hTemp->SetName(name);
  tree->Project(name,branch,cut);

  hTemp->SetLineWidth(3);
  hTemp->SetMarkerSize(0);
  hTemp->SetLineColor(color);
  hTemp->SetFillColor(color);
  hTemp->SetLineStyle(style);

  // ad overflow bin             
  double error =0.; double integral = hTemp->IntegralAndError(bins,bins+1,error);
  hTemp->SetBinContent(bins,integral);
  hTemp->SetBinError(bins,error);

  if (norm) { hTemp->Scale(1./(hTemp->Integral())); }

  return hTemp;
} //~ end of create1Dhisto
