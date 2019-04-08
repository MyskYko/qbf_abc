#include <template_data.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>

void template_data::reset() {
  num_cycle = 0;
  num_node = 0;
  num_reg.clear();
  num_spx.clear();
  num_hdx.clear();
  initial_assignment.clear();
  final_assignment.clear();
  flag_onesendrecv_spx = 0;
  flag_onehot_spx_in = 0;
  flag_onehot_spx_out = 0;
  flag_onehot_spx_between = 0;
  flag_implicit_reg = 0;
  data = "";
}

int template_data::check_setting() {
  if(num_cycle == 0) {
    std::cout <<  "num_cycle = 0 or has not been set." << std::endl;
    return 1;
  }
  if(num_node == 0) {
    std::cout <<  "num_node = 0 or has not been set." << std::endl;
    return 1;
  }
  if(num_reg.empty()) {
    if(flag_implicit_reg) {
      std::cout <<  "num_reg has not been set. Fill them by zero (implicit reg is ON)." << std::endl;
      for(int i = 0; i < num_node; i++) num_reg.push_back(1);
    }
    else {
      std::cout <<  "num_reg has not been set. Fill each by max(init assign, fin assign for a node)." << std::endl;
      int max_assignment = 0;
      for(int i = 0; i < num_node; i++) {
	if(initial_assignment.size() < (unsigned)i) {
	  std::cout <<  "initial assignment hasn't been set" << std::endl;
	  return 1;
	}
	if(final_assignment.size() < (unsigned)i) {
	  std::cout <<  "final assignment hasn't been set" << std::endl;
	  return 1;
	}
	int max_assignment_ = (int)std::max(initial_assignment[i].size(), final_assignment[i].size());
	if(max_assignment < max_assignment_) max_assignment = max_assignment_;
      }
      for(int i = 0; i < num_node; i++) num_reg.push_back(max_assignment);
    }
  }
  if(num_spx.empty()) {
    std::cout <<  "num_spx has not been set. Fill them by zero" << std::endl;
    for(int i = 0; i < num_node; i++) {
      std::vector<int> num_spx_(num_node);
      num_spx.push_back(num_spx_);
    }
  }
  if(num_hdx.empty()) {
    std::cout <<  "num_hdx has not been set. Fill them by zero" << std::endl;
    for(int i = 0; i < num_node; i++) {
      std::vector<int> num_hdx_(num_node);
      num_hdx.push_back(num_hdx_);
    }
  }
  if(initial_assignment.empty()) {
	std::cout <<  "initial assignment hasn't been set" << std::endl;
	return 1;
  };
  if(final_assignment.empty()) {
    	std::cout <<  "initial assignment hasn't been set" << std::endl;
	return 1;
  }

  return 0;
}

