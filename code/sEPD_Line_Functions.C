// Functions for the Line Test Analysis
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
#include <dirent.h>

void FillCrossTalk(TH2D *h2D_crosstalk, TProfile *h1_tile_response[], int ch_1, int ch_2){
  bool debug = false;
  if (debug) cout<<"Crosstalking... "<<ch_1<<"_"<<ch_2<<endl;
  double integral[32] = {0};
  int NTILES = 32;
  int channels[16];
  double binlength[16];// = {0};
  int size;
  if (abs(ch_1 - ch_2) == 1){
    channels[0] = ch_1;
    channels[1] = ch_2;
    size = 2;
  }
  else {
    int c = ch_1;
    int n = floor((ch_1 - ch_2 + 1)/2.);
    size = n+1;
    while (c > ch_2){
      channels[n] = c;
      c = c - 2;
      n = n - 1;
    }
    channels[0] = ch_2;
  }

  if (debug){
    cout<<"channels: ";
    for (int i = 0; i < size; i++) cout<<channels[i]<<" ";
    cout<<" ."<<endl;
  }
  double norms[32];
  double ref[32] = {0};
  double min = 0.2;
  double v_ref;
  for ( int i = 0; i < size; i++){
    norms[channels[i]] = h1_tile_response[channels[i]]->GetBinContent(h1_tile_response[channels[i]]->GetMaximumBin());
    for ( int j = 1; j <= h1_tile_response[channels[i]]->GetNbinsX(); j++){
      v_ref =  h1_tile_response[channels[i]]->GetBinContent(j);
      if (v_ref > min*norms[channels[i]]) {
        ref[channels[i]] += v_ref;
        binlength[i] = binlength[i] + 1.;
      }
    }
  }
  int nbins;
  if (debug) cout<<"Ending the crosstalk with: "<<endl;
  for (int i = 0; i < size;i++){
    for(int j = 0; j < size;j++){
      if (i == j) continue;
      nbins = h1_tile_response[channels[i]]->GetNbinsX();
      for (int b = 1; b <= nbins; b++){
        double max = 0;
        double max_i = 0;
        double v;
        for (int k = 1; k < NTILES;k++){
          v = h1_tile_response[k]->GetBinContent(b);
          if (v > max){
            max = v;
            max_i = k;
          }
        }
        if (max_i != channels[j]) continue;
        if (max < min*norms[channels[j]]) continue;

        h2D_crosstalk->Fill(channels[i], channels[j], (binlength[i]/binlength[j])*(h1_tile_response[channels[i]]->GetBinContent(b)/ref[channels[i]]));
      }
        if (debug) cout<<"Reading "<<channels[i]<<" over "<<channels[j]<<": "<<h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(channels[i], channels[j]))<<endl;
    }

  }



}


std::string ParseFileName(std::string in_file, std::string test_type, int all_runs, int ch_1, int ch_2, bool debug = false){
  char* sector = new char[100];
  char* c1 = new char[100];
  char* c2 = new char[100];
  char* date = new char[100];
  char* test = new char[100];

  std::string fname = in_file;
  //strcpy(fname, in_file);
  sprintf(date, "nothing here");

  std::string delimiter = "/";
  size_t pos = 0;
  std::string token;
  while ((pos = fname.find(delimiter)) != std::string::npos) {
      token = fname.substr(0, pos);

      if (debug) std::cout << token << std::endl;
      fname.erase(0, pos + delimiter.length());
  }
  size_t pos2 = 0;
  int n = 0;
  std::string delimiter2 = "_";
  //cout<<fname<<endl;
  while ((pos2 = fname.find(delimiter2)) != std::string::npos) {
      token = fname.substr(0, pos2);
      if (debug) std::cout << "Token "<<n<<" : -"<< token << "-"<<std::endl;

      if ( n == 2 ) strcpy(sector, token.c_str());
      else if ( n == 3 ) strcpy(test, token.c_str());
      else if ( n == 1 ) strcpy(date, token.c_str());
      else if ( n == 4 && all_runs==0) strcpy(c1, token.c_str());

      n++;
      fname.erase(0, pos2 + delimiter2.length());
  }
  std::string delimiter3 = ".";

  while ((pos = fname.find(delimiter3)) != std::string::npos) {
      token = fname.substr(0, pos);
      if (all_runs == 0) strcpy(c2, token.c_str());
      fname.erase(0, pos + delimiter3.length());
  }
  delimiter3 =  "h";
  std::string c11 = std::string(c1);

  while ((pos = c11.find(delimiter3)) != std::string::npos) {
      token = c11.substr(0, pos);
      //if (all_runs == 0) strcpy(c1, token.c_str());
      c11.erase(0, pos + delimiter3.length());
  }
  strcpy(c1, c11.c_str());
  if (debug) {
    cout<<"In_file: "<<in_file<<endl;
    cout<<"Date: "<<date<<endl;
    cout<<"Sector: "<<sector<<endl;
    cout<<"Test: "<<test<<endl;
    cout<<"channel 1: "<<c1<<endl;
    cout<<"channel 2: "<<c2<<endl;
  }

  std::string date_str = "nothing here";
  if (test != test_type) {
    if (debug) cout<< "Not the "<< test_type << " ..."<<endl;
    return date_str;
  }
  if (all_runs == 1) {
    if (debug) cout<< "Accepting all runs ..."<<endl;
    date_str = std::string(date);
    return date_str;
  }
  int c1_int = stoi(c1);
  int c2_int = stoi(c2);

  if (c1_int == ch_1 && c2_int == ch_2){
    if (debug) cout<< " Found it!"<<endl;
    date_str = std::string(date);
    return date_str;
  }

  return date_str;
}

