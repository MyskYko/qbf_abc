#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>
#include <cassert>

#define DEBUG
#define SIZE 10

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cout << "error\n";
    return 1;
  }
  std::vector<std::pair<std::string, std::string> > assignment;
  std::vector<std::string> outputs;
  std::ifstream solution_file;
  const char* file_name = argv[1];
  solution_file.open(file_name, std::ios::in);
  if(!solution_file) {
    std::cout << "error\n";
    return 1;
  }
  std::ofstream mesh_file;
  const char* mesh_file_name = "__mesh";
  mesh_file.open(mesh_file_name, std::ios::out);
  if(!mesh_file) {
    std::cout << "error\n";
    return 1;
  }
  std::ofstream con_file;
  const char* con_file_name = "__con";
  con_file.open(con_file_name, std::ios::out);
  if(!con_file) {
    std::cout << "error\n";
    return 1;
  }
  
  std::string str;
  while(getline(solution_file, str)) {
    if(str.substr(0, 6) == ".names") {
      char delim = ' ';
      std::stringstream ss(str);
      std::string str2;
      std::string last_str;
      std::string last_str_before;
      while(std::getline(ss, str2, delim)) {
	last_str_before = last_str;
	last_str = str2;
      }
      if(last_str_before == ".names") {
	/*
	std::streampos original = solution_file.tellg();
	getline(solution_file, str);
	if(str == "1") {
	  last_str_before = "__one";
	}
	solution_file.seekg(original);
	*/
	assert(0);
      }
      assignment.push_back(std::make_pair(last_str, last_str_before));
    }
    else if(str.substr(0,8) == ".outputs") {
      /*
      char delim = ' ';
      std::stringstream ss(str);
      std::string str2;
      std::getline(ss, str2, delim);
      while(std::getline(ss, str2, delim)) {
	outputs.push_back(str2);
      }
      */
      ;
    }
    else if(str == ".end") break;
  }

  int num_cycle = -1;
  int num_node = -1;
  int num_reg = -1;
  int num_spx = -1;
  int num_hdx = -1;
  
  for(auto itr = assignment.begin(); itr != assignment.end(); ++itr) {
    std::string key = itr->first;
    std::string value = itr->second;
#ifdef DEBUG
    std::cout << key << std::endl;
#endif
    if(key.substr(0,3) == "reg") {
      int pos0 = key.find("c", 3);
      int pos1 = key.find("n", 3);
      int pos2 = key.find("r", 3);
#ifdef DEBUG
      std::cout << key.substr(pos0, pos1-pos0) << ","  << key.substr(pos1, pos2-pos1) << ","  << key.substr(pos2) << std::endl;
#endif
      int c = std::stoi(key.substr(pos0+1, pos1-pos0-1));
      int n = std::stoi(key.substr(pos1+1, pos2-pos1-1));
      int r = std::stoi(key.substr(pos2+1));
      if(num_cycle < c) num_cycle = c;
      if(num_node < n) num_node = n;
      if(num_reg < r) num_reg = r;
    }
    else if (key.substr(0,3) == "spx") {
      int pos0 = key.find("c", 3);
      int pos1 = key.find("from", 3);
      int pos2 = key.find("to", 3);
      int pos3 = key.find("k", 3);
#ifdef DEBUG
      std::cout << key.substr(pos0, pos1-pos0) << ","  << key.substr(pos1, pos2-pos1) << ","  << key.substr(pos2, pos3-pos2) << "," << key.substr(pos3) << std::endl;
#endif
      int c = std::stoi(key.substr(pos0+1, pos1-pos0-1));
      int from = std::stoi(key.substr(pos1+4, pos2-pos1-4));
      int to = std::stoi(key.substr(pos2+2, pos3-pos2-2));
      int k = std::stoi(key.substr(pos3+1));
      if(num_spx < k) num_spx = k;
    }
    else if (key.substr(0,3) == "hdx") {
      int pos0 = key.find("c", 3);
      int pos1 = key.find("from", 3);
      int pos2 = key.find("to", 3);
      int pos3 = key.find("k", 3);
      int hdx_out = 0;
      if(pos1 == std::string::npos) {
	hdx_out = 1;
	pos1 = key.find("n", 3);
	pos2 = key.find("n", pos1+1);
	pos3 = key.find("k", 3);
      }
#ifdef DEBUG
      std::cout << key.substr(pos0, pos1-pos0) << ","  << key.substr(pos1, pos2-pos1) << ","  << key.substr(pos2, pos3-pos2) << "," << key.substr(pos3) << std::endl;
#endif
      if(hdx_out == 0) {
	int c = std::stoi(key.substr(pos0+1, pos1-pos0-1));
	int from = std::stoi(key.substr(pos1+4, pos2-pos1-4));
	int to = std::stoi(key.substr(pos2+2, pos3-pos2-2));
	int k = std::stoi(key.substr(pos3+1));
	if(num_hdx < k) num_hdx = k;
      }
      else {
	int c = std::stoi(key.substr(pos0+1, pos1-pos0-1));
	int n1 = std::stoi(key.substr(pos1+1, pos2-pos1-1));
	int n2 = std::stoi(key.substr(pos2+1, pos3-pos2-1));
	int k = std::stoi(key.substr(pos3+1));
	if(num_hdx < k) num_hdx = k;
      }
    }
  }

  num_cycle++;
  num_node++;
  num_reg++;
  num_spx++;
  num_hdx++;

  //array[cycle][node][count]
  std::vector<std::vector<std::vector<std::string> > > regs;
  regs = std::vector<std::vector<std::vector<std::string> > >(num_cycle, std::vector<std::vector<std::string> >(num_node, std::vector<std::string>(num_reg, "")));
  
  //array[cycle][fm][to][count]
  std::vector<std::vector<std::vector<std::vector<std::string> > > > spxs;
  spxs = std::vector<std::vector<std::vector<std::vector<std::string> > > >(num_cycle, std::vector<std::vector<std::vector<std::string> > >(num_node, std::vector<std::vector<std::string> >(num_node, std::vector<std::string>(num_spx, ""))));
  
  //array[cycle][fm][to][count]
  std::vector<std::vector<std::vector<std::vector<std::string> > > > hdx_ins;
  hdx_ins = std::vector<std::vector<std::vector<std::vector<std::string> > > >(num_cycle, std::vector<std::vector<std::vector<std::string> > >(num_node, std::vector<std::vector<std::string> >(num_node, std::vector<std::string>(num_hdx, ""))));

  //array[cycle][n1][n2][count]
  std::vector<std::vector<std::vector<std::vector<std::string> > > > hdx_outs;
  hdx_outs = std::vector<std::vector<std::vector<std::vector<std::string> > > >(num_cycle, std::vector<std::vector<std::vector<std::string> > >(num_node, std::vector<std::vector<std::string> >(num_node, std::vector<std::string>(num_hdx, ""))));
  
  
  for(auto itr = assignment.begin(); itr != assignment.end(); ++itr) {
    std::string key = itr->first;
    std::string value = itr->second;
#ifdef DEBUG
    std::cout << key << std::endl;
#endif
    if(key.substr(0,3) == "reg") {
      int pos0 = key.find("c", 3);
      int pos1 = key.find("n", 3);
      int pos2 = key.find("r", 3);
#ifdef DEBUG
      std::cout << key.substr(pos0, pos1-pos0) << ","  << key.substr(pos1, pos2-pos1) << ","  << key.substr(pos2) << "<-" << value << std::endl;
#endif
      int c = std::stoi(key.substr(pos0+1, pos1-pos0-1));
      int n = std::stoi(key.substr(pos1+1, pos2-pos1-1));
      int r = std::stoi(key.substr(pos2+1));
      regs[c][n][r] = value;
    }
    else if (key.substr(0,3) == "spx") {
      int pos0 = key.find("c", 3);
      int pos1 = key.find("from", 3);
      int pos2 = key.find("to", 3);
      int pos3 = key.find("k", 3);
#ifdef DEBUG
      std::cout << key.substr(pos0, pos1-pos0) << ","  << key.substr(pos1, pos2-pos1) << ","  << key.substr(pos2, pos3-pos2) << "," << key.substr(pos3) << std::endl;
#endif
      int c = std::stoi(key.substr(pos0+1, pos1-pos0-1));
      int from = std::stoi(key.substr(pos1+4, pos2-pos1-4));
      int to = std::stoi(key.substr(pos2+2, pos3-pos2-2));
      int k = std::stoi(key.substr(pos3+1));
      spxs[c][from][to][k] =  value;
    }
    else if (key.substr(0,3) == "hdx") {
      int pos0 = key.find("c", 3);
      int pos1 = key.find("from", 3);
      int pos2 = key.find("to", 3);
      int pos3 = key.find("k", 3);
      int hdx_out = 0;
      if(pos1 == std::string::npos) {
	hdx_out = 1;
	pos1 = key.find("n", 3);
	pos2 = key.find("n", pos1+1);
	pos3 = key.find("k", 3);
      }
#ifdef DEBUG
      std::cout << key.substr(pos0, pos1-pos0) << ","  << key.substr(pos1, pos2-pos1) << ","  << key.substr(pos2, pos3-pos2) << "," << key.substr(pos3) << std::endl;
#endif
      if(hdx_out == 0) {
	int c = std::stoi(key.substr(pos0+1, pos1-pos0-1));
	int from = std::stoi(key.substr(pos1+4, pos2-pos1-4));
	int to = std::stoi(key.substr(pos2+2, pos3-pos2-2));
	int k = std::stoi(key.substr(pos3+1));
	hdx_ins[c][from][to][k] = value;
      }
      else {
	int c = std::stoi(key.substr(pos0+1, pos1-pos0-1));
	int n1 = std::stoi(key.substr(pos1+1, pos2-pos1-1));
	int n2 = std::stoi(key.substr(pos2+1, pos3-pos2-1));
	int k = std::stoi(key.substr(pos3+1));
	hdx_outs[c][n1][n2][k] = value;
      }
    }
  }

  
