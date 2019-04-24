#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <functional>
#include <sstream>
#include <algorithm>

#include "utils/System.h"
#include "utils/ParseUtils.h"
#include "utils/Options.h"
#include "core/Dimacs.h"
#include "simp/SimpSolver.h"

int read_setting(std::ifstream &file, int &num_cycle, int &num_node, std::vector<std::vector<int> > &num_spx, std::vector<std::vector<std::string> > &init_assign, std::vector<std::vector<std::string> > &fin_assign, std::vector<std::string> &options) {
  num_cycle = 0;
  num_node = 0;
  num_spx.clear();
  init_assign.clear();
  fin_assign.clear();
  options.clear();

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
    else if(str == "initial") {
      if(!init_assign.empty()) {
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
	init_assign.push_back(initial_i);
      }
    }
    else if(str == "final") {
      if(!fin_assign.empty()) {
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
	fin_assign.push_back(final_i);
      }
    }
    else if(str == "onehot_spx_in") {
      options.push_back(str);
    }
    else if(str == "onehot_spx_out") {
      options.push_back(str);
    }
    else if(str == "implicit_reg") {
      options.push_back("imp_reg");
    }
    else if(str == "systolic") {
      options.push_back(str);
      options.push_back("imp_reg");
    }
    else if(str == "symmetric") {
      options.push_back(str);
    }
  }

  return 0;
}

void identify_var(std::vector<std::vector<std::string> > init_assign, std::vector<std::vector<std::string> > fin_assign, int &num_var, std::map<std::string, int> &var_id, std::map<int, std::string> &id_var, std::vector<std::vector<int> > &init_assign_id, std::vector<std::vector<int> > &fin_assign_id) {
  num_var = 0;
  var_id.clear();
  id_var.clear();
  init_assign_id.clear();
  fin_assign_id.clear();

  for(std::vector<std::string> row : init_assign) {
    std::vector<int> init_assign_id_row;
    for(std::string elem : row) {
      if (var_id.find(elem) == var_id.end()){
	var_id[elem] = num_var;
	id_var[num_var] = elem;
	num_var += 1;
      }
      init_assign_id_row.push_back(var_id[elem]);
    }
    init_assign_id.push_back(init_assign_id_row);
  }

  for(std::vector<std::string> row : fin_assign) {
    std::vector<int> fin_assign_id_row;
    for(std::string elem : row) {
      fin_assign_id_row.push_back(var_id[elem]);
    }
    fin_assign_id.push_back(fin_assign_id_row);
  }
}

void recursive_comb(int *indexes, int s, int rest, std::function<void(int *)> f) {
  if (rest == 0) {
    f(indexes);
  } else {
    if (s < 0) return;
    recursive_comb(indexes, s - 1, rest, f);
    indexes[rest - 1] = s;
    recursive_comb(indexes, s - 1, rest - 1, f);
  }
}

void foreach_comb(int n, int k, std::function<void(int *)> f) {
  int indexes[k];
  recursive_comb(indexes, n - 1, k, f);
}


