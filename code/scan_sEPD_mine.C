#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
#include "draw_scan.C"
#include "yjUtility.h"
void make_sEPD_rootfile_v3(TString inputFileName, TString cap, int ttt = 32);

void make_sEPD_rootfile_v1(TString inputFileName, TString cap, int ttt = 32);
// void isSourceOnTopOfTheTile(const int tile, const double xorigin, const double yorigin, const double rot); //to_be_updated
void GetParameters(vector<string> &paramNames, vector<string> &params,TString inputFileName = "20220201-1735_middle_fine.txt", TString cap="");
void make_sEPD_rootfile_v2(TString inputFileName = "20220113-1447_TEST_OUTPUT.txt", TString cap="");
double GetTimeHours(double time){
  double r = (time/100. - floor(time/100.))/0.6;
  double new_time = r + floor(time/100.);
  return new_time;
}
void time_dependence(){
  string fname = "20220214-1851_dark_signal_time_01.txt";
  gStyle->SetOptStat(0);
  SetyjPadStyle();
  int version = 1;
  int tiles = 1;
  TString cap = "_time3";
  make_sEPD_rootfile_v2(fname, cap);

  TFile* fin = new TFile(Form("../data/sEPD%s.root",cap.Data()), "read");
  TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
  Float_t xpos, ypos;
  vector<Int_t>* ch=nullptr;
  vector<Int_t>* tile=nullptr;
  vector<Int_t>* device=nullptr;
  vector<Float_t>* imon=nullptr;
  vector<Float_t>* time=nullptr;
  inTree_p->SetBranchAddress("xpos", &xpos);
  inTree_p->SetBranchAddress("ypos", &ypos);
  inTree_p->SetBranchAddress("device", &device);
  inTree_p->SetBranchAddress("channel", &ch);
  inTree_p->SetBranchAddress("tile", &tile);
  inTree_p->SetBranchAddress("imon", &imon);
  inTree_p->SetBranchAddress("time",&time);

  double nRep = 3.;
  int nentries = inTree_p->GetEntries();

  double last_xpos = 0.0;
  double last_ypos = 0.0;
  double last_time = 0.0;
  double time_zero;
  double avg[32] = {0};
  int count[32] = {0};
  int event_counter = 2;
  for (int i = 0 ; i < nentries;i++){
    inTree_p->GetEntry(i);

    if(last_xpos != xpos || last_ypos != ypos){
      event_counter++;
      last_xpos = xpos;
      last_ypos = ypos;
    }
    for (int it = 0; it < tile->size(); it++){
      last_time = time->at(it);
    }
  }
  double weird_time;
  const int ec = floor(static_cast<double>(event_counter)/32.);

  TProfile *tp_dark[32];
  TProfile *tp_signal[32];
  TProfile *tp_time_dark[32];
  TProfile *tp_time_signal[32];
  TH2D *g_all_dark[32];
  TH2D *g_all_signal[32];

  for (int i = 0; i < 32; i++){
    tp_dark[i] = new TProfile(Form("tp_dark_%d", i), "", ec, -0.5, ec -.5, "s");
    tp_signal[i] = new TProfile(Form("tp_signal_%d", i), "", ec, -0.5, ec-.5, "s");
    tp_time_dark[i] = new TProfile(Form("tp_time_dark_%d", i), "", ec, -0.5, ec-.5, "s");
    tp_time_signal[i] = new TProfile(Form("tp_time_signal_%d", i), "", ec, -0.5, ec-.5, "s");
    g_all_dark[i] = new TH2D(Form("g_all_dark_%d", i), "", 72, 0, 18, 70, 0.3, 1);
    g_all_signal[i] = new TH2D(Form("g_all_signal_%d", i), "", 72, 0, 18, 70, 0.3, 1);
  }

  for (int i = 0 ; i < nentries;i++){
    inTree_p->GetEntry(i);
    for(int it = 0; it<tile->size(); it++){
      double imonTemp = imon->at(it);
      time_zero = GetTimeHours(1851);
      if (i ==0 && it == 0){
        time_zero = GetTimeHours(time->at(it));
      }
      if(xpos==0 && ypos==0){//for dark current

        double temp_time = GetTimeHours(time->at(it)) - time_zero;
        //cout<<"Time: "<<time->at(it)<<" as "<<temp_time<<endl;
        if (time->at(it) < 1800.) {
          temp_time = GetTimeHours(time->at(it) + 2400.) - time_zero;
          //cout<<"here: "<<time->at(it)<<endl;
        }
        g_all_dark[tile->at(it)]->Fill( temp_time, imonTemp, 1);
        tp_dark[tile->at(it)]->Fill(floor(static_cast<double>(i)/(32.*3.)), imonTemp);
        tp_time_dark[tile->at(it)]->Fill(floor(static_cast<double>(i)/(32.*3.)), temp_time);

      }
      else{
        double temp_time = GetTimeHours(time->at(it)) - time_zero;
        //cout<<"Time: "<<time->at(it)<<" as "<<temp_time<<endl;
        if (time->at(it) < 1800.) {
          temp_time = GetTimeHours(time->at(it) + 2400.) - time_zero;
          //cout<<"here: "<<time->at(it)<<endl;
        }

        g_all_signal[tile->at(it)]->Fill(temp_time, imonTemp, 1);
        tp_signal[tile->at(it)]->Fill(floor(static_cast<double>(i)/(32.*3.)), imonTemp);
        tp_time_signal[tile->at(it)]->Fill(floor(static_cast<double>(i)/(32.*3.)), temp_time);
      }
    }
  }

  double dark[32][2][ec];
  double signal[32][2][ec];
  for (int j = 0; j < 32; j ++){
    for (int i = 1 ; i <= ec; i++){
        dark[j][0][i - 1] = tp_time_dark[j]->GetBinContent(i);
        dark[j][0][i - 1] = tp_dark[j]->GetBinContent(i);

        signal[j][0][i - 1] = tp_time_signal[j]->GetBinContent(i);
        signal[j][0][i - 1] = tp_signal[j]->GetBinContent(i);
    }
  }

  TGraph *g_dark_current_rms[32];
  TGraph *g_signal_current_rms[32];
  for (int i = 0; i < 32; i++){
    g_dark_current_rms[i] = new TGraph();
    g_signal_current_rms[i] = new TGraph();
    g_dark_current_rms[i]->Set(ec);
    g_signal_current_rms[i]->Set(ec);

    for (int j = 0 ; j < ec ; j++){
      g_signal_current_rms[i]->SetPoint(j, tp_time_signal[i]->GetBinContent(j+1), tp_signal[i]->GetBinError(j+1));
      g_dark_current_rms[i]->SetPoint(j, tp_time_dark[i]->GetBinContent(j+1), tp_dark[i]->GetBinError(j+1));
    }
  }

  float xPos = 0.2;
  float yPos = 0.86;
  float yPos2 = 0.2;
  float dy = 0.04;
  float dy2 = 0.05;
  float fontType = 43;
  float fontSize = 13;
  for (int i = 1; i < 32; i++){
    TCanvas *c1 = new TCanvas("c1", "", 500, 500);
    g_all_dark[i]->SetTitle(";Hours passed; I_{dark} [#mu A]");
    g_all_dark[i]->Draw("colz");
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText("sEPD Dark Current Over Time",xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Tile %d", i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    c1->SaveAs(Form("../data/figures/dark_current/s01/dark_current_%d.png", i));
    c1->SaveAs(Form("../data/figures/dark_current/s01/dark_current_%d.pdf", i));

    g_all_signal[i]->SetTitle(";time; I_{signal} [#mu A]");

    g_all_signal[i]->Draw("colz");
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText("sEPD Signal Current Over Time",xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Tile %d", i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    c1->SaveAs(Form("../data/figures/dark_current/s01/signal_current_%d.png", i));
    c1->SaveAs(Form("../data/figures/dark_current/s01/signal_current_%d.pdf", i));

    g_all_signal[i]->SetTitle(";time; I [#mu A]");

    g_all_signal[i]->SetLineColor(kRed);
    g_all_dark[i]->SetLineColor(kBlue);
    g_all_signal[i]->Draw("box");
    g_all_dark[i]->Draw("box same");
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText("sEPD Dark and Signal Current Over Time",xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Tile %d", i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    auto legend = new TLegend(0.69,0.75,0.99,0.85);
    legend->SetFillStyle(4000);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.03);


    legend->AddEntry(g_all_dark[i],"Dark ");
    legend->AddEntry(g_all_signal[i],"Signal");
    legend->Draw();
    c1->SaveAs(Form("../data/figures/dark_current/s01/signal_dark_current_%d.png", i));
    c1->SaveAs(Form("../data/figures/dark_current/s01/signal_dark_current_%d.pdf", i));
   }
   TCanvas *c2 = new TCanvas("c2", "", 500, 500);
   c2->SetLogy();
   for (int i = 1; i < 32; i++){
     g_dark_current_rms[i]->SetMarkerColor(kBlue);
     g_dark_current_rms[i]->SetMarkerColor(kBlue);

     g_dark_current_rms[i]->SetMarkerSize(.5);
     g_dark_current_rms[i]->SetMarkerStyle(25);
     g_dark_current_rms[i]->SetTitle(";Hours passed; I_{dark} [#mu A]");
     g_dark_current_rms[i]->Draw("AP");
     drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
     drawText("sEPD Dark Current RMS Over Time",xPos,yPos-dy2,0, 1, fontSize, fontType);
     drawText(Form("Tile %d", i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
     c2->SaveAs(Form("../data/figures/dark_current/s01/dark_current_rms_%d.png", i));
     c2->SaveAs(Form("../data/figures/dark_current/s01/dark_current_rms_%d.pdf", i));
     g_signal_current_rms[i]->SetMarkerColor(kRed);
     g_signal_current_rms[i]->SetMarkerSize(.5);
     g_signal_current_rms[i]->SetMarkerStyle(25);
     g_signal_current_rms[i]->SetTitle(";time; I_{signal} [#mu A]");

     g_signal_current_rms[i]->Draw("AP");
     drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
     drawText("sEPD Signal Current RMS Over Time",xPos,yPos-dy2,0, 1, fontSize, fontType);
     drawText(Form("Tile %d", i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
     c2->SaveAs(Form("../data/figures/dark_current/s01/signal_current_rms_%d.png", i));
     c2->SaveAs(Form("../data/figures/dark_current/s01/signal_current_rms_%d.pdf", i));

     g_signal_current_rms[i]->SetTitle(";time; I [#mu A]");
     g_signal_current_rms[i]->GetHistogram()->SetMaximum(1);
     g_signal_current_rms[i]->GetHistogram()->SetMinimum(.001);
     g_signal_current_rms[i]->SetLineColor(kRed);
     g_dark_current_rms[i]->SetLineColor(kBlue);
     g_signal_current_rms[i]->Draw("AP");
     g_dark_current_rms[i]->Draw("P");
     drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
     drawText("sEPD Dark and Signal Current RMS Over Time",xPos,yPos-dy2,0, 1, fontSize, fontType);
     drawText(Form("Tile %d", i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
     auto legend = new TLegend(0.69,0.75,0.99,0.85);
     legend->SetFillStyle(4000);
     legend->SetBorderSize(0);
     legend->SetTextSize(0.03);

     legend->AddEntry(g_dark_current_rms[i],"Dark ");
     legend->AddEntry(g_signal_current_rms[i],"Signal");
     legend->Draw();
     c2->SaveAs(Form("../data/figures/dark_current/s01/signal_dark_current_rms_%d.png", i));
     c2->SaveAs(Form("../data/figures/dark_current/s01/signal_dark_current_rms_%d.pdf", i));
    }

  return;
}

void compare_source_collimation(string fname, TString cap, bool makeRootFile = true){
  gStyle->SetOptStat(0);
  SetyjPadStyle();
  int version = 1;
  int tiles = 1;
  make_sEPD_rootfile_v1(fname, cap, tiles);


  TFile* fin = new TFile(Form("../data/sEPD%s.root",cap.Data()), "read");
  TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
  Float_t xpos, ypos;
  vector<Int_t>* ch=nullptr;
  vector<Int_t>* tile=nullptr;
  vector<Int_t>* device=nullptr;
  vector<Float_t>* imon=nullptr;
  vector<Float_t>* rmon=nullptr;
  vector<Float_t>* vcomp=nullptr;
  inTree_p->SetBranchAddress("xpos", &xpos);
  inTree_p->SetBranchAddress("ypos", &ypos);
  inTree_p->SetBranchAddress("device", &device);
  inTree_p->SetBranchAddress("channel", &ch);
  inTree_p->SetBranchAddress("tile", &tile);
  inTree_p->SetBranchAddress("imon", &imon);
  inTree_p->SetBranchAddress("rmon", &rmon);
  inTree_p->SetBranchAddress("vcomp", &vcomp);

  return;
}
void scan_coll(bool makeRootFile = true){

  string filenames[5] = {"coll_comp/20220210-1411_LineScan_control_tile4_xpos13.5.txt","coll_comp/20220211-1449_LineScan_alum_tile4_xpos13.5.txt","coll_comp/20220211-1458_LineScan_alum_tile4_xpos15.0.txt","coll_comp/20220214-1147_LineScan_acryl_tile4_xpos15.0.txt","coll_comp/20220214-1149_LineScan_acryl_tile4_xpos13.5.txt"};
  //string filenames[2] = {"20220214-1625_LineScan_OUTPUT_tile9_xpos23.0.txt", "20220214-1630_LineScan_OUTPUT_tile8_xpos23.0"};
  double xfinalcm = 13.5;
  double xorigincm = 13.5;
  double xstepcm = 0;
  double yfinalcm = 34 ;
  double yorigincm = 18;
  double ystepcm = 0.25;
  int nsteps = 64;
  int nRep = 1;
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  const int nf = 5;
  for (int c = 0; c < nf; c++){
    TString cap = Form("_c%d", c);
    make_sEPD_rootfile_v1(filenames[c], cap, 1);
  }
  double h_scans[nf][2][64];
  double max[5] = {0};
  double ped = 0;
  int max_i = 0;
  for ( int c = 0; c < nf; c++){
    ped = 0;
    max_i = 0;
    TString cap = Form("_c%d", c);
    TFile* fin = new TFile(Form("../data/sEPD%s.root",cap.Data()), "read");
    TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
    Float_t xpos, ypos;
    vector<Int_t>* ch=nullptr;
    vector<Int_t>* tile=nullptr;
    vector<Int_t>* device=nullptr;
    vector<Float_t>* imon=nullptr;
    vector<Float_t>* rmon=nullptr;
    vector<Float_t>* vcomp=nullptr;
    inTree_p->SetBranchAddress("xpos", &xpos);
    inTree_p->SetBranchAddress("ypos", &ypos);
    inTree_p->SetBranchAddress("device", &device);
    inTree_p->SetBranchAddress("channel", &ch);
    inTree_p->SetBranchAddress("tile", &tile);
    inTree_p->SetBranchAddress("imon", &imon);
    inTree_p->SetBranchAddress("rmon", &rmon);
    inTree_p->SetBranchAddress("vcomp", &vcomp);
    double ped = 0.;
    int en = inTree_p->GetEntries();
    for (int i = 0; i < en; i++){
      if ( i > 59){
        inTree_p->GetEntry(i);
        ped = ped + imon->at(0)/4;
      }
    }

    cout<<"events: "<<en<<endl;
    for (int i = 0; i < en; i++){
      inTree_p->GetEntry(i);
      h_scans[c][0][i] = ypos;
      h_scans[c][1][i] = imon->at(0) - ped;
      if (imon->at(0) > max[c]) {
        max[c] = imon->at(0);
        max_i = i;
      }
    }
    cout<<"Run "<<c<<endl;
  }
  TCanvas *cc = new TCanvas("cc", "", 500, 500);
  TGraph *g_scans[nf];
  for (int i = 0; i < nf; i ++){
    g_scans[i] = new TGraph(nsteps, h_scans[i][0], h_scans[i][1]);
  }
  g_scans[0]->SetLineColor(kBlack);
  g_scans[1]->SetLineColor(kBlue);
  g_scans[2]->SetLineColor(kRed);
  g_scans[3]->SetLineColor(kGreen);
  g_scans[4]->SetLineColor(kViolet);
  for (int i = 0; i < nf; i++){
    g_scans[i]->SetLineWidth(3);
  }
  g_scans[0]->Draw("AL");
  g_scans[1]->Draw("L");
  g_scans[4]->Draw("L");
  float xPos = 0.62;
  float yPos = 0.86;
  float yPos2 = 0.2;
  float dy = 0.04;
  float dy2 = 0.05;
  float fontType = 43;
  float fontSize = 13;
  double scale_x = 0.1;
  double scale_y = 5.5;
  double rott = TMath::Pi()/2;
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("sEPD Testing Amplitudes",xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("Tile 4",xPos,yPos-2*dy2,0, 1, fontSize, fontType);
  draw_scan(4, 17.5, 0.7, rott, scale_x, scale_y);
  auto legend = new TLegend(0.6,0.6,0.9,0.75);
  legend->AddEntry(g_scans[0],"No additional collimation ");
  legend->AddEntry(g_scans[1],"Aluminum Disk");
  //legend->AddEntry(g_scans[2],"","f");
  //legend->AddEntry(g_scans[3],"","f");
  legend->AddEntry(g_scans[4],"Acryllic Block");
  legend->Draw();


  cc->SaveAs("../data/coll_comp/amp_comp.pdf");
  double g_scan_norm[nf][2][64];
  for(int i = 0; i < nf; i++){
    for(int j = 0; j < 64; j++){
      g_scan_norm[i][0][j] = h_scans[i][0][j];
      g_scan_norm[i][1][j] = h_scans[i][1][j]/(max[j]);
    }
  }
  TGraph *g_scans_scale[nf];
  for (int i = 0; i < nf; i ++){
    g_scans_scale[i] = new TGraph(nsteps, g_scan_norm[i][0], g_scan_norm[i][1]);
  }

TCanvas *tc = new TCanvas("tc","", 500, 500);
g_scans_scale[0]->SetLineColor(kBlack);
g_scans_scale[1]->SetLineColor(kBlue);
g_scans_scale[2]->SetLineColor(kRed);
g_scans_scale[3]->SetLineColor(kGreen);
g_scans_scale[4]->SetLineColor(kViolet);
for (int i = 0; i < nf; i++){
  g_scans_scale[i]->SetLineWidth(3);
}
g_scans_scale[4]->SetTitle("; y [cm]; Normalized amplitude");
g_scans_scale[4]->Draw("AL");
g_scans_scale[1]->Draw("L");
//g_scans_scale[2]->Draw("L");
//g_scans_scale[3]->Draw("L");
g_scans_scale[0]->Draw("L");

//float xPos = 0.2;
xPos = 0.62;
yPos = 0.86;
yPos2 = 0.2;
dy = 0.04;
dy2 = 0.05;
fontType = 43;
fontSize = 13;
scale_x = 0.06;
scale_y = 6;
rott = TMath::Pi()/2;
drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
drawText("sEPD Testing Collimation",xPos,yPos-dy2,0, 1, fontSize, fontType);
drawText("Tile 4",xPos,yPos-2*dy2,0, 1, fontSize, fontType);
draw_scan(4, 17.5, 0.35, rott, scale_x, scale_y);
auto legenda = new TLegend(0.6,0.6,0.9,0.75);
legenda->AddEntry(g_scans[0],"No additional collimation ");
legenda->AddEntry(g_scans[1],"Aluminum Disk");
//legend->AddEntry(g_scans[2],"","f");
//legend->AddEntry(g_scans[3],"","f");
legenda->AddEntry(g_scans[4],"Acryllic Block");
legenda->Draw();
tc->SaveAs("../data/coll_comp/coll_comp.pdf");
  return;
}

void scan_middle(bool makeRootFile = true){
  gStyle->SetOptStat(0);
  SetyjPadStyle();
  int version = 5;
  int sector = 1;
  string fname = {"../data/20220217-1755_line_1_orig_sipm.txt"};

//  string fname = {"../data/20220216-2045_line_1_switch_sipm.txt"};
  //string fname = "../data/20220209-1601_TEST_OUTPUT.txt";

  TString cap = Form("_m%d",version);
  make_sEPD_rootfile_v1(fname, cap);
  TFile* fin = new TFile(Form("../data/sEPD%s.root",cap.Data()), "read");
  TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
  Float_t xpos, ypos;
  vector<Int_t>* ch=nullptr;
  vector<Int_t>* tile=nullptr;
  vector<Int_t>* device=nullptr;
  vector<Float_t>* imon=nullptr;
  vector<Float_t>* rmon=nullptr;
  vector<Float_t>* vcomp=nullptr;
  inTree_p->SetBranchAddress("xpos", &xpos);
  inTree_p->SetBranchAddress("ypos", &ypos);
  inTree_p->SetBranchAddress("device", &device);
  inTree_p->SetBranchAddress("channel", &ch);
  inTree_p->SetBranchAddress("tile", &tile);
  inTree_p->SetBranchAddress("imon", &imon);
  inTree_p->SetBranchAddress("rmon", &rmon);
  inTree_p->SetBranchAddress("vcomp", &vcomp);

  vector<string> params, paramNames;
  double xfinalcm;
  double xorigincm;
  double xstepcm;
  double yfinalcm;
  double yorigincm;
  double ystepcm;
  int nsteps;
  double nRep;

  GetParameters(paramNames, params, fname, cap);
  cout << "Done getting info: "<<params.size()<<endl;
  for ( int i = 0; i < params.size(); i++){
    cout<<paramNames[i]<<" : "<<params[i]<<endl;
  }
  for (int i = 0; i < paramNames.size(); i++){
    if (paramNames[i] == "xorigincm" || paramNames[i] == " xorigincm") xorigincm = stof(params[i]);
    if (paramNames[i] == " xfinalcm" || paramNames[i] == "xfinalcm") xfinalcm = stof(params[i]);
    if (paramNames[i] == " xsteplengthcm" || paramNames[i] == "xsteplengthcm") xstepcm = stof(params[i]);
    if (paramNames[i] == "yorigincm" || paramNames[i] == " yorigincm") yorigincm = stof(params[i]);
    if (paramNames[i] == " yfinalcm" || paramNames[i] == "yfinalcm") yfinalcm = stof(params[i]);
    if (paramNames[i] == " ysteplengthcm" || paramNames[i] == "ysteplengthcm") ystepcm = stof(params[i]);

  }
  nRep = 10.;
  nsteps = floor((xfinalcm - xorigincm)/xstepcm);
  cout<<"Out of code: "<<endl;
  cout<< "xorigincm : "<<xorigincm<<endl;
  cout<< "yorigincm : "<<yorigincm<<endl;
  cout<< "xfinalcm : "<<xfinalcm<<endl;
  cout<< "yfinalcm : "<<yfinalcm<<endl;
  cout<< "xsteplengthcm : "<<xstepcm<<endl;
  cout<< "ysteplengthcm : "<<ystepcm<<endl;
  cout<< "nsteps : "<<nsteps<<endl;
  cout<< "nRep : "<<nRep<<endl;

  string dir = "s01";
  int NTILE = 32;
  TH1D *h1_tile_dc[NTILE];
  TH1D *h1_tile_dc2[NTILE];
  TH1D *h1_temp_scan[NTILE];
  TH1D *h1_RMS_scan[NTILE];
  TH1D *h1_RMS_on[NTILE];
  TH1D *h1_RMS_off[NTILE];
  TH1D *h1_RMS_on2[NTILE];
  TH1D *h1_RMS_off2[NTILE];

  for ( int i = 0; i <= NTILE; i++ ){
      h1_tile_dc[i] = new TH1D(Form("h1_tile_dc_%d", i+1), "",10, 0.1, 0.56);
      h1_temp_scan[i] = new TH1D(Form("h1_temp_scan_%d", i+1), "",100, 0.0, 1.0);
      h1_RMS_scan[i] = new TH1D(Form("h1_RMS_scan_%d", i+1), ";RMS [#mu A];",100, 0.0, 0.025);
      h1_RMS_on[i] = new TH1D(Form("h1_RMS_on_%d", i+1), "",100, 0.0, 0.025);
      h1_RMS_off[i] = new TH1D(Form("h1_RMS_off_%d", i+1), "",100, 0.0, 0.025);
      h1_tile_dc2[i] = new TH1D(Form("h1_tile_dc2_%d", i+1), "",10, 0.1, 0.6);
      h1_RMS_on2[i] = new TH1D(Form("h1_RMS_on2_%d", i+1), "",100, 0.0, 0.025);
      h1_RMS_off2[i] = new TH1D(Form("h1_RMS_off2_%d", i+1), "",100, 0.0, 0.025);
  }

  //number of steps per line,
  int nEntries = inTree_p->GetEntries();
  double last_xpos = 0.;
  bool no_av = false;
  double max_y = .6;
  //cout<<"entering dark current"<<endl;
  for ( int i = 0; i < nEntries; i++ ){

    inTree_p->GetEntry(i);
    //calculate dark current
    //cout<<"Trial: "<< i <<"--------------"<<endl;
    for(int it = 0; it<tile->size(); it++){
      double imonTemp = imon->at(it);
      if(xpos==0 && ypos==0 && i < 500){//for dark current
        h1_tile_dc[tile->at(it)]->Fill(imonTemp);
      //  cout<<"Tile "<<it<<" Rep "<<i<<" : "<<imonTemp<<endl;
      }
      else if(xpos==0 && ypos==0){//for dark current
        h1_tile_dc2[tile->at(it)]->Fill(imonTemp);
        no_av = false;
      //  cout<<"Tile "<<it<<" Rep "<<i<<" : "<<imonTemp<<endl;
      }
      else{
        if (i == nRep) last_xpos = xpos;
        if (last_xpos == xpos){
          h1_temp_scan[tile->at(it)]->Fill(imonTemp);
        }
        else {
          //cout<<"rms: "<<h1_temp_scan[tile->at(it)]->GetRMS()<<endl;
          h1_RMS_scan[tile->at(it)]->Fill(h1_temp_scan[tile->at(it)]->GetRMS());
          h1_temp_scan[tile->at(it)]->Reset();
        }
      }
    }
    if (!(last_xpos == xpos)) last_xpos = xpos;
  }
  /////////////////////////////////////
  // Calculate Dark Current
  double mean_dc[NTILE];
  double rms_dc[NTILE];
  double mean_dc2[NTILE];
  double rms_dc2[NTILE];
  int nScanCount[NTILE];
  double slope_dc[NTILE];
  printf("DarkCurrent Values: \n");
  printf(" --------------------------------------------- \n");
  double ymid = 24;
  for(int it = 0; it < NTILE; it++){
    // if (it == 0){
    //   mean_dc[it] = h1_tile_dc[1]->GetMean();
    //   rms_dc[it] = h1_tile_dc[1]->GetRMS();
    // }
    // else {
      mean_dc[it] = h1_tile_dc[it]->GetMean();
      rms_dc[it] = h1_tile_dc[it]->GetRMS();
      mean_dc2[it] = h1_tile_dc2[it]->GetMean();
      rms_dc2[it] = h1_tile_dc2[it]->GetRMS();
      if (no_av){
        mean_dc2[it] = h1_tile_dc[it]->GetMean();
        rms_dc2[it] = h1_tile_dc[it]->GetRMS();
      }
      slope_dc[it] = (mean_dc2[it] - mean_dc[it])/(xfinalcm - xorigincm);
      if (slope_dc[it] < 0) slope_dc[it] = 0;
    //}
    printf("Tile %d: %f +/- %f -> %f +/- %f = %f\n", it, mean_dc[it], rms_dc[it], mean_dc2[it], rms_dc2[it], slope_dc[it]);
    nScanCount[it] = 0;
  }

  int zcount = 0;
  int bb =0;
  double totals = 0.;
  TProfile *h1_tile_response[NTILE];
  TH2D *h2_tile_rmon_imon[NTILE];
  TProfile *h1_tile_rmon[NTILE];
  TH1D *h1_tile_rmon_1[NTILE];
  TGraph *h1_all_rmon[NTILE];
  TGraph *h1_all_imon[NTILE];
  vector<double> vec_xpos = {};
  vector<double> vec_ypos = {};
  last_xpos = 0.0;
  double last_ypos = 0.0;
  vec_xpos.push_back(last_xpos);
  vec_ypos.push_back(last_ypos);

  for (int i = 0; i < NTILE; i++){
    h1_tile_response[i] = new TProfile(Form("h1_tile_response_%d", i), "", nsteps+1, xorigincm-xstepcm/2, xfinalcm+xstepcm/2);
    h2_tile_rmon_imon[i] = new TH2D(Form("h2_tile_rmon_imon_%d", i), "", 50, 55, 61, 60, 0.0, 1.2);
    h1_tile_rmon[i] = new TProfile(Form("h1_tile_rmon_%d", i), "", nsteps+1, xorigincm-xstepcm/2, xfinalcm+xstepcm/2);
    h1_tile_rmon_1[i] = new TH1D(Form("h1_tile_rmon_1_%d", i), "", 100, 47, 70);
    h1_all_rmon[i] = new TGraph();
    h1_all_imon[i] = new TGraph();

  }

  for (int i = 0; i < nEntries; i++){
    inTree_p->GetEntry(i);

    for(int it = 0; it<tile->size(); it++){
      double imonTemp = imon->at(it);
      double rmonTemp = rmon->at(it);

      double av_dc = slope_dc[tile->at(it)]*(xpos - xorigincm);
      if (no_av) av_dc = 0;
      h1_all_rmon[tile->at(it)]->Set(h1_all_rmon[tile->at(it)]->GetN()+1);
      h1_all_rmon[tile->at(it)]->SetPoint(h1_all_rmon[tile->at(it)]->GetN()-1, h1_all_rmon[tile->at(it)]->GetN()-1, rmonTemp*42);
      h1_tile_rmon_1[tile->at(it)]->Fill(rmonTemp*42);// = new TProfile(Form("h1_tile_rmon_%d", i), "", nsteps+1, xorigincm-xstepcm/2, xfinalcm+xstepcm/2);
      h2_tile_rmon_imon[tile->at(it)]->Fill(rmonTemp*42, imonTemp);
      if(!(xpos==0 && ypos==0)){//for dark current
        if(ypos < ymid && tile->at(it)%2 == 1){
          h1_tile_response[tile->at(it)]->Fill(xpos, imonTemp - (mean_dc2[tile->at(it)]));
          h1_tile_rmon[tile->at(it)]->Fill(xpos, rmonTemp*42);
          h1_all_imon[tile->at(it)]->Set(h1_all_imon[tile->at(it)]->GetN()+1);
          h1_all_imon[tile->at(it)]->SetPoint(h1_all_imon[tile->at(it)]->GetN()-1, h1_all_imon[tile->at(it)]->GetN()-1, imonTemp - (av_dc + mean_dc[tile->at(it)]));
        }
        if (ypos > ymid && (tile->at(it)%2 == 0 || tile->at(it) == 1)){
          if (tile->at(it) == 1) {
            h1_tile_response[0]->Fill(xpos, imonTemp - (mean_dc[tile->at(it)] + av_dc));
            h1_tile_rmon[0]->Fill(xpos, rmonTemp*42);
            h1_all_imon[0]->Set(h1_all_imon[0]->GetN()+1);
            h1_all_imon[0]->SetPoint(h1_all_imon[0]->GetN()-1, h1_all_imon[0]->GetN()-1, imonTemp);
          }
          else{
            h1_tile_response[tile->at(it)]->Fill(xpos, imonTemp - (mean_dc[tile->at(it)] + av_dc));
            h1_tile_rmon[tile->at(it)]->Fill(xpos, rmonTemp*42);
            h1_all_imon[tile->at(it)]->Set(h1_all_imon[tile->at(it)]->GetN()+1);
            h1_all_imon[tile->at(it)]->SetPoint(h1_all_imon[tile->at(it)]->GetN()-1, h1_all_imon[tile->at(it)]->GetN()-1, imonTemp);
          }
        }
      }
    }//tile
    if (last_xpos != xpos || last_ypos != ypos){
      last_xpos = xpos;
      last_ypos = ypos;
      vec_xpos.push_back(last_xpos);
      vec_ypos.push_back(last_ypos);
    }
  }
  TH1D *h_blank = new TH1D("h_blank", ";x [cm]; <I>_{sig} - <I>_{dark}", nsteps, 4, 88);
  h_blank->SetMaximum(max_y);
  h_blank->SetMinimum(0);
  int colors16[16] = {kBlack, kRed+1, kBlue+1, kGreen-3, kCyan+1, kOrange -3, kMagenta -3, kGray, kRed -1, kBlue -1, kGreen -7, kCyan - 3, kOrange+1, kMagenta -5, kViolet+6, kAzure +3};
  float xPos = 0.2;
  //float xPos = 0.85;
  float yPos = 0.86;
  float yPos2 = 0.2;
  float dy = 0.04;
  float dy2 = 0.05;
  float fontType = 43;
  float fontSize = 13;
  TCanvas *c_full = new TCanvas("c_full", "");
  // for(int i = 0; i < h1_tile_response[0]->GetNbinsX();i++){
  //   cout<<"bin "<<i<<" : "<< h1_tile_response[0]->GetBinContent(i)<<endl;
  // }
  for (int i = 0; i < 2 ; i++){
    for (int j = 0; j < 16;j++){
      if(j == 0){
        h1_tile_response[i]->SetTitle(";x [cm]; <I>_{sig} - <I>_{dark}");
        h1_tile_response[i]->SetLineColor(colors16[0]);
        h1_tile_response[i]->SetLineWidth(2);
        h1_tile_response[i]->SetMaximum(max_y);
        h1_tile_response[i]->SetMinimum(0);
        h1_tile_response[i]->Draw("hist");
      }
      else{
        h1_tile_response[2*j+i]->SetLineColor(colors16[j]);
        h1_tile_response[2*j+i]->SetLineWidth(2);
        h1_tile_response[2*j+i]->Draw("hist same");
      }
    }
    if( i == 0){
      drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
      drawText(Form("sEPD %s Crosstalk Test", dir.c_str()),xPos,yPos-dy2,0, 1, fontSize, fontType);
      drawText("Even Tiles",xPos,yPos-2*dy2,0, 1, fontSize, fontType);
      //drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);

      c_full->SaveAs(Form("../data/figures/crosstalk/%s/h1_even.png", dir.c_str()));
      c_full->SaveAs(Form("../data/figures/crosstalk/%s/h1_even.pdf", dir.c_str()));
    }
    else{
      drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
      drawText(Form("sEPD %s Crosstalk Test", dir.c_str()),xPos,yPos-dy2,0, 1, fontSize, fontType);
      drawText("Odd Tiles",xPos,yPos-2*dy2,0, 1, fontSize, fontType);
      //drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);

      c_full->SaveAs(Form("../data/figures/crosstalk/%s/h1_odd.png", dir.c_str()));
      c_full->SaveAs(Form("../data/figures/crosstalk/%s/h1_odd.pdf", dir.c_str()));
    }
  }
  TCanvas *c_four = new TCanvas("c_four", "");
  for (int k = 0; k < 2; k++){
    for (int i = 0; i <13 ; i++){
      int h1 = 0;
      int h2 = 0;
      for (int j = 0; j < 4;j++){
        if(j == 0){
          double centerbin = 0;
          int maxbin = h1_tile_response[2*(i+1)+k ]->GetMaximumBin();
          for ( int l = maxbin; l < nsteps; l++){
            if(h1_tile_response[2*(i +1)+ k]->GetBinContent(l) <= h1_tile_response[2*(i+2)+k]->GetBinContent(l)){
              centerbin = l;
              break;
            }
          }
          if (centerbin == 0)centerbin = maxbin + 5;
          double low_x = h1_tile_response[2*i+k]->GetBinCenter(centerbin) - 25;
          double high_x = h1_tile_response[2*i+k]->GetBinCenter(centerbin) + 25;

          h_blank->GetXaxis()->SetRangeUser(low_x, high_x);
          h_blank->Draw("hist");
          h1_tile_response[2*i + k]->SetLineColor(colors16[0]);
          h1_tile_response[2*i + k]->SetLineWidth(2);
          h1_tile_response[2*i + k]->SetMaximum(max_y);
          h1_tile_response[2*i + k]->Draw("hist same");
          h1 = 2*i+k;
          if(h1 == 0) h1 = 1;
        }
        else{
          h1_tile_response[2*(i+j)+k]->SetLineColor(colors16[j]);
          h1_tile_response[2*(i+j)+k]->SetLineWidth(2);
          h1_tile_response[2*(i+j)+k]->Draw("hist same");
          h2 = 2*(i+j)+k;
        }
      }
      drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
      drawText(Form("sEPD %s Crosstalk Test", dir.c_str()),xPos,yPos-dy2,0, 1, fontSize, fontType);
      drawText(Form("Sector s%d", sector),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
      // drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);
      drawText(Form( "Tiles: %d, %d, %d, %d", h1, h2 - 4, h2 -2, h2),xPos,yPos-4*dy2,0, 1, fontSize, fontType);

      c_four->SaveAs(Form("../data/figures/crosstalk/%s/h1_four_%d_%d.png", dir.c_str(),h1, h2));
      c_four->SaveAs(Form("../data/figures/crosstalk/%s/h1_four_%d_%d.pdf", dir.c_str(),h1, h2));
    }
  }
  TCanvas *c_two = new TCanvas("c_two", "");
  for (int k = 0; k < 2; k++){
    for (int i = 0; i <15 ; i++){
      int h1 = 0;
      int h2 = 0;
      for (int j = 0; j < 2;j++){
        if(j == 0){
          double centerbin = 0;
          int maxbin = h1_tile_response[2*i+k]->GetMaximumBin();
          for ( int l = maxbin; l < nsteps; l++){
            if(h1_tile_response[2*i + k]->GetBinContent(l) <= h1_tile_response[2*(i+1)+k]->GetBinContent(l)){
              centerbin = l;
              break;
            }
          }
          if (centerbin == 0)centerbin = maxbin + 5;
          double low_x = h1_tile_response[2*i+k]->GetBinCenter(centerbin) - 10;
          double high_x = h1_tile_response[2*i+k]->GetBinCenter(centerbin) + 10;
          if (low_x < 4) low_x = 4.;
          if (high_x > 88) high_x = 88.;
          h_blank->GetXaxis()->SetRangeUser(low_x, high_x);
          h_blank->Draw("hist");
          h1_tile_response[2*i + k]->SetLineColor(colors16[0]);
          h1_tile_response[2*i + k]->SetLineWidth(2);
          h1_tile_response[2*i + k]->SetMaximum(max_y);
          h1_tile_response[2*i + k]->Draw("hist same");
          h1 = 2*i+k;
          if (h1 == 0) h1 = 1;
        }
        else{
          h1_tile_response[2*(i+j)+k]->SetLineColor(colors16[j]);
          h1_tile_response[2*(i+j)+k]->SetLineWidth(2);
          h1_tile_response[2*(i+j)+k]->Draw("hist same");
          h2 = 2*(i+j)+k;
        }
      }
      drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
      drawText(Form("sEPD %s Crosstalk Test", dir.c_str()),xPos,yPos-dy2,0, 1, fontSize, fontType);
      drawText(Form("Sector s%d", sector),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
      // drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);

      drawText(Form( "Tiles: %d, %d", h1, h2),xPos,yPos-4*dy2,0, 1, fontSize, fontType);

      c_two->SaveAs(Form("../data/figures/crosstalk/%s/h1_two_%d_%d.png", dir.c_str(),h1, h2));
      c_two->SaveAs(Form("../data/figures/crosstalk/%s/h1_two_%d_%d.pdf", dir.c_str(),h1, h2));
    }
  }

  TCanvas *c_RMS = new TCanvas("c_RMS","");
  for (int i = 1; i < NTILE;i++){
    h1_RMS_scan[i]->Draw("hist");
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText(Form("sEPD %s Crosstalk Test", dir.c_str()),xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Tile %d RMS", i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    // drawText("SiPM Switched",xPos,yPos-4*dy2,0, 1, fontSize, fontType);

    c_RMS->SaveAs(Form("../data/figures/crosstalk/%s/h1_rms_%d.png", dir.c_str(),i));
    c_RMS->SaveAs(Form("../data/figures/crosstalk/%s/h1_rms_%d.pdf", dir.c_str(),i));

  }

  TCanvas *c_ri = new TCanvas("c_ri","", 500, 500);
  for (int i = 0; i < NTILE; i++){
    h2_tile_rmon_imon[i]->SetTitle("; SiPM Voltage [V]; I [#mu A]");
    h2_tile_rmon_imon[i]->Draw("colz");
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText(Form("sEPD %s Crosstalk Test", dir.c_str()),xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Tile %d", i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    // drawText("SiPM Switched",xPos,yPos-4*dy2,0, 1, fontSize, fontType);

    c_ri->SaveAs(Form("../data/figures/crosstalk/%s/h2_tile_rmon_imon_%d.png", dir.c_str(),i));
    c_ri->SaveAs(Form("../data/figures/crosstalk/%s/h2_tile_rmon_imon_%d.pdf", dir.c_str(),i));
  }

  const int si = vec_xpos.size();
  double xx[si];
  double yy[si];
  for (int i = 0; i < si;i++){
    xx[i] = vec_xpos.at(i);
    yy[i] = vec_ypos.at(i);
  }
  TGraph *g_pos = new TGraph(si, xx, yy);

  TCanvas *c_line = new TCanvas("c_line","",500, 500);

  g_pos->SetLineColor(kGreen+3);
  g_pos->SetLineWidth(2);
  g_pos->GetHistogram()->SetMaximum(50);
  g_pos->Draw("AL");
  const float xorigin = 90.5;
  const float yorigin = 24;
  const float rot = 3*TMath::Pi()/2;
  const float sx = 23.;
  const float sy = 86.;
  draw_scan(1, xorigin, yorigin, rot, sx, sy);

  TCanvas *c_ri_tile = new TCanvas("c_ri_tile","",1000, 500);
  TPad *uPad = new TPad("uPad","",0,0,1,1);
  uPad->Draw();
  TPad *overlay = new TPad("overlay","",0,0,1,1);
  overlay->SetFillStyle(4000);
  overlay->SetFillColor(0);
  overlay->SetFrameFillStyle(4000);
  overlay->Draw();
  for ( int i = 0; i < NTILE ; i++){
    uPad->cd();
    h1_tile_response[i]->SetLineColor(kBlue);
    h1_tile_response[i]->SetMaximum(max_y);
    h1_tile_response[i]->SetLineWidth(2);
    h1_tile_response[i]->SetTitle(";xpos; I [#mu A]");
    h1_tile_response[i]->Draw("hist");
    overlay->cd();
    h1_tile_rmon[i]->SetLineColor(kRed);
    h1_tile_rmon[i]->SetLineWidth(2);
    h1_tile_rmon[i]->SetTitle(";; V_{set}");
    h1_tile_rmon[i]->GetXaxis()->SetTickLength(0); //";; V_{set}");
    h1_tile_rmon[i]->GetYaxis()->SetTickLength(0); //";; V_{set}");
    h1_tile_rmon[i]->SetMinimum(45);
    h1_tile_rmon[i]->SetMaximum(75);
    h1_tile_rmon[i]->Draw(" hist Y+");
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText(Form("sEPD %s Crosstalk Test", dir.c_str()),xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Tile %d", i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    //drawText("SiPM Switched",xPos,yPos-4*dy2,0, 1, fontSize, fontType);

    c_ri_tile->SaveAs(Form("../data/figures/crosstalk/%s/h2_tile_rmon_%d.png", dir.c_str(),i));
    c_ri_tile->SaveAs(Form("../data/figures/crosstalk/%s/h2_tile_rmon_%d.pdf", dir.c_str(),i));
  }

  TCanvas *c_wave = new TCanvas("c_wave","",1000, 1000);
  TPad *p_pad[16];
  for (int i = 0; i < 4; i++){
    for ( int j = 0; j < 4; j++){
        p_pad[4*i + j] = new TPad(Form("p_%d",i), "", .25*i, .75 - .25*j, .25 + .25*i, 1 - .25*j);
        p_pad[4*i + j]->Draw();
    }
  }
  for ( int i = 0; i < 2; i++){
    for (int j = 0; j < 16; j++){
        p_pad[j]->cd();
        h1_all_rmon[2*j + i]->SetMarkerColor(kRed);
        h1_all_rmon[2*j + i]->SetMarkerSize(0.05);
        h1_all_rmon[2*j + i]->SetMarkerStyle(25);

        h1_all_rmon[2*j + i]->Draw("AP");
        drawText(Form("Tile %d", 2*j+i),xPos,yPos,0, 1, fontSize, fontType);
        drawText(Form("Avg SiPM Voltage %.2f #pm %.2f", h1_tile_rmon_1[2*j+1]->GetMean(), h1_tile_rmon_1[2*j+1]->GetRMS()),xPos,yPos-dy2,0, 1, fontSize, fontType);
    }
    c_wave->SaveAs(Form("../data/figures/crosstalk/%s/h2_all_rmon_%d.png", dir.c_str(),i));
  }
  for ( int i = 0; i < 2; i++){
    for (int j = 0; j < 16; j++){
        p_pad[j]->cd();
        h1_all_imon[2*j + i]->SetLineColor(kBlue);
        h1_all_imon[2*j + i]->SetLineWidth(2);
        h1_all_imon[2*j + i]->Draw("AP");
        drawText(Form("Tile %d", 2*j+i),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    }
    c_wave->SaveAs(Form("../data/figures/crosstalk/%s/h2_all_imon_%d.png", dir.c_str(),i));
  }
  TCanvas *c = new TCanvas("c","", 1500, 200);
  h1_all_imon[7]->SetMarkerSize(0.01);
  h1_all_imon[7]->SetMarkerStyle(4);
  h1_all_imon[7]->Draw("AP");

}

void scan_middle_y(bool makeRottFile = true){
  string fname[2] =
      {"20220214-1625_LineScan_OUTPUT_tile9_xpos23.0.txt", "20220214-1630_LineScan_OUTPUT_tile8_xpos23.0.txt"};

  gStyle->SetOptStat(0);
  SetyjPadStyle();
  int version = 1;
  int sector = 1;
  int tiles = 1;
  TString cap1 = Form("_c9_%d",version);
  TString cap2 = Form("_c8_%d",version);

  make_sEPD_rootfile_v1(fname[0], cap1, tiles);
  make_sEPD_rootfile_v1(fname[1], cap2, tiles);
  TFile* fin = new TFile(Form("../data/sEPD%s.root",cap1.Data()), "read");
  TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
  Float_t xpos, ypos;
  vector<Int_t>* ch=nullptr;
  vector<Int_t>* tile=nullptr;
  vector<Int_t>* device=nullptr;
  vector<Float_t>* imon=nullptr;
  vector<Float_t>* rmon=nullptr;
  vector<Float_t>* vcomp=nullptr;
  inTree_p->SetBranchAddress("xpos", &xpos);
  inTree_p->SetBranchAddress("ypos", &ypos);
  inTree_p->SetBranchAddress("device", &device);
  inTree_p->SetBranchAddress("channel", &ch);
  inTree_p->SetBranchAddress("tile", &tile);
  inTree_p->SetBranchAddress("imon", &imon);
  inTree_p->SetBranchAddress("rmon", &rmon);
  inTree_p->SetBranchAddress("vcomp", &vcomp);
  vector<string> params, paramNames;
  double xfinalcm = 23;
  double xorigincm = 23;
  double xstepcm = 0;
  double yfinalcm = 27;
  double yorigincm = 15;
  double ystepcm = 0.25;
  int nsteps = 48;
  double nRep;

  nRep = 5.;

  nsteps = floor((yfinalcm - yorigincm)/ystepcm);
  cout<< "xorigincm : "<<xorigincm<<endl;
  cout<< "yorigincm : "<<yorigincm<<endl;
  cout<< "xfinalcm : "<<xfinalcm<<endl;
  cout<< "yfinalcm : "<<yfinalcm<<endl;
  cout<< "xsteplengthcm : "<<xstepcm<<endl;
  cout<< "ysteplengthcm : "<<ystepcm<<endl;
  cout<< "nsteps : "<<nsteps<<endl;
  cout<< "nRep : "<<nRep<<endl;

  //number of steps per line,
  int nEntries = inTree_p->GetEntries();

  int zcount = 0;
  int bb =0;
  double totals = 0.;

  TH1D *h1_tile_response_9;

  h1_tile_response_9 = new TH1D("h1_tile_response_9", "", nsteps, yorigincm, yfinalcm);


  for (int i = 0; i < nEntries; i++){
    inTree_p->GetEntry(i);
    for(int it = 0; it<tile->size(); it++){
      double imonTemp = imon->at(it);
      h1_tile_response_9->Fill(ypos, imonTemp/nRep -0.435/nRep);
    }//tile
  }


  TFile *fin2 = new TFile(Form("../data/sEPD%s.root",cap2.Data()), "read");
  TTree *inTree_p2 = (TTree*) fin2 ->Get("sEPDTree");
  Float_t xpos2, ypos2;
  vector<Int_t>* ch2=nullptr;
  vector<Int_t>* tile2=nullptr;
  vector<Int_t>* device2=nullptr;
  vector<Float_t>* imon2=nullptr;
  vector<Float_t>* rmon2=nullptr;
  vector<Float_t>* vcomp2=nullptr;
  inTree_p2->SetBranchAddress("xpos", &xpos2);
  inTree_p2->SetBranchAddress("ypos", &ypos2);
  inTree_p2->SetBranchAddress("device", &device2);
  inTree_p2->SetBranchAddress("channel", &ch2);
  inTree_p2->SetBranchAddress("tile", &tile2);
  inTree_p2->SetBranchAddress("imon", &imon2);
  inTree_p2->SetBranchAddress("rmon", &rmon2);
  inTree_p2->SetBranchAddress("vcomp", &vcomp2);

  vector<string> params2, paramNames2;
  double xfinalcm2 = 23;
  double xorigincm2 = 23;
  double xstepcm2 = 0;
  double yfinalcm2 = 32;
  double yorigincm2 = 22;
  double ystepcm2 = 0.25;
  int nsteps2 = 40;
  double nRep2 = 5.;
  cout<< "xorigincm : "<<xorigincm2<<endl;
  cout<< "yorigincm : "<<yorigincm2<<endl;
  cout<< "xfinalcm : "<<xfinalcm2<<endl;
  cout<< "yfinalcm : "<<yfinalcm2<<endl;
  cout<< "xsteplengthcm : "<<xstepcm2<<endl;
  cout<< "ysteplengthcm : "<<ystepcm2<<endl;
  cout<< "nsteps : "<<nsteps2<<endl;
  cout<< "nRep : "<<nRep2<<endl;

  //number of steps per line,
  int nEntries2 = inTree_p2->GetEntries();

  TH1D *h1_tile_response_8;
  TH1D *h_blank = new TH1D("h_blank","",1, yorigincm, yfinalcm2);
  h_blank->SetMaximum(.3);
  h_blank->SetMinimum(0);
  h1_tile_response_8 = new TH1D("h1_tile_response_8", "", nsteps2, yorigincm2, yfinalcm2);


  for (int i = 0; i < nEntries2; i++){
    inTree_p2->GetEntry(i);
    for(int it = 0; it<tile2->size(); it++){
      double imonTemp = imon2->at(it);
      h1_tile_response_8->Fill(ypos2, imonTemp/nRep2 - 0.375/nRep2);
    }//tile
  }

  int colors16[16] = {kBlack, kRed+1, kBlue+1, kGreen-3, kCyan+1, kOrange -3, kMagenta -3, kGray, kRed -1, kBlue -1, kGreen -7, kCyan - 3, kOrange+1, kMagenta -5, kViolet+6, kAzure +3};
  float xPos = 0.2;
  //float xPos = 0.85;
  float yPos = 0.86;
  float yPos2 = 0.2;
  float dy = 0.04;
  float dy2 = 0.05;
  float fontType = 43;
  TCanvas *cc = new TCanvas("cc", "");
  h1_tile_response_8->SetLineColor(kBlack);
  h1_tile_response_9->SetLineColor(kRed);

  h1_tile_response_8->SetLineWidth(2);
  h1_tile_response_9->SetLineWidth(2);
  h1_tile_response_9->GetXaxis()->SetRangeUser(15, 33);

  h_blank->Draw("hist");
  h1_tile_response_9->Draw("hist same");
  h1_tile_response_8->Draw("hist same");
}

void scan_sEPD_mine(bool makeRootFile = true){

  gStyle->SetOptStat(0);
  SetyjPadStyle();

  /////////////////////////////////////
  // Match version and the text file name
  int version = 12;
  TString cap = Form("_v%d", version);
  if(makeRootFile){
    if(version == 1){
      make_sEPD_rootfile_v1("../data/20220128-1913_TEST_OUTPUT.txt", cap);
    }
    if(version == 5){
      make_sEPD_rootfile_v1("../data/20220211-1836_sector1_Full.txt", cap);
    }
    if(version == 6){
      make_sEPD_rootfile_v1("../data/20220211-1836_sector1_Full_comp.txt", cap);
    }
    if(version == 3){
      make_sEPD_rootfile_v1("../data/20220201-1735_middle_fine.txt", cap);
    }
    if(version == 4){
      make_sEPD_rootfile_v1("../data/20220209-1601_TEST_OUTPUT.txt", cap);
    }
    if(version == 2){
      make_sEPD_rootfile_v1("../data/20220131-1540_middlescan.txt", cap);
    }
    if(version == 12){
      make_sEPD_rootfile_v3("../data/20220218-1858_sector1_Full.txt", cap);
    }
    else if(version == 7){
      make_sEPD_rootfile_v1("20220121-1054_TEST_OUTPUT_x7to15_y15to23_dx2_dy2.txt",cap);//v7
    } else if(version == 8){
      make_sEPD_rootfile_v1("20220121-1720_TEST_OUTPUT.txt",cap);//v8
      //make_sEPD_rootfile_v1("20220121-1720_TEST_OUTPUT_temp_x85_y0.txt",cap);//v8
      //make_sEPD_rootfile_v1("20220121-1720_TEST_OUTPUT_temp_x75_y15.txt",cap);//v8
    } else if(version == 9){
      make_sEPD_rootfile_v1("20220125-0925_TEST_OUTPUT_x15_y30.txt",cap);//v9
      //make_sEPD_rootfile_v1("20220125-0925_TEST_OUTPUT_x5_y22p5.txt",cap);//v9
    } else if(version == 10){
      make_sEPD_rootfile_v1("20220125-1951_TEST_OUTPUT.txt",cap);//v10
    } else if(version == 11){
      make_sEPD_rootfile_v1("20220128-1823_TEST_OUTPUT_v11.txt",cap);//v11
      //make_sEPD_rootfile_v1("20220128-1812_TEST_OUTPUT.txt",cap);//v11_temp
    }
  }


  /////////////////////////////////////
  // IMPORT THE TREE FROM ROOT FILE
  TFile* fin = new TFile(Form("../data/sEPD%s.root",cap.Data()), "read");
  TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
  Float_t xpos, ypos;
  vector<Int_t>* ch=nullptr;
  vector<Int_t>* tile=nullptr;
  vector<Int_t>* device=nullptr;
  vector<Float_t>* imon=nullptr;
  vector<Float_t>* rmon=nullptr;
  vector<Float_t>* vcomp=nullptr;
  inTree_p->SetBranchAddress("xpos", &xpos);
  inTree_p->SetBranchAddress("ypos", &ypos);
  inTree_p->SetBranchAddress("device", &device);
  inTree_p->SetBranchAddress("channel", &ch);
  inTree_p->SetBranchAddress("tile", &tile);
  inTree_p->SetBranchAddress("imon", &imon);
  inTree_p->SetBranchAddress("rmon", &rmon);
  inTree_p->SetBranchAddress("vcomp", &vcomp);

  TFile* fout = new TFile(Form("../data/hist_sEPD%s.root",cap.Data()), "recreate");

  /////////////////////////////////////
  // DEFINE HISTOGRAMS
  const int nTILE = 32;

  TH1D* h1D_imon[nTILE];//imon dist for all source positions including dark current
  TH1D* h1D_imon_dc[nTILE];//imon dist for only for dark current

  const double xMin = 3;
  const double xMax = 95;
  const double yMin = 0.0;
  const double yMax = 49.0;
  double nSep_x = .25;//x bin width in cm
  double nSep_y = .25;//y bin width in cm
  //float nSep_y = 0.25;//y bin width in cm
  int nx = (xMax - xMin)/nSep_x;
  int ny = (yMax - yMin)/nSep_y;
  cout<<"bins: "<<nx<<" , "<<ny<<endl;
  TH2D* h2D_x_y_imon_all = new TH2D(Form("h2D_x_y_imon_%s", "all"), ";x [cm];y [cm]", nx + 1, xMin - 0.125, xMin + nx*nSep_x + 0.125, ny + 1, yMin - 0.125, yMin+ny*nSep_y + 0.125);//source x,y position map for all source positions, to check the total scanning map
  TH2D* h2D_x_y_imon[nTILE];//imon response (z-axis) as a function of source x, y positions for each tile separately.
  TGraph *g_all_locs[nTILE];
  for(int it = 0; it < nTILE; it++){
    g_all_locs[it] = new TGraph();
    h2D_x_y_imon[it] = new TH2D(Form("h2D_x_y_imon_tile%d", it), ";x [cm];y [cm]", nx, xMin, xMax, ny, yMin, yMax);
    h1D_imon[it] = new TH1D(Form("h1D_imon_tile%d", it), ";SiPM current (IMON) [#muA];", 2000, 0, 20.000);
    h1D_imon_dc[it] = new TH1D(Form("h1D_imon_darkCurrent_tile%d", it), ";SiPM current (IMON) [#muA];", 2000, 0, 20.000);
  }

  /////////////////////////////////////
  // EVENT LOOP
  int nEntries = inTree_p->GetEntries();
  int nDiv = TMath::Max((int)1, nEntries/100);
  std::cout << "Total number of scan steps = " << nEntries << std::endl;
  for(int entry = 0; entry < nEntries; entry++){
    if(nEntries%nDiv == 0) std::cout << " Entry " << entry << "/" << nEntries << std::endl;
    inTree_p->GetEntry(entry);
    for(int it = 0; it<tile->size(); it++){
      double imonTemp = imon->at(it);
      if(xpos==3 && ypos==0){//for dark current
        h1D_imon_dc[tile->at(it)]->Fill(imonTemp);
      }
    }//tile
  }//event loop
  /////////////////////////////////////
  // Calculate Dark Current
  double mean_dc[nTILE];
  double rms_dc[nTILE];
  int nScanCount[nTILE];
  printf("DarkCurrent Values: \n");
  printf(" --------------------------------------------- \n");

  for(int it = 0; it < nTILE; it++){
    mean_dc[it] = h1D_imon_dc[it]->GetMean();
    rms_dc[it] = h1D_imon_dc[it]->GetRMS();
    printf("Tile %d: %f +/- %f \n", it, mean_dc[it], rms_dc[it]);
    nScanCount[it] = 0;
  }
  /////////////////////////////////////
  // EVENT LOOP

  for(int entry = 0; entry < nEntries; entry++){
    if(nEntries%nDiv == 0) std::cout << " Entry " << entry << "/" << nEntries << std::endl;
    inTree_p->GetEntry(entry);
    g_all_locs[0]->Set(g_all_locs[0]->GetN() + 1);
    g_all_locs[0]->SetPoint(g_all_locs[0]->GetN() - 1, xpos, ypos);
    for(int it = 0; it<tile->size(); it++){
      double imonTemp = imon->at(it);
      h1D_imon[tile->at(it)]->Fill(imonTemp);//imon dist for all source positions including dark current

      if(!(xpos==3 && ypos==0)){//for dark current
        g_all_locs[tile->at(it)]->Set(g_all_locs[tile->at(it)]->GetN() + 1);
        g_all_locs[tile->at(it)]->SetPoint(g_all_locs[tile->at(it)]->GetN() - 1, xpos, ypos);
        h2D_x_y_imon_all->Fill(xpos, ypos, (imonTemp/5.)/32.);
        h2D_x_y_imon[tile->at(it)]->Fill(xpos, ypos, imonTemp/5. - mean_dc[tile->at(it)]/5.);
      }
    }//tile
  }//event loop

  /////////////////////////////////////
  // // EVENT LOOP - To estimate normalization factor
  // for(ULong64_t entry = 0; entry < nEntries; ++entry){
  //   if(nEntries%nDiv == 0) std::cout << " Entry " << entry << "/" << nEntries << std::endl;
  //   inTree_p->GetEntry(entry);
  //
  //   for(int it = 0; it<tile->size(); it++){
  //     double imonTemp = imon->at(it);
  //     int tilePos = tile->at(it);
  //
  //     // 2022Jan30: in the future, add a function/condition which examine if the source is on top of the tile
  //     if(imonTemp > (mean_dc[tilePos]+1.0*rms_dc[tilePos])){
  //       nScanCount[tilePos]+=1;
  //     }
  //   }//tile
  // }//event loop


  ////////////////////////////////////////////////////////
  // 2022Jan29: Work-in-progress
  // normalize the 2D hist
  // for(int it = 0; it < nTILE; it++){
  //   cout << "Tile #" << it << ": dark current mean, rms, number of scan = " << mean_dc[it] << ", " << rms_dc[it] << ", " << nScanCount[it] << endl;
  //   if(it==nTILE-1) {
  //     cout << "before normalization" << endl;
  //     h2D_x_y_imon[it]->Print("all");
  //   }
  //   h2D_x_y_imon[it]->Scale(1./(double)nScanCount[it]);
  //   for(int ix = 0; ix < h2D_x_y_imon[it]->GetNbinsX(); ++ix){
  //     for(int iy = 0; iy < h2D_x_y_imon[it]->GetNbinsY(); ++iy){
  //       float tempCont = h2D_x_y_imon[it]->GetBinContent(ix+1,iy+1);
  //       if(tempCont!=0){
  //         // cout << "OrgCont, mean_dc[it], bkSubCont = " << tempCont << ", " << mean_dc[it] << ", " << tempCont-mean_dc[it] << endl;
  //         h2D_x_y_imon[it]->SetBinContent(ix+1,iy+1, tempCont-mean_dc[it]);
  //       }
  //     }
  //   }
  // if(it==nTILE-1){
  // cout << "after normalization" << endl;
  //  h2D_x_y_imon[it]->Print("all");
  // }

  // for(int it = 0; it < nTILE; it++){
  //   for(int ix = 0; ix < h2D_x_y_imon[it]->GetNbinsX(); ++ix){
  //     for(int iy = 0; iy < h2D_x_y_imon[it]->GetNbinsY(); ++iy){
  //       float tempCont = h2D_x_y_imon[it]->GetBinContent(ix+1,iy+1);
  //       if(tempCont){
  //         tempCont = (tempCont/5.)-mean_dc[it];
  //         h2D_x_y_imon[it]->SetBinContent(ix+1,iy+1, tempCont);
  //       }
  //     }
  //   }
  // }


  /////////////////////////////////////
  // START DRAWING
  TString savedir = "../data/";
  float xPos = 0.12;
  //float xPos = 0.85;
  float yPos = 0.86;
  float yPos2 = 0.2;
  float dy = 0.05;
  float dy2 = 0.05;
  float fontType = 43;
  float fontSize = 13;
  float max_tile[31];
  float tilesss[31];
  //tile overlay position
  const float xorigin = 90.5;
  const float yorigin = 24;

  const float rot = 3*TMath::Pi()/2;

  /////////////////////////////////////
  // DRAW PLOTS 2D IMON
  cout << "Plotting x-y imon weighted 2D histograms for each tile" << endl;

  TCanvas* c_2d_2 = new TCanvas(Form("c_2d_1_tile%s","all"),"", 750,400);
  c_2d_2->SetRightMargin(0.13);
  c_2d_2->SetLeftMargin(0.10);
  SetHistTextSize(h2D_x_y_imon_all);
  h2D_x_y_imon_all->SetTitleOffset(0.8, "Y");
  h2D_x_y_imon_all->Draw("colz");
  draw_scan(1, xorigin, yorigin, rot);
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("sEPD s01",xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);

  c_2d_2->SaveAs(Form("%s/figures/full_scan/s01_switch/hist2D_x_y_IMON%s_allChannel.pdf",savedir.Data(),cap.Data()));

  /////////////////////////////////////
  // DRAW PLOTS 2D IMON
  cout << "Plotting x-y imon weighted 2D histograms for each tile" << endl;

  TCanvas* c_2d_1[nTILE];
  for(int it = 1; it < nTILE; it++){
    c_2d_1[it] = new TCanvas(Form("c_2d_1_tile%d",it),"", 750,400);
    c_2d_1[it]->SetRightMargin(0.13);
    c_2d_1[it]->SetLeftMargin(0.10);
    SetHistTextSize(h2D_x_y_imon[it]);
    h2D_x_y_imon[it]->SetTitleOffset(0.8, "Y");
    //float zMax = h2D_x_y_imon[it]->GetBinContent(h2D_x_y_imon[it]->GetMaximumBin());
    //cout << " zMax = " << zMax << endl;
    //h2D_x_y_imon[it]->GetZaxis()->SetRangeUser(0.2,2.0);
    //h2D_x_y_imon[it]->GetZaxis()->SetRangeUser(-0.5,zMax*1.2);
    h2D_x_y_imon[it]->Draw("colz");
    max_tile[it - 1] = h2D_x_y_imon[it]->GetBinContent(h2D_x_y_imon[it]->GetMaximumBin());
    tilesss[it - 1] = it;

    if(it==1)
      draw_scan(it, xorigin, yorigin, rot);
    else if(it!=1 && it%2==0)
      draw_scan(it+1, xorigin, yorigin, rot);
    else if(it!=1 && it%2==1)
      draw_scan(it-1, xorigin, yorigin, rot);

    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText("sEPD s01",xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Tile ##bf{%d}", it),xPos,yPos-dy2*2,0, 1, fontSize, fontType);
    drawText(Form("Dark Current %0.2f #mu A", mean_dc[it]),xPos,yPos-dy2*3,0, 1, fontSize, fontType);
    drawText("<I>_{scan} - <I>_{dark} w/ source at (x, y)",xPos,yPos2,0, 1, fontSize, fontType);

    c_2d_1[it]->SaveAs(Form("%s/figures/full_scan/s01_switch/hist2D_x_y_IMON%s_tile%d.pdf",savedir.Data(),cap.Data(),it));
    c_2d_1[it]->SaveAs(Form("%s/figures/full_scan/s01_switch/hist2D_x_y_IMON%s_tile%d.png",savedir.Data(),cap.Data(),it));

  }
  TCanvas *cg = new TCanvas("cg","cg");
  TGraph *gg = new TGraph(31, tilesss, max_tile);
  gg->SetMarkerSize(2);
  gg->SetMarkerStyle(20);
  gg->SetMarkerColor(kBlack);
  gg->SetTitle("");
  gg->GetYaxis()->SetTitle("Peak signal above dark current [#mu A]");
  gg->GetXaxis()->SetTitle("Tile");
  gg->Draw("AP");
  drawText("#bf{sPHENIX} #it{Internal}",xPos + 0.1,yPos,0, 1, fontSize+2, fontType);
  drawText("Peak signal of tile above dark current",xPos + 0.1,yPos -dy2,0, 1, fontSize+2, fontType);
  cg->SaveAs(Form("%s/figures/full_scan/s01_switch/CompareHeights.png",savedir.Data()));

  /////////////////////////////////////
  // DRAW PLOTS 1D IMON FOR EACH TILE
  cout << "Plotting imon 1D dist for each tile" << endl;
  int colHere[] = {kPink+5,kRed+1,kMagenta+2,kGreen+1,kGreen+3};
  xPos = 0.85;
  yPos = 0.86;
  dy = 0.05;
  dy2 = 0.06;
  fontType = 43;
  fontSize = 13;

  TLegend* l1[nTILE];
  TCanvas* c_1d_1[nTILE];
  for(int it = 1; it < nTILE; it++){
    c_1d_1[it] = new TCanvas(Form("c_1d_1_tile%d",it),"", 450,400);
    c_1d_1[it]->SetRightMargin(0.13);

    l1[it] = new TLegend(0.18,0.90-0.15*1,0.50,0.90);
    legStyle(l1[it]);
    l1[it]->SetTextSize(0.03);
    l1[it]->SetMargin(0.14);

    h1D_imon[it]->Draw("hist");
    // l1[it]->AddEntry(h1D_imon_woSr[it], Form("#splitline{w/o source}{#splitline{Mean %.3f}{RMS %.3f}}",mean_woSr[it],rms_woSr[it]), "l");
    //l1[it]->AddEntry(h1D_imon_woSr[it], Form("#splitline{x,y=(0,0)cm ~ w/o source}{#splitline{Mean %.3f}{RMS %.3f}}",mean_woSr[it],rms_woSr[it]), "l");
    setHistLineStyle(h1D_imon[it], kBlack, 1, 2);
    float xmax_temp = getHistHighXvalue(h1D_imon[it]);
    h1D_imon[it]->GetXaxis()->SetRangeUser(0.0, xmax_temp*1.2);
    h1D_imon[it]->GetYaxis()->SetRangeUser(0.5, 10000);
    gPad->SetLogy();

    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,1, 1, fontSize+2, fontType);
    drawText("sEPD s01",xPos,yPos-dy2,1, 1, fontSize, fontType);
    drawText(Form("Tile ##bf{%d}", it),xPos,yPos-dy2*2,1, 1, fontSize, fontType);

    c_1d_1[it]->SaveAs(Form("%s/figures/full_scan/s01_switch/hist1D_IMON%s_tile%d.pdf",savedir.Data(),cap.Data(),it));
  }

  /////////////////////////////////////
  // DRAW PLOTS 1D IMON FOR EACH TILE
  cout << "Plotting imon 1D dist for each tile in one canvas" << endl;
  TCanvas* c_1d_2 = new TCanvas("c_1d_2", "", 6*250,6*200);
  makeMultiPanelCanvas(c_1d_2, 6, 6, 0.02, 0.04, 0.02, 0.1, 0.15, 0.10, 0.05);
  int colTemp = 9;
  for(int it = 1; it < nTILE; it++){
    c_1d_2->cd(it);
    SetHistTextSize(h1D_imon[it]);

    h1D_imon[it]->Draw("hist");
    h1D_imon[it]->GetXaxis()->SetTitleOffset(6.5);
    setHistLineStyle(h1D_imon[it], colTemp, 1, 1);
    gPad->SetLogy();

    if(it<=6) yPos = 0.75;
    else yPos = 0.82;
    if((it-1)%6==0) xPos = 0.85;
    else xPos = 0.80;
    // if((it-1)%6==0) xPos = 0.2;
    // else xPos = 0.18;
    drawText(Form("#color[%d]{Tile ##bf{%d}}", kBlack, it), xPos,yPos, 1,1, fontSize+2, fontType);
    //drawText(Form("#color[%d]{Tile ##bf{%d}}", colTemp, it), xPos,yPos, 1,1, fontSize+2, fontType);
    // drawText(Form("#color[%d]{Mean %.3f}", colTemp, mean_woSr[it]), xPos,yPos-dy, 0,1, fontSize, fontType);
    // drawText(Form("#color[%d]{RMS %.3f}", colTemp, rms_woSr[it]), xPos,yPos-dy*2, 0,1, fontSize, fontType);
    // drawText(Form("CH ##bf{%d}", it), xPos,yPos, 0,1, fontSize+2, fontType);
    // drawText(Form("Mean %.3f", mean_woSr[it]), xPos,yPos-dy, 0,1, fontSize, fontType);
    // drawText(Form("RMS %.3f", rms_woSr[it]), xPos,yPos-dy*2, 0,1, fontSize, fontType);
  }
  c_1d_2->cd(nTILE+1-1);
  xPos = 0.2;
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("sEPD s01",xPos,yPos-dy2,0, 1, fontSize, fontType);
  // drawText(Form("With {}^{90}_{38}Sr on CH##color[%d]{%d}",col,ch_s),xPos,yPos-dy2*2,0, 1, fontSize, fontType);
  // drawText("#splitline{(CH# does not}{ match to tile #)}",xPos,yPos-dy2*3.5,0, 1, fontSize, fontType);

  c_1d_2->SaveAs(Form("%s/figures/full_scan/s01_switch/hist1D_IMON%s_allChannels.pdf",savedir.Data(),cap.Data()));

  /////////////////////////////////////
  // DRAW PLOTS 1D IMON Dark Current FOR EACH TILE
  cout << "Plotting imon 1D dist for dark current for each tile in one canvas" << endl;
  TCanvas* c_1d_3 = new TCanvas("c_1d_3", "", 6*250,6*200);
  makeMultiPanelCanvas(c_1d_3, 6, 6, 0.02, 0.04, 0.02, 0.1, 0.15, 0.10, 0.05);

  for(int it = 1; it < nTILE; it++){
    c_1d_3->cd(it);
    SetHistTextSize(h1D_imon_dc[it]);

    h1D_imon_dc[it]->Draw("hist");
    h1D_imon_dc[it]->GetXaxis()->SetTitleOffset(6.5);
    h1D_imon_dc[it]->SetTitle(Form(";Dark Current [#muA];"));
    float xmax_temp = getHistHighXvalue(h1D_imon_dc[it]);
    h1D_imon_dc[it]->GetXaxis()->SetRangeUser(0.0, 0.6);
    setHistLineStyle(h1D_imon_dc[it], colTemp, 1, 1);
    // gPad->SetLogy();

    if(it<=6) yPos = 0.75;
    else yPos = 0.82;
    if((it-1)%6==0) xPos = 0.85;
    else xPos = 0.80;
    drawText(Form("#color[%d]{Tile ##bf{%d}}", kBlack, it), xPos,yPos, 1,1, fontSize+2, fontType);
  }
  c_1d_3->cd(nTILE+1-1);
  xPos = 0.2;
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("sEPD s01",xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("Dark Current",xPos,yPos-dy2*2,0, 1, fontSize, fontType);

  c_1d_3->SaveAs(Form("%s/figures/full_scan/s01_switch/hist1D_IMON_darkCurrent%s_allChannels.pdf",savedir.Data(),cap.Data()));


  ///////////////////////////////////////
  //// SAVE HISTOGRAMS

  //fout->cd();
  //for(int it = 1; it < nTILE; it++){
  //  h1D_imon_woSr[it]->Write();
  //}
  //h1D_mean->Write();
  //h1D_rms->Write();
  //fout->Close();
  TCanvas *ccc = new TCanvas("ccc","", 500, 500);
  g_all_locs[0]->Draw("AP");
}

void GetParameters(vector<string> &paramNames, vector<string> &params, TString inputFileName = "20220131-1540_middlescan.txt", TString cap=""){

  ifstream infile(Form("../data/%s", inputFileName.Data()));

  /////////////////////////////////////
  // IMPORT NUMBERS FROM THE TXT FILE
  Long64_t nLines = 0;
  Bool_t isDiffPosition = false;
  Float_t xpos_temp = 0;
  Float_t ypos_temp = 0;
  int ll = 0;


  while (infile)
  {
    string s;
    if (!getline( infile, s )) break;
    if (s.empty()) continue;
    if (isdigit(s.at(0))) continue;
    istringstream ss( s );
    vector<string> vec_str;

    while (ss)
    {
      string stemp;
      if (!getline( ss, stemp, '=' )) break;
      vec_str.push_back( stemp );
    }
    if(vec_str.size() < 2) continue;

    istringstream sN(vec_str.at(0));
    istringstream sP(vec_str.at(1));

    while (sN){
      string stemp;
      if (!getline( sN, stemp, ',')) break;
      paramNames.push_back( stemp );
    }
    while (sP){
      string stemp;
      if (!getline( sP, stemp, ',')) break;
      params.push_back( stemp );
    }
  }
}

// void isSourceOnTopOfTheTile(const int tile = 8, const double xorigin = 96., const double yorigin = 25., const double rot = 3*TMath::Pi()/2){
// }

void make_sEPD_rootfile_v3(TString inputFileName = "20220113-1447_TEST_OUTPUT.txt", TString cap="", int ttt = 32){

  cout << "RUNNING: make_sEPD_rootfile_v3 for " << inputFileName << " input file" << endl;

  ifstream infile(Form("../data/%s", inputFileName.Data()));
  Float_t xpos, ypos;
  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile;
  vector<Float_t> imon, rmon, vcomp;

  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TString fname = Form("../data/sEPD%s.root",cap.Data());
  TFile* fout = new TFile(Form("%s",fname.Data()), "recreate");
  TTree* tr = new TTree("sEPDTree", "");
  tr->Branch("xpos", &xpos, "xpos/F");
  tr->Branch("ypos", &ypos, "xpos/F");
  tr->Branch("device", &device);
  tr->Branch("channel", &ch);
  tr->Branch("tile", &tile);
  tr->Branch("imon", &imon);
  tr->Branch("rmon", &rmon);
  tr->Branch("vcomp", &vcomp);

  /////////////////////////////////////
  // IMPORT NUMBERS FROM THE TXT FILE
  Long64_t nLines = 0;
  Bool_t isDiffPosition = false;
  Float_t xpos_temp = 3;
  Float_t ypos_temp = 0;
  while (infile)
  {
    string s;
    if (!getline( infile, s )) break;
    if (s.empty()) continue;
    if (!isdigit(s.at(0))) continue;

    istringstream ss( s );
    vector<string> vec_str;

    while (ss)
    {
      string stemp;
      if (!getline( ss, stemp, ',' )) break;
      vec_str.push_back( stemp );
    }

    if(vec_str.size()==9){
      xpos_temp = stof(vec_str[4]);
      ypos_temp = stof(vec_str[5]);
      if (!(xpos_temp == xpos && ypos_temp == ypos)){
        tr->Fill();
        device.clear();
        ch.clear();
        tile.clear();
        imon.clear();
        rmon.clear();
        vcomp.clear();
        xpos = xpos_temp;
        ypos = ypos_temp;
      }
      else {
        xpos = xpos_temp;
        ypos = ypos_temp;
      }
      device.push_back(stoi(vec_str[0]));
      ch.push_back(stoi(vec_str[1]));
      tile.push_back(stoi(vec_str[2]));
      imon.push_back(stof(vec_str[6]));
      rmon.push_back(stof(vec_str[7]));
      vcomp.push_back(stof(vec_str[8]));
      nLines++;
    }

    // if(tile.size() == ttt){
    //   tr->Fill();
    //   device.clear();
    //   ch.clear();
    //   tile.clear();
    //   imon.clear();
    //   rmon.clear();
    //   vcomp.clear();
    // }
  }

  cout << "total numer of lines analyzed = " << nLines << endl;
  fout->cd();
  tr->Write("", TObject::kOverwrite);
  delete tr;
  fout->Close();
  cout << "DONE: " << fname << " has been created" << endl;
}

void make_sEPD_rootfile_v1(TString inputFileName = "20220113-1447_TEST_OUTPUT.txt", TString cap="", int ttt = 32){

  cout << "RUNNING: make_sEPD_rootfile_v1 for " << inputFileName << " input file" << endl;

  ifstream infile(Form("../data/%s", inputFileName.Data()));
  Float_t xpos, ypos;
  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile;
  vector<Float_t> imon, rmon, vcomp;

  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TString fname = Form("../data/sEPD%s.root",cap.Data());
  TFile* fout = new TFile(Form("%s",fname.Data()), "recreate");
  TTree* tr = new TTree("sEPDTree", "");
  tr->Branch("xpos", &xpos, "xpos/F");
  tr->Branch("ypos", &ypos, "xpos/F");
  tr->Branch("device", &device);
  tr->Branch("channel", &ch);
  tr->Branch("tile", &tile);
  tr->Branch("imon", &imon);
  tr->Branch("rmon", &rmon);
  tr->Branch("vcomp", &vcomp);

  /////////////////////////////////////
  // IMPORT NUMBERS FROM THE TXT FILE
  Long64_t nLines = 0;
  Bool_t isDiffPosition = false;
  Float_t xpos_temp = 0;
  Float_t ypos_temp = 0;
  while (infile)
  {
    string s;
    if (!getline( infile, s )) break;
    if (s.empty()) continue;
    if (!isdigit(s.at(0))) continue;

    istringstream ss( s );
    vector<string> vec_str;

    while (ss)
    {
      string stemp;
      if (!getline( ss, stemp, ',' )) break;
      vec_str.push_back( stemp );
    }

    if(vec_str.size()==9){
      device.push_back(stoi(vec_str[0]));
      ch.push_back(stoi(vec_str[1]));
      tile.push_back(stoi(vec_str[2]));
      xpos = stof(vec_str[4]);
      ypos = stof(vec_str[5]);
      imon.push_back(stof(vec_str[6]));
      rmon.push_back(stof(vec_str[7]));
      vcomp.push_back(stof(vec_str[8]));
      nLines++;
    }

    if(tile.size() == ttt){
      tr->Fill();
      device.clear();
      ch.clear();
      tile.clear();
      imon.clear();
      rmon.clear();
      vcomp.clear();
    }
  }

  cout << "total numer of lines analyzed = " << nLines << endl;
  fout->cd();
  tr->Write("", TObject::kOverwrite);
  delete tr;
  fout->Close();
  cout << "DONE: " << fname << " has been created" << endl;
}
void make_sEPD_rootfile_v2(TString inputFileName = "20220113-1447_TEST_OUTPUT.txt", TString cap=""){

  cout << "RUNNING: make_sEPD_rootfile_v2 for " << inputFileName << " input file" << endl;

  ifstream infile(Form("../data/%s", inputFileName.Data()));
  Float_t xpos, ypos;
  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile;
  vector<Float_t> time;
  vector<Float_t> imon, rmon, vcomp;

  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TString fname = Form("../data/sEPD%s.root",cap.Data());
  TFile* fout = new TFile(Form("%s",fname.Data()), "recreate");
  TTree* tr = new TTree("sEPDTree", "");
  tr->Branch("xpos", &xpos, "xpos/F");
  tr->Branch("ypos", &ypos, "xpos/F");
  tr->Branch("time", &time);
  tr->Branch("device", &device);
  tr->Branch("channel", &ch);
  tr->Branch("tile", &tile);
  tr->Branch("imon", &imon);

  /////////////////////////////////////
  // IMPORT NUMBERS FROM THE TXT FILE
  Long64_t nLines = 0;
  Bool_t isDiffPosition = false;
  Float_t xpos_temp = 0;
  Float_t ypos_temp = 0;
  while (infile)
  {
    string s;
    if (!getline( infile, s )) break;
    if (s.empty()) continue;
    if (!isdigit(s.at(0))) continue;

    istringstream ss( s );
    vector<string> vec_str;
    //printf(" %s \n", ss);
    while (ss)
    {
      string stemp;
      if (!getline( ss, stemp, ',' )) break;
      vec_str.push_back( stemp );
    }

    if(vec_str.size()==9){
      device.push_back(stoi(vec_str[0]));
      ch.push_back(stoi(vec_str[1]));
      tile.push_back(stoi(vec_str[2]));
      xpos = stof(vec_str[5]);
      ypos = stof(vec_str[6]);
      time.push_back(stof(vec_str[7]));
      imon.push_back(stof(vec_str[8]));
      nLines++;
    }
    if (xpos == 0 && ypos == 0){
      if(tile.size()==32){
        tr->Fill();
        device.clear();
        ch.clear();
        tile.clear();
        imon.clear();
        time.clear();
      }
    }
    else{
      if(tile.size()==1){
        tr->Fill();
        device.clear();
        ch.clear();
        tile.clear();
        imon.clear();
        time.clear();
      }
    }
  }

  cout << "total numer of lines analyzed = " << nLines << endl;
  fout->cd();
  tr->Write("", TObject::kOverwrite);
  delete tr;
  fout->Close();
  cout << "DONE: " << fname << " has been created" << endl;
}

// void isSourceOnTopOfTheTile(const int tile = 8, const double xorigin = 96., const double yorigin = 25., const double rot = 3*TMath::Pi()/2){
// }
