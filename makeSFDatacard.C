void makeSFDatacard(std::string inputname) {

  TFile *fpass = TFile::Open((TString)inputname+"_pass.root","READONLY");
  TFile *ffail = TFile::Open((TString)inputname+"_fail.root","READONLY");
  TH1D  *h_obs_pass = (TH1D*)fpass->Get("data_obs");
  TH1D  *h_obs_fail = (TH1D*)ffail->Get("data_obs");
  TH1D  *h_catp2_pass = (TH1D*)fpass->Get("catp2");
  TH1D  *h_catp2_fail = (TH1D*)ffail->Get("catp2");
  TH1D  *h_catp1_pass = (TH1D*)fpass->Get("catp1");
  TH1D  *h_catp1_fail = (TH1D*)ffail->Get("catp1");

  std::cout << "imax 2  number of channels\n";
  std::cout << "jmax 1  number of backgrounds\n";
  std::cout << "kmax *  number of nuisance parameters (sources of systematical uncertainties)\n";
  std::cout << "------------\n";
  std::cout << "shapes  *  pass   " << inputname << "_pass.root  $PROCESS $PROCESS_$SYSTEMATIC\n";
  std::cout << "shapes  *  fail   " << inputname << "_fail.root  $PROCESS $PROCESS_$SYSTEMATIC\n";
  std::cout << "------------\n";
  std::cout << "# we have just one channel, in which we observe 0 events\n";
  std::cout << "bin             pass    fail\n";
  std::cout << "observation      " << h_obs_pass->Integral(1,h_obs_pass->GetNbinsX()) << "  " <<  h_obs_fail->Integral(1,h_obs_fail->GetNbinsX())  << "\n";
  std::cout << "------------\n";
  std::cout << "# now we list the expected events for signal and all backgrounds in that bin\n";
  std::cout << "# the second 'process' line must have a positive number for backgrounds, and 0 for signal\n";
  std::cout << "# then we list the independent sources of uncertainties, and give their effect (syst. error)\n";
  std::cout << "# on each process and bin\n";
  std::cout << "bin             pass   pass       fail   fail\n";
  std::cout << "process         catp2   catp1      catp2   catp1\n";  
  std::cout << "process            -1     2         -1     2\n";
  std::cout << "rate            " 
	    << h_catp2_pass->Integral(1,h_catp2_pass->GetNbinsX()) << " " << h_catp1_pass->Integral(1,h_catp1_pass->GetNbinsX()) << " "
	    << h_catp2_fail->Integral(1,h_catp2_fail->GetNbinsX()) << " " << h_catp1_fail->Integral(1,h_catp1_fail->GetNbinsX()) << "\n";
  std::cout << "------------\n";
  std::cout << "lumi    lnN    1.023  1.023  1.023  1.023\n";
  std::cout << "jecs    lnN    1.02   1.02   1.02   1.02 \n";
  std::cout << "pu      lnN    1.05   1.05   1.05   1.05 \n";
  std::cout << "scale  shapeU   1      -     1      -  \n";
  std::cout << "smear  shapeU   10     -     10      -  \n";
  std::cout << "*  autoMCStats  0\n";
  
}
