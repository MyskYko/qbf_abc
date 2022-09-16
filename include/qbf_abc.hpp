#pragma once

#include <string>

int SolveQBF(std::string specname, std::string implname, std::string outname, int conflimit = 0, std::string prefix = "_", bool fVerbose = false, bool fVerbose2 = false);
