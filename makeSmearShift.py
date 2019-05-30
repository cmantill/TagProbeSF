#!/usr/bin/env python
import ROOT as r,sys,math,array,os
from hist import hist
from optparse import OptionParser

def shift(iVar,iDataHist,iShift=5.):
    lInt    = iDataHist.createHistogram("x").Integral()
    lDM     = r.RooRealVar   ("Xdm","Xdm", 0.,-10,10)
    lShift  = r.RooFormulaVar("Xshift",iVar.GetName()+"-Xdm",r.RooArgList(iVar,lDM))  
    if f2D:
        lSPdf   = r.RooHistPdf(iDataHist.GetName()+"P",iDataHist.GetName()+"P", r.RooArgList(lShift,fVars[1]),r.RooArgList(iVar,fVars[1]),iDataHist,0)
    else:
        lSPdf   = r.RooHistPdf(iDataHist.GetName()+"P",iDataHist.GetName()+"P", r.RooArgList(lShift),r.RooArgList(iVar),iDataHist,0)
    lDM.setVal(iShift)
    lHUp   = lSPdf.createHistogram("x")
    lHUp.Scale(lInt)
    lUp    = r.RooDataHist(iDataHist.GetName()+"_scaleUp",iDataHist.GetName()+"_scaleUp", r.RooArgList(iVar),lHUp)
    lDM.setVal(-iShift)
    lHDown = lSPdf.createHistogram("x")
    lHDown.Scale(lInt)
    lDown  = r.RooDataHist(iDataHist.GetName()+"_scaleDown",iDataHist.GetName()+"_scaleDown", r.RooArgList(iVar),lHDown)
    return (lUp,lDown)

def smear(iVar,iDataHist,iScale=0.1):
    lDM     = r.RooRealVar("Xshift","Xshift", 1.,0.,2.)
    lVar    = iDataHist.createHistogram("x").GetMean()
    lInt    = iDataHist.createHistogram("x").Integral()
    lShift  = r.RooFormulaVar("Xsmear","("+iVar.GetName()+"-"+str(lVar)+")/Xshift+"+str(lVar),r.RooArgList(iVar,lDM))  
    if f2D:
        lHPdf   = r.RooHistPdf(iDataHist.GetName()+"S",iDataHist.GetName()+"S", r.RooArgList(lShift,fVars[1]),r.RooArgList(iVar,fVars[1]),iDataHist,0)
    else:
        lHPdf   = r.RooHistPdf(iDataHist.GetName()+"S",iDataHist.GetName()+"S", r.RooArgList(lShift),r.RooArgList(iVar),iDataHist,0)
    lDM.setVal(1.+iScale)
    lHUp = lHPdf.createHistogram("x")
    lHUp.Scale(lInt)
    lUp = r.RooDataHist(iDataHist.GetName()+"_smearUp",iDataHist.GetName()+"_smearUp", r.RooArgList(iVar),lHUp)    
    lDM.setVal(1.-iScale)
    lHDown = lHPdf.createHistogram("x")
    lHDown.Scale(lInt)
    lDown  = r.RooDataHist(iDataHist.GetName()+"_smearDown",iDataHist.GetName()+"_smearDown", r.RooArgList(iVar),lHDown)
    return [lUp,lDown]  

def create(options):
    # load histograms
    lFile  = r.TFile.Open(options.ifile);
    signal = "catp2"
    lHSig = lFile.Get(signal).Clone() # this is the matched signal
    lHSig.SetDirectory(0)
    lHOthers = []
    for key in lFile.GetListOfKeys():
        lh = key.ReadObj();
        if lh.GetName() == signal:
            print lh.GetName()
            continue
        lh.SetDirectory(0)
        lHOthers.append(lh)
    for h in lHOthers:
        h.SetDirectory(0)
    lFile.Close()

    # hist container
    mass = 80.4
    hist_container = hist( [mass],[lHSig] );

    # shift (to measure mass scale)
    mass_shift = 0.995
    shift_val = mass - mass*mass_shift;
    tmp_shifted_h = hist_container.shift( lHSig, shift_val);

    # smear (to measure mass resolution)
    #res_shift = 1.01
    res_shift = 1.1
    smear_val = res_shift - 1.;
    tmp_smeared_h =  hist_container.smear( tmp_shifted_h[0] , smear_val)

    # update central
    hmatched_new_central = tmp_smeared_h[0];
    hmatched_new_central.SetName("catp2"); hmatched_new_central.SetTitle("catp2");
    
    # get shift up and down variations
    #mass_sigma = 1000 # 1/mass_sigma goes in the datacard
    #mass_shift_unc = 0.01 * mass_sigma;
    hmatchedsys_shift = hist_container.shift(hmatched_new_central, 10)
    hmatchedsys_shift[0].SetName("catp2_scaleUp"); hmatchedsys_shift[0].SetTitle("catp2_scaleUp")
    hmatchedsys_shift[1].SetName("catp2_scaleDown");  hmatchedsys_shift[1].SetTitle("catp2_scaleDown");

    # get res up/down
    #res_sigma = 10 # 1/res_sigma goes in the datacard
    #res_shift_unc = 0.05 * res_sigma; 
    hmatchedsys_smear = hist_container.smear(hmatched_new_central, 0.25)
    hmatchedsys_smear[0].SetName("catp2_smearUp"); hmatchedsys_smear[0].SetTitle("catp2_smearUp");
    hmatchedsys_smear[1].SetName("catp2_smearDown"); hmatchedsys_smear[1].SetTitle("catp2_smearDown");

    # clone and save up and down variations in .root file
    lOutFile  = r.TFile.Open(options.ifile.replace('_pre.root','.root'),"RECREATE");
    hmatched_new_central.Write(); 
    hmatchedsys_shift[0].Write();
    hmatchedsys_shift[1].Write();
    hmatchedsys_smear[0].Write();
    hmatchedsys_smear[1].Write();
    #lHSig.Write();
    for val in lHOthers:
        val.Write();
    lOutFile.ls()
    lOutFile.Close()

def parser():
    parser = OptionParser()
    parser.add_option('--ifile',action='store',dest='ifile',help='input file')
    (options,args) = parser.parse_args()
    return options

if __name__ == "__main__":
    options = parser()
    create(options)



