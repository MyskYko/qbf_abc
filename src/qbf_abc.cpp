#include <iostream>
#include <string>

#include "spec_data.hpp"
#include "impl_data.hpp"
#include "top_data.hpp"

#include "qbf_abc.hpp"

int SolveQBF(std::string specname, std::string implname, std::string outname, bool fVerbose, bool fVerbose2) {
  //read spec and store inputs, outputs, top
  spec_data spec;
  if(spec.read_file(specname)) {
    return -1;
  }
  if(fVerbose2) { spec.show_detail(); }
  
  // read impl and store candidates, x, top, group
  impl_data impl;
  if(impl.read_file(implname)) {
    return -1;
  }
  impl.create_selection_signal();
  impl.create_selector();
  impl.create_subckt();
  if(fVerbose2) { impl.show_detail(); }

  // create constraint signal(onehot, eq)
  top_data top;
  if(top.create_onehot_signal(impl.copy_of_onehot_candidate_names(), impl.copy_of_candidate_selection_signals(), impl.copy_of_x_names(), impl.copy_of_x_selection_signals())) {
    return -1;
  }
  top.create_output_constraint_signal(spec.copy_of_outputs());
  top.create_onehot();
  top.create_constraint_subckt();
  top.create_circuit_subckt(spec.get_top_name(), impl.get_top_name(), spec.copy_of_inputs(), impl.copy_of_all_selection_signals());
  if(fVerbose2) { top.show_detail(); }

  // write tmp
  std::string tmp_file_name = "__tmp_top.blif";
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
  if(fVerbose) { impl.show_simple(); }
  std::string log_file_name = "__log.txt";
  std::string command = "abc -c \"read " + tmp_file_name + "; strash; qbf -v -P " + std::to_string(top.copy_of_selection_signals().size()) + ";\" > " + log_file_name;
  system(command.c_str());

  // get result
  int r = impl.read_result(log_file_name);
  if(r < 0) {
    return -1;
  }
  if(r == 0) {
    if(fVerbose) { std::cout << "UNSAT" << std::endl; }
    return 0;
  }
  if(fVerbose) { std::cout << "SAT" << std::endl; }
  if(fVerbose2) { impl.show_detail(); }
  
  // generate out file
  if(impl.write_out(outname)) {
    return -1;
  }
  
  return 1;
}
