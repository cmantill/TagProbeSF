#include "setTDRStyle.h"
#include <sstream>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include <vector>
#include "Rtypes.h"
#include "TColor.h"
#include "TVectorF.h"
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <string>

TH1F *rescaleXaxis(TH1F *inputhisto, float xmin, float xmax);
void rescaleXaxis(TGraphAsymmErrors *inputhisto, double xmin, double scale);
TH1F *getDataMCratio(TGraphAsymmErrors *indata, TH1F *inMC);
void setTDRStyle();
void makeDataMCPlotsFromCombine(TString path2file, TString filename, TString score, TString ptrange, TString category,
 float xmin, float xmax, int nbins,TString xaxisname, bool log);


void makePlots(TString path2file, TString filename, TString score, TString ptrange, float xmin, float xmax, int nbins, TString xaxis) {
  TH1::SetDefaultSumw2(kTRUE);

  setTDRStyle();
  gROOT->SetBatch(false);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  gStyle->SetPalette(1);
  gStyle->SetOptTitle(0);
  TH1::SetDefaultSumw2(kTRUE);
  
  // make pre/post fit plots
  makeDataMCPlotsFromCombine(path2file,filename,score,ptrange,"pass",xmin,xmax,nbins,xaxis,false);
  makeDataMCPlotsFromCombine(path2file,filename,score,ptrange,"fail",xmin,xmax,nbins,xaxis,false);

}