int template_data::read_file(std::string filename) {
  std::ifstream file;
  file.open(filename, std::ios::in);
  if(!file) {
    std::cout <<  "cannot open impl file" << std::endl;
    return 1;
  }

  reset();
  
  std::string str;
  while (getline(file, str)) {
    if(str == "cycle") {
      getline(file, str);
      num_cycle = std::stoi(str);
    }
    else if(str == "node") {
      getline(file, str);
      num_node = std::stoi(str);
    }
    else if(str == "reg") {
      if(!num_reg.empty()) {
	std::cout <<  "reg is declared more than once" << std::endl;
	return 1;
      }
      num_reg.clear();
      if(num_node == 0) {
	std::cout <<  "write num node before reg" << std::endl;	
	return 1;
      }
      getline(file, str);
      char delim = ' ';
      std::stringstream ss(str);
      std::string str2;
      for(int i = 0; i < num_node; i++) {
	if(!std::getline(ss, str2, delim)) {
	  std::cout <<  "something wrong about num reg" << std::endl;	
	  return 1;
	}
	num_reg.push_back(std::stoi(str2));
      }
    }
    else if(str == "spx") {
      if(!num_spx.empty()) {
	std::cout <<  "spx is declared more than once" << std::endl;
	return 1;
      }
      if(num_node == 0 ) {
	std::cout <<  "write num node before spx" << std::endl;	
	return 1;
      }
      for(int i = 0; i < num_node; i++) {
	std::vector<int> num_spx_i;
	getline(file, str);
	char delim = ' ';
	std::stringstream ss(str);
	std::string str2;
	while(std::getline(ss, str2, delim)) {
	  num_spx_i.push_back(std::stoi(str2));
	}
	num_spx.push_back(num_spx_i);
      }
    }
    else if(str == "hdx") {
      if(!num_hdx.empty()) {
	std::cout <<  "hdx is declared more than once" << std::endl;
	return 1;
      }
      if(num_node == 0 ) {
	std::cout <<  "write num node before hdx" << std::endl;	
	return 1;
      }
      for(int i = 0; i < num_node; i++) {
	std::vector<int> num_hdx_i;
	getline(file, str);
	char delim = ' ';
	std::stringstream ss(str);
	std::string str2;
	while(std::getline(ss, str2, delim)) {
	  num_hdx_i.push_back(std::stoi(str2));
	}
	num_hdx.push_back(num_hdx_i);
      }
    }
    else if(str == "initial") {
      if(!initial_assignment.empty()) {
	std::cout <<  "initial assignment is declared more than once" << std::endl;
	return 1;
      }
      if(num_node == 0 ) {
	std::cout <<  "write num node before initial assignment" << std::endl;	
	return 1;
      }
      for(int i = 0; i < num_node; i++) {
	std::vector<std::string> initial_i;
	getline(file, str);
	char delim = ' ';
	std::stringstream ss(str);
	std::string str2;
	while(std::getline(ss, str2, delim)) {
	  initial_i.push_back(str2);
	}
	initial_assignment.push_back(initial_i);
      }
    }
    else if(str == "final") {
      if(!final_assignment.empty()) {
	std::cout <<  "final assignment is declared more than once" << std::endl;
	return 1;
      }
      if(num_node == 0 ) {
	std::cout <<  "write num node before final assignment" << std::endl;	
	return 1;
      }
      for(int i = 0; i < num_node; i++) {
	std::vector<std::string> final_i;
	getline(file, str);
	char delim = ' ';
	std::stringstream ss(str);
	std::string str2;
	while(std::getline(ss, str2, delim)) {
	  final_i.push_back(str2);
	}
	final_assignment.push_back(final_i);
      }
    }
    else if(str == "onesendrecv_spx") {
      flag_onesendrecv_spx = 1;
    }
    else if(str == "onehot_spx_in") {
      flag_onehot_spx_in = 1;
    }
    else if(str == "onehot_spx_out") {
      flag_onehot_spx_out = 1;
    }
    else if(str == "onehot_spx_between") {
      flag_onehot_spx_between = 1;
    }
    else if(str == "implicit_reg") {
      flag_implicit_reg = 1;
    }
  }

  return check_setting();
}

int template_data::gen_reg() {
  for(int c = 0; c < num_cycle+1; c++) {
    for(int n = 0; n < num_node; n++) {
      if(num_reg.size() < (unsigned)n) {
	std::cout <<  "num reg hasn't been set" << std::endl;		
	return 1;
      }
      for(int r = 0; r < num_reg[n]; r++) {
	data += ".names x_reg_c" + std::to_string(c) + "n" + std::to_string(n) + "r" + std::to_string(r) + " reg_c" + std::to_string(c) + "n" + std::to_string(n) + "r" + std::to_string(r) + "\n1 1\n";
      }    
    }
  }
  return 0;
}

