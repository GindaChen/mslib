//STL Includes
#include<fstream>
#include<string>
#include<vector>
#include<iostream>

//MSL Includes

#include "PDBReader.h"
#include "PDBWriter.h"
#include "System.h"
#include "Matrix.h"
#include "Chain.h"  //New
#include "MatrixWindow.h"
#include "DistanceMatrix.h"
#include "OptionParser.h"
#include "DistanceMatrixDatabase.h" //new
#include "Transforms.h"
#include "MslTools.h"
#include "AtomSelection.h"
#include "ManageDistanceMatrixResults.h"
#include "AtomPointerVector.h"
#include "multiSearchDM.h"

using namespace std;
using namespace MslTools;
using namespace MSL;

int main(int argc, char *argv[]){
    
    // Option Parser
    Options opt = setupOptions(argc,argv);

    ifstream fs2;

    //create system and dm for first PDB
    PDBReader reader;
    reader.open(opt.inputPDB);
    reader.read();
    reader.close();

    System *constSys = new System(reader.getAtomPointers());

    // load the external matrix window binary file
    DistanceMatrixDatabase dmd;
    //dmd.load_checkpoint("dmd.12.bin");
    dmd.load_checkpoint(opt.dmd);

    // List of distance matrices, one for each PDB
    vector<DistanceMatrix *> &DMVec = dmd.getDistanceMatrixList();

//new variable myChians
    vector <Chain*> myChains = constSys->getChains();

//add CA atoms to the atom vectors 
//for Chains A+B
    for (int i=0; i<myChains.size(); i++){

        Chain *A = myChains[i];
	
	for (int j=i+1; j<myChains.size(); j++){

	    Chain *B = myChains[j];

	    cout <<"Working on Round "<<A->getChainId()<<":"<<B->getChainId()<<endl;

	//Combination of Chains A and B    
	    DistanceMatrix constDM;

   	    for (int l=0; l<A->positionSize(); l++){
	        Residue &tempRes=A->getResidue(l);
	        if (tempRes.atomExists("CA")){
	            constDM.addAtom(tempRes("CA"));
	        }
    	    }//end for on l
	    for (int m=0; m<B->positionSize(); m++){
                Residue &tempRes=B->getResidue(m);
                if (tempRes.atomExists("CA")){
                    constDM.addAtom(tempRes("CA"));
                }   
            }//end for on m
    
    //fill the DistanceMatrix and set window size
	    constDM.setGeneralWinSize(opt.windowSize);
	    constDM.createDistanceMatrix();
	    constDM.setIntraChain(opt.intraChainCompare);
	    constDM.setPDBid(opt.inputPDB);
	    constDM.setDebug(opt.debug);

    //create matrix windows
	    constDM.createMatrixWindows();

	    if (constDM.getMatrixWindows().size()==0){
		    cout<<"Uh-oh.All the windows got filtered in the PDB you wanted to compare against."<<endl;
		    exit(111);
	    }

	//ManageResults to take care of printing/sorting at end
  	    ManageDistanceMatrixResults resultManager;
	    
	    for(int n=0; n<DMVec.size(); n++){

	    	cout<< "Trying "<<DMVec[n]->getPDBid()<<" ("<<n<<") # Residues: "<<DMVec.size()<<" Number of MatrixWindows to compare: "<<DMVec[n]->getMatrixWindows().size();

	    //don't compare if all of the windows got filtered out
	    	if (DMVec[n]->getMatrixWindows().size() == 0){
		    cout << " Sorry Zero Matrix Windows !"<<endl;
		    continue;
	    	}
	    
	   	cout <<endl;

	    	vector<DistanceMatrixResult> resultsToAdd;

	    	if(opt.searchCriteria=="standard"){
			resultsToAdd = constDM.multiCompareAllWindows(*DMVec[n], DistanceMatrix::standard, opt.numberOfIterations);
	    	}//end if
	    	if(opt.searchCriteria=="diagonal"){
			resultsToAdd = constDM.multiCompareAllWindows(*DMVec[n], DistanceMatrix::diag, opt.numberOfIterations);
       	    	}
	    	if(opt.searchCriteria=="doubleDiagonal"){
			resultsToAdd = constDM.multiCompareAllWindows(*DMVec[n], DistanceMatrix::doubleDiag, opt.numberOfIterations);
	    	}	
	    	if(opt.searchCriteria=="minDistance"){
			resultsToAdd = constDM.multiCompareAllWindows(*DMVec[n], DistanceMatrix::minDist, opt.numberOfIterations);
	    	}
	    	if(opt.searchCriteria=="minDistanceRow"){
			resultsToAdd = constDM.multiCompareAllWindows(*DMVec[n], DistanceMatrix::minDistRow, opt.numberOfIterations);
	    	}

	    	bool addFlag = false;
 	    	for (uint y = 0; y< resultsToAdd.size();y++){
 		    if (opt.likenessTolerance == MslTools::doubleMax || resultsToAdd[j].getLikeness() <= opt.likenessTolerance){
			    addFlag = true;
			    break;
 		    }
	    	}//end for on y 

	    	if (addFlag &&  resultsToAdd.size() > 0){

		    resultManager.addResults(resultsToAdd);	    		    
	    	}

	    }//end for on n

    
	    cout << "Printing"<<endl;
	    resultManager.setAlignPdbs(opt.alignPdbs);
	    resultManager.setRmsdTol(opt.rmsdTol);
	    
	    resultManager.printResults();
	    
	    cout<<"Done with Round "<<A->getChainId()<<":"<<B->getChainId()<<endl<<endl;

	}//end for on j

    }//end for on i

    cout << "Done."<<endl;
    return 0;
}




