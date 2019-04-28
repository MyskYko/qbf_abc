#include <iostream>
#include <string>
#include <vector>
//for i in `seq 1 8`; do for j in `seq $(($i+1)) 8`; do echo $i $j;  done; done;

void run(int cycle, int len, std::vector<int> &pat) {
  std::string cmd = "timeout 30m ./run.sh SAT 8 8 3 3 " + std::to_string(cycle) + " 2 2 1 systolic onehot_spx_out sparse";
  for(int i = 0; i < len; i++) {
    cmd += "_" + std::to_string(pat[i]);
  }
  
  std::string log_filename = "log/log" + std::to_string(cycle);
  for(int i = 0; i < len; i++) {
    log_filename += "_" + std::to_string(pat[i]);
  }
  cmd += " > " + log_filename;
  //  std::cout << cmd << std::endl;
  std::system(cmd.c_str());
  
  std::string cmd2 = "grep took " + log_filename + "; grep error " + log_filename + "; grep succeeded " + log_filename;
  std::system(cmd2.c_str());
}
  
void recur(int cycle, int depth, int len, std::vector<int> &pat) {
  if(len == depth) {
    for(int i = 0; i < len; i++) {
      std:: cout <<  "_" + std::to_string(pat[i]);
    }
    std::cout << std::endl;
    run(cycle, len, pat);
    return;
  }
  
  int last;
  if(depth == 0) last = 0;
  else last = pat[depth-1];

  for(int i = last + 1; i <= 8; i++) {
    pat[depth] = i;
    recur(cycle, depth + 1, len, pat);
  }
}

int main (int argc, char ** argv) {
  int l = atoi(argv[1]);
  int c = atoi(argv[2]);
  std::vector<int> pat(l, 0);
  recur(c, 0, l, pat);
  return 0;
}