#ifdef DEBUG
  for(unsigned int c = 0; c < regs.size(); c++) {
    for(unsigned int b = 0; b < regs[c].size(); b++) {
      for(unsigned int r = 0; r < regs[c][b].size(); r++) {
	std::cout << regs[c][b][r] << ",";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
  
  for(unsigned int c = 0; c < spxs.size()-1; c++) {
    for(unsigned int b = 0; b < spxs[c].size(); b++) {
      for(unsigned int r = 0; r < spxs[c][b].size(); r++) {
	for(unsigned int k = 0; k < spxs[c][b][r].size(); k++) {
	  std::cout << spxs[c][b][r][k] << ",";
	}
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
#endif
  
  std::map<std::string, std::string> value;
  std::map<std::string, int> isSpxUsed;
  
  for(unsigned int c = 0; c < num_cycle; c++) {
    
    for(unsigned int b = 0; b < regs[c].size(); b++) {
      for(unsigned int r = 0; r < regs[c][b].size(); r++) {
	str = regs[c][b][r];
	if(str.substr(0,3) == "reg") {
	  str = value[str];
	}
	if(str.substr(0,3) == "spx") {
	  isSpxUsed[str] = 1;
	  str = value[str];
	}
	if(str.substr(0,3) == "hdx") {
	  str = value[str];
	}
	std::string key = "reg_c" + std::to_string(c) + "n" + std::to_string(b) + "r" + std::to_string(r);
	value[key] = str;
      }
    }
    
    for(unsigned int b = 0; b < spxs[c].size(); b++) {
      for(unsigned int r = 0; r < spxs[c][b].size(); r++) {
	for(unsigned int k = 0; k < spxs[c][b][r].size(); k++) {
	  str = spxs[c][b][r][k];
	  str = value[str];
	  std::string key = "spx_c" + std::to_string(c) + "from" + std::to_string(b) + "to" + std::to_string(r) + "k" + std::to_string(k);
	  value[key] = str;
	}
      }
    }

    for(unsigned int b = 0; b < hdx_ins[c].size(); b++) {
      for(unsigned int r = 0; r < hdx_ins[c][b].size(); r++) {
	for(unsigned int k = 0; k < hdx_ins[c][b][r].size(); k++) {
	  str = hdx_ins[c][b][r][k];
	  str = value[str];
	  std::string key = "hdx_c" + std::to_string(c) + "from" + std::to_string(b) + "to" + std::to_string(r) + "k" + std::to_string(k);
	  value[key] = str;
	}
      }
    }
   
    for(unsigned int b = 0; b < hdx_outs[c].size(); b++) {
      for(unsigned int r = 0; r < hdx_outs[c][b].size(); r++) {
	for(unsigned int k = 0; k < hdx_outs[c][b][r].size(); k++) {
	  str = hdx_outs[c][b][r][k];
	  str = value[str];
	  std::string key = "hdx_c" + std::to_string(c) + "n" + std::to_string(b) + "n" + std::to_string(r) + "k" + std::to_string(k);
	  value[key] = str;
	}
      }
    }
  }


#ifdef DEBUG
  for(unsigned int c = 0; c < regs.size(); c++) {
    for(unsigned int b = 0; b < regs[c].size(); b++) {
      for(unsigned int r = 0; r < regs[c][b].size(); r++) {
	std::string key = "reg_c" + std::to_string(c) + "n" + std::to_string(b) + "r" + std::to_string(r);
	std::cout << value[key] << ",";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
  
  for(unsigned int c = 0; c < spxs.size()-1; c++) {
    for(unsigned int b = 0; b < spxs[c].size(); b++) {
      for(unsigned int r = 0; r < spxs[c][b].size(); r++) {
	for(unsigned int k = 0; k < spxs[c][b][r].size(); k++) {
	  std::string key = "spx_c" + std::to_string(c) + "from" + std::to_string(b) + "to" + std::to_string(r) + "k" + std::to_string(k);
	  std::cout << value[key] << ",";
	}
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
#endif
  std::map<std::string, int> value_int;
  std::vector<std::string> values;
  for(auto itr = value.begin(); itr != value.end(); ++itr) {
    values.push_back(itr->second);
  }
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  int value_count = 1;
  for(auto str : values) {
    if(value_int[str] == 0) {
      value_int[str] = value_count;
      value_count++;
    }
  }

  for(unsigned int b = 0; b < regs[0].size(); b++) {
    for(unsigned int c = 0; c < regs.size(); c++) {
      for(unsigned int r = 0; r < regs[c][b].size(); r++) {
	std::string key = "reg_c" + std::to_string(c) + "n" + std::to_string(b) + "r" + std::to_string(r);
	if(value[key] == "zero") continue;
	mesh_file << c << " " << b << " " << value_int[value[key]] << std::endl;
      }
    }
  }

  if(argc > 2) {
    for(unsigned int c = 0; c < spxs.size()-1; c++) {
      for(unsigned int b = 0; b < spxs[c].size(); b++) {
	for(unsigned int r = 0; r < spxs[c][b].size(); r++) {
	  for(unsigned int k = 0; k < spxs[c][b][r].size(); k++) {
	    std::string key = "spx_c" + std::to_string(c) + "from" + std::to_string(b) + "to" + std::to_string(r) + "k" + std::to_string(k);
	    if(isSpxUsed[key]) {
	      con_file << std::to_string(c) << " " << value_int[value[key]] << " " << std::to_string(b) << " ";
	      if(b < r) {
		if(r == b+1) con_file << "E";
		else con_file << "S";
	      }
	      else {
		if(r == b-1) con_file << "W";
		else con_file << "N";
	      }
	      con_file << std::endl;
	    }
	  }
	}
      }
    }
  }


  flush(mesh_file);
  
  //  std::string command = "python3 mesh.py " + std::string(mesh_file_name) + argv[2];
  //  system(command.c_str());
  return 0;
}
