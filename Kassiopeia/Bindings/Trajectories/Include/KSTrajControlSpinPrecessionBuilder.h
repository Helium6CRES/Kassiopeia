#ifndef Kassiopeia_KSTrajControlSpinPrecessionBuilder_h_
#define Kassiopeia_KSTrajControlSpinPrecessionBuilder_h_

#include "KSTrajControlSpinPrecession.h"
#include "KComplexElement.hh"

using namespace Kassiopeia;
namespace katrin
{

    typedef KComplexElement< KSTrajControlSpinPrecession > KSTrajControlSpinPrecessionBuilder;

    template< >
    inline bool KSTrajControlSpinPrecessionBuilder::AddAttribute( KContainer* aContainer )
    {
        if( aContainer->GetName() == "name" )
        {
            aContainer->CopyTo( fObject, &KNamed::SetName );
            return true;
        }
        if( aContainer->GetName() == "fraction" )
        {
            aContainer->CopyTo( fObject, &KSTrajControlSpinPrecession::SetFraction );
            return true;
        }
        return false;
    }

}

#endif
