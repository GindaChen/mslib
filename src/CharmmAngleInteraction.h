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

#ifndef CHARMMANGLEINTERACTION_H
#define CHARMMANGLEINTERACTION_H

#include <iostream>
#include <vector>
#include <string>

#include "ThreeBodyInteraction.h"
#include "CharmmEnergy.h"


namespace MSL { 
class CharmmAngleInteraction: public ThreeBodyInteraction {

	/*******************************************************
	 *   Inherits from TwoBodyInteraction (a prototype object
	 *   for the interaction of two atoms)
	 *******************************************************/

	public:
		CharmmAngleInteraction();
		CharmmAngleInteraction(Atom & _a1, Atom & _a2, Atom & _a3, double _Ktheta, double _Theta0Radians); // min angle theta0 must be in radians

		// should implement an operator= as well 
		CharmmAngleInteraction(const CharmmAngleInteraction & _interaction);
		~CharmmAngleInteraction();

		/* setting and getting the parameters */
		void setParams(std::vector<double> _params);
		void setParams(double _Ktheta, double _Theta0Radians);
		double getMinAngle() const;
		double getConstant() const;
		
		double getEnergy();
		double getEnergy(double _angleDegrees,std::vector<double> *_ad=NULL);

		std::vector<double> getEnergyGrad();
		std::vector<double> getEnergyGrad(Atom& a1, Atom& a2, Atom& a3, double Ktheta, double Theta0Radians);


		friend std::ostream & operator<<(std::ostream &_os, CharmmAngleInteraction & _term) {_os << _term.toString(); return _os;};
		std::string toString() ;

		//unsigned int getType() const;
		std::string getName() const;
		void setName(std::string _name);
		
	private:
		void setup(Atom * _pA1, Atom * _pA2, Atom * _pA3, double _Ktheta, double _Theta0Radians);
		void copy(const CharmmAngleInteraction & _interaction);
		//static const unsigned int type = 3;
		std::string typeName;
		

};

inline void CharmmAngleInteraction::setParams(std::vector<double> _params) { if (_params.size() != 2) {std::cerr << "ERROR 49123: invalid number of parameters in inline void CharmmAngleInteraction::setParams(std::vector<double> _params)" << std::endl; exit(49123);} params = _params;}
inline void CharmmAngleInteraction::setParams(double _Ktheta, double _Theta0Radians) {params[0] = _Ktheta; params[1] = _Theta0Radians;}
inline double CharmmAngleInteraction::getMinAngle() const {return params[1];};
inline double CharmmAngleInteraction::getConstant() const {return params[0];};
inline double CharmmAngleInteraction::getEnergy() {
	return CharmmEnergy::instance()->spring(pAtoms[0]->angleRadians(*pAtoms[1], *pAtoms[2]), params[0], params[1]); 
}
inline double CharmmAngleInteraction::getEnergy(double _angleDegrees,std::vector<double> *_ad) {
	return CharmmEnergy::instance()->spring(_angleDegrees * M_PI / 180.0, params[0], params[1],_ad);
}
inline std::string CharmmAngleInteraction::toString() { 
	char c [1000]; 
	sprintf(c, "CHARMM ANGL %s %s %s %9.4f %9.4f %9.4f %20.6f", pAtoms[0]->toString().c_str(), pAtoms[1]->toString().c_str(), pAtoms[2]->toString().c_str(), params[0], params[1] * 180.0 / M_PI, pAtoms[0]->angle(*pAtoms[1], *pAtoms[2]), getEnergy()); 
	return (std::string)c; 
}
//inline unsigned int CharmmAngleInteraction::getType() const {return type;}
inline std::string CharmmAngleInteraction::getName() const {return typeName;}
inline void CharmmAngleInteraction::setName(std::string _name) {typeName = _name;}

inline std::vector<double> CharmmAngleInteraction::getEnergyGrad(){
	return getEnergyGrad(*pAtoms[0],*pAtoms[1],*pAtoms[2],params[0],params[1]);
}

}

#endif

