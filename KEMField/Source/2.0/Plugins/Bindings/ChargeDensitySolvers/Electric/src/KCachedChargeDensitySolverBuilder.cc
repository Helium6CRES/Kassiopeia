/*
 * KCachedChargeDensitySolverBuilder.cc
 *
 *  Created on: 17 Jun 2015
 *      Author: wolfgang
 */

#ifndef KEMFIELD_SOURCE_2_0_PLUGINS_BINDINGS_CHARGEDENSITYSOLVERS_SRC_KCACHEDCHARGEDENSITYSOLVERBUILDER_CC_
#define KEMFIELD_SOURCE_2_0_PLUGINS_BINDINGS_CHARGEDENSITYSOLVERS_SRC_KCACHEDCHARGEDENSITYSOLVERBUILDER_CC_

#include "KCachedChargeDensitySolverBuilder.hh"
#include "KElectrostaticBoundaryFieldBuilder.hh"

using namespace KEMField;

namespace katrin {

template< >
KCachedChargeDensitySolverBuilder::~KComplexElement()
{
}

STATICINT sKCachedChargeDensitySolverStructure =
		KCachedChargeDensitySolverBuilder::Attribute< string >( "name" ) +
		KCachedChargeDensitySolverBuilder::Attribute< string >( "hash" );

STATICINT sKElectrostaticBoundaryField =
KElectrostaticBoundaryFieldBuilder::ComplexElement< KCachedChargeDensitySolver >( "cached_bem_solver" ) +
KElectrostaticBoundaryFieldBuilder::ComplexElement< KCachedChargeDensitySolver >( "cached_charge_density_solver" ) ;
}



#endif /* KEMFIELD_SOURCE_2_0_PLUGINS_BINDINGS_CHARGEDENSITYSOLVERS_SRC_KCACHEDCHARGEDENSITYSOLVERBUILDER_CC_ */