void makeDataMCPlotsFromCombine(TString path2file, TString filename, TString score, TString ptrange, TString category,
 float xmin, float xmax, int nbins,TString xaxisname, bool log) {

  setTDRStyle();
  gROOT->SetBatch(true);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetPalette(1);
  TH1::SetDefaultSumw2(kTRUE);

  if (xaxisname == "mass") { xaxisname = "m(jet) [GeV]"; }

  const int dir_err = system("mkdir -p ./"+(TString)path2file+"/plots_datamc");
  if (-1 == dir_err) { printf("Error creating directory!n"); exit(1); }
  TFile *fdiag = TFile::Open("./"+path2file+"/sf_fitDiagnostics_"+filename+".root", "READONLY" );

  // get prefit histograms
  TString prefitstr = "shapes_prefit";
  TH1F *h_prefit_catp2_ = (TH1F*)fdiag->Get(prefitstr+"/"+category+"/catp2"); h_prefit_catp2_->SetName("h_catp2_");
  TH1F *h_prefit_catp1_ = (TH1F*)fdiag->Get(prefitstr+"/"+category+"/catp1"); h_prefit_catp1_->SetName("h_catp1_");
  TH1F *h_prefit_total_ = (TH1F*)fdiag->Get(prefitstr+"/"+category+"/total"); h_prefit_total_->SetName("h_total_");
  TH1F *h_prefit_catp2 = rescaleXaxis(h_prefit_catp2_,xmin,xmax); h_prefit_catp2->SetName("h_catp2");
  TH1F *h_prefit_catp1 = rescaleXaxis(h_prefit_catp1_,xmin,xmax); h_prefit_catp1->SetName("h_catp1");
  TH1F *h_prefit_total = rescaleXaxis(h_prefit_total_,xmin,xmax); h_prefit_total->SetName("h_total");
  
  // get postfit histograms
  TTree *tree = (TTree*)fdiag->Get("tree_fit_sb");
  TH1F *h_fit_status = new TH1F("h_fit_status_","h_fit_status_",20,-10.,10.); 
  tree->Project("h_fit_status_","fit_status");
  TString postfitstr = "shapes_fit_s"; if (h_fit_status->GetMean()<0.) { postfitstr = "shapes_prefit"; }
  TH1F *h_postfit_catp2_ = (TH1F*)fdiag->Get(postfitstr+"/"+category+"/catp2"); h_postfit_catp2_->SetName("h_catp2_");
  TH1F *h_postfit_catp1_ = (TH1F*)fdiag->Get(postfitstr+"/"+category+"/catp1"); h_postfit_catp1_->SetName("h_catp1_");
  TH1F *h_postfit_total_ = (TH1F*)fdiag->Get(postfitstr+"/"+category+"/total"); h_postfit_total_->SetName("h_total_");
  TH1F *h_postfit_catp2 = rescaleXaxis(h_postfit_catp2_,xmin,xmax); h_postfit_catp2->SetName("h_catp2");
  TH1F *h_postfit_catp1 = rescaleXaxis(h_postfit_catp1_,xmin,xmax); h_postfit_catp1->SetName("h_catp1");
  TH1F *h_postfit_total = rescaleXaxis(h_postfit_total_,xmin,xmax); h_postfit_total->SetName("h_total");
  
  // get data
  TGraphAsymmErrors *h_data = (TGraphAsymmErrors*)fdiag->Get("shapes_prefit/"+category+"/data"); h_data->SetName("h_data_");
  std::cout << h_data->GetN() << "\n";
  rescaleXaxis(h_data, xmin, (xmax-xmin)/(float)nbins); 
  
  //cosmetics
  h_prefit_catp2->SetLineColor(633);     h_prefit_catp2->SetLineStyle(2); h_prefit_catp2->SetLineWidth(3); h_prefit_catp2->SetMarkerSize(0);
  h_prefit_catp1->SetLineColor(415);     h_prefit_catp1->SetLineStyle(2); h_prefit_catp1->SetLineWidth(3); h_prefit_catp1->SetMarkerSize(0);
  h_prefit_total->SetLineColor(kBlue+2); h_prefit_total->SetLineStyle(2); h_prefit_total->SetLineWidth(3); h_prefit_total->SetMarkerSize(0);
  
  h_postfit_catp2->SetLineColor(633);     h_postfit_catp2->SetLineStyle(1); h_postfit_catp2->SetLineWidth(4); h_postfit_catp2->SetMarkerSize(0);
  h_postfit_catp1->SetLineColor(415);     h_postfit_catp1->SetLineStyle(1); h_postfit_catp1->SetLineWidth(4); h_postfit_catp1->SetMarkerSize(0);
  h_postfit_total->SetLineColor(kBlue+2); h_postfit_total->SetLineStyle(1); h_postfit_total->SetLineWidth(4); h_postfit_total->SetMarkerSize(0);
  
  h_data->SetLineColor(1); h_data->SetLineWidth(3); 
  h_data->SetMarkerColor(1); h_data->SetMarkerStyle(20); h_data->SetMarkerSize(1.2); 
  
  //TGraphAsymmErrors *h_r_prefit = h_data->Clone("h_r_prefit"); h_r_prefit->Divide(h_data,h_prefit_total);
  TH1F *h_r_prefit = getDataMCratio(h_data,h_prefit_total); h_r_prefit->SetName("h_r_prefit_");
  h_r_prefit->SetMarkerSize(0); h_r_prefit->SetLineColor(kMagenta); h_r_prefit->SetLineStyle(2);
  
  TH1F *h_r_postfit = getDataMCratio(h_data,h_postfit_total); h_r_postfit->SetName("h_r_postfit_");
  h_r_postfit->SetMarkerColor(1); h_r_postfit->SetLineColor(1);
  
  TLegend* leg = new TLegend(0.70,0.55,1.04,0.86);
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineWidth(0);
  leg->AddEntry(h_postfit_catp2,"matched","L");
  leg->AddEntry(h_postfit_catp1,"unmatched","L");
  leg->AddEntry(h_postfit_total,"Total SM","L");
  leg->AddEntry(h_data,"Data","P");
  
  TLegend* legfit = new TLegend(0.4,0.7,0.65,0.86);
  legfit->SetFillStyle(0);
  legfit->SetFillColor(0);
  legfit->SetLineWidth(0);
  legfit->AddEntry(h_prefit_total,"Pre-fit","L");
  legfit->AddEntry(h_postfit_total,"Post-fit","L");
  
  TLegend* legfitr = new TLegend(0.35,0.80,0.55,0.93);
  legfitr->SetFillStyle(0);
  legfitr->SetFillColor(0);
  legfitr->SetLineWidth(0);
  legfitr->AddEntry(h_r_prefit,"Pre-fit","L");
  legfitr->AddEntry(h_r_postfit,"Post-fit","L");

  TPaveText *pt_cms = new TPaveText(0.11,0.77,0.4,0.9,"NDC");
  pt_cms->SetFillStyle(0);
  pt_cms->SetFillColor(0);
  pt_cms->SetLineWidth(0);
  pt_cms->AddText("CMS");
  pt_cms->SetTextSize(0.08);

  TPaveText *pt_preliminary = new TPaveText(0.2,0.63,0.4,0.9,"NDC");
  pt_preliminary->SetFillStyle(0);
  pt_preliminary->SetFillColor(0);
  pt_preliminary->SetLineWidth(0);
  pt_preliminary->AddText("Preliminary");
  pt_preliminary->SetTextFont(52);
  pt_preliminary->SetTextSize(0.06);

  TLatex pt_lumi;
  const char *longstring = "41.1 fb^{-1} (13 TeV)";
  pt_lumi.SetTextSize(0.07);
  pt_lumi.SetTextFont(42);
  
  TString logstr = "lin"; if (log) { logstr = "log"; } 
  TCanvas *c = new TCanvas("c_"+category+"_"+score+"_"+ptrange+"_"+logstr,"c_"+category+"_"+score+"_"+ptrange+"_"+logstr,600,600); 
  c->SetName("c_"+category+"_"+score+"_"+ptrange+"_"+logstr);
  
  TPad *pMain  = new TPad("pMain_"+category+"_"+score+"_"+ptrange+"_"+logstr,"pMain+"+category+"_"+score+"_"+ptrange+"_"+logstr,0.0,0.35,1.0,1.0);
  pMain->SetRightMargin(0.05);
  pMain->SetLeftMargin(0.17);
  pMain->SetBottomMargin(0.02);
  pMain->SetTopMargin(0.1);
  
  TPad *pRatio = new TPad("pRatio_"+category+"_"+score+"_"+ptrange+"_"+logstr,"pRatio_"+category+"_"+score+"_"+ptrange+"_"+logstr,0.0,0.0,1.0,0.35);
  pRatio->SetRightMargin(0.05);
  pRatio->SetLeftMargin(0.17);
  pRatio->SetTopMargin(0.02);
  pRatio->SetBottomMargin(0.4);
  pMain->Draw();
  pRatio->Draw();
  
  pMain->cd();

  //Display SF 
  TString strCat, strCatLoErr, strCatHiErr;   
  strCat = "SF_catp2"; strCatLoErr = "SF_catp2LoErr"; strCatHiErr = "SF_catp2HiErr";
  TH1F *h_sf_cat      = new TH1F("h_sf_cat"      , "h_sf_cat"      , 10000,0.,10.);
  TH1F *h_sf_catLoErr = new TH1F("h_sf_catLoErr" , "h_sf_catLoErr" , 10000,0.,10.);
  TH1F *h_sf_catHiErr = new TH1F("h_sf_catHiErr" , "h_sf_catHiErr" , 10000,0.,10.);
  tree->Draw(strCat+" >> h_sf_cat");
  tree->Draw(strCatLoErr+" >> h_sf_catLoErr");
  tree->Draw(strCatHiErr+" >> h_sf_catHiErr");
  double SF, SFLoErr, SFHiErr; 
  SF    = h_sf_cat->GetMean();
  SFLoErr = h_sf_catLoErr->GetMean();
  SFHiErr = h_sf_catHiErr->GetMean();
  h_sf_cat->Delete();
  h_sf_catLoErr->Delete();
  h_sf_catHiErr->Delete();

  TLatex pt_SF;
  pt_SF.SetTextSize(0.07);
  pt_SF.SetTextFont(42);
  std::ostringstream out;
  out << fixed << setprecision(3) << "SF = " << SF << "  +" << SFHiErr << " -" << SFLoErr;

  
  float maxyld = h_postfit_total->GetMaximum(); 
  if (h_prefit_total->GetMaximum()>h_postfit_total->GetMaximum()) { maxyld = h_prefit_total->GetMaximum(); }
  if (log) { gPad->SetLogy(); h_prefit_total->GetYaxis()->SetRangeUser(0.1,10.*maxyld); } else { h_prefit_total->GetYaxis()->SetRangeUser(0.,1.8*maxyld); }
  h_prefit_total->GetXaxis()->SetLabelSize(0.);
  h_prefit_total->GetYaxis()->SetTitle("Events / bin");
  h_prefit_total->GetXaxis()->SetTitle(xaxisname);
  h_prefit_total->Draw("HIST P");
  //h_prefit_catp2->Draw("HIST E0 sames");
  //h_prefit_catp1->Draw("HIST E0 sames");
  h_postfit_catp2->Draw("HIST E0 sames");
  h_postfit_catp1->Draw("HIST E0 sames");
  h_postfit_total->Draw("HIST E0 sames");
  h_data->Draw("P sames");
  leg->Draw("sames");
  pt_cms->Draw("sames");
  pt_preliminary->Draw("sames");
  pt_lumi.DrawLatexNDC(0.64,0.93,longstring);
  pt_SF.DrawLatexNDC(0.2,0.65, out.str().c_str());
  c->RedrawAxis();
  
  pRatio->cd();
  gPad->SetBottomMargin(0.2);
  h_r_postfit->GetYaxis()->SetTitleOffset(0.75);
  h_r_postfit->GetYaxis()->SetTitleSize(0.1);
  h_r_postfit->GetYaxis()->SetLabelSize(0.08);
  h_r_postfit->GetXaxis()->SetTitleSize(0.1);
  h_r_postfit->GetXaxis()->SetLabelSize(0.08);
  h_r_postfit->GetXaxis()->SetTitle(xaxisname);
  h_r_postfit->GetYaxis()->SetTitle("Data / Post-fit");
  h_r_postfit->GetYaxis()->SetRangeUser(0.01,1.99);
  h_r_postfit->Draw("P E0");
  //  h_r_prefit->Draw("HIST sames");
  c->RedrawAxis();
  
  if (log) {
    c->Print(path2file+"/plots_datamc/"+category+"_"+score+"_"+ptrange+"_log.pdf");
    c->Print(path2file+"/plots_datamc/"+category+"_"+score+"_"+ptrange+"_log.png");
  } 
  else {
    c->Print(path2file+"/plots_datamc/"+category+"_"+score+"_"+ptrange+"_lin.pdf");
    c->Print(path2file+"/plots_datamc/"+category+"_"+score+"_"+ptrange+"_lin.png");
  }
  //c->Delete();

} // end of makeDataMCPlotsFromCombine


