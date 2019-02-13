#ifndef SPEC_DATA_H
#define SPEC_DATA_H

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
  int setup(std::string filename);
  std::string get_top_name() { return top_name; }
  std::vector<std::string> copy_of_inputs() { return inputs; }
  std::vector<std::string> copy_of_outputs() { return outputs; }
  int read_file(std::string file_name);
  void write_circuit(std::ofstream *write_file);
  void show_detail();
};

#endif