void getRMSD(MatrixWindow *_win1, MatrixWindow *_win2, System *_sys2){
    Transforms t;
    AtomPointerVector ca1 = (*_win1).getSmallAVec();
    AtomPointerVector ca2 = (*_win2).getSmallAVec();
    AtomPointerVector a2 = (*_sys2).getAtomPointers();

    bool result = t.rmsdAlignment(ca2, ca1, a2);
    if(!result){
	cout<<"Alignment has failed!"<<endl;
	exit(1211);
    }

    double r=ca1.rmsd(ca2);
    fprintf(stdout, "RMSD: %8.3f\n", r);

    //write out aligned pdb

    /*char a[80];
      sprintf(a, "/snap/cluster/jdegrado/pizza/%03d.aligned.%03d.pdb",j,i);

      PDBWriter w(a);
      w.write(a2);
      w.close();*/

}


Options setupOptions(int theArgc, char * theArgv[]){
	Options opt;

	OptionParser OP;

	OP.readArgv(theArgc, theArgv);
	OP.setRequired(opt.required);
	OP.setAllowed(opt.optional);

	if (OP.countOptions() == 0){
		cout << "Usage:" << endl;
		cout << endl;
		cout << "multiSearchDM --inputPDB PDB --searchCriteria SEARCH_STRING --windowSize NUM  ( --pdbList pdb_list --binaryDMs BINARY_DM) [ --numberOfIterations NUM --allowIntraChainCompare  --likenessTolernace TOL --alignPdbs  --rmsdTol TOL --debug ]\n";
		cout << endl<<"\tsearchCriteria can be:\n";
		cout << "\tstandard       - sum of the diff. for every item between two MatrixWindows.\n";
		cout << "\tdiagnol        - sum of the diff. for the diagnol (top left-bottom right) between two MatrixWindows.\n";
		cout << "\tdoubleDiagnol  - sum of the diff. for both diagnols between two MatrixWindows.\n";
		cout << "\tminDistance    - sum of the diff. between the minimal distance for each row,col of the given MatrixWindows.\n";
		cout << "\tminDistanceRow - sum of the diff. between the minimal distance for each row of the given MatrixWindows.\n";
		cout << "\n\n";
		cout << "Either pdbList OR binaryDMs are used for input structures, however pdbList is commented out at the momement 2/9/10 dwkulp.\n";
		exit(0);
	}

	opt.inputPDB = OP.getString("inputPDB");
	if (OP.fail()){
		cerr << "ERROR 1111 inputPDB not specified.\n";
		exit(1111);
	}

	opt.pdbList = OP.getString("pdbList");
	if (OP.fail()){
	  opt.pdbList = "";
	}

	opt.searchCriteria = OP.getString("searchCriteria");
	if (OP.fail()){
		cerr <<"ERROR 1111 searchCriteria not specified."<<endl;
		exit(1111);
	}

	opt.windowSize = OP.getInt("windowSize");
	if (OP.fail()){
		cerr << "ERROR 1111 windowSize not specified."<<endl;
		exit(1111);
	}

	opt.numberOfIterations = OP.getInt("numberOfIterations");
	if (OP.fail()){
		opt.numberOfIterations = 1;
	}

	opt.intraChainCompare = OP.getBool("allowIntraChainCompare");
	if (OP.fail()){
		opt.intraChainCompare = false;
	}


	opt.likenessTolerance = OP.getDouble("likenessTolerance");
	if (OP.fail()){
		opt.likenessTolerance = MslTools::doubleMax;
	}

	opt.alignPdbs         = OP.getBool("alignPdbs");
	if (OP.fail()){
		opt.alignPdbs = false;
	}

	opt.rmsdTol   = OP.getDouble("rmsdTol");
	if (OP.fail()) {
		opt.rmsdTol = 2.0;
	}
	
	opt.dmd = OP.getString("binaryDMs");
	if (OP.fail()){
	  opt.dmd = "";
	  if (opt.pdbList == ""){
	    cerr << "ERROR 1111 Need to specify either pdbList or binaryDMs, <= pdbList not implemented at the momement"<<endl;
	    exit(1111);
	  }
	}

	opt.debug = OP.getBool("debug");
	if (OP.fail()){
		opt.debug = false;
	}


	return opt;
}
