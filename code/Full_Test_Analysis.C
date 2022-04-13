
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
#include "sEPD_Full_Functions.C"
// Line Scan Analysis code

void GetUniformity(TGraphErrors *g_uni, TH2D *h_imon[], bool debug = true){
  if (debug) cout<<"In Uniformity..."<<endl;
  int NTILE = 32;
  Tile *arr_tile[31];
  int cheese = TileInfo(arr_tile);
  int nx, ny, bin;
  double scale, value, err, norm;
  double hx, lx, hy, ly;
  double xloc, yloc;
  double min = 0.8;
  TProfile *h_uni_test = new TProfile("h_uni_test","", 31, 0.5, 31.5,"s");
  for(int i = 1; i < NTILE; i++){

    nx = h_imon[i]->GetNbinsX();
    ny = h_imon[i]->GetNbinsX();
    norm = h_imon[i]->GetBinContent(h_imon[i]->GetMaximumBin());
    hx = arr_tile[i-1]->xc + arr_tile[i-1]->xr;
    lx = arr_tile[i-1]->xc - arr_tile[i-1]->xr;
    hy = arr_tile[i-1]->yc + arr_tile[i-1]->yr;
    ly = arr_tile[i-1]->yc - arr_tile[i-1]->yr;

    if (debug)cout<<"Channel "<<i<<": nx = "<<nx<<", ny = "<<ny<<", norm = "<<norm<<endl;
    for (int j = 1; j <= nx; j++){
      for (int k = 1; k <= ny; k++){
        xloc = h_imon[i]->GetXaxis()->GetBinCenter(j);
        yloc = h_imon[i]->GetYaxis()->GetBinCenter(k);

        bin = h_imon[i]->GetBin(j, k);
        value = h_imon[i]->GetBinContent(bin);///norm;
        if (debug && xloc< hx && xloc > lx && yloc < hy && yloc > ly&& value/norm > min)cout<<"bin "<<bin<<": value = "<<value<<", value/norm = "<<value/norm<<endl;

        if (xloc< hx && xloc > lx && yloc < hy && yloc > ly&& value/norm > min) {
          //if (debug) cout<<"in it"<<endl;
          h_uni_test->Fill(i, value);
        }
      }
    }
    scale = h_uni_test->GetBinContent(i);
    err = h_uni_test->GetBinError(i);

    g_uni->SetPoint(i, i, 1);
    g_uni->SetPointError(i, 0,err/scale);

    if (debug)cout<<"bin "<<bin<<": scale= "<<scale<<"rms = "<< err<<endl;

  }
  g_uni->SetTitle(";tile; Uniformity");
  g_uni->SetMarkerStyle(22);
  g_uni->SetMarkerSize(2);
  g_uni->SetMarkerColor(kBlue);
  g_uni->GetHistogram()->SetMinimum(0.7);
  g_uni->GetHistogram()->SetMaximum(1.3);
  return;
}
void Analyze(std::string filename, const int sector, const std::string data_dir, const std::string save_dir_raw, const std::string save_dir_plot, const std::string save_dir_root, const int ch_1 = 0, const int ch_2 = 0, const int ch_3 = 0, const int ch_4 = 0, bool debug = true)
{

  gStyle->SetOptStat(0);
  SetyjPadStyle();
  int size = 1;
  std::vector<int> channels;
  int n_channels = 0;
  if (ch_1 > 0){
    GetChannels(filename, channels, debug);
    n_channels = channels.size();
  }
  std::string fname;
  fname = MakeRootFile_Full(filename, data_dir, save_dir_raw, n_channels, debug);
  char *sector_addon = new char[5];
  if (sector < 10) sprintf(sector_addon, "s0%d", sector);
  else sprintf(sector_addon,"s%d", sector);


  char *root_path = new char[100];
  sprintf(root_path, "%s%s.root", save_dir_raw.c_str(), fname.c_str());
  TFile* fin = new TFile(root_path, "read");
  if(!fin){
    cout<<"No File Found here"<<endl;
  }
  TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
  if(!inTree_p){
    cout<<"No TTree found here..."<<endl;
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
  GetParameters_Full(paramNames, params, data_path, data_dir, debug);
  if (debug) cout << "Done getting info: "<<params.size()<<endl;
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
    if (paramNames[i] == " nRepeat" || paramNames[i] == "nRepeat") nRep = stof(params[i]);

  }


  /////////////////////////////////////
  // DEFINE HISTOGRAMS
  const int nTILE = 32;

  TH1D* h1D_imon[nTILE];//imon dist for all source positions including dark current
  TH1D* h1D_imon_dc[nTILE];//imon dist for only for dark current

  const double xo = 3;
  const double yo = 0;
  const double xMin = 3;
  const double xMax = 95;
  const double yMin = 0.0;
  const double yMax = 49.0;
  double nSep_x = .5;//x bin width in cm
  double nSep_y = .5;//y bin width in cm
  //float nSep_y = 0.25;//y bin width in cm
  int nx = (xMax - xMin)/nSep_x;
  int ny = (yMax - yMin)/nSep_y;
  cout<<"bins: "<<nx<<" , "<<ny<<endl;
  TH2D* h2D_x_y_imon_all_norm = new TH2D(Form("h2D_x_y_imon_%s", "all_norm"), ";x [cm];y [cm]", nx, xMin, xMax, ny, yMin, yMax);

  TH2D* h2D_x_y_imon_all = new TH2D(Form("h2D_x_y_imon_%s", "all"), ";x [cm];y [cm]", nx, xMin, xMax, ny, yMin, yMax);
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
  for(int entry = 0; entry < 3; entry++){
    if(nEntries%nDiv == 0) std::cout << " Entry " << entry << "/" << nEntries << std::endl;
    inTree_p->GetEntry(entry);
    for(int it = 0; it<tile->size(); it++){
      double imonTemp = imon->at(it);
      //if(xpos==xo && ypos==yo){//for dark current
        h1D_imon_dc[tile->at(it)]->Fill(imonTemp);
    //  }
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

  if(debug) cout<<__LINE__<<": Going through event loop..."<<endl;
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
        h2D_x_y_imon[tile->at(it)]->Fill(xpos, ypos, imonTemp/3. - mean_dc[tile->at(it)]/3.);
      }
    }//tile
  }//event loop
  if(debug) cout<<__LINE__<<": All of the hists..."<<endl;

  // Loop through all the bins and get the max;
  int n_bins_x = h2D_x_y_imon_all->GetNbinsX();
  int n_bins_y = h2D_x_y_imon_all->GetNbinsY();

  int max_t;
  double max_v = 0;
  TH2D *h2d_max_tile = new TH2D("h2d_max_tile", "", nx, xMin, xMax, ny, yMin, yMax);
  int b;
  double v;
  double norm;
  for ( int i = 1 ; i <= n_bins_x; i++){
    for ( int j = 1; j <= n_bins_y; j++){
      max_v = 0;
      b = h2D_x_y_imon_all->GetBin(i, j);
      for ( int k = 1; k < 32; k++){
        v = h2D_x_y_imon[k]->GetBinContent(b);
        if(debug) cout<<"value: "<<v<<endl;

        if ( v > max_v){
          max_v = v;
          max_t = k;
          if(debug) cout<<"new max: "<<max_t<<endl;

        }
      }
      if (max_v == 0) continue;
      if(debug) cout<<"At normalizing..."<<endl;
      norm = h2D_x_y_imon[max_t]->GetBinContent(h2D_x_y_imon[max_t]->GetMaximumBin());
      h2d_max_tile->SetBinContent(b, max_t);
      h2D_x_y_imon_all->SetBinContent(b, max_v);
      h2D_x_y_imon_all_norm->SetBinContent(b, max_v/norm);

    }
  }

  if(debug) cout<<__LINE__<<": Drawing..."<<endl;

  /////////////////////////////////////
  // START DRAWING
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
  drawText(Form("sEPD s%d", sector),xPos,yPos-dy2,0, 1, fontSize, fontType);

  c_2d_2->SaveAs(Form("%s/hist2D_x_y_IMON_allChannel_%s.pdf",save_dir_plot.c_str(), sector_addon));
  c_2d_2->SaveAs(Form("%s/hist2D_x_y_IMON_allChannel_%s.png",save_dir_plot.c_str(), sector_addon));

  /////////////////////////////////////
  // DRAW PLOTS 2D IMON
  cout << "Plotting x-y imon weighted 2D histograms for each tile" << endl;

  TCanvas* c_2d_1 = new TCanvas("c_2d_1_tile","", 750,400);

  for(int it = 1; it < nTILE; it++){
    c_2d_1->SetRightMargin(0.13);
    c_2d_1->SetLeftMargin(0.10);
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
    drawText(Form("sEPD s%d", sector),xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Tile ##bf{%d}", it),xPos,yPos-dy2*2,0, 1, fontSize, fontType);
    drawText(Form("Dark Current %0.2f #mu A", mean_dc[it]),xPos,yPos-dy2*3,0, 1, fontSize, fontType);
    drawText("<I>_{scan} - <I>_{dark} w/ source at (x, y)",xPos,yPos2,0, 1, fontSize, fontType);

    c_2d_1->SaveAs(Form("%s/hist2D_x_y_IMON_tile%d_%s.pdf",save_dir_plot.c_str(),it, sector_addon));
    c_2d_1->SaveAs(Form("%s/hist2D_x_y_IMON_tile%d_%s.png",save_dir_plot.c_str(),it, sector_addon));
  }

  TCanvas *c_all = new TCanvas("c_all","c_all", 740, 400);
  c_all->SetRightMargin(0.13);
  c_all->SetLeftMargin(0.10);
  SetHistTextSize(h2D_x_y_imon_all);
  h2D_x_y_imon_all->SetTitleOffset(0.8, "Y");

  h2D_x_y_imon_all->Draw("colz");
//  draw_scan(0, xorigin, yorigin, rot);

  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText(Form("sEPD s%d", sector),xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("All tiles",xPos,yPos-dy2*2,0, 1, fontSize, fontType);
  drawText("<I>_{scan} - <I>_{dark} w/ source at (x, y)",xPos,yPos2,0, 1, fontSize, fontType);

  c_all->SaveAs(Form("%s/hist2D_x_y_IMON_all_%s.pdf",save_dir_plot.c_str(), sector_addon));
  c_all->SaveAs(Form("%s/hist2D_x_y_IMON_all_%s.png",save_dir_plot.c_str(), sector_addon));

  TCanvas *c_all_norm = new TCanvas("c_all_norm","c_all_norm", 740, 400);
  c_all_norm->SetRightMargin(0.13);
  c_all_norm->SetLeftMargin(0.10);
  SetHistTextSize(h2D_x_y_imon_all_norm);
  h2D_x_y_imon_all_norm->SetTitleOffset(0.8, "Y");

  h2D_x_y_imon_all_norm->Draw("colz");
  //  draw_scan(0, xorigin, yorigin, rot);

  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText(Form("sEPD %s", sector_addon),xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("All tiles Normalized",xPos,yPos-dy2*2,0, 1, fontSize, fontType);
  drawText("<I>_{scan} - <I>_{dark} w/ source at (x, y)",xPos,yPos2,0, 1, fontSize, fontType);

  c_all_norm->SaveAs(Form("%s/hist2D_x_y_IMON_all_norm_%s.pdf",save_dir_plot.c_str(), sector_addon));
  c_all_norm->SaveAs(Form("%s/hist2D_x_y_IMON_all_norm_%s.png",save_dir_plot.c_str(), sector_addon));


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
  cg->SaveAs(Form("%s/CompareHeights_%s.png",save_dir_plot.c_str(), sector_addon));

  TGraphErrors *h_uniformity = new TGraphErrors(31);
  GetUniformity(h_uniformity, h2D_x_y_imon);
  gStyle->SetOptTitle(0);
  TCanvas *c_uni = new TCanvas("c_uni","",500, 500);
  h_uniformity->Draw("AP");
  drawText("#bf{sPHENIX} #it{Internal}",xPos + 0.1,yPos,0, 1, fontSize+2, fontType);
  drawText("Uniformity Test",xPos + 0.1,yPos -dy2,0, 1, fontSize+2, fontType);
  drawText("RMS of tile responses above 0.5 #times max response",xPos + 0.1,yPos -2*dy2,0, 1, fontSize+2, fontType);

  c_uni->SaveAs(Form("%s/h_uniformity_%s.pdf",save_dir_plot.c_str(), sector_addon));
  c_uni->SaveAs(Form("%s/h_uniformity_%s.png",save_dir_plot.c_str(), sector_addon));

  TFile *out_hist_file = new TFile(Form("%s%s_hists.root", save_dir_root.c_str(), fname.c_str() ), "recreate");
  for (int i = 1; i < 32; i++){
    g_all_locs[i]->Write();
    h2D_x_y_imon[i]->Write();
    h1D_imon[i]->Write();
    h1D_imon_dc[i]->Write();
  }
  h2D_x_y_imon_all_norm->Write();
  h2D_x_y_imon_all->Write();
  out_hist_file->Close();

  TFile *health = new TFile(Form("%shealth_hist.root", save_dir_root.c_str(), fname.c_str() ), "recreate");
  h_uniformity->SetName("h_uniformity");
  h_uniformity->Write();
  h2D_x_y_imon_all_norm->Write();
  health->Close();
  return;
}

int Full_Test_Analysis(const std::string &config_file= "full_config.config")
{
// Input configuration file
  TEnv *config_p = new TEnv(config_file.c_str());
  const int sector = config_p->GetValue("SECTOR", 0);
  const int ch1 = config_p->GetValue("CHANNEL1", 0);
  const int ch2 = config_p->GetValue("CHANNEL2", 0);
  const int ch3 = config_p->GetValue("CHANNEL3", 0);
  const int ch4 = config_p->GetValue("CHANNEL4", 0);

  char *sector_addon = new char[10];
  if (sector < 10) sprintf(sector_addon, "s0%d", sector);
  else sprintf(sector_addon,"s%d", sector);

  const std::string data_dir = "../data/" + std::string(sector_addon);
  const std::string save_dir_root = "../Results/"+std::string(sector_addon)+"/root_hist/";
  const std::string save_dir_plot = "../Results/"+std::string(sector_addon)+"/plots/";
  const std::string save_dir_raw = "../Results/"+std::string(sector_addon)+"/root_raw/";

  bool debug = false;
  std::vector<std::string> filenames;
  int all_runs = 1;
  std::string test_type = "full";
  GetFileName(filenames, test_type, sector, all_runs, ch1, ch2, ch3, ch4, debug);
  std::string filename = filenames.at(0);
  cout<<"File: "<<filename<<endl;
  // Now we have out file names

  // analyze the data
  Analyze(filename, sector, data_dir, save_dir_raw, save_dir_plot, save_dir_root, ch1, ch2, ch3, ch4, debug);

  return 1;
}
