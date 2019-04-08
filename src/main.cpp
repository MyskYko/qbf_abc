#include <iostream>
#include <string>
#include <template_data.h>

int main(int argc, char **argv) {
  // prepare to read file
  if(argc < 2) {
    std::cout << "./a.out setting.txt (flag_show_detail)" << std::endl;
    return -1;
  }
  
  std::string spec_filename = "__spec.blif";
  std::string setting_filename = std::string(argv[1]);
  std::string out_filename = std::string(argv[1]) + ".out.blif";
  std::string impl_filename = "__impl.blif";
  bool flag_show_detail = (argc > 2);
  
  template_data t;
  if(t.setup(setting_filename) || t.write_circuit(impl_filename) || t.write_spec(spec_filename)) {
    std::cout << "error" << std::endl;
    return 1;
  }
  if(flag_show_detail) t.print();
  
  extern int synthesis(std::string spec_filename, std::string impl_filename, std::string out_filename, int fVerbose);

  synthesis(spec_filename, impl_filename, out_filename, flag_show_detail);

  return 0;
}
