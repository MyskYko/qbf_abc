#include <impl_data.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

void impl_data::read_file(char* file_name) {
  
  file.open(file_name, std::ios::in);
  if(!file) {
    throw "cannot open impl file";
  }
  
  std::string str;
  bool flag_inside_top = false;
  top_name = "__hoge";
  group_count = 0;
  
  while (getline(file, str)) {
    int comment_out = str.find("#");
    if(comment_out != -1) {
      if(str.length() <= (unsigned int)comment_out+1) {
	str = str.substr(0,comment_out);
      }
      else if(str[comment_out+1] == '.') {
	str[comment_out] = ' ';
      }
      else {
	str = str.substr(0,comment_out);
      }
    }
    
    char delim = ' ';
    std::stringstream ss(str);
    std::string str2;
    std::vector<std::string> c_names;
    bool flag_top = false;
    bool flag_inputs = false;
    bool flag_outputs = false;
    bool flag_candidates = false;
    bool flag_group = false;
    bool flag_onehot = false;
    
    while(std::getline(ss, str2, delim)) {
      if(flag_top) {
	top_name = str2;
      }
      else if(flag_inputs) {
	inputs.push_back(str2);
      }
      else if(flag_outputs) {
	outputs.push_back(str2);
      }
      else if(flag_candidates) {
	c_names.push_back(str2);
      }
      else if(flag_group) {
	x_group[str2] = group_count;
      }
      else if(flag_onehot) {
	onehot_candidate_names.push_back(str2);
      }
      
      if(str2 == ".top") {
	flag_top = true;
      }
      else if(str2 == top_name && !flag_top) {
	flag_inside_top = true;
      }
      else if(str2 == ".inputs" && flag_inside_top) {
	flag_inputs = true;
      }
      else if(str2 == ".outputs" && flag_inside_top) {
	flag_outputs = true;
      }
      else if(str2 == ".end") {
	flag_inside_top = false;
      }
      if(str2 == ".candidates") {
	flag_candidates = true;
      }
      else if(str2 == ".group") {
	flag_group = true;
	group_count++;
      }
      else if(str2 == ".onehot") {
	flag_onehot = true;
      }
    }
    
    if(flag_candidates) {
      std::string x_name = c_names.back();
      c_names.pop_back();
      x_names.push_back(x_name);
      x_candidate_names[x_name] = c_names;
    }
  }
  
  if(top_name == "__hoge") {
    throw ".top not found in impl";
  }
  
  max_candidate_count_x = 0;
  for(auto x_name : x_names) {
    if(max_candidate_count_x < x_candidate_names[x_name].size()) {
      max_candidate_count_x = x_candidate_names[x_name].size();
    }
    
    for(auto candidate_name: x_candidate_names[x_name]) {
      candidate_names.push_back(candidate_name);
    }
  }  
  std::sort(candidate_names.begin(), candidate_names.end());
  candidate_names.erase(std::unique(candidate_names.begin(), candidate_names.end()), candidate_names.end());
  
  for(unsigned int i = 1; i <= group_count; i++) {
    group_symbol[i] = "hoge";
    std::vector<std::string> vec;
    group_x_names[i] = vec;
  }
  
  for(auto x_name : x_names) {
    int group = x_group[x_name];
    if(group != 0) {
      group_x_names[group].push_back(x_name);
      if(group_symbol[group] == "hoge") {
	group_symbol[group] = x_name;
      }
    }
  }
}

void impl_data::create_selection_signal() {
  for(auto x_name: x_names) {
    std::vector<std::string> vec;
    x_selection_signals[x_name] = vec;
  }
  
  for(auto candidate_name: candidate_names) {
    std::vector<std::string> vec;
    candidate_selection_signals[candidate_name] = vec;
  }
  
  for(auto x_name: x_names) {
    for(unsigned int i = 0; i < x_candidate_names[x_name].size(); i++) {
      std::string selection_signal;
      if(x_group[x_name] == 0) {
	selection_signal = "__from_" + x_candidate_names[x_name][i] + "_to_" + x_name;
      }
      else {
	selection_signal = "__from_" + x_candidate_names[group_symbol[x_group[x_name]]][i] + "_to_" + group_symbol[x_group[x_name]];	
      }
      x_selection_signals[x_name].push_back(selection_signal);
      candidate_selection_signals[x_candidate_names[x_name][i]].push_back(selection_signal);
    }
  }
  
  for(auto x_name: x_names) {
    for(auto selection_signal: x_selection_signals[x_name]) {
      all_selection_signals.push_back(selection_signal);
    }
  }
  std::sort(all_selection_signals.begin(), all_selection_signals.end());
  all_selection_signals.erase(std::unique(all_selection_signals.begin(), all_selection_signals.end()), all_selection_signals.end());
}

