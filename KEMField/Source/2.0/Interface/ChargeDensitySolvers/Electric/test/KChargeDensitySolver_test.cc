/*
 * KChargeDensitySolver_test.cc
 *
 *  Created on: 25 Jun 2015
 *      Author: wolfgang
 */

#include "KSmartPointer.hh"
#include "KChargeDensitySolver.hh"
#include "KGaussianEliminationChargeDensitySolver.hh"

using namespace KEMField;

int main(int /*argc*/, char** /*args*/) {
	KSmartPointer<KChargeDensitySolver> ptr(NULL);
	{
		KGaussianEliminationChargeDensitySolver* raw = new KGaussianEliminationChargeDensitySolver;
		KSmartPointer<KChargeDensitySolver> ptr1(raw);
		ptr = ptr1;
	}
	KSmartPointer<KSurfaceContainer> smartContainer = new KSurfaceContainer;
	ptr->Initialize(*smartContainer);
	return 0;
}



