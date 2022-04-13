
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
#include <dirent.h>
#include "yjUtility.h"

void Analyze(const int sector, const std::string data_dir, const std::string save_dir_raw, const std::string save_dir_plot, const std::string save_dir_root, bool debug = true){
  char *sec_head = new char[10];
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  if (sector < 10) sprintf(sec_head, "s0%d", sector);
  else sprintf(sec_head,"s%d", sector);

  TFile *in_Line = new TFile(Form("%sLine_Test_hists.root", save_dir_root.c_str()), "read");
  TH2D *h_cross = (TH2D*) in_Line->Get("h2D_crosstalk");
  TH1D *h_short_o = (TH1D*) in_Line->Get("h1_tile_response_16_17_ch17");
  TH1D *h_short_e = (TH1D*) in_Line->Get("h1_tile_response_16_17_ch16");

  TH1D *h1_tile_response[32];
  h1_tile_response[0] = (TH1D*) in_Line->Get("h1_tile_response_30_1_ch1");
  for (int i = 1; i < 32;i++){
    if (i%2 == 0) h1_tile_response[i] = (TH1D*) in_Line->Get(Form("h1_tile_response_30_1_ch%d", i));
    else h1_tile_response[i] = (TH1D*) in_Line->Get(Form("h1_tile_response_31_1_ch%d", i));
  }

  TFile *in_Full = new TFile(Form("%shealth_hist.root", save_dir_root.c_str()), "read");
  TGraphErrors *h_uniformity = (TGraphErrors*) in_Full->Get("h_uniformity");
  TH2D *h_all_norm = (TH2D*) in_Full->Get("h2D_x_y_imon_all_norm");

  TCanvas* c_health_sheet = new TCanvas("c_health_sheet","", 1000, 1000);
  TPad *top_name = new TPad("top_name", "", 0.0, 0.9, 1.0, 1.0);
  top_name->Draw();
  TPad *top_right = new TPad("top_right", "",0.5, 0.6, 1.0, 0.9);
  top_right->Draw();
  TPad *top_left = new TPad("top_left", "",0.0, 0.6, 0.5, 0.9);
  top_left->Draw();
  TPad *mid_right = new TPad("mid_right", "",0.5, 0.3, 1.0, 0.6);
  mid_right->Draw();
  TPad *mid_left = new TPad("mid_left", "",0, 0.3, 0.5, 0.6);
  mid_left->Draw();
  TPad *bot_right = new TPad("bot_right", "",0.5, 0, 1.0, 0.3);
  bot_right->Draw();
  TPad *bot_left = new TPad("bot_left", "",0, 0, 0.5, 0.3);
  bot_left->Draw();

  int colors16[16] = {kBlack, kRed+1, kBlue+1, kGreen-3, kCyan+1, kOrange -3, kMagenta -3, kGray, kRed -1, kBlue -1, kGreen -7, kCyan - 3, kOrange+1, kMagenta -5, kViolet+6, kAzure +3};
  float xPos = 0.13;
  //float xPos = 0.85;
  float yPos = 0.82;
  float yPos2 = 0.2;
  float dy = 0.04;
  float dy2 = 0.05;
  float fontType = 43;
  float fontSize = 13;
  double max_y = 1.2;

  top_name->cd();
  TText *t = new TText(0.33, 0.5, Form("%s Health Sheet", sec_head));
  t->SetTextSize(0.5);
  t->Draw();
  top_left->cd();
  h_all_norm->Draw("colz");
  //  draw_scan(0, xorigin, yorigin, rot);

  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText(Form("sEPD %s", sec_head),xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("All tiles Normalized",xPos,yPos-dy2*2,0, 1, fontSize, fontType);
  drawText("<I>_{scan} - <I>_{dark} w/ source at (x, y)",xPos,yPos2,0, 1, fontSize, fontType);


  top_right->cd();
  top_right->SetGridx();
  top_right->SetGridy();
  xPos = 0.06;
  //float xPos = 0.85;
  yPos = 0.82;
  yPos2 = 0.2;
  dy = 0.04;
  dy2 = 0.05;
  fontType = 43;
  fontSize = 13;
   max_y = 1.2;

  h_uniformity->Draw("AP");
  drawText("#bf{sPHENIX} #it{Internal}",xPos + 0.1,yPos,0, 1, fontSize+2, fontType);
  drawText("Uniformity Test",xPos + 0.1,yPos -dy2,0, 1, fontSize+2, fontType);
  drawText("RMS of tile responses above 0.5 #times max response",xPos + 0.1,yPos -2*dy2,0, 1, fontSize+2, fontType);


  mid_left->cd();

  xPos = 0.13;
  //float xPos = 0.85;
  yPos = 0.82;
  yPos2 = 0.2;
  dy = 0.04;
  dy2 = 0.05;
  fontType = 43;
  fontSize = 13;
   max_y = 1.35;
  for (int i = 0; i < 16; i++){
    if(i == 0){
      h1_tile_response[2*i]->SetTitle(";x [cm]; <I>_{sig} - <I>_{dark}");
      h1_tile_response[2*i]->SetLineColor(colors16[i]);
      h1_tile_response[2*i]->SetLineWidth(2);
      h1_tile_response[2*i]->SetMaximum(max_y);
      h1_tile_response[2*i]->SetMinimum(0);
      h1_tile_response[2*i]->Draw("hist");
    }
    else{
      h1_tile_response[2*i]->SetLineColor(colors16[i]);
      h1_tile_response[2*i]->SetLineWidth(2);
      h1_tile_response[2*i]->Draw("hist same");
    }
  }
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText(Form("sEPD %s Crosstalk Test", sec_head),xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("Channels 1 to 30",xPos,yPos-2*dy2,0, 1, fontSize, fontType);

  mid_right->cd();
  for (int i = 0; i < 16; i++){
    if(i == 0){
      h1_tile_response[2*i+1]->SetTitle(";x [cm]; <I>_{sig} - <I>_{dark}");
      h1_tile_response[2*i+1]->SetLineColor(colors16[i]);
      h1_tile_response[2*i+1]->SetLineWidth(2);
      h1_tile_response[2*i+1]->SetMaximum(max_y);
      h1_tile_response[2*i+1]->SetMinimum(0);
      h1_tile_response[2*i+1]->Draw("hist");
    }
    else{
      h1_tile_response[2*i+1]->SetLineColor(colors16[i]);
      h1_tile_response[2*i+1]->SetLineWidth(2);
      h1_tile_response[2*i+1]->Draw("hist same");
    }
  }
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText(Form("sEPD %s Crosstalk Test", sec_head),xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("Channels 1 to 31",xPos,yPos-2*dy2,0, 1, fontSize, fontType);
  bot_left->cd();

  h_cross->Draw("colz");

  bot_right->cd();
  h_short_o->SetMaximum(max_y);
  h_short_o->Draw("hist");
  h_short_e->Draw("same hist");
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText(Form("sEPD %s Crosstalk Test", sec_head),xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("Channels 16 to 17",xPos,yPos-2*dy2,0, 1, fontSize, fontType);

  c_health_sheet->SaveAs(Form("../Results/Health_Sheets/%s_Health_Sheet.pdf", sec_head));
  c_health_sheet->SaveAs(Form("%s%s_Health_Sheet.pdf", save_dir_plot.c_str(), sec_head));

  return;
}

void Make_Health_Sheet(const int &sector = 1){

  char *sec_head = new char[10];
  if (sector < 10) sprintf(sec_head, "s0%d", sector);
  else sprintf(sec_head,"s%d", sector);

  const std::string data_dir = "../data/" + std::string(sec_head);
  const std::string save_dir_root = "../Results/"+std::string(sec_head)+"/root_hist/";
  const std::string save_dir_plot = "../Results/"+std::string(sec_head)+"/plots/";
  const std::string save_dir_raw = "../Results/"+std::string(sec_head)+"/root_raw/";

  // Now we have out file names
  bool debug = true;
  // analyze the data
  Analyze(sector, data_dir, save_dir_raw, save_dir_plot, save_dir_root, debug);


return;
}