void getSFSummary(TString object, TString wp) {

  setTDRStyle();
  gROOT->SetBatch(false);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  gStyle->SetPalette(1);
  TH1::SetDefaultSumw2(kTRUE);
  
  std::vector<TString> algos;    algos.clear();
  std::vector<TString> legnames; legnames.clear();
  std::vector<TString> ptrange;  ptrange.clear();
  std::vector<int> colors; colors.clear();
  
  if (object=="T") {
    algos.push_back("sdtau32");     legnames.push_back("m_{SD}+#tau_{32}");       colors.push_back(800);
    algos.push_back("sdtau32btag"); legnames.push_back("m_{SD}+#tau_{32}+b-tag"); colors.push_back(632);
    //algos.push_back("hotvr");       legnames.push_back("HOTVR");                  colors.push_back(812);
    algos.push_back("ecftoptag");   legnames.push_back("CA15-ECF");               colors.push_back(419);
    algos.push_back("best");        legnames.push_back("BEST");                   colors.push_back(882);
    algos.push_back("imagetop");    legnames.push_back("ImageTop");               colors.push_back(603);
    algos.push_back("imagetopmd");  legnames.push_back("ImageTop-MD");            colors.push_back(600);
    algos.push_back("deepak8");     legnames.push_back("DeepAK8");                colors.push_back(866);
    algos.push_back("deepak8md");   legnames.push_back("DeepAK8-MD");             colors.push_back(616);
    
    ptrange.push_back("low");
    ptrange.push_back("lowmed");
    ptrange.push_back("med");
    ptrange.push_back("medhi");
  }

  if (object=="W") {
    algos.push_back("sdtau21");     legnames.push_back("m_{SD}+#tau_{21}");   colors.push_back(800);
      /*algos.push_back("sdn2");        legnames.push_back("m_{SD}+N_{2}");       colors.push_back(408);
	algos.push_back("sdn2ddt");     legnames.push_back("m_{SD}+N_{2}^{DDT}"); colors.push_back(419);*/
    algos.push_back("best");        legnames.push_back("BEST");               colors.push_back(882);
    algos.push_back("deepak8");     legnames.push_back("DeepAK8");            colors.push_back(866);
    algos.push_back("deepak8md");   legnames.push_back("DeepAK8-MD");         colors.push_back(616);
    
    ptrange.push_back("low");
    ptrange.push_back("lowmed");
    ptrange.push_back("med");
  }


  std::string names[100];
  TH1F *h_dummy; int nbins; int xmax;
  if (object=="T") { names[0] = "300-400"; names[1] = "400-480"; names[2] = "480-600"; names[3] = "600-1200"; nbins = 4, xmax = 4.; }
  if (object=="W") { names[0] = "200-300"; names[1] = "300-400"; names[2] = "400-800";  nbins = 3, xmax = 3.; }
  h_dummy = new TH1F("h_dummy","h_dummy",nbins,0.,xmax);
  for (unsigned int i1=0; i1<h_dummy->GetNbinsX(); ++i1) { h_dummy->GetXaxis()->SetBinLabel(i1+1,names[i1].c_str()); }

    std::vector<TGraphAsymmErrors*> gr_sf;
  for (unsigned int ialgo=0; ialgo<algos.size(); ++ialgo) {

    double xval[20], xvalerr[20];
    double yval[20], yvalerrlo[20], yvalerrhi[20];
    
    for (unsigned int iptrange=0; iptrange<ptrange.size(); ++iptrange) {
      TFile *fdiag = TFile::Open("./"+object+"_"+algos[ialgo]+"/sf_fitDiagnostics_"+object+"_"+algos[ialgo]+"_"+wp+"_"+ptrange[iptrange]+".root", "READONLY" );
      TTree *t_   = (TTree*)fdiag->Get("tree_fit_sb");

      TString strCat, strCatLoErr, strCatHiErr;   
      strCat = "SF_catp2"; strCatLoErr = "SF_catp2LoErr"; strCatHiErr = "SF_catp2HiErr";
      TH1F *h_sf_cat      = new TH1F("h_sf_cat"      , "h_sf_cat"      , 10000,0.,10.);
      TH1F *h_sf_catLoErr = new TH1F("h_sf_catLoErr" , "h_sf_catLoErr" , 10000,0.,10.);
      TH1F *h_sf_catHiErr = new TH1F("h_sf_catHiErr" , "h_sf_catHiErr" , 10000,0.,10.);
      t_->Draw(strCat+" >> h_sf_cat");
      t_->Draw(strCatLoErr+" >> h_sf_catLoErr");
      t_->Draw(strCatHiErr+" >> h_sf_catHiErr");

      xval[iptrange]    = (iptrange+0.5) + 0.03*(ialgo - 1.*(algos.size()-2));
      xvalerr[iptrange] = 0.;
      yval[iptrange]    = h_sf_cat->GetMean();
      yvalerrlo[iptrange] = h_sf_catLoErr->GetMean();
      yvalerrhi[iptrange] = h_sf_catHiErr->GetMean();

      h_sf_cat->Delete();
      h_sf_catLoErr->Delete();
      h_sf_catHiErr->Delete();

    } // end looping over the pt bins

    TGraphAsymmErrors *gr_sf_tmp = new TGraphAsymmErrors(ptrange.size(),xval,yval,xvalerr,xvalerr,yvalerrlo,yvalerrhi);
    gr_sf_tmp->SetLineColor(colors[ialgo]);
    gr_sf_tmp->SetMarkerColor(colors[ialgo]);
    gr_sf.push_back(gr_sf_tmp);

  } // end looping over files

  TLegend* leg;
  if (object=="T") { leg = new TLegend(0.75,0.46,0.92,0.89); }
  if (object=="W") { leg = new TLegend(0.75,0.66,0.92,0.89); }
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineWidth(0);

  TPaveText *pt_cms = new TPaveText(0.01,0.77,0.4,0.9,"NDC");
  pt_cms->SetFillStyle(0);
  pt_cms->SetFillColor(0);
  pt_cms->SetLineWidth(0);
  pt_cms->AddText("CMS");
  pt_cms->SetTextSize(0.08);

  TPaveText *pt_preliminary = new TPaveText(0.06,0.63,0.4,0.9,"NDC");
  pt_preliminary->SetFillStyle(0);
  pt_preliminary->SetFillColor(0);
  pt_preliminary->SetLineWidth(0);
  pt_preliminary->AddText("Preliminary");
  pt_preliminary->SetTextFont(52);
  pt_preliminary->SetTextSize(0.06);

  TLatex pt_lumi;
  const char *longstring = "41.1 fb^{-1} (13 TeV)";
  pt_lumi.SetTextSize(0.07);
  pt_lumi.SetTextFont(42);

  TCanvas *c_sf = new TCanvas("c_sf","c_sf",700,500);
  gPad->SetLeftMargin(0.1);
  gPad->SetBottomMargin(0.13);
  gPad->SetTopMargin(1.02);
  //  gPad->SetGridy();
  h_dummy->GetYaxis()->SetRangeUser(0.5,2.);
  h_dummy->GetYaxis()->SetTitleOffset(0.8);
  h_dummy->GetYaxis()->SetTitle("#epsilon_{DATA} / #epsilon_{MC}");
  h_dummy->GetXaxis()->SetLabelSize(0.06);
  h_dummy->GetXaxis()->SetTitle("p_{T} (jet) [GeV]");
  h_dummy->Draw("HIST");
  for (unsigned int i0=0; i0<gr_sf.size(); ++i0) {
    gr_sf[i0]->Draw("P sames");
    leg->AddEntry(gr_sf[i0],legnames[i0],"PL");
    for (unsigned int igr = 0; igr<gr_sf[i0]->GetN(); ++igr) {
      double x_, y_;
      gr_sf[i0]->GetPoint(igr,x_,y_);
      std::cout << std::fixed;
      std::cout << std::setprecision(2);
      if (igr < (gr_sf[i0]->GetN()-1)) { std::cout << " & " << y_ << "$^{+" << gr_sf[i0]->GetErrorYhigh(igr) << "}_{-" << gr_sf[i0]->GetErrorYlow(igr) << "}$"; }
      else                             { std::cout << " & " << y_ << "$^{+" << gr_sf[i0]->GetErrorYhigh(igr) << "}_{-" << gr_sf[i0]->GetErrorYlow(igr) << "}$ \\\\ \n"; }
    }
  }
  leg->Draw("sames");
  pt_cms->Draw("sames");
  pt_preliminary->Draw("sames");
  pt_lumi.DrawLatexNDC(0.58,0.93,longstring);



}


