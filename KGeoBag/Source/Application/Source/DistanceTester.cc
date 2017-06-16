#include "KGCoreMessage.hh"
#include "KGInterfaceBuilder.hh"
#include "KGVTKGeometryPainter.hh"
#include "KGVTKDistanceTester.hh"

#include "KMessage.h"
#include "KTextFile.h"
#include "KCommandLineTokenizer.hh"
#include "KXMLTokenizer.hh"
#include "KVariableProcessor.hh"
#include "KIncludeProcessor.hh"
#include "KLoopProcessor.hh"
#include "KConditionProcessor.hh"
#include "KElementProcessor.hh"
#include "KTagProcessor.hh"
#include "KVTKWindow.h"

#ifdef KGeoBag_USE_ROOT
#include "KFormulaProcessor.hh"
#endif

using namespace KGeoBag;
using namespace katrin;
using namespace std;

int main( int argc, char** argv )
{
    if( argc < 3 )
    {
        cout << "usage: ./DistanceTester <config_file_name.xml> <geometry_path>" << endl;
        return -1;
    }

    string tFileName( argv[ 1 ] );
    string tPath( argv[ 2 ] );

    KXMLTokenizer tTokenizer;
    KVariableProcessor tVariableProcessor;
    KIncludeProcessor tIncludeProcessor;
    KLoopProcessor tLoopProcessor;
    KConditionProcessor tConditionProcessor;
    KTagProcessor tTagProcessor;
    KElementProcessor tElementProcessor;

    tVariableProcessor.InsertAfter( &tTokenizer );
    tIncludeProcessor.InsertAfter( &tVariableProcessor );

  #ifdef KGeoBag_USE_ROOT
  	KFormulaProcessor tFormulaProcessor;
  	tFormulaProcessor.InsertAfter( &tVariableProcessor );
  	tIncludeProcessor.InsertAfter( &tFormulaProcessor );
  #endif

    tLoopProcessor.InsertAfter( &tIncludeProcessor );
    tConditionProcessor.InsertAfter( &tLoopProcessor );
    tTagProcessor.InsertAfter( &tConditionProcessor );
    tElementProcessor.InsertAfter( &tTagProcessor );

    coremsg( eNormal ) << "starting initialization..." << eom;

    KTextFile tFile;
    tFile.SetDefaultPath( SCRATCH_DEFAULT_DIR );
    tFile.AddToNames( tFileName );
    tTokenizer.ProcessFile( &tFile );

    coremsg( eNormal ) << "...initialization finished" << eom;

    KVTKWindow tWindow;
    tWindow.SetName( "KGeoBag Distance Tester" );
    tWindow.SetFrameColorRed( 0. );
    tWindow.SetFrameColorGreen( 0. );
    tWindow.SetFrameColorBlue( 0. );
    tWindow.SetDisplayMode( true );
    tWindow.SetWriteMode( true );

    KGVTKGeometryPainter tPainter;
    tPainter.SetName( "GeometryPainter" );
    tPainter.SetDisplayMode( true );
    tPainter.SetWriteMode( true );

    KGVTKDistanceTester tTester;
    tTester.SetName( "DistanceTester" );
    tTester.SetDisplayMode( true );
    tTester.SetWriteMode( true );
    tTester.SetSampleDiskOrigin( KThreeVector( 0., 0., 1. ) );
    tTester.SetSampleDiskNormal( KThreeVector( 0., 0., 1. ) );
    tTester.SetSampleDiskRadius( .7 );
    tTester.SetSampleCount( 100000 );
    tTester.SetVertexSize( 1.0 );

    vector< KGSurface* > tSurfaces = KGInterface::GetInstance()->RetrieveSurfaces( tPath );
    vector< KGSurface* >::iterator tSurfaceIt;

    vector< KGSpace* > tSpaces = KGInterface::GetInstance()->RetrieveSpaces( tPath );
    vector< KGSpace* >::iterator tSpaceIt;

    for( tSurfaceIt = tSurfaces.begin(); tSurfaceIt != tSurfaces.end(); tSurfaceIt++ )
    {
        tPainter.AddSurface( *tSurfaceIt );
        tTester.AddSurface( *tSurfaceIt );
    }
    for( tSpaceIt = tSpaces.begin(); tSpaceIt != tSpaces.end(); tSpaceIt++ )
    {
        tPainter.AddSpace( *tSpaceIt );
        tTester.AddSpace( *tSpaceIt );
    }

    tWindow.AddPainter( &tPainter );
    tWindow.AddPainter( &tTester );
    tWindow.Render();
    tWindow.Write();
    tWindow.Display();
    tWindow.RemovePainter( &tPainter );
    tWindow.RemovePainter( &tTester );

    return 0;
}
