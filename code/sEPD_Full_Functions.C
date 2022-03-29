

std::string ParseFileName(std::string in_file, int all_runs, int ch_1, int ch_2, bool debug = false){
  char* sector = new char[100];
  char* c1 = new char[100];
  char* c2 = new char[100];

  char* date = new char[100];
  char* test = new char[100];

  std::string fname;
  strcpy(fname, in_file);
  sprintf(date, "nothing here");

  std::string delimiter = "/";
  size_t pos = 0;
  std::string token;
  while ((pos = fname.find(delimiter)) != std::string::npos) {
      token = fname.substr(0, pos);
      //std::cout << token << std::endl;
      fname.erase(0, pos + delimiter.length());
  }
  size_t pos2 = 0;
  int n = 0;
  std::string delimiter2 = "_";
  //cout<<fname<<endl;
  while ((pos2 = fname.find(delimiter2)) != std::string::npos) {
      token = fname.substr(0, pos2);
      if ( n == 2 ) strcpy(date, token.c_str());
      else if ( n == 3 ) strcpy(test, token.c_str());
      else if ( n == 1 ) strcpy(sector, token.c_str());
      else if ( n == 4 && ch1!=0) strcpy(c1, token.c_str());

      n++;
      fname.erase(0, pos2 + delimiter2.length());
  }
  std::string delimiter3 = ".";

  while ((pos = fname.find(delimiter3)) != std::string::npos) {
      token = fname.substr(0, pos);
      if (ch2 != 0) strcpy(c2, token.c_str());
      fname.erase(0, pos + delimiter3.length());
  }
  cout<<date<<endl;
  cout<<sector<<endl;
  cout<<test<<endl;
  cout<<c1<<endl;
  cout<<c2<<endl;

  std::string date_str = "nothing here";
  if (strncmp(test, "full", 4)) return date_str;

  if (all_runs == 1) {
    date_str = std::string(date);
    return date_str;
  }
  int c1_int = stoi(c1);
  int c2_int = stoi(c2);

  if (c1_int == ch_1 && c2_int == ch_2){
    date_str = std::string(date);
    return date_str;
  }

  return date_str;
;

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

void GetFileName(std::vector<std::string> *filenames, int sector, int all_runs, int ch1, int ch2, int ch3, int ch4, bool debug = false){
  // Define the sector addon string
  char *sector_addon = new char[5];
  if (sector < 10) sector_addon = "s0";
  else sector_addon = "s";


  // path of all the raw data files
  std::string path = Form("../data/%s%d/", sector_addon, sector);
  std::string in_file;
  std::vector<int> dates;

  struct dirent *entry = nullptr;
  DIR *dp = nullptr;

  dp = opendir(path);
  // if it is good or not
  bool good = false;
  // Get the date to compare to others.
  std::string date;
  while ((entry = readdir(dp))){
    in_file = entry->d_name();
    date = ParseFile(in_file, all_runs, ch1, ch2, ch3, CHANNEL4);
    if (strncmp(date,"nothing here", 11) != 0){
      filenames->push_back(in_file);
      int date_int = integerify(date);
      dates.push_back(date_int);
    }
  }
  if (paths.size() < 1) {
    cout<<"Nothing is here ..."<<endl;
  }

  if (all_runs == 0 && filenames->size() > 1) {
    int n = 0;
    int max = 0;
    for (std::vector<int>::iterator it = dates.begin(); it != dates.end(); ++it){
      if (*it > max) {
        max = *it;
        max_n = n;
      }
      n++;
    }
    for (int j = 0; j < filenames->size(); j++){
      if (j == n) continue;
      dates.erase(dates.begin() + j);
      filenames->erase(filenames->begin() + j);
    }
    if (filenames->size() > 1) cout<<"I give up... "<<endl;
  }
  dp->closedir(dp)
  return;
}

std::string MakeRootFile_Line(std::string inputFileName, const std::string datadir, const std::string savedir, int ttt = 32, bool debug = false){

  cout << "RUNNING: make_sEPD_rootfile_v1 for " << inputFileName << " input file" << endl;
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
  ifstream infile(inputFileName);
  Float_t xpos, ypos;
  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile;
  vector<Float_t> imon, rmon, vcomp;

  std::string last_part;

  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TFile* fout = new TFile(rootname, "recreate");
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
  std::string cc1, cc2;
  std::string delim = ".";
  std::string token;
  size_t pos;
  int n = 0;
  std::string delim2 = "_";

  while ((pos = fname.find(delim)) != std::string::npos) {
    fname = fname.substr(0, pos);
  }
  pos = 0;
  delim = "_";
  while ((pos = fname.find(delim)) != std::string::npos) {
      token = fname.substr(0, pos2);
      if ( n == 4 ) strcpy(cc1, token.c_str());
      else if ( n == 5) strcpy(cc2, token.c_str());
      n++;
      fname.erase(0, pos + delim.length());
  }

  c1 = stoi(cc1);
  c2 = stoi(cc2);

  if (c1 == 1){
    int c = c2;
    while (c - c1 > 0){
      channels.push_back(c);
      c - 2;
    }
  }
  else if ((c1%2 == 0 && c2%2==1) || (c1%2 == 1 && c2%2 == 0){
    channels.push_back(c1);
    channels.push_back(c2);
  }

  return;
}


void GetParameters(vector<string> &paramNames, vector<string> &params, std::string inputFileName, std::string datadir, bool debug = false){

  string path = datadir + inputFileName;

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
    }
    while (sP){
      string stemp;
      if (!getline( sP, stemp, ',')) break;
      params.push_back( stemp );
    }
  }
}
