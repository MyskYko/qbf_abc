#include <iostream>
#include <string>

int main(int argc, char **argv) {

  // prepare to read file
  if(argc < 4) {
    std::cout << "./a.out spec.blif impl.blif output.blif (flag_show_detail)" << std::endl;
    return -1;
  }

  bool flag_show_detail = (argc > 4);

  extern int synthesis(std::string spec_filename, std::string impl_filename, std::string out_filename, int fVerbose);

  synthesis(std::string(argv[1]), std::string(argv[2]), std::string(argv[3]), flag_show_detail);

  return 0;
}
