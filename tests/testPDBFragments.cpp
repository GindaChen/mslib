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
#include "testData.h"
#include "PDBFragments.h"
#include "PDBWriter.h"


int main(){

	// Write some PDBs
	writePdbFile();

	System pdb;
	pdb.readPdb("/tmp/xtalLattice.pdb");

	cout << "Read in fragDB"<<endl;
	//PDBFragments fragDB("/home/dwkulp/pdbs/out2.fragdb");
	PDBFragments fragDB("/home/dwkulp/software/mslib/trunk/nr1000.fragdb");
	fragDB.loadFragmentDatabase();

	//fragDB.printMe();

	vector<int> stems;
	stems.push_back(16);
	stems.push_back(17);
	stems.push_back(21);
	stems.push_back(22);

	int numFrags = fragDB.searchForMatchingFragments(pdb.getChain("A"),stems);
	cout << "DONE SEARCHING!"<<endl;
	System &frags = fragDB.getLastSearchResults();

	fprintf(stdout, "SYSTEM SIZE: %d\n",frags.size());

	AtomVector &fragAts = frags.getAtoms();
	
	PDBWriter pout;


	for (uint i = 0; i < numFrags;i++){
		for (uint a = 0; a < fragAts.size();a++){
			fragAts(a).setActiveConformation(i);
		}

		char fname[100];
		sprintf(fname,"/tmp/frag-%06d.pdb",i);
		pout.open(fname);
		pout.write(fragAts);
		pout.close();		
	}
	
}