void cnf_generate(Glucose::SimpSolver &S, int num_var, int num_node, int num_cycle, std::vector<std::vector<int> > init_assign, std::vector<std::vector<int> > num_spx, std::vector<std::vector<int> > fin_assign, std::vector<std::string> options, int &num_lit, int &num_cla) {
  num_lit = 0;
  num_cla = 0;
  
  int  num_con = 0;
  for(auto row : num_spx) {
    for(auto elem : row) {
      if (elem > 0) {
	num_con += 1;
      }
    }
  }
	  
  //v in n at c
  int num_assign = (num_cycle + 1) * num_node * num_var;
  num_lit += num_assign;
  //com v in k at c
  int num_com = num_cycle * num_con * num_var;
  num_lit += num_com;

  while(num_lit > S.nVars()) S.newVar(); 

  //initial assignment
  for(int j = 0; j < num_node; j++) {
    if(init_assign.size() <= (unsigned int)j) break;
    for(int i = 0; i < num_var; i++) {
      Glucose::Lit l = Glucose::mkLit(i + j * num_var);
      if (std::find(init_assign[j].begin(), init_assign[j].end(), i) == init_assign[j].end()) {
	S.addClause(~l);
      }
      else{
	S.addClause(l);
      }
    }
  }
  num_cla += num_node * num_var;

  //communication data ready
  //com i from j to k at l
  for(int l = 0; l < num_cycle; l++) {
    int count = 0;
    for(int k = 0; k < num_node; k++) {
      for(int j = 0; j< num_node; j++) {
	if(num_spx[j][k] > 0) {
	  for(int i = 0; i < num_var; i++) {
	    Glucose::Lit l1 = Glucose::mkLit(i + count * num_var + l * num_con * num_var + num_assign, true);
	    Glucose::Lit l2 = Glucose::mkLit(i + j * num_var + l * num_node * num_var);
	    S.addClause(l1, l2);
	  }
	  count += 1;
	}
      }
    }
  }
  num_cla += num_cycle * num_con * num_var;

  //communication
  //com i from j to k at l
  for(int l = 0; l < num_cycle; l++) {
    int count = 0;
    for(int k = 0; k < num_node; k++) {
      int count_ = 0;
      for(int i = 0; i < num_var; i++) {
	Glucose::vec<Glucose::Lit> ls;
	count_ = 0;
	ls.push(Glucose::mkLit(i + k * num_var + l * num_node * num_var));
	ls.push(Glucose::mkLit(i + k * num_var + (l + 1) * num_node * num_var, true));
	for(int j = 0; j < num_node; j++) {
	  if(num_spx[j][k] > 0) {
	    ls.push(Glucose::mkLit(i + (count + count_) * num_var + l * num_con * num_var + num_assign));
	    count_ += 1;
	  }
	}
	S.addClause(ls);
      }
      count += count_;
    }
  }
  num_cla += num_cycle * num_node * num_var;

  //final assignment
  for(int j = 0; j < num_node; j++) {
    for(int i = 0; i< num_var; i++) {
      if (std::find(fin_assign[j].begin(), fin_assign[j].end(), i) != fin_assign[j].end()) {
	if(std::find(options.begin(), options.end(), "imp_reg") == options.end()) {
	  S.addClause(Glucose::mkLit(i + j * num_var + num_cycle * num_node * num_var));
	}
	else {
	  Glucose::vec<Glucose::Lit> ls;	  
	  for(int k = 0; k < num_cycle + 1; k++) {
	    ls.push(Glucose::mkLit(i + j * num_var + k * num_node * num_var));
	  }
	  S.addClause(ls);
	}
      }
      else {
	if(std::find(options.begin(), options.end(), "imp_reg") == options.end()) {
	  S.addClause(Glucose::mkLit(i + j * num_var + num_cycle * num_node * num_var, true));
	}
      }
    }
  }
                
  for(int j = 0; j < num_node; j++) {
    if(std::find(options.begin(), options.end(), "imp_reg") == options.end()) {
      num_cla += num_var;
    }
    else {
      num_cla += (int)fin_assign[j].size();
    }
  }

  //sinplex connection
  //com i from j to k at l
  for(int l = 0; l < num_cycle; l++) {
    int count = 0;
    for(int k = 0; k < num_node; k++) {
      for(int j = 0; j < num_node; j++) {
	if(num_spx[j][k] > 0) {
	  foreach_comb(num_var, num_spx[j][k] + 1, [&](int *indexes) {
						     Glucose::vec<Glucose::Lit> ls;						     
						     for(int i = 0; i <= num_spx[j][k]; i++) {
						       ls.push(Glucose::mkLit(indexes[i] + count * num_var + l * num_con * num_var + num_assign, true));
						     }
						     S.addClause(ls);
						     num_cla += 1;
						   });
	  count += 1;
	}
      }
    }
  }

  //onehot spx in
  //com i from j to k at l
  if(std::find(options.begin(), options.end(), "onehot_spx_in") != options.end()) {
    for(int l = 0; l < num_cycle; l++) {
      int count = 0;
      for(int k = 0; k < num_node; k++) {
	int count_ = 0;
	for(int j = 0; j < num_node; j++) {
	  if(num_spx[j][k] > 0) {
	    count_ += 1;
	  }
	}
	if(count_ == 0) continue;
	for(int i0 = 0; i0 < count_ * num_var; i0++) {
	  for(int i1 = i0 + 1; i1 < count_ * num_var; i1++) {
	    Glucose::Lit l0 = Glucose::mkLit(i0 + count * num_var + l * num_con * num_var + num_assign, true);
	    Glucose::Lit l1 = Glucose::mkLit(i1 + count * num_var + l * num_con * num_var + num_assign, true);
	    S.addClause(l0, l1);
	    num_cla += 1;
	  }
	}
	count += count_;
      }
    }
  }

  //onehot spx out
  //com i from j to k at l
  if(std::find(options.begin(), options.end(), "onehot_spx_out") != options.end()) {
    std::vector<std::vector<int> > onehot_groups(num_node * num_cycle, std::vector<int>(0));
    for(int l = 0; l < num_cycle; l++) {
      int count = 0;
      for(int k = 0; k < num_node; k++) {
	for(int j = 0; j < num_node; j++) {
	  if(num_spx[j][k] > 0) {
	    for(int i = 0; i < num_var; i++) {
	      onehot_groups[j + l * num_node].push_back(i + count * num_var + l * num_con * num_var + num_assign + 1);
	    }
	    count += 1;
	  }
	}
      }
    }
    for(auto onehot_group : onehot_groups) {
      for(int i0 = 0; i0 < (int)onehot_group.size(); i0++) {
	for(int i1 = i0 + 1; i1 < (int)onehot_group.size(); i1++) {
	  Glucose::Lit l0 = Glucose::mkLit(onehot_group[i0], true);
	  Glucose::Lit l1 = Glucose::mkLit(onehot_group[i1], true);
	  S.addClause(l0, l1);
	  num_cla += 1;
	}
      }
    }
  }

  //systolic
  if(std::find(options.begin(), options.end(), "systolic") != options.end()) {
    //onehot var in node per cycle
    for(int k = 0; k < num_cycle + 1; k++) {
      for(int j = 0; j < num_node; j++) {
	for(int i0 = 0; i0 < num_var; i0++) {
	  for(int i1 = i0 + 1; i1 < num_var; i1++) {
	    Glucose::Lit l0 = Glucose::mkLit(i0 + j * num_var + k * num_node * num_var, true);
	    Glucose::Lit l1 = Glucose::mkLit(i1 + j * num_var + k * num_node * num_var, true);
	    S.addClause(l0, l1);
	    num_cla += 1;
	  }
	}
      }
    }
  }

  //symmetric
  //com i from j to k at l
  if(std::find(options.begin(), options.end(), "symmetric") != options.end()) {
    std::vector<int> com_types;
    for(int l = 0; l < num_cycle; l++) {
      int count = 0;
      for(int k = 0; k < num_node; k++) {
	for(int j = 0; j < num_node; j++) {
	  if(num_spx[j][k] > 0) {
	    if(std::find(com_types.begin(), com_types.end(), k - j) == com_types.end()) {
	      com_types.push_back(k - j);
	    }
	    int com_type = std::find(com_types.begin(), com_types.end(), k - j) - com_types.begin();
	    for(int i = 0; i < num_var; i++) {
	      Glucose::Lit l0 = Glucose::mkLit(i + count * num_var + l * num_con * num_var + num_assign, true);
	      Glucose::Lit l1 = Glucose::mkLit(com_type + l * (int)com_types.size() + num_com + num_assign);
	      S.addClause(l0, l1);
	    }
	    count += 1;
	  }
	}
      }
    }
    num_cla += num_cycle * num_con * num_var;
    num_lit += num_cycle * (int)com_types.size();
    for(int i0 = 0; i0 < (int)com_types.size(); i0++) {
      for(int i1 = 0; i1 < (int)com_types.size(); i1++) {
	for(int l = 0; l < num_cycle; l++) {
	  Glucose::Lit l0 = Glucose::mkLit(i0 + l * (int)com_types.size() + num_com + num_assign, true);
	  Glucose::Lit l1 = Glucose::mkLit(i1 + l * (int)com_types.size() + num_com + num_assign, true);
	  S.addClause(l0, l1);
	  num_cla += 1;
	}
      }
    }
  }
  
  //  f << "p cnf " << num_lit << " " << num_cla << "\n";
}
  /*
    #symmetric
    #com i from j to k at l
    if "symmetric" in options:
        com_types = []
        for l in range(0, num_cycle):
            count = 0
            for k in range(0, num_node):
                for j in range(0, num_node):
                    if num_spx[j][k] > 0:
                        if k - j not in com_types:
                            com_types.append(k - j)
                        com_type = com_types.index(k - j)
                        for i in range(0, num_var):
                            f.write("-" + str(i + count * num_var + l * num_con * num_var + num_assign + 1) + " ")
                            f.write(str(com_type + l * len(com_types) + num_com + num_assign + 1) + " 0\n")
                        count += 1
        num_cla += num_cycle * num_con * num_var
        num_lit += num_cycle * len(com_types)
        data = [i for i in range(0, len(com_types))]
        for pattern in itertools.combinations(data, 2):
            for l in range(0, num_cycle):
                for com_type in pattern:
                    f.write("-" + str(com_type + l * len(com_types) + num_com + num_assign + 1) + " ")
                f.write("0\n")
                num_cla += 1
        
    return num_lit, num_cla
*/

