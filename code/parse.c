#include <stdio.h>
#include <string.h>
#include <filesystem>

namespace fs = std::filesystem;
int parse(){

  // path of all the raw data files
  std::string path = "../data/";


  // if it is good or not

  for (const auto & entry : fs::directory_iterator(path)){
    std::string in_file = entry.path();
    cout<<in_file<<endl;
  }
  return 1;
}
/*
int parse()
{
  char* sector = new char[100];
  char* c1 = new char[100];
  char* c2 = new char[100];
  int all_runs = 0;
  std::string fname = "dir/s01/sEPD_s01_2022-0316-120000_Line_12_13.txt";

  char* date = new char[100];
  sprintf(date, "nothing here");
  char* test = new char[100];

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
  cout<<fname<<endl;
  while ((pos2 = fname.find(delimiter2)) != std::string::npos) {
      token = fname.substr(0, pos2);
      if ( n == 2 ) strcpy(date, token.c_str());
      else if ( n == 3 ) strcpy(test, token.c_str());
      else if ( n == 1 ) strcpy(sector, token.c_str());
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
  std::string ds = "hello";
  ds = "nothing here";
  cout<<ds<<endl;
  cout<<sector<<endl;
  cout<<test<<endl;
  cout<<c1<<endl;
  cout<<c2<<endl;

  cout<<fname<<endl;
  return 1;
}
*/