int integerify(std::string date, bool debug = false){
  std::string delimiter = "-";
  std::string token;
  std::string smooth_date = "";
  size_t pos = 0;

  while ((pos = date.find(delimiter)) != std::string::npos) {
      token = date.substr(0, pos);
      smooth_date = smooth_date + token;
      date.erase(0, pos + delimiter.length());
  }

  int i = stoi(smooth_date);
  return i;
}

void GetFileName(std::vector<std::string> &filenames, std::string test_type, int sector, int all_runs, int ch1, int ch2, bool debug = false){
  // Define the sector addon string
  char *sector_addon = new char[5];
  if (sector < 10) sprintf(sector_addon, "s0");
  else sprintf(sector_addon,"s");

  // path of all the raw data files
  char *path = new char[100];
  sprintf(path,"../data/%s%d/", sector_addon, sector);
  std::string in_file;
  std::vector<int> dates;

  if (debug) cout<<"Setting entry"<<endl;
  struct dirent *entry = nullptr;
  if (debug) cout<<"making directory..."<<endl;
  DIR *dp = nullptr;

  if (debug) cout<<"Opening directory at ... "<<path<<endl;
  dp = opendir(path);
  // if it is good or not
  bool good = false;
  // Get the date to compare to others.
  std::string date;
  while ((entry = readdir(dp))){
    in_file = entry->d_name;
    if (debug) cout<<"Looking at file ... "<<in_file<<"... Parsing now"<<endl;

    date = ParseFileName(in_file, test_type, all_runs, ch1, ch2, debug);
    if (debug) cout<<"Date of File: "<<date<<endl;
    if (debug) cout<<"Still Looking at File: "<<in_file<<endl;

    if (date != "nothing here"){
      filenames.push_back(in_file);
      int date_int = integerify(date);
      dates.push_back(date_int);
    }
  }
  if (dates.size() < 1) {
    cout<<"Nothing is here ..."<<endl;
  }

  if (all_runs == 0 && filenames.size() > 1) {
    int n = 0;
    int max = 0;
    int max_n = 0;
    for (std::vector<int>::iterator it = dates.begin(); it != dates.end(); ++it){
      if (*it > max) {
        max = *it;
        max_n = n;
      }
      n++;
    }
    for (int j = 0; j < filenames.size(); j++){
      if (j == n) continue;
      dates.erase(dates.begin() + j);
      filenames.erase(filenames.begin() + j);
    }
    if (filenames.size() > 1) cout<<"I give up... "<<endl;
  }
  closedir(dp);
  return;
}
/*
std::string MakeRootFile_Full(std::string inputFileName, const std::string datadir, const std::string savedir, int ttt = 32, bool debug = false){
  cout << "RUNNING: make_sEPD_rootfile_v3 for " << inputFileName << " input file" << endl;


  std::string path = datadir + "/" + inputFileName;
  std::string rootname = inputFileName;
  std::string delimiter = ".";
  size_t pos = 0;
  std::string token;
  while ((pos = rootname.find(delimiter)) != std::string::npos) {
      rootname = rootname.substr(0, pos);
  }
  std::string fname = rootname;
  rootname = savedir + "/" + rootname + ".root";
  ifstream infile(Form("/%s%s", inputFileName);
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
*/
std::string MakeRootFile_Line(std::string inputFileName, const std::string datadir, const std::string savedir, int ttt = 32, bool debug = false){

 if (debug) cout << "RUNNING: make_sEPD_rootfile_v1 for " << inputFileName << " input file" << endl;
  if (debug) {
    cout<<"Printing Parameters: "<<endl;
    cout<<"datadir: "<<datadir<<endl;
    cout<<"savedir: "<<savedir<<endl;
    cout<<"ttt: "<<ttt<<endl;
  }
  std::string path = datadir + "/" + inputFileName;
  std::string rootname = inputFileName;
  std::string delimiter = ".";
  size_t pos = 0;
  std::string token;
  while ((pos = rootname.find(delimiter)) != std::string::npos) {
      rootname = rootname.substr(0, pos);
  }
  std::string fname = rootname;
  rootname = savedir + rootname + ".root";
  ifstream infile(path);

  Float_t xpos, ypos;
  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile;
  vector<Float_t> imon, rmon, vcomp;

  std::string last_part;
  if (debug) {
    cout<<"Printing paths: "<<endl;
    cout<<"datapath: "<<path<<endl;
    cout<<"savedir: "<<rootname<<endl;
  }
  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TFile* fout = new TFile(rootname.c_str(), "recreate");
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

  while (infile){
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
      if (debug){
      //  cout<<"sample Imon: "<<vec_str[6]<<endl;
      }
      tr->Fill();
      device.clear();
      ch.clear();
      tile.clear();
      imon.clear();
      rmon.clear();
      vcomp.clear();
    }
  }

  if (debug) cout << "total numer of lines analyzed = " << nLines << endl;
  fout->cd();
  tr->Write("", TObject::kOverwrite);
  //delete tr;
  fout->Close();
  if (debug) cout << "DONE: " <<  rootname << " has been created" << endl;

  return fname;
}


