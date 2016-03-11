#include "KSRootElectricField.h"
#include "KSFieldsMessage.h"

namespace Kassiopeia
{

    KSRootElectricField::KSRootElectricField() :
        fCurrentPotential(),
        fCurrentField(),
        fElectricFields( 128 )
    {
    }
    KSRootElectricField::KSRootElectricField( const KSRootElectricField& aCopy ) :
            KSComponent(),
            fCurrentPotential( aCopy.fCurrentPotential ),
            fCurrentField( aCopy.fCurrentField ),
            fElectricFields( aCopy.fElectricFields )
    {
    }
    KSRootElectricField* KSRootElectricField::Clone() const
    {
        return new KSRootElectricField( *this );
    }
    KSRootElectricField::~KSRootElectricField()
    {
    }

    void KSRootElectricField::CalculatePotential( const KThreeVector& aSamplePoint, const double& aSampleTime, double& aPotential )
    {
        aPotential = 0.;
        for( int tIndex = 0; tIndex < fElectricFields.End(); tIndex++ )
        {
            fElectricFields.ElementAt( tIndex )->CalculatePotential( aSamplePoint, aSampleTime, fCurrentPotential );
            aPotential += fCurrentPotential;
        }
        return;
    }
    void KSRootElectricField::CalculateField( const KThreeVector& aSamplePoint, const double& aSampleTime, KThreeVector& aField )
    {
        aField = KThreeVector::sZero;
        for( int tIndex = 0; tIndex < fElectricFields.End(); tIndex++ )
        {
            fElectricFields.ElementAt( tIndex )->CalculateField( aSamplePoint, aSampleTime, fCurrentField );
            aField += fCurrentField;
        }
        return;
    }
    void KSRootElectricField::CalculateFieldAndPotential( const KThreeVector& aSamplePoint, const double& aSampleTime, KThreeVector& aField, double& aPotential )
    {
        aField = KThreeVector::sZero;
        aPotential = 0.;
        for( int tIndex = 0; tIndex < fElectricFields.End(); tIndex++ )
        {
            fElectricFields.ElementAt( tIndex )->CalculateFieldAndPotential( aSamplePoint, aSampleTime, fCurrentField, fCurrentPotential );
            aField += fCurrentField;
            aPotential += fCurrentPotential;
        }
        return;
    }

    void KSRootElectricField::AddElectricField( KSElectricField* anElectricField )
    {
        //check that field is not already present
        for( int tIndex = 0; tIndex < fElectricFields.End(); tIndex++ )
        {
            if(anElectricField == fElectricFields.ElementAt( tIndex ) )
            {
                fieldmsg_debug( "<" << GetName() << "> attempted to add electric field <" << anElectricField->GetName() << "> which is already present."  << eom );
                return;
            }
        }

        if( fElectricFields.AddElement( anElectricField ) == -1 )
        {
            fieldmsg( eError ) << "<" << GetName() << "> could not add electric field <" << anElectricField->GetName() << ">" << eom;
            return;
        }
        fieldmsg_debug( "<" << GetName() << "> adding electric field <" << anElectricField->GetName() << ">" << eom );
        return;
    }
    void KSRootElectricField::RemoveElectricField( KSElectricField* anElectricField )
    {
        if( fElectricFields.RemoveElement( anElectricField ) == -1 )
        {
            fieldmsg( eError ) << "<" << GetName() << "> could not remove electric field <" << anElectricField->GetName() << ">" << eom;
            return;
        }
        fieldmsg_debug( "<" << GetName() << "> removing electric field <" << anElectricField->GetName() << ">" << eom );
        return;
    }

    STATICINT sKSRootElectricFieldDict =
        KSDictionary< KSRootElectricField >::AddCommand( &KSRootElectricField::AddElectricField, &KSRootElectricField::RemoveElectricField, "add_electric_field", "remove_electric_field" );

}
