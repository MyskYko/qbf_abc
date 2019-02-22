#ifndef TEMPLATE_DATA_H
#define TEMPLATE_DATA_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class template_data {
private:
  int num_cycle = 0;
  
  int num_node = 0;
  std::vector<int> num_reg;
  
  std::vector<std::vector<int> > num_spx; // fdx is expressed by 2 spx.
  std::vector<std::vector<int> > num_hdx; // hdx[i][j], i < j.
  int num_com = 0;
  
  std::vector<std::vector<std::string> > initial_assignment;
  std::vector<std::vector<std::string> > final_assignment;
  
  std::string data;
  
public:
  int read_file(std::string filename);
  int gen_reg();
  int gen_com();
  int set_reg();
  int set_com();
  int set_out();
  int write_circuit(std::string filename);
  void print() { std::cout << data; }

  int setup(std::string filename);
  
};

#endif