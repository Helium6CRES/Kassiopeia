#ifndef Kassiopeia_KSIntCalculatorHydrogen_h_
#define Kassiopeia_KSIntCalculatorHydrogen_h_

/** @file
 @brief contains
 @details
 <b>Revision History:</b>
 \verbatim
 Date       Name                Brief description
 -----------------------------------------------------
 2014-03-19 N.Trost, S.Groh     first version
 \endverbatim
 */

#include "KSIntCalculator.h"

namespace Kassiopeia
{

    /////////////////////////////////////
    /////		Elastic	Base		/////
    /////////////////////////////////////

    class KSIntCalculatorHydrogenElasticBase :
        public KSComponentTemplate< KSIntCalculatorHydrogenElasticBase, KSIntCalculator >
    {
        public:
            KSIntCalculatorHydrogenElasticBase();
            virtual ~KSIntCalculatorHydrogenElasticBase();

        public:
            virtual void CalculateCrossSection( const KSParticle& aParticle, double& aCrossSection );
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection ) = 0;
            virtual void ExecuteInteraction( const KSParticle& anInitialParticle, KSParticle& aFinalParticle, KSParticleQueue& aSecondaries );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss ) = 0;

        protected:
            void CalculateTheta( const double anEnergy, double& aTheta );
            void CalculateDifferentialCrossSection( const double anEnergy, const double cosTheta, double& aCrossSection );

    };

    /////////////////////////////////
    /////		Elastic			/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenElastic :
        public KSComponentTemplate< KSIntCalculatorHydrogenElastic, KSIntCalculatorHydrogenElasticBase >
    {
        public:
            KSIntCalculatorHydrogenElastic();
            KSIntCalculatorHydrogenElastic( const KSIntCalculatorHydrogenElastic& aCopy );
            KSIntCalculatorHydrogenElastic* Clone() const;
            virtual ~KSIntCalculatorHydrogenElastic();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

    };

    /////////////////////////////////
    /////		Vibration		/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenVib :
        public KSComponentTemplate< KSIntCalculatorHydrogenVib, KSIntCalculatorHydrogenElasticBase >
    {
        public:
            KSIntCalculatorHydrogenVib();
            KSIntCalculatorHydrogenVib( const KSIntCalculatorHydrogenVib& aCopy );
            KSIntCalculatorHydrogenVib* Clone() const;
            virtual ~KSIntCalculatorHydrogenVib();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

    };

    /////////////////////////////////
    /////		Rot02			/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenRot02 :
        public KSComponentTemplate< KSIntCalculatorHydrogenRot02, KSIntCalculatorHydrogenElasticBase >
    {
        public:
            KSIntCalculatorHydrogenRot02();
            KSIntCalculatorHydrogenRot02( const KSIntCalculatorHydrogenRot02& aCopy );
            KSIntCalculatorHydrogenRot02* Clone() const;
            virtual ~KSIntCalculatorHydrogenRot02();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

    };

    /////////////////////////////////
    /////		Rot13			/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenRot13 :
        public KSComponentTemplate< KSIntCalculatorHydrogenRot13, KSIntCalculatorHydrogenElasticBase >
    {
        public:
            KSIntCalculatorHydrogenRot13();
            KSIntCalculatorHydrogenRot13( const KSIntCalculatorHydrogenRot13& aCopy );
            KSIntCalculatorHydrogenRot13* Clone() const;
            virtual ~KSIntCalculatorHydrogenRot13();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

    };

    /////////////////////////////////
    /////		Rot20			/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenRot20 :
        public KSComponentTemplate< KSIntCalculatorHydrogenRot20, KSIntCalculatorHydrogenElasticBase >
    {
        public:
            KSIntCalculatorHydrogenRot20();
            KSIntCalculatorHydrogenRot20( const KSIntCalculatorHydrogenRot20& aCopy );
            KSIntCalculatorHydrogenRot20* Clone() const;
            virtual ~KSIntCalculatorHydrogenRot20();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

    };

    /////////////////////////////////////
    /////		Excitation Base		/////
    /////////////////////////////////////

    class KSIntCalculatorHydrogenExcitationBase :
        public KSComponentTemplate< KSIntCalculatorHydrogenExcitationBase, KSIntCalculator >
    {
        public:
            KSIntCalculatorHydrogenExcitationBase();
            virtual ~KSIntCalculatorHydrogenExcitationBase();

        public:
            virtual void CalculateCrossSection( const KSParticle& aParticle, double& aCrossSection );
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection ) = 0;
            virtual void ExecuteInteraction( const KSParticle& anInitialParticle, KSParticle& aFinalParticle, KSParticleQueue& aSecondaries );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss ) = 0;
            static void CalculateDifferentialCrossSection( const double anEnergy, const double cosTheta, double& aCrossSection );
            static double sumexc( double K );
            static double Lagrange( int n, double *xn, double *fn, double x );

        protected:
            void CalculateTheta( const double anEnergy, double& aTheta );

        public:
            void initialize_sum();

        private:
            double sum[ 1001 ];
            double T, c, K, xmin, ymin, ymax, x, y, fy, dy, D, Dmax, Ecen, fmax;
    };

    /////////////////////////////////
    /////		Excitation B	/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenExcitationB :
        public KSComponentTemplate< KSIntCalculatorHydrogenExcitationB, KSIntCalculatorHydrogenExcitationBase >
    {
        public:
            KSIntCalculatorHydrogenExcitationB();
            KSIntCalculatorHydrogenExcitationB( const KSIntCalculatorHydrogenExcitationB& aCopy );
            KSIntCalculatorHydrogenExcitationB* Clone() const;
            virtual ~KSIntCalculatorHydrogenExcitationB();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

        private:
            static const double EnergyLevels[ 28 ];
            static const double FrankCondonFactors[ 28 ];
            static const double CrossSectionParameters[ 9 ];
            static const double FCFactorsElectronicExcitation[7];

            double FrankCondonSum;
            double ElExcitationFCSum;
            double Emin;
    };

    /////////////////////////////////
    /////		Excitation C	/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenExcitationC :
        public KSComponentTemplate< KSIntCalculatorHydrogenExcitationC, KSIntCalculatorHydrogenExcitationBase >
    {
        public:
            KSIntCalculatorHydrogenExcitationC();
            KSIntCalculatorHydrogenExcitationC( const KSIntCalculatorHydrogenExcitationC& aCopy );
            KSIntCalculatorHydrogenExcitationC* Clone() const;
            virtual ~KSIntCalculatorHydrogenExcitationC();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

        private:
            static const double EnergyLevels[ 14 ];
            static const double FrankCondonFactors[ 14 ];
            static const double CrossSectionParameters[ 9 ];
            double FrankCondonSum;
            double Emin;
    };

    /////////////////////////////////
    /////   Dissoziation 10 eV	/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenDissoziation10 :
        public KSComponentTemplate< KSIntCalculatorHydrogenDissoziation10, KSIntCalculatorHydrogenExcitationBase >
    {
        public:
            KSIntCalculatorHydrogenDissoziation10();
            KSIntCalculatorHydrogenDissoziation10( const KSIntCalculatorHydrogenDissoziation10& aCopy );
            KSIntCalculatorHydrogenDissoziation10* Clone() const;
            virtual ~KSIntCalculatorHydrogenDissoziation10();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

        private:
            double Emin;
            static const double CrossSectionParameters[ 9 ];
    };

    /////////////////////////////////
    /////   Dissoziation 15 eV	/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenDissoziation15 :
        public KSComponentTemplate< KSIntCalculatorHydrogenDissoziation15, KSIntCalculatorHydrogenExcitationBase >
    {
        public:
            KSIntCalculatorHydrogenDissoziation15();
            KSIntCalculatorHydrogenDissoziation15( const KSIntCalculatorHydrogenDissoziation15& aCopy );
            KSIntCalculatorHydrogenDissoziation15* Clone() const;
            virtual ~KSIntCalculatorHydrogenDissoziation15();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

        private:
            double Emin;
            static const double CrossSectionParameters[ 9 ];
    };

    /////////////////////////////////
    ////  Electronic Excitation  ////
    /////////////////////////////////

    class KSIntCalculatorHydrogenExcitationElectronic :
        public KSComponentTemplate< KSIntCalculatorHydrogenExcitationElectronic, KSIntCalculatorHydrogenExcitationBase >
    {
        public:
            KSIntCalculatorHydrogenExcitationElectronic();
            KSIntCalculatorHydrogenExcitationElectronic( const KSIntCalculatorHydrogenExcitationElectronic& aCopy );
            KSIntCalculatorHydrogenExcitationElectronic* Clone() const;
            virtual ~KSIntCalculatorHydrogenExcitationElectronic();

        public:
            virtual void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            virtual void CalculateEloss( const double anEnergy, const double aTheta, double& anEloss );

        public:
            double Emin;
            static const double EnergyLevels[ 7 ];
            static const double FrankCondonFactors[ 7 ];
            double FrankCondonSum;
            double ExcitationSum;
            double ExcitationProbability;
            double pmax;
            int nElectronicStates;
    };

    /////////////////////////////////
    /////	  New Ionisation	/////
    /////////////////////////////////

    class KSIntCalculatorHydrogenIonisation :
        public KSComponentTemplate< KSIntCalculatorHydrogenIonisation, KSIntCalculator >
    {
        public:
            KSIntCalculatorHydrogenIonisation();
            KSIntCalculatorHydrogenIonisation( const double aCrossParam_A1, const double aCrossParam_A2, const double aCrossParam_A3, const double aCrossExponent, const double abeta, const double agamma, const double aG_5, const double ag_b, const double aBindingEnergy );
            KSIntCalculatorHydrogenIonisation( const KSIntCalculatorHydrogenIonisation& aCopy );
            KSIntCalculatorHydrogenIonisation* Clone() const;
            virtual ~KSIntCalculatorHydrogenIonisation();

        public:
            void CalculateCrossSection( const KSParticle& aParticle, double& aCrossSection );
            void CalculateCrossSection( const double anEnergy, double& aCrossSection );
            void ExecuteInteraction( const KSParticle& anInitialParticle, KSParticle& aFinalParticle, KSParticleQueue& aSecondaries );
            void CalculateDoublyDifferentialCrossSection( const double aReducedInitialEnergy, const double aReducedFinalEnergy, const double aCosTheta, double& aCrossSection );

        protected:
            void CalculateEnergyDifferentialCrossSection( const double aReducedInitialEnergy, const double aReducedFinalEnergy, double& aCrossSection );
            void CalculateTheta( const double aReducedInitialEnergy, const double aReducedFinalEnergy, double& aTheta );
            void CalculateFinalEnergy( const double aReducedInitialEnergy, double& aReducedFinalEnergy );

            double CrossParam_A1, CrossParam_A2, CrossParam_A3;
            double CrossExponent;
            double beta;
            double gamma;
            double G_5;
            double g_b;
            double BindingEnergy;
            double Normalisation;

            double Formfactor( const double aReducedEnergy );
            double g_1( const double aReducedEnergy );
            double g_BE( const double aReducedInitialEnergy, const double aReducedFinalEnergy );
            double f_1( const double aReducedInitialEnergy, const double aReducedFinalEnergy );
            double f_b( const double aCosTheta );
            double f_BE( const double aReducedInitialEnergy, const double aReducedFinalEnergy, const double aCosTheta );

            double G_1( const double aReducedInitialEnergy, const double aReducedFinalEnergy );
            double G_2( const double aReducedInitialEnergy, const double aReducedFinalEnergy );
            double G_3( const double aReducedInitialEnergy, const double aReducedFinalEnergy );
            double G_4( const double aReducedInitialEnergy, const double aReducedFinalEnergy );
    };

} /* namespace Kassiopeia */

#endif /* Kassiopeia_KSIntCalculatorHydrogen_h_ */