void getdiscval(float sigeff, TString algo, TString object, TString cut="(0==0)") {

  TFile *f_sig = TFile::Open("/eos/uscms/store/user/lpcjme/LGOutputs/hrt_muon_20190225/ttbar-powheg_tree.root" , "READONLY" );
  TTree *t_sig = (TTree*)f_sig->Get("Events");


  int nbins = 4000;
  TString cut_final; int start = 1; int end = nbins+1; 
  TH1F *h_sig = new TH1F("h_sig_"+algo,"h_sig_"+algo,nbins,0.,1.);
  if (algo=="sdtau32") { 
    if (object=="T") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>300. && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8)) && (ak8_1_mass>105 && ak8_1_mass<210.))"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_tau3/ak8_1_tau2",cut_final); 
  }

  if (algo=="sdtau32btag") { 
    if (object=="T") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>300. && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8)) && (ak8_1_mass>105 && ak8_1_mass<210.) && max(ak8_1_sj1_btagCSVV2,ak8_1_sj2_btagCSVV2)>0.5426 )"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_tau3/ak8_1_tau2",cut_final); 
  }

  if (algo=="sdtau21") { 
    if (object=="W") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>200. && (!((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8))) && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b>0.8)) && (ak8_1_mass>65 && ak8_1_mass<105.) )"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_tau2/ak8_1_tau1",cut_final); 
  }

  if (algo=="sdn2") { 
    if (object=="W") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>200. && (!((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8))) && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b>0.8)) && (ak8_1_mass>65 && ak8_1_mass<105.) )"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_n2b1",cut_final); 
  }

  if (algo=="sdn2ddt") { 
    if (object=="W") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>200. && (!((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8))) && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b>0.8)) && (ak8_1_mass>65 && ak8_1_mass<105.) )"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_n2b1ddt",cut_final); 
  }

  if (algo=="ecftoptag") {  // transformation: 0.5+0.5*ca15_ecfTopTagBDT
    if (object=="T") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ca15_1_pt>300. && ((ca15_1_dr_fj_top_wqmax<0.8) && (ca15_1_dr_fj_top_b<0.8)) && (ca15_1_mass>105 && ca15_1_mass<210.) && ca15_1_ecfTopTagBDT>-1. )"; }
    t_sig->Project("h_sig_"+algo,"(0.5+0.5*ca15_1_ecfTopTagBDT)",cut_final); 
  }

  if (algo=="hotvr") {  
    if (object=="T") { cut_final = "(xsecWeight*puWeight)*("+cut+" && hotvr_1_pt>300. && ((hotvr_1_dr_fj_top_wqmax<0.8) && (hotvr_1_dr_fj_top_b<0.8)) && (hotvr_1_mass>140 && hotvr_1_mass<220.) && hotvr_1_fpt<0.8 && hotvr_1_nsubjets>=3 && hotvr_1_mmin>=50. )"; }
    t_sig->Project("h_sig_"+algo,"(1-hotvr_1_tau32)",cut_final); 
  }

  if (algo=="best") { 
    if (object=="T") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>300. && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8)))"; }
    if (object=="W") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>200. && (!((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8))) && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b>0.8)))"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_best_"+object+"vsQCD",cut_final); 
  }

  if (algo=="imagetop") { 
    if (object=="T") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>300. && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8)))"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_image_top",cut_final); 
  }

  if (algo=="imagetopmd") { 
    if (object=="T") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>300. && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8)))"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_image_top_md",cut_final); 
  }

  if (algo=="deepak8") { 
    if (object=="T") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>300. && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8)))"; }
    if (object=="W") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>200. && ((!((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8))) && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b>0.8))))"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_DeepAK8_"+object+"vsQCD",cut_final); 
  }

  if (algo=="deepak8md") { 
    if (object=="T") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>300. && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8)))"; }
    if (object=="W") { cut_final = "(xsecWeight*puWeight)*("+cut+" && ak8_1_pt>200. && (!((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b<0.8))) && ((ak8_1_dr_fj_top_wqmax<0.8) && (ak8_1_dr_fj_top_b>0.8)))"; }
    t_sig->Project("h_sig_"+algo,"ak8_1_DeepAK8MD_"+object+"vsQCD",cut_final); 
  }


  float ntot_sig = h_sig->Integral(0,nbins+1); 
  for (unsigned int i0=start; i0<end; ++i0) {
    float tmp_sig = h_sig->Integral(i0,nbins+1); 
    if (algo=="sdtau32" || algo=="sdtau32btag" || algo=="sdtau21" || algo=="sdn2" || algo=="sdn2ddt") { tmp_sig = h_sig->Integral(start,(nbins-i0)); }
    if ( (tmp_sig/ntot_sig)<sigeff && (tmp_sig/ntot_sig)>0. ) { std::cout << tmp_sig/ntot_sig << " " << h_sig->GetBinCenter(i0) << "\n"; break;  }
  } 

}