int template_data::gen_com() {
  for(int c = 0; c < num_cycle; c++) {
    for(int n1 = 0; n1 < num_node; n1++) {
      for(int n2 = 0; n2 < num_node; n2++) {
	for(int k = 0; k < num_spx[n1][n2]; k++) {
	  data += ".names x_spx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k) + " spx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k) + "\n1 1\n";
	}
      }
    }
  }

  for(int c = 0; c < num_cycle; c++) {
    for(int n1 = 0; n1 < num_node-1; n1++) {
      for(int n2 = n1+1; n2 < num_node; n2++) {
	for(int k = 0; k < num_hdx[n1][n2]; k++) {
	  data += ".names x_hdx_c" + std::to_string(c) + "n" + std::to_string(n1) + "n" + std::to_string(n2) + "k" + std::to_string(k) + " hdx_c" + std::to_string(c) + "n" + std::to_string(n1) + "n" + std::to_string(n2) + "k" + std::to_string(k) + "\n1 1\n";
	  
	  data += ".names x_hdx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k) + " hdx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k) + "\n1 1\n";
	  data += ".names x_hdx_c" + std::to_string(c) + "from" + std::to_string(n2) + "to" + std::to_string(n1) + "k" + std::to_string(k) + " hdx_c" + std::to_string(c) + "from" + std::to_string(n2) + "to" + std::to_string(n1) + "k" + std::to_string(k) + "\n1 1\n";
	}
      }
    }
  }
  
  return 0;
}
int template_data::set_reg() {
  for(int n = 0; n < num_node; n++) {
    if(num_reg.size() < (unsigned)n) {
      std::cout <<  "num reg hasn't been set" << std::endl;		
      return 1;
    }
    for(int r = 0; r < num_reg[n]; r++) {
      data += "#.candidates ";
      if(initial_assignment.size() < (unsigned)n) {
	std::cout <<  "initial assignment hasn't been set" << std::endl;
	return 1;
      }
      for(int i = 0; i < (int)initial_assignment[n].size(); i++) {
	data += initial_assignment[n][i] + " ";
      }
      if(initial_assignment[n].size() == 0) data += "zero ";
      data += "x_reg_c0n" + std::to_string(n) + "r" + std::to_string(r) + "\n";
    }
  }
  
  for(int c = 1; c < num_cycle+1; c++) {
    for(int n = 0; n < num_node; n++) {
      if(num_reg.size() < (unsigned)n) {
	std::cout <<  "num reg hasn't been set" << std::endl;		
	return 1;
      }
      for(int r = 0; r < num_reg[n]; r++) {
	data += "#.candidates ";
	for(int r_from = 0; r_from < num_reg[n]; r_from++) {
	  data += "reg_c" + std::to_string(c-1) + "n" + std::to_string(n) + "r" + std::to_string(r_from) + " ";
	}
	for(int n_from = 0; n_from < num_node; n_from++) {
	  for(int k = 0; k < num_spx[n_from][n]; k++) {
	    data += "spx_c" + std::to_string(c-1) + "from" + std::to_string(n_from) + "to" + std::to_string(n) + "k" + std::to_string(k) + " ";
	  }
	}
	for(int n_from = 0; n_from < num_node; n_from++) {
	  if(n < n_from) {
	    for(int k = 0; k < num_hdx[n][n_from]; k++) {
	      data += "hdx_c" + std::to_string(c-1) + "n" + std::to_string(n) + "n" + std::to_string(n_from) + "k" + std::to_string(k) + " ";
	    }
	  }
	  else {
	    for(int k = 0; k < num_hdx[n_from][n]; k++) {
	      data += "hdx_c" + std::to_string(c-1) + "n" + std::to_string(n_from) + "n" + std::to_string(n) + "k" + std::to_string(k) + " ";
	    }
	  }
	}
	
	data += "x_reg_c" + std::to_string(c) + "n" + std::to_string(n) + "r" + std::to_string(r) + "\n";
      }    
    }
  }
  
  return 0;
}
int template_data::set_com() {
  for(int c = 0; c < num_cycle; c++) {
    for(int n1 = 0; n1 < num_node; n1++) {
      for(int n2 = 0; n2 < num_node; n2++) {
	for(int k = 0; k < num_spx[n1][n2]; k++) {
	  data += "#.candidates ";
	  if(num_reg.size() < (unsigned)n1) {
	    std::cout << "num reg hasn't been set" << std::endl;
	    return 1;
	  }
	  for(int r = 0; r < num_reg[n1]; r++) {
	    data += "reg_c" + std::to_string(c) + "n" + std::to_string(n1) + "r" + std::to_string(r) + " ";
	  }
	  
	  data += "x_spx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k) + "\n";
	}
      }
    }
  }
  
  for(int c = 0; c < num_cycle; c++) {
    for(int n1 = 0; n1 < num_node-1; n1++) {
      for(int n2 = n1+1; n2 < num_node; n2++) {
	for(int k = 0; k < num_hdx[n1][n2]; k++) {
	  // candidates from n1
	  data += "#.candidates ";
	  if(num_reg.size() < (unsigned)n1) {
	    std::cout << "num reg hasn't been set" << std::endl;
	    return 1;
	  }
	  for(int r = 0; r < num_reg[n1]; r++) {
	    data += "reg_c" + std::to_string(c) + "n" + std::to_string(n1) + "r" + std::to_string(r) + " ";
	  }
	  data += "x_hdx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k) + "\n";
	  
	  // candidates from n2
	  data += "#.candidates ";
	  if(num_reg.size() < (unsigned)n2) {
	    std::cout << "num reg hasn't been set" << std::endl;
	    return 1;
	  }
	  for(int r = 0; r < num_reg[n2]; r++) {
	    data += "reg_c" + std::to_string(c) + "n" + std::to_string(n2) + "r" + std::to_string(r) + " ";
	  }
	  data += "x_hdx_c" + std::to_string(c) + "from" + std::to_string(n2) + "to" + std::to_string(n1) + "k" + std::to_string(k) + "\n";

	  // merge both n1 n2
	  data += "#.candidates ";
	  data += "hdx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k) + " ";
	  data += "hdx_c" + std::to_string(c) + "from" + std::to_string(n2) + "to" + std::to_string(n1) + "k" + std::to_string(k) + " ";	  
	  data += "x_hdx_c" + std::to_string(c) + "n" + std::to_string(n1) + "n" + std::to_string(n2) + "k" + std::to_string(k) + "\n";
	}
      }
    }
  }
  
  return 0;
}
int template_data::set_out() {
  for(int n = 0; n < num_node; n++) {
    data += ".names ";
    if(num_reg.size() < (unsigned)n) {
      std::cout << "num reg hasn't been set" << std::endl;
      return 1;
    }
    if(final_assignment.size() < (unsigned)n) {
      std::cout << "final assignment hasn't been set" << std::endl;
      return 1;
    }
    if(!flag_implicit_reg) {
      for(int r = 0; r < num_reg[n]; r++) {
	data += "reg_c" + std::to_string(num_cycle) + "n" + std::to_string(n) + "r" + std::to_string(r) + " ";
      }
    }
    else {
      for(int c = 0; c <= num_cycle; c++) {
	for(int r = 0; r < num_reg[n]; r++) {
	  data += "reg_c" + std::to_string(c) + "n" + std::to_string(n) + "r" + std::to_string(r) + " ";
	}
      }
    }

    data += "t_out" + std::to_string(n) + "\n";

    if(!flag_implicit_reg) {
      for(int r = 0; r < num_reg[n]; r++) data += "0";
    }
    else {
      for(int c = 0; c <= num_cycle; c++) for(int r = 0; r < num_reg[n]; r++) data += "0";
    }
    data += " 0\n";

    data += ".names";
    for(int i = 0; i < (int)final_assignment[n].size(); i++) {
      data += " " + final_assignment[n][i];
    }
    data += " s_out" + std::to_string(n) +"\n";
    for(int i = 0; i < (int)final_assignment[n].size(); i++) {
      data += "0";
    }
    data += " 1\n";
    
    data += ".names t_out" + std::to_string(n) + " s_out" + std::to_string(n) + " out" + std::to_string(n) + "\n";
    data += "00 0\n";
  }
  return 0;
}

