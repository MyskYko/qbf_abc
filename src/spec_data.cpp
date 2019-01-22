#include <spec_data.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

void spec_data::read_file(char* file_name) {
  
  file.open(file_name, std::ios::in);
  if(!file) {
    throw "cannot open spec file";
  }
  
  std::string str;
  bool flag_inside_top = false;
  top_name = "__hoge";
  
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
    bool flag_top = false;
    bool flag_inputs = false;
    bool flag_outputs = false;
    
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
    }
  }

  if(top_name == "__hoge") {
    throw ".top not found in spec";
  }
}

void spec_data::write_circuit(std::ofstream *write_file) {
  file.clear();
  file.seekg(0, std::ios_base::beg);
  
  std::string str;
  while (getline(file, str)) {
    *write_file << str << std::endl;
  }
  *write_file << std::endl;
}

void spec_data::show_detail() {
  std::cout << "spec_top_name:" << top_name << std::endl;
  
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
}
