//Line Test Analysis
//Author: Daniel Lis
//Brief: Runs through the raw text file and makes root files and histograms relevant to the data.


// This is the analysis for the Line Test
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
//#include "draw_scan.C"
#include "yjUtility.h"
#include "sEPD_functions.C"
#include "sEPD_Line_Functions.C"
// Line Scan Analysis code

void Analyze(std::vector<std::string> &filenames, const int sector, const std::string data_dir, const std::string save_dir_raw, const std::string save_dir_plot, const std::string save_dir_root, int ch1, int ch2, bool debug = false){

  gStyle->SetOptStat(0);
  int size = filenames.size();
  std::vector<int> channels;
  char *sector_addon = new char[5];
  if (sector < 10) sprintf(sector_addon, "s0%d", sector);
  else sprintf(sector_addon,"s%d", sector);
  if (debug) cout<< "Destination directory for all the stuff: "<<save_dir_raw<<endl;

  TH2D *h2D_crosstalk = new TH2D("h2D_crosstalk","",31, 0.5, 31.5, 31, 0.5, 31.5);

  for (int ff = 0; ff < size; ff++){
    channels.clear();
    if (debug) cout<<"In Directory: "<<filenames.at(ff)<<endl;
    // for ( int i = 0; i < NTILE; i++ ){
    //   h1_tile_dc[i]->Reset();
    //   h1_temp_scan[i]->Reset();
    //   h1_RMS_scan[i]->Reset();
    //   h1_RMS_on[i]->Reset();
    //   h1_RMS_off[i]->Reset();
    //   h1_tile_dc2[i]->Reset();
    //   h1_RMS_on2[i]->Reset();
    //   h1_RMS_off2[i]->Reset();
    //   h1_tile_response[i]->Reset();
    //   h2_tile_rmon_imon[i]->Reset();
    //   h1_tile_rmon[i]->Reset();
    //   h1_tile_rmon_1[i]->Reset();
    //   h1_all_rmon[i]->Reset();
    //   h1_all_imon[i]->Reset();
    //   mean_dc[i]= 0;
    //   rms_dc[i]= 0;
    //   mean_dc2[i]= 0;
    //   rms_dc2[i]= 0;
    //   nScanCount[i]= 0;
    //   slope_dc[i]= 0;
    // }


    if (debug) cout<<"Line Scan "<<ff<<"...."<<endl;
    // Make the root file and save it to save_dir_raw
    GetChannels(filenames.at(ff), channels, debug);
    int n_channels = channels.size();

    if (true) {
      cout<<"Channels scanned: ";
      for (int j = 0; j < n_channels; j++){
        cout<<channels.at(j)<<" ";
      }
      cout<< ". "<<endl;
    }
    std::string fname;
    fname = MakeRootFile_Line(filenames.at(ff), data_dir, save_dir_raw, n_channels, debug);
    if (debug){
      cout<<"Made Root File"<<endl;
    }
    char *root_path = new char[100];
    sprintf(root_path, "%s%s.root", save_dir_raw.c_str(), fname.c_str());
    TFile* fin = new TFile(root_path, "read");
    if(!fin){
      cout<<"No File Found here"<<endl;
      continue;
    }
    TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
    if(!inTree_p){
      cout<<"No TTree found here..."<<endl;
      continue;
    }
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
    std::string data_path = "/" + fname + ".txt";
    if (debug) cout<< "Getting Parameters from file..."<<data_path<<endl;
    GetParameters_Line(paramNames, params, data_path, data_dir, debug);
    if (debug) {
      cout << "Done getting info: "<<params.size()<<endl;
      for ( int i = 0; i < params.size(); i++){
        cout<<paramNames[i]<<" : "<<params[i]<<endl;
      }
    }
    for (int i = 0; i < paramNames.size(); i++){
      if (paramNames[i] == "xorigincm" || paramNames[i] == " xorigincm") xorigincm = stof(params[i]);
      if (paramNames[i] == " xfinalcm" || paramNames[i] == "xfinalcm") xfinalcm = stof(params[i]);
      if (paramNames[i] == " xsteplengthcm" || paramNames[i] == "xsteplengthcm") xstepcm = stof(params[i]);
      if (paramNames[i] == "yorigincm" || paramNames[i] == " yorigincm") yorigincm = stof(params[i]);
      if (paramNames[i] == " yfinalcm" || paramNames[i] == "yfinalcm") yfinalcm = stof(params[i]);
      if (paramNames[i] == " ysteplengthcm" || paramNames[i] == "ysteplengthcm") ystepcm = stof(params[i]);
      if (paramNames[i] == " nRepeat" || paramNames[i] == "nRepeat") nRep = stof(params[i]);

    }
    //
    double origincm;
    double finalcm;
    double stepcm;
    int x_or_y;
    if (xorigincm == xfinalcm){
      x_or_y = 1;
      origincm = yorigincm;
      finalcm = yfinalcm;
      stepcm = ystepcm;
    }
    else {
      x_or_y = 0;
      origincm = xorigincm;
      finalcm = xfinalcm;
      stepcm = xstepcm;
    }

    nsteps = floor((finalcm - origincm)/stepcm);
    if (debug){
      cout<<"Parameters: "<<endl;
      cout<< "xorigincm : "<<xorigincm<<endl;
      cout<< "yorigincm : "<<yorigincm<<endl;
      cout<< "xfinalcm : "<<xfinalcm<<endl;
      cout<< "yfinalcm : "<<yfinalcm<<endl;
      cout<< "xsteplengthcm : "<<xstepcm<<endl;
      cout<< "ysteplengthcm : "<<ystepcm<<endl;
      cout<< "nsteps : "<<nsteps<<endl;
      cout<< "nRep : "<<nRep<<endl;
    }
    int NTILE = 32;
    TH1D *h1_tile_dc[NTILE];
    TH1D *h1_tile_dc2[NTILE];
    TH1D *h1_temp_scan[NTILE];
    TH1D *h1_RMS_scan[NTILE];
    TH1D *h1_RMS_on[NTILE];
    TH1D *h1_RMS_off[NTILE];
    TH1D *h1_RMS_on2[NTILE];
    TH1D *h1_RMS_off2[NTILE];

    for ( int i = 0; i < NTILE; i++ ){
        h1_tile_dc[i] = new TH1D(Form("h1_tile_dc_%d", i), "",10, 0.1, 0.56);
        h1_temp_scan[i] = new TH1D(Form("h1_temp_scan_%d", i), "",100, 0.0, 1.0);
        h1_RMS_scan[i] = new TH1D(Form("h1_RMS_scan_%d", i), ";RMS [#mu A];",100, 0.0, 0.025);
        h1_RMS_on[i] = new TH1D(Form("h1_RMS_on_%d", i), "",100, 0.0, 0.025);
        h1_RMS_off[i] = new TH1D(Form("h1_RMS_off_%d", i), "",100, 0.0, 0.025);
        h1_tile_dc2[i] = new TH1D(Form("h1_tile_dc2_%d", i), "",10, 0.1, 0.6);
        h1_RMS_on2[i] = new TH1D(Form("h1_RMS_on2_%d", i), "",100, 0.0, 0.025);
        h1_RMS_off2[i] = new TH1D(Form("h1_RMS_off2_%d", i), "",100, 0.0, 0.025);
    }

    TProfile *h1_tile_response[NTILE];
    TH2D *h2_tile_rmon_imon[NTILE];
    TProfile *h1_tile_rmon[NTILE];
    TH1D *h1_tile_rmon_1[NTILE];
    TGraph *h1_all_rmon[NTILE];
    TGraph *h1_all_imon[NTILE];
    for (int i = 0; i < NTILE; i++){
      h1_tile_response[i] = new TProfile(Form("h1_tile_response_%d", i), "", nsteps+1, origincm-stepcm/2, finalcm+stepcm/2);
      h2_tile_rmon_imon[i] = new TH2D(Form("h2_tile_rmon_imon_%d", i), "", 50, 55, 61, 60, 0.0, 1.2);
      h1_tile_rmon[i] = new TProfile(Form("h1_tile_rmon_%d", i), "", nsteps+1, origincm-stepcm/2, finalcm+stepcm/2);
      h1_tile_rmon_1[i] = new TH1D(Form("h1_tile_rmon_1_%d", i), "", 100, 47, 70);
      h1_all_rmon[i] = new TGraph();
      h1_all_imon[i] = new TGraph();
    }

    double mean_dc[NTILE];
    double rms_dc[NTILE];
    double mean_dc2[NTILE];
    double rms_dc2[NTILE];
    int nScanCount[NTILE];
    double slope_dc[NTILE];

    //number of steps per line,
    int nEntries = inTree_p->GetEntries();
    double last_pos[2] = {0};
    double pos[2];
    double max_y = 1.2;

    //cout<<"entering dark current"<<endl;
  //  if (debug) nEntries = 100;
    bool use_two = false;
    bool no_av = true;
    bool normalize = true;
    for ( int i = 0; i < nEntries; i++ ){

      inTree_p->GetEntry(i);
      //calculate dark current
      //if (debug) cout<<"Trial: "<< i <<"--------------"<<endl;
      //if (debug) PrintChannels(channels);
      for(int it = 0; it<tile->size(); it++){
        double imonTemp = imon->at(it);
        pos[0] = xpos;
        pos[1] = ypos;
        if(xpos==0 && ypos==0 && i < 500){//for dark current
          h1_tile_dc[tile->at(it)]->Fill(imonTemp);
        }
        else if(xpos==0 && ypos==0){//for dark current
          h1_tile_dc2[tile->at(it)]->Fill(imonTemp);
          //no_av = false;
        }
        else{
          if (i == nRep) last_pos[x_or_y] = pos[x_or_y];
          if (last_pos[x_or_y] == pos[x_or_y]){
            h1_temp_scan[tile->at(it)]->Fill(imonTemp);
          }
          else {
            //cout<<"rms: "<<h1_temp_scan[tile->at(it)]->GetRMS()<<endl;
            h1_RMS_scan[tile->at(it)]->Fill(h1_temp_scan[tile->at(it)]->GetRMS());
            h1_temp_scan[tile->at(it)]->Reset();
          }
        }
      }
      if (!(last_pos[x_or_y] == pos[x_or_y])) last_pos[x_or_y] = pos[x_or_y];
    }
    /////////////////////////////////////
    // Calculate Dark Current

    if (debug) printf("DarkCurrent Values: \n");
    if (debug) printf(" --------------------------------------------- \n");
    double ymid = 24;
    for(int it = 0; it < NTILE; it++){
        mean_dc[it] = h1_tile_dc[it]->GetMean();
        rms_dc[it] = h1_tile_dc[it]->GetRMS();
        if (use_two){
          mean_dc[it] = h1_tile_dc2[it]->GetMean();
          rms_dc[it] = h1_tile_dc2[it]->GetRMS();
        }
        mean_dc2[it] = h1_tile_dc2[it]->GetMean();
        rms_dc2[it] = h1_tile_dc2[it]->GetRMS();

        slope_dc[it] = (mean_dc2[it] - mean_dc[it])/(xfinalcm - xorigincm);
        if (no_av) slope_dc[it] = 0;
    //    if (slope_dc[it] < 0) slope_dc[it] = 0;
      //}
      if (debug) printf("Tile %d: %f +/- %f -> %f +/- %f = %f\n", it, mean_dc[it], rms_dc[it], mean_dc2[it], rms_dc2[it], slope_dc[it]);
      nScanCount[it] = 0;
    }


    int zcount = 0;
    int bb =0;
    double totals = 0.;
    vector<double> vec_xpos = {};
    vector<double> vec_ypos = {};

    vec_xpos.push_back(last_pos[0]);
    vec_ypos.push_back(last_pos[1]);

    for (int i = 0; i < nEntries; i++){
      inTree_p->GetEntry(i);
      pos[0] = xpos;
      pos[1] = ypos;
      for(int it = 0; it<tile->size(); it++){
        double imonTemp = imon->at(it);
        double rmonTemp = rmon->at(it);

        double av_dc = slope_dc[tile->at(it)]*(pos[x_or_y] - origincm);
        if (no_av) av_dc = 0;
        h1_all_rmon[tile->at(it)]->Set(h1_all_rmon[tile->at(it)]->GetN()+1);
        h1_all_rmon[tile->at(it)]->SetPoint(h1_all_rmon[tile->at(it)]->GetN()-1, h1_all_rmon[tile->at(it)]->GetN()-1, rmonTemp*42);
        h1_tile_rmon_1[tile->at(it)]->Fill(rmonTemp*42);// = new TProfile(Form("h1_tile_rmon_%d", i), "", nsteps+1, xorigincm-xstepcm/2, xfinalcm+xstepcm/2);
        h2_tile_rmon_imon[tile->at(it)]->Fill(rmonTemp*42, imonTemp);
        if(!(pos[0]==0 && pos[1]==0)){//for dark current

            h1_tile_response[tile->at(it)]->Fill(pos[x_or_y], imonTemp - (mean_dc[tile->at(it)]));
            h1_tile_rmon[tile->at(it)]->Fill(pos[x_or_y], rmonTemp*42);
            h1_all_imon[tile->at(it)]->Set(h1_all_imon[tile->at(it)]->GetN()+1);
            h1_all_imon[tile->at(it)]->SetPoint(h1_all_imon[tile->at(it)]->GetN()-1, h1_all_imon[tile->at(it)]->GetN()-1, imonTemp - (av_dc + mean_dc[tile->at(it)]));
            //std::cout<<pos[0]<<", "<<pos[1]<<": "<< tile->at(it)<<" = "<<imonTemp<<endl;
        }
      }//tile
      if (last_pos[0] != pos[0] || last_pos[1] != pos[1]){
        last_pos[0] = pos[0];
        last_pos[1] = pos[1];
        vec_xpos.push_back(last_pos[0]);
        vec_ypos.push_back(last_pos[1]);

      }
    }
    if (normalize){
        for (int i = 0; i < NTILE; i++){
          double s = h1_tile_response[i]->GetBinContent(h1_tile_response[i]->GetMaximumBin());
          h1_tile_response[i]->Scale(1./s);
        }
    }

    TH1D *h_blank = new TH1D("h_blank", ";x [cm]; <I>_{sig} - <I>_{dark}", nsteps, 4, 88);
    h_blank->SetMaximum(max_y);
    h_blank->SetMinimum(-0.1);
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
    int oo = 0;
    int ch_1  = channels.at(0);
    int ch_2 = channels.at(n_channels - 1);
    for (int i = 0; i < NTILE; i++){
      int ns = 0;
      for (std::vector<int>::iterator it = channels.begin(); it != channels.end(); ++it){
        if (i == *it) ns++;
      }
      if (ns == 0) continue;
      if (oo == 0){
        h1_tile_response[i]->SetTitle(";x [cm]; <I>_{sig} - <I>_{dark}");
        h1_tile_response[i]->SetLineColor(colors16[0]);
        h1_tile_response[i]->SetLineWidth(2);
        h1_tile_response[i]->SetMaximum(max_y);
        h1_tile_response[i]->SetMinimum(0);
        h1_tile_response[i]->Draw("hist");
      }
      else{
        h1_tile_response[i]->SetLineColor(colors16[oo]);
        h1_tile_response[i]->SetLineWidth(2);
        h1_tile_response[i]->Draw("hist same");
      }
      oo++;
    }
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText(Form("sEPD %s Crosstalk Test", sector_addon),xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Channels %d to %d",ch_1, ch_2),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    //drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);

    c_full->SaveAs(Form("%s%s.png", save_dir_plot.c_str(), fname.c_str()));
    c_full->SaveAs(Form("%s%s.pdf", save_dir_plot.c_str(), fname.c_str()));

    FillCrossTalk(h2D_crosstalk, h1_tile_response, ch_1, ch_2);

    TFile *out_hist_file = new TFile(Form("%s%s_hists.root", save_dir_root.c_str(), fname.c_str() ), "recreate");
    for (int i = 0; i < NTILE; i++){
      int ns = 0;
      int oo = 0;
      for (std::vector<int>::iterator it = channels.begin(); it != channels.end(); ++it){
        if (i == *it) ns++;
      }
      if (ns == 0) continue;
      h1_tile_dc[i]->Write();
      h1_RMS_scan[i]->Write();
      h1_RMS_on[i]->Write();
      h1_RMS_off[i]->Write();
      h1_tile_response[i]->Write();
      h2_tile_rmon_imon[i]->Write();
      h1_tile_rmon[i]->Write();
      h1_tile_rmon_1[i]->Write();
      h1_all_rmon[i]->Write();
      h1_all_imon[i]->Write();
    }
    out_hist_file->Close();
    if (debug) cout<<"End file "<<ff<<endl;
  }

  TCanvas *c_cross = new TCanvas("c_cross","",1000, 1000);
  gPad->SetLogz();
  for (int i = 1; i< 32; i++){
    h2D_crosstalk->Fill(i, i, 1);
  }
  h2D_crosstalk->SetTitle(";Read Channel; Source Over Channel");
  h2D_crosstalk->Draw("colz");
  c_cross->SaveAs(Form("%s/crosstalk_test.png", save_dir_plot.c_str()));
  c_cross->SaveAs(Form("%s/crosstalk_test.pdf", save_dir_plot.c_str()));

  TFile *out_hist_file = new TFile(Form("%scrosstalk_hist.root", save_dir_root.c_str()), "recreate");
  h2D_crosstalk->Write();
  out_hist_file->Close();
  return;
}

int Line_Test_Analysis(const std::string &config_file= "line_config.config")
{
  bool debug = false;
  if (debug) cout<< "In the code... Getting config file"<<endl;
// Input configuration file
  TEnv *config_p = new TEnv(config_file.c_str());
  const int sector = config_p->GetValue("SECTOR", 0);
  const int all_runs = config_p->GetValue("ALLRUNS", 1);
  const int ch1 = config_p->GetValue("CHANNEL1", 0);
  const int ch2 = config_p->GetValue("CHANNEL2", 0);

  if (debug) cout<<"Setting sector header for sector "<<sector<<endl;
  char *sec_head = new char[10];
  if (sector < 10) sprintf(sec_head, "s0%d", sector);
  else sprintf(sec_head,"s%d", sector);

  const std::string data_dir = "../data/" + std::string(sec_head);
  const std::string save_dir_root = "../Results/"+std::string(sec_head)+"/root_hist/";
  const std::string save_dir_plot = "../Results/"+std::string(sec_head)+"/plots/";
  const std::string save_dir_raw = "../Results/"+std::string(sec_head)+"/root_raw/";

  std::vector<std::string> filenames;
  std::string test_type = "line";

  if (debug) cout<<"Getting File names for test: "<<test_type<<endl;

  GetFileName(filenames, test_type, sector, all_runs, ch1, ch2, debug);

  if (debug) cout<<"Got File names, going to analyze..."<<endl;
  if (debug) {
    for ( std::vector<std::string>::iterator it = filenames.begin(); it != filenames.end();++it ){
      cout<<*it<<endl;
    }
  }

  // Now we have out file names

  // analyze the data
  Analyze(filenames, sector, data_dir, save_dir_raw, save_dir_plot, save_dir_root, ch1, ch2, debug);

  return 1;
}
