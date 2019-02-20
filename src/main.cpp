#include <iostream>
#include <string>
#include <template_data.h>

int main(int argc, char **argv) {
  // prepare to read file
  if(argc < 4) {
    std::cout << "./a.out spec.blif setting.txt output.blif (flag_show_detail)" << std::endl;
    return -1;
  }
  
  std::string spec_filename = std::string(argv[1]);
  std::string setting_filename = std::string(argv[2]);
  std::string out_filename = std::string(argv[3]);
  std::string impl_filename = "__impl.blif";
  bool flag_show_detail = (argc > 4);
  
  template_data t;
  t.setup(setting_filename);
  t.gen_reg();
  t.gen_com();
  t.set_reg();
  t.set_com();
  t.set_out();
  t.write_circuit(impl_filename);
  t.print();
  
  extern int synthesis(std::string spec_filename, std::string impl_filename, std::string out_filename, int fVerbose);

  synthesis(spec_filename, impl_filename, out_filename, flag_show_detail);

  return 0;
}
