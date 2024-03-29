#include <iostream>

#include "top_data.hpp"

bool top_data:: create_onehot_signal(std::vector<std::string> copy_of_onehot_candidate_names, std::map<std::string, std::vector<std::string> > copy_of_candidate_selection_signals, std::vector<std::string> copy_of_x_names, std::map<std::string, std::vector<std::string> > copy_of_x_selection_signals) {
  onehot_candidate_names = copy_of_onehot_candidate_names;
  candidate_selection_signals = copy_of_candidate_selection_signals;
  x_names = copy_of_x_names;
  x_selection_signals = copy_of_x_selection_signals;

  max_signal_count_onehot = 0;
  for(auto candidate_name: onehot_candidate_names) {
    if(candidate_selection_signals[candidate_name].size() == 0) {
      std::cout << "There is no using \"" + candidate_name + "\" although it is in onehot" << std::endl;
      return 1;
    }
    else {
      if(max_signal_count_onehot < candidate_selection_signals[candidate_name].size()) {
	max_signal_count_onehot = candidate_selection_signals[candidate_name].size();
      }
    }
  }

  for(auto x_name: x_names) {
    if(x_selection_signals[x_name].size() == 0) {
      std::cout << x_name + " doesn't have candidates" << std::endl;
      return 1;
    }
    else {
      if(max_signal_count_onehot < x_selection_signals[x_name].size()) {
	max_signal_count_onehot = x_selection_signals[x_name].size();
      }
    }
  }
  
  for(auto candidate_name: onehot_candidate_names) {
    std::string constraint_signal;
    constraint_signal = "__constraint_onehot_" + candidate_name;
    constraint_signals.push_back(constraint_signal);
    onehot_candidate_to_constraint_signal[candidate_name] = constraint_signal;
  }
  
  for(auto x_name: x_names) {
    std::string constraint_signal;
    constraint_signal = "__constraint_onehot_" + x_name;
    constraint_signals.push_back(constraint_signal);
    onehot_x_name_to_constraint_signal[x_name] = constraint_signal;
  }
  
  return 0;
}

void top_data::create_output_constraint_signal(std::vector<std::string> copy_of_outputs) {
  outputs_to_be_compared = copy_of_outputs;
  for(auto output: outputs_to_be_compared) {
    std::string constraint_signal;
    constraint_signal = "__" + output + "_eq";
    constraint_signals.push_back(constraint_signal);
    output_to_constraint_signal[output] = constraint_signal;
  }
}

void top_data::create_output_constraint_signal(std::vector<std::string> copy_of_spec_outputs, std::vector<std::string> copy_of_impl_outputs) {
  spec_outputs = copy_of_spec_outputs;
  impl_outputs = copy_of_impl_outputs;
  relation_constraint_signal = "__relation_sat";
  constraint_signals.push_back(relation_constraint_signal);
}

void top_data::create_onehot() {
  onehot += ".model __onehot\n";
  onehot += ".inputs";
  for(unsigned int i = 0; i < max_signal_count_onehot; i++) {
    onehot += " in" + std::to_string(i);
  }
  onehot += "\n";
  onehot += ".outputs out\n";
  unsigned int level = 0;
  unsigned int nout = 0;
  unsigned int t = max_signal_count_onehot;
  while(t > 1) {
    for(unsigned int i = 0; i < t/2; i++) {
      onehot += ".names";
      onehot += " in";
      for(unsigned int j = 0; j < level; j++) {
        onehot += "t";
      }
      onehot += std::to_string(i+i);
      onehot += " in";
      for(unsigned int j = 0; j < level; j++) {
        onehot += "t";
      }
      onehot += std::to_string(i+i+1);
      onehot += " o" + std::to_string(nout++);
      onehot += "\n11 1\n";
      onehot += ".names";
      onehot += " in";
      for(unsigned int j = 0; j < level; j++) {
        onehot += "t";
      }
      onehot += std::to_string(i+i);
      onehot += " in";
      for(unsigned int j = 0; j < level; j++) {
        onehot += "t";
      }
      onehot += std::to_string(i+i+1);
      onehot += " in";
      for(unsigned int j = 0; j < level + 1; j++) {
        onehot += "t";
      }
      onehot += std::to_string(i);
      onehot += "\n00 0\n";
    }
    if(t % 2) {
      onehot += ".names";
      onehot += " in";
      for(unsigned int j = 0; j < level; j++) {
        onehot += "t";
      }
      onehot += std::to_string(t-1);
      onehot += " in";
      for(unsigned int j = 0; j < level + 1; j++) {
        onehot += "t";
      }
      onehot += std::to_string(t/2);
      onehot += "\n1 1\n";
    }
    t = (t + 1) / 2;
    level++;
  }
  onehot += ".names";
  for(unsigned int i = 0; i < max_signal_count_onehot; i++) {
    onehot += " in" + std::to_string(i);
  }
  onehot += " allzero\n";
  for(unsigned int i = 0; i < max_signal_count_onehot; i++) {
    onehot += "0";
  }
  onehot += " 1\n";

  onehot += ".names";
  for(unsigned int i = 0; i < nout; i++) {
    onehot += " o" + std::to_string(i);
  }
  onehot += " allzero out\n";
  for(unsigned int i = 0; i < nout + 1; i++) {
    onehot += "0";
  }
  onehot += " 1\n";
  onehot += ".end\n";
}

