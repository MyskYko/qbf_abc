#ifndef TOP_DATA_H
#define TOP_DATA_H

#include <string>
#include <vector>
#include <map>
#include <fstream>

class top_data {
 private:
  std::vector<std::string> constraint_signals;
  std::vector<std::string> onehot_candidate_names;
  std::vector<std::string> zeroonehot_candidate_names;
  std::vector<std::vector<std::string> > groupzeroonehot_candidate_names;
  std::map<std::string, std::vector<std::string> > candidate_selection_signals;
  std::vector<std::string> x_names;
  std::map<std::string, std::vector<std::string> > x_selection_signals;

  unsigned int max_signal_count_onehot;
  std::map<std::string, std::string> onehot_candidate_to_constraint_signal;
  std::map<std::string, std::string> onehot_x_name_to_constraint_signal;

  unsigned int max_signal_count_zeroonehot;
  std::map<std::string, std::string> zeroonehot_candidate_to_constraint_signal;
  std::map<std::string, std::string> groupzeroonehot_candidate_to_constraint_signal;
  
  std::map<std::string, std::string> output_to_spec_output;
  std::map<std::string, std::string> output_to_impl_output;
  std::vector<std::string> outputs_to_be_compared;
  std::map<std::string, std::string> output_to_constraint_signal;

  std::string onehot = "";
  std::string zeroonehot = "";
  std::string eq = ".model __eq\n.inputs spec impl\n.outputs eq\n.names spec impl eq\n11 1\n00 1\n.end\n";
  std::vector<std::string> constraint_subckts;
  
  std::vector<std::string> circuit_inputs;
  std::vector<std::string> circuit_subckts;
  std::vector<std::string> selection_signals;
  std::string constraint_output = "__constraint";

 public:
  int setup(std::map<std::string, std::vector<std::string> > copy_of_candidate_selection_signals, std::vector<std::string> copy_of_x_names, std::map<std::string, std::vector<std::string> > copy_of_x_selection_signals, std::vector<std::string> copy_of_onehot_candidate_names, std::vector<std::string> copy_of_zeroonehot_candidate_names, std::vector<std::vector<std::string> > copy_of_groupzeroonehot_candidate_names, std::vector<std::string> copy_of_outputs, std::vector<std::string> copy_of_inputs, std::vector<std::string> copy_of_selection_signals, std::string spec_top, std::string impl_top);
  std::vector<std::string> copy_of_selection_signals() { return selection_signals; }

  int create_onehot_signal();
  int create_zeroonehot_signal();
  int create_groupzeroonehot_signal();
  void create_output_constraint_signal();
  void create_onehot();
  void create_zeroonehot();
  void create_constraint_subckt();
  void create_circuit_subckt(std::string spec_top, std::string impl_top);
  void write_circuit(std::ofstream *write_file);
  void show_detail();
};

#endif
