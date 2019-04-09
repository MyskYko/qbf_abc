#include <iostream>
#include <string>
#include <template_data.h>
#include <time.h>

int main(int argc, char **argv) {
  // prepare to read file
  if(argc < 2) {
    std::cout << "./a.out setting.txt (flag_show_detail)" << std::endl;
    return -1;
  }
  
  std::string spec_filename = "__spec.blif";
  std::string setting_filename = std::string(argv[1]);
  std::string log_filename = std::string(argv[1]) + ".qbf.log";
  std::string qbf_filename = std::string(argv[1]) + ".qbf.blif";
  std::string out_filename = std::string(argv[1]) + ".qbf.out.blif";
  std::string impl_filename = "__impl.blif";
  bool flag_show_detail = (argc > 2);

  clock_t start = clock();
  template_data t;
  if(t.setup(setting_filename) || t.write_circuit(impl_filename) || t.write_spec(spec_filename)) {
    std::cout << "error" << std::endl;
    return 1;
  }
  if(flag_show_detail) t.print();
  clock_t end = clock();
  std::string log = "generated template, " + std::to_string((double)(end - start) / CLOCKS_PER_SEC) + "sec";
  std::cout << log << std::endl;
  std::string command = "echo \"" + log + "\" > " + log_filename;
  system(command.c_str());
  
  extern int synthesis(std::string spec_filename, std::string impl_filename, std::string qbf_filename, std::string out_filename, std::string log_filename, int fVerbose);

  synthesis(spec_filename, impl_filename, qbf_filename, out_filename, log_filename, flag_show_detail);

  return 0;
}