void top_data::create_relation() {
  relation += ".model __relation\n";
  relation += ".inputs";
  for(unsigned int i = 0; i < spec_outputs.size(); i++) {
    relation += " spec" + std::to_string(i);
  }
  for(unsigned int i = 0; i < impl_outputs.size(); i++) {
    relation += " impl" + std::to_string(i);
  }
  relation += "\n";
  relation += ".outputs sat\n";
  relation += ".names";
  for(unsigned int i = 0; i < impl_outputs.size(); i++) {
    relation += " impl" + std::to_string(i);
  }
  for(unsigned int i = 0; i < spec_outputs.size(); i++) {
    relation += " spec" + std::to_string(i);
  }
  relation += " sat\n";
  for(unsigned int i = 0; i < spec_outputs.size(); i++) {
    unsigned int tmp = i;
    for(unsigned int j = 0; j < impl_outputs.size(); j++) {
      relation += std::to_string(tmp & 1);
      tmp >>= 1;
    }
    for(unsigned int j = 0; j < spec_outputs.size(); j++) {
      if(i == j) {
        relation += "1";
      } else {
        relation += "-";
      }
    }
    relation += " 1\n";
  }
  relation += ".end\n";
}

void top_data::create_constraint_subckt() {
  for(auto candidate_name: onehot_candidate_names) {
    std::string subckt;
    subckt += ".subckt __onehot";
    for(unsigned int i = 0; i < candidate_selection_signals[candidate_name].size(); i++) {
      subckt += " in" + std::to_string(i) + "=" + candidate_selection_signals[candidate_name][i];
    }
    subckt += " out=" + onehot_candidate_to_constraint_signal[candidate_name];
    subckt += "\n";
    constraint_subckts.push_back(subckt);
  }

  for(auto x_name: x_names) {
    std::string subckt;
    subckt += ".subckt __onehot";
    for(unsigned int i = 0; i < x_selection_signals[x_name].size(); i++) {
      subckt += " in" + std::to_string(i) + "=" + x_selection_signals[x_name][i];
    }
    subckt += " out=" + onehot_x_name_to_constraint_signal[x_name];
    subckt += "\n";
    constraint_subckts.push_back(subckt);
  }
  
  for(auto output: outputs_to_be_compared) {
    std::string spec_output = "__spec_" + output;
    std::string impl_output = "__impl_" + output;
    output_to_spec_output[output] = spec_output;
    output_to_impl_output[output] = impl_output;
    
    std::string subckt;
    subckt += ".subckt __eq";
    subckt += " spec=" + spec_output;
    subckt += " impl=" + impl_output;
    subckt += " eq=" + output_to_constraint_signal[output];
    subckt += "\n";
    constraint_subckts.push_back(subckt);
  }

  if(!relation_constraint_signal.empty()) {
    std::string subckt;
    subckt += ".subckt __relation";
    for(unsigned int i = 0; i < spec_outputs.size(); i++) {
      subckt += " spec" + std::to_string(i) + "=__spec" + std::to_string(i);
    }
    for(unsigned int i = 0; i < impl_outputs.size(); i++) {
      subckt += " impl" + std::to_string(i) + "=__impl" + std::to_string(i);
    }
    subckt += " sat=" + relation_constraint_signal;
    subckt += "\n";
    constraint_subckts.push_back(subckt);
  }

  std::string subckt;
  subckt += ".names";
  for(auto constraint_signal: constraint_signals) {
    subckt += " " + constraint_signal;
  }
  subckt += " " + constraint_output;
  subckt += "\n";
  for(unsigned int i = 0; i < constraint_signals.size(); i++) {
    subckt += "1";
  }
  subckt += " 1\n";
  constraint_subckts.push_back(subckt);
}

