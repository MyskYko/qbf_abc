#pragma once

#include <string>

int SolveQBF(std::string specname, std::string implname, std::string outname, std::string prefix = "_", bool fVerbose = false, bool fVerbose2 = false);

int SolveQBFRelation(std::string specname, std::string implname, std::string outname, std::string prefix = "_", bool fVerbose = false, bool fVerbose2 = false);
