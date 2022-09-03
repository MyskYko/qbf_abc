#pragma once

#include <string>
#include <vector>
#include <fstream>

class spec_data {
 private:
  std::ifstream file;
  std::string top_name;
  std::vector<std::string> inputs;
  std::vector<std::string> outputs;
  
 public:
  std::string get_top_name() { return top_name; }
  std::vector<std::string> copy_of_inputs() { return inputs; }
  std::vector<std::string> copy_of_outputs() { return outputs; }
  bool read_file(std::string file_name);
  void write_circuit(std::ofstream *write_file);
  void show_detail();
};