void top_data::create_circuit_subckt(std::string spec_top, std::string impl_top, std::vector<std::string> copy_of_inputs, std::vector<std::string> copy_of_selection_signals) {
  circuit_inputs = copy_of_inputs;
  selection_signals = copy_of_selection_signals;
  std::string spec_subckt;
  spec_subckt += ".subckt " + spec_top;
  for(auto input: circuit_inputs) {
    spec_subckt += " " + input + "=" + input; 
  }
  for(auto output: outputs_to_be_compared) {
    spec_subckt += " " + output + "=" + output_to_spec_output[output]; 
  }
  for(unsigned int i = 0; i < spec_outputs.size(); i++) {
    spec_subckt += " " + spec_outputs[i] + "=__spec" + std::to_string(i);
  }
  spec_subckt += '\n';

  circuit_subckts.push_back(spec_subckt);

  std::string impl_subckt;
  impl_subckt += ".subckt " + impl_top;
  for(auto selection_signal: selection_signals) {
    impl_subckt += " " + selection_signal + "=" + selection_signal; 
  }
  for(auto input: circuit_inputs) {
    impl_subckt += " " + input + "=" + input; 
  }
  for(auto output: outputs_to_be_compared) {
    impl_subckt += " " + output + "=" + output_to_impl_output[output]; 
  }
  for(unsigned int i = 0; i < impl_outputs.size(); i++) {
    impl_subckt += " " + impl_outputs[i] + "=__impl" + std::to_string(i);
  }
  impl_subckt += '\n';

  circuit_subckts.push_back(impl_subckt);
}

void top_data::write_circuit(std::ofstream *write_file) {
  *write_file << ".model __top" << std::endl;
  *write_file << ".inputs";
  for(auto selection_signal: selection_signals) {
    *write_file << " " <<  selection_signal;
  }
  *write_file << std::endl;
  *write_file << ".inputs";
  for(auto input: circuit_inputs) {
    *write_file << " " << input;
  }
  *write_file << std::endl;
  *write_file << ".outputs " << constraint_output << std::endl;

  for(auto subckt: circuit_subckts) {
    *write_file << subckt;
  }
  for(auto subckt: constraint_subckts) {
    *write_file << subckt;
  }
  *write_file << ".end" << std::endl << std::endl;

  *write_file << onehot << std::endl;
  if(!relation.empty()) {
    *write_file << relation << std::endl;
  } else {
    *write_file << eq << std::endl;
  }
}

void top_data::show_detail() {
  std::cout << "max_signal_count_for_each_onehot:" << std::to_string(max_signal_count_onehot) << std::endl;
  
  std::cout << "---constraint_signal---" << std::endl;
  for(auto constraint_signal: constraint_signals) {
    std::cout << constraint_signal << " ";
  }
  std::cout << std::endl << std::endl;

  if(max_signal_count_onehot != 0 && onehot != "") {
    std::cout << "---onehot---" << std::endl;
    std::cout << onehot << std::endl;
  }

  if(constraint_subckts.size() != 0) {
    std::cout << "---constraint_subckt---" << std::endl;
    for(auto subckt: constraint_subckts) {
      std:: cout << subckt;
    }
    std::cout << std::endl;
  }

  if(circuit_subckts.size() != 0) {
    std::cout << "---circuit_subckt---" << std::endl;
    for(auto subckt: circuit_subckts) {
      std::cout << subckt;
    }
    std::cout << std::endl;
  }
}
