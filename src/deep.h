#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <map>
#include <random>
#include <set>
#include "protein.h"
#include "protein_profile.h"
#include "evaluator.h"
#include "constants.h"
#include "ThreadPool.h"
#include "utility.h"



class DeepAI {
public:
	void CheckFamilies(std::string sFDir);

	void GenerateDataset(std::string sFamDir,
	                     std::string fCSV,
	                     int nFamCutoff);

	void GenerateData(ProteinProfile& profile, Protein prot,
	                  std::ofstream& outFile, std::string sep = ",");

	double PairAlgnScore(std::string PDB_1, std::string PDB_2);
};