TH1F *rescaleXaxis(TH1F *inputhisto, float xmin, float xmax) {
  TH1::SetDefaultSumw2(kTRUE);

  int nbins = inputhisto->GetNbinsX();
  TString name = inputhisto->GetName();
  TH1F *outputhisto = new TH1F(name,name,nbins,xmin,xmax);

  for (unsigned int i0=0; i0<nbins; ++i0) {
    outputhisto->SetBinContent(i0+1,inputhisto->GetBinContent(i0+1));
    outputhisto->SetBinError(i0+1,inputhisto->GetBinError(i0+1));
  }

  return outputhisto;
}// end of rescaleXaxis



void rescaleXaxis(TGraphAsymmErrors *g, double xmin, double scaleX) {
  TH1::SetDefaultSumw2(kTRUE);

  int N=g->GetN();
  double *x=g->GetX();
  double *xerrh=g->GetEXhigh();
  double *xerrl=g->GetEXlow();
  int i=0; while(i<N) {
    x[i]=xmin+x[i]*scaleX;
    xerrh[i]=0.;
    xerrl[i]=0.;
    i=i+1;
  }
  g->GetHistogram()->Delete();
  g->SetHistogram(0);
}


TGraphAsymmErrors *getTGraphAsymmErrorsFromHisto(TH1 *input) {
  TH1::SetDefaultSumw2(kTRUE);

  int nbins = input->GetNbinsX();
  double x[nbins], y[nbins], exl[nbins], exh[nbins], eyl[nbins], eyh[nbins];
  for (unsigned int i0=0; i0<nbins; ++i0) {
    x[i0] = input->GetBinCenter(i0+1); 
    exl[i0] = 0.;
    exh[i0] = 0.;
    y[i0] = input->GetBinContent(i0+1);
    eyl[i0] = input->GetBinError(i0+1);
    eyh[i0] = input->GetBinError(i0+1);
  }
  TGraphAsymmErrors *gr = new TGraphAsymmErrors(nbins,x,y,exl,exh,eyl,eyh);

  return gr;
}


TH1F *getDataMCratio(TGraphAsymmErrors *indata, TH1F *inMC) {
  TH1::SetDefaultSumw2(kTRUE);

  TH1F *h_data = (TH1F*)inMC->Clone("h_data"); h_data->SetName("h_data");
  for (unsigned int i0=0; i0<inMC->GetNbinsX(); ++i0) {
    h_data->SetBinContent(i0+1, indata->GetY()[i0]);
    h_data->SetBinError(i0+1, (indata->GetEYlow()[i0]+indata->GetEYhigh()[i0])/2.);
  }

  TH1F *h_ratio = (TH1F*)h_data->Clone("h_ratio");
  h_ratio->Divide(h_data,inMC);
  h_ratio->SetMarkerStyle(20); h_ratio->SetMarkerSize(1);
  h_ratio->SetLineWidth(2); h_ratio->SetLineStyle(1);

  return h_ratio;
}
