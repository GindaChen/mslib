/*
----------------------------------------------------------------------------
This file is part of MSL (Molecular Simulation Library)n
 Copyright (C) 2009 Dan Kulp, Alessandro Senes, Jason Donald, Brett Hannigan

This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, 
 USA, or go to http://www.gnu.org/copyleft/lesser.txt.
----------------------------------------------------------------------------
*/

#include <string>
#include <sstream>
#include <vector>
#include <ostream>
#include <fstream>
#include <cmath>



#include "tableEnergies.h"
#include "Quench.h"
#include "OptionParser.h"
#include "PDBWriter.h"
#include "System.h"

using namespace std;


int main(int argc, char *argv[]){

	
	// Option Parser
	Options opt = setupOptions(argc,argv);

	TwoBodyDistanceDependentPotentialTable tbd;
	tbd.readPotentialTable(opt.potfile);

	System initialSystem;
	initialSystem.readPdb(opt.pdb);

	PairwiseEnergyCalculator pec(opt.parfile);
	double energy = pec.calculateTotalEnergy(initialSystem, tbd);

	cout << "Total: " << energy << endl;
	if (opt.deltaG) {
		vector<Chain *> theChains = initialSystem.getChains(); 
		double chainEnergies = 0.;
		for (uint i = 0; i < theChains.size(); i++) {
			System thisChain(*theChains[i]);
			double thisEnergy = pec.calculateTotalEnergy(thisChain, tbd);
			chainEnergies += thisEnergy;
			cout << "Chain " << theChains[i]->getChainId() << ": " << thisEnergy << endl;
		}
		energy = energy - chainEnergies;
		cout << "Delta G energy: " << energy << endl;
	}

	return 0;
}


Options setupOptions(int theArgc, char * theArgv[]){
	Options opt;

	OptionParser OP;
	OP.setRequired(opt.required);
	OP.setAllowed(opt.optional);
	OP.readArgv(theArgc, theArgv);


	if (OP.countOptions() == 0){
		cout << "Usage:" << endl;
		cout << endl;
		cout << "tableEnergies --pdb PDB [--potfile KBFILE --parfile PARFILE --deltaG]\n";
		cout << "For deltaG option, energy is calculated for total, then individual chain energies is subtracted out" << endl;
		exit(0);
	}

	opt.pdb  = OP.getString("pdb");
	if (OP.fail()){
		cerr << "ERROR 1111 pdb not specified.\n";
		exit(1111);
	}

	opt.potfile = OP.getString("potfile");
	if (OP.fail()){
		cerr << "WARNING no potfile specified, using default /snap/cluster/jedonald/projects/KB_Spring2009/convert2MSL/DPHER_dfalpha1.61_rcut14.5_lcalpha0.5.equate.tbd\n";
		opt.potfile = "/snap/cluster/jedonald/projects/KB_Spring2009/convert2MSL/DPHER_dfalpha1.61_rcut14.5_lcalpha0.5.equate.tbd";
	}

	opt.parfile = OP.getString("parfile");
	if (OP.fail()){
		cerr << "WARNING no parfile specified, using default /library/charmmTopPar/par_all22_prot.inp\n";
		opt.parfile = "/library/charmmTopPar/par_all22_prot.inp";
	}

	opt.deltaG = OP.getBool("deltaG");

	return opt;
}