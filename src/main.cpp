#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <time.h>
#include <cstdlib>
#include <spec_data.h>
#include <impl_data.h>
#include <top_data.h>

// spec.blif ... spec file.
// impl.blif ... file to implement.

//  - assertion
// You cannot use same name model and same name internal signal in top model.
// "#." is used for description of structure. Simple way to avoid error is use double hash("##") when you want to comment out something.
// You should designate top module by "#.top module_name"

//  - impl.blif
// File has blank wire whose name begins with "x".
// File has "#.candidates wire0 wire1 ... x". 
// One of those candidates will be assigned to x.

// "#.group x3 x2 x1 x0" assignes "x"s to the candidate of same place in "#.candidates".
// "#.onehot wire_a" means wire_a shoud be used for one x. You can also call this like "#.onehot wire_a wire_b".
// "#.zeroonehot wire_a" means wire_a shoud be used for only one x or nothing. You can also call this like "#.zeroonehot wire_a wire_b".

// 0=sat, 1=unsat, -1=error
int main(int argc, char **argv) {

  // prepare to read file
  if(argc < 4) {
    std::cout << "./a.out spec.blif impl.blif output.blif (flag_show_detail)" << std::endl;
    return -1;
  }

  bool flag_show_detail = (argc > 4);

  clock_t start = clock();

  // setup
  spec_data spec;
  impl_data impl;
  top_data top;
  if(
     spec.setup(argv[1]) ||
     impl.setup(argv[2]) ||
     top.setup(impl.copy_of_candidate_selection_signals(), impl.copy_of_x_names(), impl.copy_of_x_selection_signals(), impl.copy_of_onehot_candidate_names(), impl.copy_of_zeroonehot_candidate_names(), spec.copy_of_outputs(), spec.copy_of_inputs(), impl.copy_of_all_selection_signals(), spec.get_top_name(), impl.get_top_name())
     )
    return -1;
  
  if(flag_show_detail) {
    spec.show_detail(); 
    impl.show_detail();
    top.show_detail();
  }
  
  // write tmp
  std::string tmp_file_name = "__tmp_top.blif"; // "__tmp_" + std::string(argv[3]) + ".blif";
  std::ofstream tmp_file;
  tmp_file.open(tmp_file_name, std::ios::out);
  if(!tmp_file) {
    std::cout << "can't write tmp file" << std::endl;
    return -1;
  }
  top.write_circuit(&tmp_file);
  spec.write_circuit(&tmp_file);
  impl.write_circuit(&tmp_file);
  tmp_file.flush();
  
  // solve
  impl.show_simple();
  std::string log_file_name = "__log.txt"; // "__log_" + std::string(argv[3]) + ".txt";
  std::string command = "abc -c \"read " + tmp_file_name + "; strash; qbf -v -P " + std::to_string(top.copy_of_selection_signals().size()) + ";\" > " + log_file_name;
  system(command.c_str());
  
  // get result
  if(impl.read_result(log_file_name)) {
    clock_t end = clock();
    double runtime = (double)(end - start) / CLOCKS_PER_SEC + impl.get_runtime();  
    std::cout << "time:" << runtime<< std::endl;
    return 1;
  }
  if(flag_show_detail) impl.show_detail();
  
  // generate out file
  if(impl.write_out(argv[3])) return -1;
  
  // finish program
  clock_t end = clock();
  double runtime = (double)(end - start) / CLOCKS_PER_SEC + impl.get_runtime();  
  std::cout << "time:" << runtime<< std::endl;
  
  return 0;
}
