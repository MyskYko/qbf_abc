#ifndef IMPL_DATA_H
#define IMPL_DATA_H

#include <string>
#include <vector>
#include <fstream>
#include <map>

class impl_data {
 private:
  std::ifstream file;
  std::string top_name;
  std::vector<std::string> inputs;
  std::vector<std::string> outputs;
  std::vector<std::string> x_names;
  std::vector<std::string> candidate_names;
  std::map<std::string, std::vector<std::string> > x_candidate_names;
  unsigned int max_candidate_count_x;  
  std::vector<std::string> onehot_candidate_names;
  unsigned int group_count;
  std::map<std::string, int> x_group;
  std::map<int, std::string> group_symbol;
  std::map<int, std::vector<std::string> > group_x_names;

  std::map<std::string, std::vector<std::string> > x_selection_signals;
  std::map<std::string, std::vector<std::string> > candidate_selection_signals;
  std::vector<std::string> all_selection_signals;
  std::string selector = "";
  std::vector<std::string> subckts;

  double runtime = 0;
  std::map<std::string, std::string> x_result;
  
 public:
  std::string get_top_name() { return top_name; }
  std::vector<std::string> copy_of_inputs() { return inputs; }
  std::vector<std::string> copy_of_outputs() { return outputs; }
  std::vector<std::string> copy_of_x_names() { return x_names; }
  std::map<std::string, std::vector<std::string> > copy_of_x_selection_signals() { return x_selection_signals; }
  std::vector<std::string> copy_of_onehot_candidate_names() { return onehot_candidate_names; }
  std::map<std::string, std::vector<std::string> > copy_of_candidate_selection_signals() { return candidate_selection_signals; }
  std::vector<std::string> copy_of_all_selection_signals() { return all_selection_signals; }
  double get_runtime() { return runtime; }
  void read_file(char* file_name);
  void create_selection_signal();
  void create_selector();
  void create_subckt();
  void write_circuit(std::ofstream *write_file);
  void read_result(std::string file_name);
  void write_out(char* file_name);
  void show_simple();
  void show_detail();
};

#endif