void impl_data::create_selector() {
  selector += ".model __selector\n";
  selector += ".inputs";
  for(unsigned int i = 0; i < max_candidate_count_x; i++) {
    selector += " in" + std::to_string(i);
  }
  for(unsigned int i = 0; i < max_candidate_count_x; i++) {
    selector += " sel" + std::to_string(i);
  }
  selector += "\n";
  selector += ".outputs out\n";

  selector += ".names";
  for(unsigned int i = 0; i < max_candidate_count_x; i++) {
    selector += " in" + std::to_string(i);
  }
  for(unsigned int i = 0; i < max_candidate_count_x; i++) {
    selector += " sel" + std::to_string(i);
  }
  selector += " out\n";
  
  for(unsigned int i = 0; i < max_candidate_count_x; i++) {
    for(unsigned int j = 0; j < max_candidate_count_x; j++) {
      if(i == j) {
	selector += "1";
      }
      else {
	selector += "-";
      }
    }
    for(unsigned int j = 0; j < max_candidate_count_x; j++) {
      if(i == j) {
	selector += "1";
      }
      else {
	selector += "0";
      }
    }
    selector += " 1\n";
  }
  selector += ".end\n";
}

void impl_data::create_subckt() {
  for(auto x_name: x_names) {
    std::string subckt;
    subckt += ".subckt __selector";
    for (unsigned int i = 0; i < x_candidate_names[x_name].size(); i++){
      subckt += " in" + std::to_string(i) + "=" + x_candidate_names[x_name][i];
    }
    for (unsigned int i = 0; i < x_selection_signals[x_name].size(); i++) {
      subckt += " sel" + std::to_string(i) + "=" + x_selection_signals[x_name][i];
    }
    subckt += " out=" + x_name;
    subckt += '\n';
    subckts.push_back(subckt);
  }
}

void impl_data::write_circuit(std::ofstream *write_file) {
  file.clear();
  file.seekg(0, std::ios_base::beg);

  std::string str;
  while (getline(file, str)) {
    std::string str_without_comment;
    int comment_out = str.find("#");
    if(comment_out != -1) {
      str_without_comment = str.substr(0,comment_out);
    }
    else {
      str_without_comment = str;
    }
    char delim = ' ';
    std::stringstream ss(str_without_comment);
    
    std::string str2;
    bool flag_inside_top = false;
    while(std::getline(ss, str2, delim)) {
      if(str2 == top_name) {
	flag_inside_top = true;
      }
    }

    *write_file << str << std::endl;
    
    if(flag_inside_top) {
      *write_file << ".inputs";
      for(auto selection_signal: all_selection_signals) {
	*write_file << " " << selection_signal;
      }
      *write_file << std::endl;
      
      for(auto subckt: subckts) {
	*write_file << subckt;
      }
    }
  }
  *write_file << std::endl;

  *write_file << selector;
}

void impl_data::read_result(std::string file_name) {
  std::ifstream result_file;
  result_file.open(file_name, std::ios::in);
  if(!result_file) {
    throw "cannot open result file";
  }

  std::string str;
  std::string result = "hoge";
  while (getline(result_file, str)) {
    char delim = ' ';
    std::stringstream ss(str);

    std::string str2;
    bool flag_result = false;
    bool flag_runtime = false;
    while(std::getline(ss, str2, delim)) {
      if(flag_result) {
	result = str2;
	flag_result = false;
      }
      if(flag_runtime) {
	if(str2[0] >= '0' && str2[0] <= '9') {
	  runtime = std::stod(str2);
	}
      }
      if(str2 == "Parameters:") {
	flag_result = true;
      }
      if(str2 == "runtime") {
	flag_runtime = true;
      }
    }
  }
  
  if(result == "hoge") {
    throw "no solution";
  }
  
  std::map<std::string, bool> selection_signal_result;
  for(unsigned int i = 0; i < all_selection_signals.size(); i++) {
    if(result[i] == '1') {
      selection_signal_result[all_selection_signals[i]] = true;
    }
    else {
      selection_signal_result[all_selection_signals[i]] = false;
    }
  }

  x_result = std::map<std::string, std::string>();
  for(auto x_name: x_names) {
    x_result[x_name] = "zero";
    for(unsigned int i = 0; i < x_selection_signals[x_name].size(); i++) {
      if(selection_signal_result[x_selection_signals[x_name][i]]) {
	x_result[x_name] = x_candidate_names[x_name][i];
	break;
      }
    }
  }
}

