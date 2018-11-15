#include "dDFIRE2.h"


// dDFIRE

DFIRE2::DFIRE2(std::string libPath) {
	ReadLib(libPath);
}



void DFIRE2::ReadLib(std::string libPath) {
	if (!FileExists(libPath))
		throw std::runtime_error("ERR: dDFIRE energy file not found!");

	std::string HEAD, AA_name1, AA_name2,
	    atom1, atom2,
	    atomtype1, atomtype2;

	int num_types, num_bins,
		count_now = 0, // for printing
	    pos = 1, // zero is reserved for unknown residues
	    total; // total atom pairs

	std::ifstream libFile(libPath);
	libFile >> HEAD >> num_types >> HEAD >> num_bins;
	total = num_types * (num_types + 1) / 2;
	
	nTypes = num_types; // set class variables
	nBins = num_bins;
	// 3D array  [nTypes * nTypes * nBins]
	edDFIRE = std::vector<std::vector<std::vector<double> > >(nTypes+1, std::vector<std::vector<double> >(nTypes+1, std::vector<double>(nBins+1)));


	while (libFile >> AA_name1 >> atom1 >> AA_name2 >> atom2) {
		atomtype1 = std::string() + residue_map[AA_name1] + " " + atom1;
		atomtype2 = std::string() + residue_map[AA_name2] + " " + atom2;
		if (atom_map.count(atomtype1) == false)
			atom_map[atomtype1] = pos++;
		if (atom_map.count(atomtype2) ==
		        false) atom_map[atomtype2] = pos++;

		int id1 = atom_map[atomtype1],
		    id2 = atom_map[atomtype2];

		if (id1 > num_types || id2 > num_types)
			throw std::runtime_error("Something wrong with the energy file\n");

		for (int i = 0; i < num_bins; ++i) {
			libFile >> edDFIRE[id1][id2][i];     // 30 bins of energy
			edDFIRE[id2][id1][i] = edDFIRE[id1][id2][i];  // symmetry
		}
		progress_indicator("dDFIRE energy file", count_now++, total);
	}
	progress_indicator("dDFIRE energy file", 1, 1);
	libReady = true;
}



double DFIRE2::Calculate_CFE(std::string& target_sequence,
                  std::array<std::array<double, MAX_PROT_LEN>, MAX_PROT_LEN> &target_atom_distance) {
	double eAll = 0;
	int n = target_sequence.size();
	std::vector<int> atom_id( n );

	for (int i = 0; i < n; ++i) {
		std::string key = std::string() + target_sequence[i] + " CA"; // char wont concant to string!
		atom_id[i] = atom_map[key];
	}

	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			int b = target_atom_distance[i][j] * 2;     // distance to bin
			if (b >= nBins)
				continue;
			eAll += edDFIRE[atom_id[i]][atom_id[j]][b];
		}
	}
	return eAll;
}