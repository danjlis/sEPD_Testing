
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
#include <dirent.h>
#include "yjUtility.h"


void GetRootFiles(std::vector<std::string> &filenames, const std::string save_dir_root, const int sector = 0, bool debug = true){


  char *sector_addon = new char[5];
  if (sector < 10) sprintf(sector_addon, "s0");
  else sprintf(sector_addon,"s");

  char *path = new char[100];
  sprintf(path,"../data/%s%d/", sector_addon, sector);
  // path of all the raw data filesss  sprintf(path,"../data/%s%d/", sector_addon, sector);
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

    date = ParseFileName(in_file, test_type, all_runs, ch1, ch2, ch3, ch4, debug);
    if (debug) cout<<"Date of File: "<<date<<endl;
    if (debug) cout<<"Still Looking at File: "<<in_file<<endl;

    if (date != "nothing here"){
      filenames.push_back(in_file);
      int date_int = integerify(date);
      dates.push_back(date_int);
      if (debug) cout<<"Found one"<<endl;

    }
  }
  if (debug) cout<<"Made it through..."<<endl;
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

void Make_Health_Sheet(const int &sector = 0){

  char *sec_head = new char[10];
  if (sector < 10) sprintf(sec_head, "s0%d", sector);
  else sprintf(sec_head,"s%d", sector);

  const std::string data_dir = "../data/" + std::string(sec_head);
  const std::string save_dir_root = "../Results/"+std::string(sec_head)+"/root_hist/";
  const std::string save_dir_plot = "../Results/"+std::string(sec_head)+"/plots/";
  const std::string save_dir_raw = "../Results/"+std::string(sec_head)+"/root_raw/";

  bool debug = true;
  std::vector<std::string> filenames;
  int all_runs = 1;
  GetRootFiles(filenames, save_dir_root, sector, debug);
  std::string filename = filenames.at(0);
  cout<<"File: "<<filename<<endl;
  // Now we have out file names

  // analyze the data
  Analyze(filename, sector, data_dir, save_dir_raw, save_dir_plot, save_dir_root, ch1, ch2, ch3, ch4, debug);


return;
}
