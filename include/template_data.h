#ifndef TEMPLATE_DATA_H
#define TEMPLATE_DATA_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class template_data {
private:
  int num_cycle;
  
  int num_node;
  std::vector<int> num_reg;
  
  std::vector<std::vector<int> > num_spx; // fdx is expressed by 2 spx.
  std::vector<std::vector<int> > num_hdx; // hdx[i][j], i < j.
  
  std::vector<std::vector<std::string> > initial_assignment;
  std::vector<std::vector<std::string> > final_assignment;

  int flag_onesendrecv_spx;
  int flag_onehot_spx_in;
  int flag_onehot_spx_out;
  int flag_onehot_spx_between;
  int flag_implicit_reg;
  int flag_systolic;
  std::string data;

  void reset();
  int check_setting();
  int read_file(std::string filename);
  int gen_reg();
  int gen_com();
  int set_reg();
  int set_com();
  int set_out();
  int set_onesendrecv_spx();
  int set_onehot_spx_in();
  int set_onehot_spx_out();
  int set_onehot_spx_between();
  
public:
  int write_circuit(std::string filename);
  int write_spec(std::string filename);
  void print() { std::cout << data; }

  int setup(std::string filename);
  
};

#endif