int template_data::set_onesendrecv_spx() {
  for(int c = 0; c < num_cycle; c++) {
    for(int n1 = 0; n1 < num_node; n1++) {
      std::string onesendrecv = "#.groupzeroonehot ";
      for(int n2 = 0; n2 < num_node; n2++) {
	for(int k = 0; k < num_spx[n1][n2]; k++) {
	  onesendrecv += "spx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k);
	  if(k != num_spx[n1][n2] - 1)
	    onesendrecv += ",";
	}
	if(num_spx[n1][n2] != 0)
	  onesendrecv += " ";
      }
      data += onesendrecv + "\b\n";
    }
  }
  for(int c = 0; c < num_cycle; c++) {
    for(int n2 = 0; n2 < num_node; n2++) {
      std::string onesendrecv = "#.groupzeroonehot ";
      for(int n1 = 0; n1 < num_node; n1++) {
	for(int k = 0; k < num_spx[n1][n2]; k++) {
	  onesendrecv += "spx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k);
	  if(k != num_spx[n1][n2] - 1)
	    onesendrecv += ",";
	}
	if(num_spx[n1][n2] != 0)
	  onesendrecv += " ";
      }
      data += onesendrecv + "\b\n";
    }
  }
  return 0;
}

int template_data::set_onehot_spx_in() {
  for(int c = 0; c < num_cycle; c++) {
    for(int n1 = 0; n1 < num_node; n1++) {
      std::string groupzeroonehot_in = "#.groupzeroonehot";
      for(int n2 = 0; n2 < num_node; n2++) {
	for(int k = 0; k < num_spx[n1][n2]; k++) {
	  groupzeroonehot_in += " spx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k);
	}
      }
      data += groupzeroonehot_in + "\n";
    }
  }
  return 0;
}
int template_data::set_onehot_spx_out() {
  for(int c = 0; c < num_cycle; c++) {
    for(int n2 = 0; n2 < num_node; n2++) {
      std::string groupzeroonehot_out = "#.groupzeroonehot";
      for(int n1 = 0; n1 < num_node; n1++) {
	for(int k = 0; k < num_spx[n1][n2]; k++) {
	  groupzeroonehot_out += " spx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k);
	}
      }
      data += groupzeroonehot_out + "\n";
    }
  }
  return 0;
}
int template_data::set_onehot_spx_between() {
  for(int c = 0; c < num_cycle; c++) {
    for(int n2 = 0; n2 < num_node; n2++) {
      for(int n1 = n2+1; n1 < num_node; n1++) {
	if( num_spx[n1][n2] == 0 && num_spx[n2][n1] == 0 ) continue;
	std::string groupzeroonehot_ashdx = "#.groupzeroonehot";
	for(int k = 0; k < num_spx[n1][n2]; k++) {
	  groupzeroonehot_ashdx += " spx_c" + std::to_string(c) + "from" + std::to_string(n1) + "to" + std::to_string(n2) + "k" + std::to_string(k);
	}
	for(int k = 0; k < num_spx[n2][n1]; k++) {
	  groupzeroonehot_ashdx += " spx_c" + std::to_string(c) + "from" + std::to_string(n2) + "to" + std::to_string(n1) + "k" + std::to_string(k);
	}
      data += groupzeroonehot_ashdx + "\n";
      }
    }
  }
  return 0;
}
int template_data::write_circuit(std::string filename) {
  std::ofstream file;
  file.open(filename, std::ios::out);
  if(!file) {
    std::cout <<  "cannot open impl file" << std::endl;
    return 1;
  }
  
  file << "#.top impl\n";
  file << ".model impl\n";
  file << ".inputs";
  
  std::vector<std::string> inputs;
  for(int n = 0; n < num_node; n++) {
    if(initial_assignment.size() < (unsigned)n) {
      std::cout <<  "initial assignment hasn't been set" << std::endl;
      return 1;
    }
    for(int i = 0; i < (int)initial_assignment[n].size(); i++) {
      inputs.push_back(initial_assignment[n][i]);
    }
  }
  std::sort(inputs.begin(), inputs.end());
  inputs.erase(std::unique(inputs.begin(), inputs.end()), inputs.end());
  
  for(int i = 0; i < (int)inputs.size(); i++) {
    file << " " << inputs[i];
  }
  file << std::endl;
  
  file << ".outputs";
  
  for(int n = 0; n < num_node; n++) {
    file << " out" << n;
  }
  file << std::endl;

  file << data;
  
  file << ".end\n";
  return 0;
}