std::string text_omit_int(std::vector<std::vector<int> > array) {
  std::string str = "";
  for(std::vector<int> vec : array) {
    str += "[";
    for(int elem : vec) {
      str += std::to_string(elem) + ",";
    }
    str += "]";
    if(str.length() > 20) {
      str += "...";
      break;
    }
  }
  return str;
}
		  
std::string text_omit(std::vector<std::vector<std::string> > array) {
  std::string str = "";
  for(std::vector<std::string> vec : array) {
    str += "[";
    for(std::string elem : vec) {
      str += elem + ",";
    }
    str += "]";
    if(str.length() > 20) {
      str += "...";
      break;
    }
  }
  return str;
}
 
int main(int argc, char ** argv) {
  if(argc < 2) {
    std::cout << "setting file should be given as second command line parameter" << std::endl;
    return 1;
  }

  std::string setting_file_name = std::string(argv[1]);
  std::string sat_file_name = setting_file_name + ".sat.out";
  std::ifstream setting_file(setting_file_name);
  if (!setting_file) {
    std::cerr << "setting file should be given as second command line parameter" << std::endl;
    std::cerr << "setting file cannot be open" << std::endl;
    return 1;
  }
  std::ofstream sat_file(sat_file_name);
  if (!sat_file) {
    std::cerr << "setting file should be given as second command line parameter" << std::endl;
    std::cerr << "tmp_file(__[setting file].sat.blif) cannot be open" << std::endl;
    return 1;
  }

  int num_cycle, num_node;
  std::vector<std::vector<int> > num_spx;
  std::vector<std::vector<std::string> > init_assign, fin_assign;
  std::vector<std::string> options;
  if(read_setting(setting_file, num_cycle, num_node, num_spx, init_assign, fin_assign, options)) {
    std::cerr << "something wrong in read setting" << std::endl;
    return 1;
  }
  std::cout << "setting file:" << setting_file_name << std::endl;
  std::cout << "num_cycle: " << num_cycle << std::endl;
  std::cout << "num_node: " << num_node << std::endl;
  std::cout << "num_spx: " << text_omit_int(num_spx) << std::endl;
  std::cout << "initial assignment: " << text_omit(init_assign) << std::endl;
  std::cout << "final assignment: " << text_omit(fin_assign) << std::endl;
  for(auto option : options) {
    std::cout << option << std::endl;
  }
  
  setting_file.close();

  int num_var;
  std::map<std::string, int> var_id;
  std::map<int, std::string> id_var;
  std::vector<std::vector<int> > init_assign_id, fin_assign_id;
  identify_var(init_assign, fin_assign, num_var, var_id, id_var, init_assign_id, fin_assign_id);
    
  std::cout << "num_var: " << num_var << std::endl;
  std::cout << "initial assignment: " << text_omit_int(init_assign_id) << std::endl;
  std::cout << "final assignment: " << text_omit_int(fin_assign_id) << std::endl;

  int num_lit, num_cla;
  clock_t start = clock();
  Glucose::SimpSolver S;
  cnf_generate(S, num_var, num_node, num_cycle, init_assign_id, num_spx, fin_assign_id, options, num_lit, num_cla);
  bool r = S.solve();
  clock_t end = clock();
  
  if(r) {
    std::cout << "SAT" << std::endl;
    sat_file << "SAT" << std::endl;
    for(int i = 0; i < num_lit; i++) {
      if(S.model[i] == l_True) {
	sat_file << i + 1 << " ";
      }
      else {
	sat_file << "-" << i + 1 << " ";
      }
    }
  }
  else {
    std::cout << "UNSAT" << std::endl;
    sat_file << "UNSAT" << std::endl;
  }
  
  std::cout << "SAT took " << (double)(end - start) / CLOCKS_PER_SEC << "s in CNF file" << std::endl;
  std::cout << "There are " << num_lit << " literals, " << num_cla << " clauses" << std::endl;
  //  std::cout << "The CNF file is " << (int)sat_file.tellp() / 1024 << "KB" << std::endl;
  
  //  sat_file.close();

  return 0;
}
