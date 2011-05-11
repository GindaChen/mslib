/*
----------------------------------------------------------------------------
This file is part of MSL (Molecular Software Libraries)
 Copyright (C) 2010 Dan Kulp, Alessandro Senes, Jason Donald, Brett Hannigan,
 Sabareesh Subramaniam, Ben Mueller

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


#include "CharmmEEF1Interaction.h"

using namespace MSL;
using namespace std;



CharmmEEF1Interaction::CharmmEEF1Interaction() {
	setup(NULL, NULL, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

CharmmEEF1Interaction::CharmmEEF1Interaction(Atom & _a1, Atom & _a2, double _V_i, double _Gfree_i, double _Sigw_i, double _rmin_i, double _V_j, double _Gfree_j, double _Sigw_j, double _rmin_j) {
	setup (&_a1, &_a2, _V_i, _Gfree_i, _Sigw_i, _rmin_i, _V_j, _Gfree_j, _Sigw_j, _rmin_j);
}

CharmmEEF1Interaction::CharmmEEF1Interaction(const CharmmEEF1Interaction & _interaction) {
	setup(NULL, NULL, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	copy(_interaction);
}

CharmmEEF1Interaction::~CharmmEEF1Interaction() {
}




void CharmmEEF1Interaction::setup(Atom * _pA1, Atom * _pA2, double _V_i, double _Gfree_i, double _Sigw_i, double _rmin_i, double _V_j, double _Gfree_j, double _Sigw_j, double _rmin_j) {
	pAtoms = vector<Atom*> (2, (Atom*)NULL);
	setAtoms(*_pA1, *_pA2);	
	params = vector<double>(8, 0.0);
	setParams(_V_i, _Gfree_i, _Sigw_i, _rmin_i, _V_j, _Gfree_j, _Sigw_j, _rmin_j);
	useNonBondCutoffs = false;
	nonBondCutoffOn = 997;
	nonBondCutoffOff = 998;
	typeName = "CHARMM_EEF1";
}

void CharmmEEF1Interaction::copy(const CharmmEEF1Interaction & _interaction) {
	pAtoms = _interaction.pAtoms;
	params = _interaction.params;
	typeName = _interaction.typeName;
	useNonBondCutoffs = _interaction.useNonBondCutoffs;
	nonBondCutoffOn = _interaction.nonBondCutoffOn;
	nonBondCutoffOff = _interaction.nonBondCutoffOff;
}