void PrintChannels(std::vector<int> &channels){
  cout<< "Channels in this scan: ";
  for (std::vector<int>::iterator it = channels.begin(); it != channels.end(); ++it){
    cout<<*it<<" ";
  }
  cout<<"."<<endl;
}

void GetChannels(std::string fname, std::vector<int> &channels, bool debug = false){

  int c1, c2;
  char *cc1 = new char[10];
  char *cc2 = new char[10];
  std::string delim = ".";
  std::string token;
  size_t pos;
  int n = 0;
  std::string delim2 = "_";

  while ((pos = fname.find(delim)) != std::string::npos) {
    fname = fname.substr(0, pos);
    if (debug) cout<<"filename: "<<fname <<endl;

  }
  pos = 0;
  delim = "_";
  while ((pos = fname.find(delim)) != std::string::npos) {
      token = fname.substr(0, pos);
      if (debug) cout<<"Token "<<n<<" : "<<token<<endl;
      if ( n == 4 ) sprintf(cc1, "%s", token.c_str());
      n++;
      fname.erase(0, pos + delim.length());
  }
  if (debug) {
    cout<<"c1 : "<<cc1<<endl;
    cout<<"c2 : "<<fname<<endl;
  }
  std::string delimiter3 =  "h";
  std::string c11 = std::string(cc1);

  while ((pos = c11.find(delimiter3)) != std::string::npos) {
      token = c11.substr(0, pos);
      if (debug) cout<<"Token "<<": "<<token<<endl;

      //if (all_runs == 0) strcpy(c1, token.c_str());
      c11.erase(0, pos + delimiter3.length());
  }
  strcpy(cc1, c11.c_str());
  if (debug) {
    cout<<"c1 : "<<cc1<<endl;
    cout<<"c2 : "<<fname<<endl;
  }
  c1 = stoi(cc1);
  c2 = stoi(fname);
  if (debug) cout<<"Adding channels..."<<endl;

  if (c1 == 1){
    int c = c2;
    while (c - c1 > 0){
      channels.push_back(c);
      if (debug) cout<<c<<",";

      c = c - 2;
    }
    channels.push_back(c1);
  }
  else if ((c1%2 == 0 && c2%2==1) || (c1%2 == 1 && c2%2 == 0)){
    channels.push_back(c1);
    channels.push_back(c2);
    if (debug) cout<<"Adding channels..."<<endl;
  }
  if (debug) cout<<"... done."<<endl;

  return;
}


void GetParameters_Line(vector<string> &paramNames, vector<string> &params, std::string inputFileName, std::string datadir, bool debug = false){

  string path = datadir + inputFileName;

  if (debug) cout<<"Path: "<<path<<endl;
  ifstream infile(path);

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
      if (debug) cout<<"name: "<<stemp<<" ; ";

    }
    while (sP){
      string stemp;
      if (!getline( sP, stemp, ',')) break;
      params.push_back( stemp );
      if (debug) cout<<"param: "<<stemp<<endl;

    }
  }
}
