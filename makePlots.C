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
#include "RooFitResult.h"
#include "RooArgSet.h"
#include "RooArgList.h"
#include "RooRealVar.h"

using namespace RooFit ;


TH1F *rescaleXaxis(TH1F *inputhisto, float xmin, float xmax);
void rescaleXaxis(TGraphAsymmErrors *inputhisto, double xmin, double scale);
TH1F *getDataMCratio(TGraphAsymmErrors *indata, TH1F *inMC);
void setTDRStyle();
void makeDataMCPlotsFromCombine(TString path2file, TString filename, TString score, TString ptrange,TString whichbit, TString category,
 float xmin, float xmax, int nbins,TString xaxisname, bool log);
void getSF(RooFitResult* r,  const char *strPar, double &par, double &parerr);

void makePlots(TString path2file, TString filename, TString score, TString ptrange,TString whichbit, float xmin, float xmax, int nbins, TString xaxis) {
  TH1::SetDefaultSumw2(kTRUE);

  setTDRStyle();
  gROOT->SetBatch(false);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  gStyle->SetPalette(1);
  gStyle->SetOptTitle(0);
  TH1::SetDefaultSumw2(kTRUE);
  
  // make pre/post fit plots
  makeDataMCPlotsFromCombine(path2file,filename,score,ptrange,whichbit,"pass",xmin,xmax,nbins,xaxis,false);
  makeDataMCPlotsFromCombine(path2file,filename,score,ptrange,whichbit,"fail",xmin,xmax,nbins,xaxis,false);

}


void makeDataMCPlotsFromCombine(TString path2file, TString filename, TString score, TString ptrange, TString whichbit, TString category,
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
  //const char *longstring = "41.1 fb^{-1} (13 TeV)";
  std::ostringstream lumistring;
  if(whichbit == "05"){lumistring << "59.0 fb^{-1} (13 TeV)";}
  else if(whichbit == "04"){lumistring << "41.1 fb^{-1} (13 TeV)";}


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

  RooFitResult* r = (RooFitResult*)fdiag->Get("fit_s");
  //Display SF 
  const char *strCat = "SF_catp2";
  double SF, SFErr;  getSF(r, strCat, SF, SFErr);

  TLatex pt_SF;
  pt_SF.SetTextSize(0.07);
  pt_SF.SetTextFont(42);
  std::ostringstream out;
  out << fixed << setprecision(3) << "SF = " << SF << "  +" << SFErr << " -" << SFErr;

  // Display Scale
  const char *strShift = "scale";
  double Shift, ShiftErr; getSF(r, strShift, Shift, ShiftErr);

  TLatex pt_Shift;
  pt_Shift.SetTextSize(0.07);
  pt_Shift.SetTextFont(42);
  std::ostringstream out_shift;
  out_shift << fixed << setprecision(3) << "Shift = " << (80.379+Shift*10)/80.379 << "  +" << ShiftErr*10/80.379 << " -" << ShiftErr*10/80.379;

  // Display Smear
  const char *strSmear = "smear";
  double Smear, SmearErr; getSF(r, strSmear, Smear, SmearErr);

  TLatex pt_Smear;
  pt_Smear.SetTextSize(0.07);
  pt_Smear.SetTextFont(42);
  std::ostringstream out_smear;
  out_smear << fixed << setprecision(5) << "Smear(0.5) = " << Smear << "  +" << SmearErr << " -" << SmearErr;

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
  pt_lumi.DrawLatexNDC(0.64,0.93,lumistring.str().c_str());
  if(category=="pass"){
    pt_SF.DrawLatexNDC(0.2,0.65, out.str().c_str());
    pt_Shift.DrawLatexNDC(0.2,0.55, out_shift.str().c_str());
    pt_Smear.DrawLatexNDC(0.2,0.45, out_smear.str().c_str());
  }
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
    c->Print(path2file+"/plots_datamc/"+category+"_"+score+"_"+whichbit+"_"+ptrange+"_log.pdf");
    c->Print(path2file+"/plots_datamc/"+category+"_"+score+"_"+whichbit+"_"+ptrange+"_log.png");
  } 
  else {
    c->Print(path2file+"/plots_datamc/"+category+"_"+score+"_"+whichbit+"_"+ptrange+"_lin.pdf");
    c->Print(path2file+"/plots_datamc/"+category+"_"+score+"_"+whichbit+"_"+ptrange+"_lin.png");
  }
  //c->Delete();

} // end of makeDataMCPlotsFromCombine

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

void getSF(RooFitResult* r, const char *strPar, double &par, double &parerr) {
  RooRealVar* par1_fitresult = (RooRealVar*) r->floatParsFinal().find(strPar);
  par     = par1_fitresult->getVal();
  parerr  = par1_fitresult->getError();
}