void impl_data::write_out(char* file_name) {
  std::ofstream write_out;
  write_out.open(file_name, std::ios::out);
  if(!write_out) {
    throw "cannot write out file";
  }
  
  file.clear();
  file.seekg(0,std::ios::beg);

  std::string str;
  while(getline(file, str)) {
    std::string str_without_comment;
    int comment_out = str.find("#");
    if(comment_out != -1) {
      str_without_comment = str.substr(0,comment_out);
    }
    else {
      str_without_comment = str;
    }    

    for(auto x_name: x_names) {
      int x_pos = 0;
      while ((x_pos = str_without_comment.find(x_name, x_pos)) != -1) {
	int x_len = x_name.length();
	unsigned int x_pos_end = x_pos+x_len;
	if(str_without_comment.length() > x_pos_end) {
	  if(str_without_comment[x_pos_end] != ' ') {
	    int x_pos_next = str_without_comment.find(" ", x_pos_end);
	    if(x_pos_next != -1) {
	      x_pos = x_pos_next;
	      continue;
	    }
	    else {
	      break;
	    }
	  }
	}
	str_without_comment.replace(x_pos, x_len, x_result[x_name]);
	str.replace(x_pos, x_len, x_result[x_name]);
	x_pos += x_result[x_name].length();
      }
    }
    write_out << str << std::endl;
  }
}

void impl_data::show_simple() {
  std::cout << "impl_top_name:" << top_name << std::endl;
  std::cout << "x_count:" << x_names.size() << std::endl;
  std::cout << "candidate_count:" << candidate_names.size() << std::endl;
  std::cout << "onehot_count:" << onehot_candidate_names.size() << std::endl;
  std::cout << "max_candidate_count_for_each_x:" << std::to_string(max_candidate_count_x) << std::endl;
  std::cout << "group_count:" << group_count << std::endl;
  std::cout << "selection_signal_count:" << all_selection_signals.size() << std::endl;
}

void impl_data::show_detail() {
  std::cout << "impl_top_name:" << top_name << std::endl;
  std::cout << "x_count:" << x_names.size() << std::endl;
  std::cout << "candidate_count:" << candidate_names.size() << std::endl;
  std::cout << "onehot_count:" << onehot_candidate_names.size() << std::endl;
  std::cout << "max_candidate_count_for_each_x:" << std::to_string(max_candidate_count_x) << std::endl;
  std::cout << "group_count:" << group_count << std::endl;

  std::cout << "---input---" << std::endl;
  for(auto input: inputs) {
    std::cout << input << " ";
  }
  std::cout << std::endl << std::endl;

  std::cout << "---output---" << std::endl;
  for(auto output: outputs) {
    std::cout << output << " ";
  }
  std::cout << std::endl << std::endl;

  std::cout << "---x_candidates---" << std::endl;
  for(auto x_name : x_names) {
    std::cout << x_name << ":";
    for(auto candidate_name: x_candidate_names[x_name]) {
      std::cout << candidate_name << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  
  if(onehot_candidate_names.size() != 0) {
    std::cout << "---onehot_candidates---" << std::endl;
    for(auto candidate_name: onehot_candidate_names) {
      std::cout << candidate_name << " ";
    }
    std::cout << std::endl << std::endl;
  }

  if(group_count != 0) {
    std::cout << "---groups---" << std::endl;
    for(unsigned int i = 1; i <= group_count; i++) {
      std::cout << std::to_string(i) << ":";
      for(auto x_name: group_x_names[i]) {
	std::cout << x_name << " ";
      }
      std::cout << ",symbol=" << group_symbol[i];
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
  
  if(all_selection_signals.size() != 0) {
    std::cout << "selection_signal_count:" << all_selection_signals.size() << std::endl;
    std::cout << "---selection_signals---" << std::endl;
    for(auto selection_signal: all_selection_signals) {
      std::cout << selection_signal + " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }
  if(selector != "") {
    std::cout << "---selector---" << std::endl;
    std::cout << selector << std::endl;
  }

  if(subckts.size() != 0) {
    std::cout << "---subckt---" << std::endl;
    for(auto subckt: subckts) {
      std::cout << subckt;
    }
    std::cout << std::endl;
  }

  if(runtime != 0) {
    std::cout << "---result---" << std::endl;
    for(auto x_name : x_names) {
      std::cout << x_name  << ":" << x_result[x_name] << std::endl;
    }
    std::cout << std::endl;
  }
}