int template_data::write_spec(std::string filename) {
  std::ofstream file;
  file.open(filename, std::ios::out);
  if(!file) {
    std::cout <<  "cannot open impl file" << std::endl;
    return 1;
  }
  
  file << "#.top spec\n";
  file << ".model spec\n";
  file << ".inputs";
  
  std::vector<std::string> inputs;
  for(int n = 0; n < num_node; n++) {
    if(initial_assignment.size() < (unsigned)n) {
      std::cout <<  "initial assignment hasn't been set" << std::endl;
      return 1;
    }
    for(int i = 0; i < (int)initial_assignment[n].size(); i++) {
      inputs.push_back(initial_assignment[n][i]);
    }
  }
  std::sort(inputs.begin(), inputs.end());
  inputs.erase(std::unique(inputs.begin(), inputs.end()), inputs.end());
  
  for(int i = 0; i < (int)inputs.size(); i++) {
    file << " " << inputs[i];
  }
  file << std::endl;
  
  file << ".outputs";
  
  for(int n = 0; n < num_node; n++) {
    file << " out" << n;
  }
  file << std::endl;

  for(int n = 0; n < num_node; n++) {
    file << ".names";
    file << " out" << n << std::endl;
    file << "1\n";
  }
  
  file << ".end\n";
  return 0;
}

int template_data::setup(std::string filename) {
  if(read_file(filename)) return 1;
  if(gen_reg()) return 1;
  if(gen_com()) return 1;
  if(set_reg()) return 1;
  if(set_com()) return 1;
  if(set_out()) return 1;
  if(flag_onesendrecv_spx)
    if(set_onesendrecv_spx()) return 1;
  if(flag_onehot_spx_in)
    if(set_onehot_spx_in()) return 1;
  if(flag_onehot_spx_out)
    if(set_onehot_spx_out()) return 1;
  if(flag_onehot_spx_between)
    if(set_onehot_spx_between()) return 1;
  return 0;
}
