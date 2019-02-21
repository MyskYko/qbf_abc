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
  if(argc < 3) {
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
  std::ofstream dot_file;
  const char* dot_file_name = "__dot";
  dot_file.open(dot_file_name, std::ios::out);
  if(!dot_file) {
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
  
  for(unsigned int c = 0; c < num_cycle; c++) {
    
    for(unsigned int b = 0; b < regs[c].size(); b++) {
      for(unsigned int r = 0; r < regs[c][b].size(); r++) {
	str = regs[c][b][r];
	if(str.substr(0,3) == "reg") {
	  str = value[str];
	}
	if(str.substr(0,3) == "spx") {
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

  dot_file << "digraph graph_name {\ngraph [ \ncharset = \"UTF-8\",\nrankdir = TB, ranksep = 0.5, newrank=true\n];\n";//, size=\"8.27,11.69\"
  dot_file << "node [\nshape = record,\nfontname = \"Migu 1M\",\nfontsize = 12,\n];\n";
  
  for(unsigned int b = 0; b < regs[0].size(); b++) {
    dot_file << "subgraph cluster_node" << b << " {\n";
    for(unsigned int c = 0; c < regs.size(); c++) {
      dot_file << "reg_c" << c << "n" << b << " [label = \"";
      for(unsigned int r = 0; r < regs[c][b].size(); r++) {
	std::string key = "reg_c" + std::to_string(c) + "n" + std::to_string(b) + "r" + std::to_string(r);
	dot_file << "<r" << r << ">" << value[key] << "|";
      }
      dot_file.seekp(-1, std::ios::cur);
      dot_file <<  "\"];\n";
    }

    for(unsigned int c = 0; c < spxs.size()-1; c++) {
      int spx_flag = 0;
      for(unsigned int r = 0; r < spxs[c][b].size(); r++) {
	for(unsigned int k = 0; k < spxs[c][b][r].size(); k++) {
	  if(spxs[c][b][r][k] != "") {
	    spx_flag = 1;
	    break;
	  }
	}
	if(spx_flag) break;
      }
      
      if(spx_flag) {
	dot_file << "spx_c" << c << "n" << b << " [label = \"";
	for(unsigned int r = 0; r < spxs[c][b].size(); r++) {
	  for(unsigned int k = 0; k < spxs[c][b][r].size(); k++) {
	    if(spxs[c][b][r][k] != "") {
	      std::string key = "spx_c" + std::to_string(c) + "from" + std::to_string(b) + "to" + std::to_string(r) + "k" + std::to_string(k);
	      dot_file << "<k" << r << k << ">" << value[key] << "|";
	    }
	  }
	}
	dot_file.seekp(-1, std::ios::cur);
	dot_file <<  "\"];\n";
      }
    }

    for(unsigned int c = 0; c < spxs.size()-1; c++) {
      for(unsigned int r = 0; r < spxs[c][b].size(); r++) {
	for(unsigned int k = 0; k < spxs[c][b][r].size(); k++) {
	  if(spxs[c][b][r][k] != "") {
	    str = spxs[c][b][r][k];
	    int pos = str.find("r", 3);
	    int r1 = std::stoi(str.substr(pos+1));
	    dot_file << "reg_c" << c << "n" << b << ":" << "r" << r1 << " -> " <<  "spx_c" << c << "n" << b << ":" << "k" << r << k << " ;\n";
	  }
	}
      }
    }


    for(unsigned int c = 0; c < hdx_ins.size()-1; c++) {
      int hdx_in_flag = 0;
      for(unsigned int r = 0; r < hdx_ins[c][b].size(); r++) {
	for(unsigned int k = 0; k < hdx_ins[c][b][r].size(); k++) {
	  if(hdx_ins[c][b][r][k] != "") {
	    hdx_in_flag = 1;
	    break;
	  }
	}
	if(hdx_in_flag) break;
      }
      
      if(hdx_in_flag) {
	dot_file << "hdx_c" << c << "n" << b << " [label = \"";
	for(unsigned int r = 0; r < hdx_ins[c][b].size(); r++) {
	  for(unsigned int k = 0; k < hdx_ins[c][b][r].size(); k++) {
	    if(hdx_ins[c][b][r][k] != "") {
	      std::string key = "hdx_c" + std::to_string(c) + "from" + std::to_string(b) + "to" + std::to_string(r) + "k" + std::to_string(k);
	      dot_file << "<k" << r << k << ">" << value[key] << "|";
	    }
	  }
	}
	dot_file.seekp(-1, std::ios::cur);
	dot_file <<  "\"];\n";
      }
    }

    for(unsigned int c = 0; c < hdx_ins.size()-1; c++) {
      for(unsigned int r = 0; r < hdx_ins[c][b].size(); r++) {
	for(unsigned int k = 0; k < hdx_ins[c][b][r].size(); k++) {
	  if(hdx_ins[c][b][r][k] != "") {
	    str = hdx_ins[c][b][r][k];
	    int pos = str.find("r", 3);
	    int r1 = std::stoi(str.substr(pos+1));
	    dot_file << "reg_c" << c << "n" << b << ":" << "r" << r1 << " -> " <<  "hdx_c" << c << "n" << b << ":" << "k" << r << k << " ;\n";
	  }
	}
      }
    }

    dot_file <<  "}\n";
  }

  for(unsigned int c = 0; c < hdx_outs.size()-1; c++) {
    for(unsigned int b = 0; b < hdx_outs[c].size(); b++) {
      for(unsigned int r = b+1; r < hdx_outs[c][b].size(); r++) {
	int hdx_out_flag = 0;
	for(unsigned int k = 0; k < hdx_outs[c][b][r].size(); k++) {
	  if(hdx_outs[c][b][r][k] != "") {
	    hdx_out_flag = 1;
	    break;
	  }
	}
	
	if(hdx_out_flag) {
	  dot_file << "hdx_c" << c << "n" << b << "n" << r << " [label = \"";
	  for(unsigned int k = 0; k < hdx_outs[c][b][r].size(); k++) {
	    if(hdx_outs[c][b][r][k] != "") {
	      std::string key = "hdx_c" + std::to_string(c) + "n" + std::to_string(b) + "n" + std::to_string(r) + "k" + std::to_string(k);
	      dot_file << "<k" << k << ">" << value[key] << "|";
	    }
	  }
	  dot_file.seekp(-1, std::ios::cur);
	  dot_file <<  "\"];\n";
	}
      }
    }    
  }
  
  for(unsigned int c = 0; c < hdx_outs.size()-1; c++) {
    for(unsigned int b = 0; b < hdx_outs[c].size(); b++) {
      for(unsigned int r = b+1; r < hdx_outs[c][b].size(); r++) {
	for(unsigned int k = 0; k < hdx_outs[c][b][r].size(); k++) {
	  if(hdx_outs[c][b][r][k] != "") {
	    str = hdx_outs[c][b][r][k];
	    int pos1 = str.find("from", 3);
	    int pos2 = str.find("to", 3);
	    int from = std::stoi(str.substr(pos1+4, pos2-pos1-4));
	    if(from == b) {
	      dot_file << "hdx_c" << c << "n" << b << ":" << "k" << r << k << " -> " << "hdx_c" << c << "n" << b << "n" << r << ":" << "k" << k << " ;\n";
	    }
	    else {
	      dot_file << "hdx_c" << c << "n" << r << ":" << "k" << b << k << " -> " << "hdx_c" << c << "n" << b << "n" << r << ":" << "k" << k << " ;\n";
	    }
	  }
	}
      }
    }
  }
  
  
  for(unsigned int c = 0; c < regs.size(); c++) {
    for(unsigned int b = 0; b < regs[c].size(); b++) {
      for(unsigned int r = 0; r < regs[c][b].size(); r++) {
	if(regs[c][b][r].substr(0,3) == "spx") {
	  str = regs[c][b][r];
	  int pos0 = str.find("c", 3);
	  int pos1 = str.find("from", 3);
	  int pos2 = str.find("to", 3);
	  int pos3 = str.find("k", 3);
	  int c2 = std::stoi(str.substr(pos0+1, pos1-pos0-1));
	  int from2 = std::stoi(str.substr(pos1+4, pos2-pos1-4));
	  int to2 = std::stoi(str.substr(pos2+2, pos3-pos2-2));
	  int k2 = std::stoi(str.substr(pos3+1));

	  dot_file <<  "spx_c" << c2 << "n" << from2 << ":" << "k" << to2<< k2 << " -> " <<  "reg_c" << c << "n" << b << ":" << "r" << r << " [minlen = 2.0];\n";
	}
	else if(regs[c][b][r].substr(0,3) == "hdx") {
	  str = regs[c][b][r];

	  int pos0 = str.find("c", 3);
	  int pos1 = str.find("n", 3);
	  int pos2 = str.find("n", pos1+1);
	  int pos3 = str.find("k", 3);

	  int c2 = std::stoi(str.substr(pos0+1, pos1-pos0-1));
	  int n1 = std::stoi(str.substr(pos1+1, pos2-pos1-1));
	  int n2 = std::stoi(str.substr(pos2+1, pos3-pos2-1));
	  int k2 = std::stoi(str.substr(pos3+1));

	  dot_file <<  "hdx_c" << c2 << "n" << n1 << "n" << n2 << ":" << "k" << k2 << " -> " <<  "reg_c" << c << "n" << b << ":" << "r" << r << " [minlen = 2.0];\n";
	}
      }
    }
  }
  
  dot_file << "}\n";

  flush(dot_file);
  
  std::string command = "dot -T png " + std::string(dot_file_name) + " -o " + argv[2];
  system(command.c_str());
  return 0;
}
