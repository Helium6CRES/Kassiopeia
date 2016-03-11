#include "KSVTKTrackPainterBuilder.h"
#include "KVTKWindow.h"

using namespace Kassiopeia;
namespace katrin
{

    STATICINT sKSVTKTrackPainterStructure =
        KSVTKTrackPainterBuilder::Attribute< string >( "name" ) +
        KSVTKTrackPainterBuilder::Attribute< string >( "file" ) +
        KSVTKTrackPainterBuilder::Attribute< string >( "path" ) +
        KSVTKTrackPainterBuilder::Attribute< string >( "outfile" ) +
        KSVTKTrackPainterBuilder::Attribute< string >( "point_object" ) +
        KSVTKTrackPainterBuilder::Attribute< string >( "point_variable" ) +
        KSVTKTrackPainterBuilder::Attribute< string >( "color_object" ) +
        KSVTKTrackPainterBuilder::Attribute< string >( "color_variable" );

    STATICINT sKSVTKTrackPainterWindow =
        KVTKWindowBuilder::ComplexElement< KSVTKTrackPainter >( "vtk_track_painter" );

}
