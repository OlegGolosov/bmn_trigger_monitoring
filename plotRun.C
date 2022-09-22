void plotRun(const char* in="raw_1000.root", const char* out="plots.root") {
  RDataFrame d("bmndata", in);
  cout << "Number of Events: " << *(d.Count()) << endl;
  vector <RResultPtr<::TH1D >> hists1d;
  vector <RResultPtr<::TH2D >> hists2d;
  TFile fOut(out,"recreate");
  auto dd=d
//    .Filter("recVtxChi2>0.")
//    .Define("M", vsize, {"recChi2"})
//    .Define("qp", "recCharge*p")
  ;
//    dd.Display("")->Print();
  dd.Foreach([](uint evtId){if (evtId % 100 == 0) cout << "\r" << evtId;}, {"evtId"}); // progress display
  hists1d.push_back(dd.Histo1D({"hBC1","primary proton multiplicity;M_{tracks}",200,0,200}, "Mproton")); 
//  hists2d.push_back(dd.Histo2D({"hMB","multiplicity vs impact parameter;b (fm);M_{tracks}",160,0,16,200,0,200}, "b", "M"));

  cout << endl;
  for (auto& hist:hists1d)
    hist->Write();
  for (auto& hist:hists2d)
    hist->Write();
  fOut.Close();
//  hists1d.back()->DrawClone();
//  hists2d.back()->DrawClone("colz");
}
