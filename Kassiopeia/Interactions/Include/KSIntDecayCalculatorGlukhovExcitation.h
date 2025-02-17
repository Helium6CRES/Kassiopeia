#ifndef Kassiopeia_KSIntDecayCalculatorGlukhovExcitation_h_
#define Kassiopeia_KSIntDecayCalculatorGlukhovExcitation_h_

#include "KSIntDecayCalculator.h"
#include "KField.h"

namespace Kassiopeia
{
    class KSIntDecayCalculatorGlukhovExcitation :
        public KSComponentTemplate< KSIntDecayCalculatorGlukhovExcitation, KSIntDecayCalculator >
    {
        public:
            KSIntDecayCalculatorGlukhovExcitation();
            KSIntDecayCalculatorGlukhovExcitation( const KSIntDecayCalculatorGlukhovExcitation& aCopy );
            KSIntDecayCalculatorGlukhovExcitation* Clone() const;
            virtual ~KSIntDecayCalculatorGlukhovExcitation();

        public:
            void CalculateLifeTime( const KSParticle& aParticle, double& aLifeTime );
            void ExecuteInteraction( const KSParticle& anInitialParticle,
                                     KSParticle& aFinalParticle,
                                     KSParticleQueue& aSecondaries );


        public:
            K_SET_GET( long long, TargetPID )
            K_SET_GET( long long, minPID )
            K_SET_GET( long long, maxPID )
            K_SET_GET( double, Temperature )

            private:
                static const double p_coefficients[3][4];
                double CalculateSpontaneousDecayRate(int n, int l);

                static const double b_ex[3][3][3];
                double a_ex(int l, int i, double T);
                static const double T_a_tilde;
                double tau(double T);
                double x(int n, double T);
                double CalculateRelativeExcitationRate(int n,int l,double T);
    };


}

#endif
