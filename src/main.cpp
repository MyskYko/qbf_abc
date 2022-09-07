#include <iostream>

#include "qbf_abc.hpp"

// spec.blif ... spec file.
// impl.blif ... file to implement.

//  - assertion
// You cannot use same name model and same name internal signal in top model.
// "#." is used for description of structure. Simple way to avoid error is use double hash("##") when you want to comment out something.
// You should designate top module by "#.top module_name"

//  - impl.blif
// File has blank wire whose name begins with "x".
// File has "#.candidates wire0 wire1 ... x". 
// One of those candidates will be assigned to x.

// "#.group x3 x2 x1 x0" assignes "x"s to the candidate of same place in "#.candidates".
// "#.onehot wire_a" means wire_a shoud be used for one x. You can also call this like "#.onehot wire_a wire_b".


int main(int argc, char **argv) {
  // prepare to read file
  if(argc < 4) {
    std::cout << "./a.out spec.blif impl.blif output.blif" << std::endl;
    return 1;
  }

  SolveQBF(argv[1], argv[2], argv[3]);

  return 0;
}
