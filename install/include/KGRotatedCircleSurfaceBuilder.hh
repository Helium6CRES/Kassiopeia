#ifndef KGROTATEDCIRCLESURFACEBUILDER_HH_
#define KGROTATEDCIRCLESURFACEBUILDER_HH_

#include "KGPlanarCircleBuilder.hh"
#include "KGRotatedCircleSurface.hh"

namespace katrin
{

    typedef KComplexElement< KGRotatedCircleSurface > KGRotatedCircleSurfaceBuilder;

    template< >
    inline bool KGRotatedCircleSurfaceBuilder::AddAttribute( KContainer* anAttribute )
    {
        if( anAttribute->GetName() == string( "name" ) )
        {
            anAttribute->CopyTo( fObject, &KGRotatedCircleSurface::SetName );
            return true;
        }
        if( anAttribute->GetName() == string( "rotated_mesh_count" ) )
        {
            anAttribute->CopyTo( fObject, &KGRotatedCircleSurface::RotatedMeshCount );
            return true;
        }
        return false;
    }

    template< >
    inline bool KGRotatedCircleSurfaceBuilder::AddElement( KContainer* anElement )
    {
        if( anElement->GetName() == string( "circle" ) )
        {
            anElement->CopyTo( fObject->Path().operator ->(), &KGPlanarCircle::CopyFrom );
            return true;
        }
        return false;
    }

}

#endif
