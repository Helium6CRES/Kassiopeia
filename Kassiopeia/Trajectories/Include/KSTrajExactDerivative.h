#ifndef Kassiopeia_KSTrajExactDerivative_h_
#define Kassiopeia_KSTrajExactDerivative_h_

#include "KSMathArray.h"

#include "KThreeVector.hh"
using KGeoBag::KThreeVector;

namespace Kassiopeia
{

    class KSTrajExactDerivative :
        public KSMathArray< 8 >
    {
        public:
            KSTrajExactDerivative();
            virtual ~KSTrajExactDerivative();

            //**********
            //assignment
            //**********

        public:
            KSTrajExactDerivative& operator=( const double& anOperand );

            KSTrajExactDerivative& operator=( const KSMathArray< 8 >& anOperand );

            template< class XLeft, class XOperation, class XRight >
            KSTrajExactDerivative& operator=( const KSMathExpression< XLeft, XOperation, XRight >& anOperand );

            KSTrajExactDerivative& operator=( const KSTrajExactDerivative& anOperand );

            //*********
            //variables
            //*********

        public:
            void AddToTime( const double& aTime );
            void AddToSpeed( const double& aSpeed );
            void AddToVelocity( const KThreeVector& aVelocity );
            void AddToForce( const KThreeVector& aForce );
    };

    inline KSTrajExactDerivative& KSTrajExactDerivative::operator=( const double& anOperand )
    {
        this->KSMathArray< 8 >::operator =( anOperand );
        return *this;
    }

    inline KSTrajExactDerivative& KSTrajExactDerivative::operator=( const KSMathArray< 8 >& anOperand )
    {
        this->KSMathArray< 8 >::operator =( anOperand );
        return *this;
    }

    template< class XLeft, class XOperation, class XRight >
    inline KSTrajExactDerivative& KSTrajExactDerivative::operator=( const KSMathExpression< XLeft, XOperation, XRight >& anOperand )
    {
        this->KSMathArray< 8 >::operator =( anOperand );
        return *this;
    }

    inline KSTrajExactDerivative& KSTrajExactDerivative::operator=( const KSTrajExactDerivative& anOperand )
    {
        this->KSMathArray< 8 >::operator =( anOperand );
        return *this;
    }

}

#endif
