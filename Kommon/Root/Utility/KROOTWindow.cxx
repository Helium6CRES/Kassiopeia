#include "KROOTWindow.h"
#include "KROOTPainter.h"
#include "KROOTPad.h"
#include "KUtilityMessage.h"

#include <cmath>
#include <limits>

#include <TStyle.h>

using namespace std;

namespace katrin
{

	KROOTWindow::KROOTWindow() :
			fPainters(),
			fPads(),
			fFrame( 0 ),
			fApplication( 0 ),
			fCanvas( 0 ),
			fCanvasWidth( 1000 ),
			fCanvasHeight( 600 ),
			fActive( true ),
			fWriteEnabled( false ),
			fPath( "" )
    {
    }

	KROOTWindow::~KROOTWindow()
    {
        return;
    }

    void KROOTWindow::Render()
    {
    	utilmsg( eNormal ) <<"KROOTWindow starts to render!"<<eom;

//        gStyle->SetPadBottomMargin(0.1);
//      	gStyle->SetPadRightMargin(0.1);
//       	gStyle->SetPadLeftMargin(0.1);
//       	gStyle->SetPadTopMargin(0.1);
        gStyle->SetTitleX( 0.5 );
        gStyle->SetTitleAlign( 23 );
        gStyle->SetTitleSize( 0.08 , "t" );

    	if ( gApplication )
    	{
    		fApplication = gApplication;
    	}
    	else
    	{
			fApplication = new TApplication("My ROOT Application", 0, 0 );
    	}
        fCanvas = new TCanvas( fName.c_str(), fName.c_str() , 10, 10, fCanvasWidth, fCanvasHeight);
        fCanvas->Connect("Closed()", "TApplication", fApplication, "Terminate()");

        double tXMin(std::numeric_limits<double>::max());
        double tXMax(-1.0*std::numeric_limits<double>::max());
        double tYMin(std::numeric_limits<double>::max());
        double tYMax(-1.0*std::numeric_limits<double>::max());

        /* render painters */
        PainterIt tIt;
        if ( fPainters.size() > 0 )
        {
			for( tIt = fPainters.begin(); tIt != fPainters.end(); tIt++ )
			{
				(*tIt)->Render();
				double tLocalXMin = (*tIt)->GetXMin();
				if ( tLocalXMin < tXMin ) tXMin = tLocalXMin;
				double tLocalXMax = (*tIt)->GetXMax();
				if ( tLocalXMax > tXMax ) tXMax = tLocalXMax;
				double tLocalYMin = (*tIt)->GetYMin();
				if ( tLocalYMin < tYMin ) tYMin = tLocalYMin;
				double tLocalYMax = (*tIt)->GetYMax();
				if ( tLocalYMax > tYMax ) tYMax = tLocalYMax;
			}

			utilmsg_debug( "XMin: "<<tXMin<<eom);
			utilmsg_debug( "XMax: "<<tXMax<<eom);
			utilmsg_debug( "YMin: "<<tYMin<<eom);
			utilmsg_debug( "YMax: "<<tYMax<<eom);

			tXMin = tXMin - ( tXMax - tXMin )/20.;
			tXMax = tXMax + ( tXMax - tXMin )/20.;
			tYMin = tYMin - ( tYMax - tYMin )/20.;
			tYMax = tYMax + ( tYMax - tYMin )/20.;

			if ( tXMin == tXMax)
			{
				tXMin = tXMin - tXMin / 20.;
				tXMax = tXMax + tXMax / 20.;
			}

			if ( tYMin == tYMax)
			{
				tYMin = tYMin - tYMin / 20.;
				tYMax = tYMax + tYMax / 20.;
			}

			Int_t tNBins = 1000;
			fFrame = new TH2F( GetName().c_str(), "", tNBins, tXMin, tXMax, tNBins, tYMin, tYMax);
			fFrame->SetStats(0);

			//take axis label of last painter
			KROOTPainter* tLastPainter = fPainters.at( fPainters.size() - 1 );
			if ( tLastPainter )
			{
				fFrame->GetXaxis()->SetTitle( tLastPainter->GetXAxisLabel().c_str() );
				fFrame->GetYaxis()->SetTitle( tLastPainter->GetYAxisLabel().c_str() );
			}
			fFrame->GetXaxis()->SetTitleSize( 0.05 );
			fFrame->GetXaxis()->SetTitleOffset( 1.0 );
			fFrame->GetXaxis()->SetLabelSize( 0.05 );
			fFrame->GetYaxis()->SetTitleSize( 0.05 );
			fFrame->GetYaxis()->SetTitleOffset( 1.0 );
			fFrame->GetYaxis()->SetLabelSize( 0.05 );

        }

        //render pads
        PadIt tPadIt;
        for( tPadIt = fPads.begin(); tPadIt != fPads.end(); tPadIt++ )
        {
            (*tPadIt)->Render();
        }

        utilmsg( eNormal ) <<"KROOTWindow finished to render!"<<eom;

        return;
    }

    void KROOTWindow::Display()
    {
    	utilmsg( eNormal ) <<"KROOTWindow starts to display!"<<eom;

		fCanvas->cd();
    	if ( fFrame )
    	{
			fFrame->Draw( "axis" );
    	}

        /* display painters */
        PainterIt tIt;
        for( tIt = fPainters.begin(); tIt != fPainters.end(); tIt++ )
        {
            (*tIt)->Display();
        }

        /* display pads */
        PadIt tPadIt;
        for( tPadIt = fPads.begin(); tPadIt != fPads.end(); tPadIt++ )
        {
        	fCanvas->cd();
            (*tPadIt)->Display();
        }

    	utilmsg( eNormal ) <<"KROOTWindow finished to display!"<<eom;
        return;
    }

    void KROOTWindow::Write()
    {
    	if ( fWriteEnabled )
    	{
    		string tOutputStringRoot;
    		string tOutputStringPNG;
    		if ( fPath.empty() )
    		{
    			tOutputStringRoot = OUTPUT_DEFAULT_DIR + string("/") + GetName() + string(".root");
    			tOutputStringPNG = OUTPUT_DEFAULT_DIR + string("/") + GetName() + string(".png");
    		}
    		else
    		{
    			tOutputStringRoot = fPath + string("/") + GetName() + string(".root");
    			tOutputStringPNG = fPath + string("/") + GetName() + string(".png");
    		}
            fCanvas->SaveAs( tOutputStringRoot.c_str() );
            fCanvas->SaveAs( tOutputStringPNG.c_str() );
    	}

        /* write painters */
        PainterIt tIt;
        for( tIt = fPainters.begin(); tIt != fPainters.end(); tIt++ )
        {
            (*tIt)->Write();
        }

        /* write pads */
        PadIt tPadIt;
        for( tPadIt = fPads.begin(); tPadIt != fPads.end(); tPadIt++ )
        {
            (*tPadIt)->Write();
        }

        if ( fActive )
        {
			fApplication->Run();
        }

        return;
    }

    void KROOTWindow::AddPainter( KPainter* aPainter )
    {
    	KROOTPainter* tPainter = dynamic_cast< KROOTPainter* >( aPainter );
        if( tPainter != NULL )
        {
            fPainters.push_back( tPainter );
			tPainter->SetWindow( this );
			return;
        }
        utilmsg( eError ) << "cannot add non-root painter <" << aPainter->GetName() << "> to root window <" << GetName() << ">" << eom;
        return;
    }
    void KROOTWindow::RemovePainter( KPainter* aPainter )
    {
    	KROOTPainter* tPainter = dynamic_cast< KROOTPainter* >( aPainter );
        if( tPainter != NULL )
        {
            PainterIt tIt;
            for( tIt = fPainters.begin(); tIt != fPainters.end(); tIt++ )
            {
                if ( (*tIt) == tPainter )
                {
                	fPainters.erase( tIt );
                	tPainter->ClearWindow( this );
                	return;
                }
            }
            utilmsg( eError ) << "cannot remove root painter <" << tPainter->GetName() << "> from root window <" << GetName() << ">" << eom;
        }
        utilmsg( eError ) << "cannot remove non-root painter <" << aPainter->GetName() << "> from root window <" << GetName() << ">" << eom;
        return;
    }

    void KROOTWindow::AddWindow( KWindow* aWindow )
    {
    	KROOTPad* tPad = dynamic_cast< KROOTPad* >( aWindow );
        if( tPad != NULL )
        {
            fPads.push_back( tPad );
            tPad->SetWindow( this );
			return;
        }
        utilmsg( eError ) << "cannot add <" << aWindow->GetName() << "> to root window <" << GetName() << ">" << eom;
        return;
    }
    void KROOTWindow::RemoveWindow( KWindow* aWindow )
    {
    	KROOTPad* tPad = dynamic_cast< KROOTPad* >( aWindow );
        if( tPad != NULL )
        {
            PadIt tIt;
            for( tIt = fPads.begin(); tIt != fPads.end(); tIt++ )
            {
                if ( (*tIt) == tPad )
                {
                	fPads.erase( tIt );
                	tPad->ClearWindow( this );
                	return;
                }
            }
            utilmsg( eError ) << "cannot remove root pad <" << tPad->GetName() << "> from root window <" << GetName() << ">" << eom;
        }
        utilmsg( eError ) << "cannot remove <" << aWindow->GetName() << "> from root window <" << GetName() << ">" << eom;
        return;
    }

    TPad* KROOTWindow::GetPad()
	{
    	return fCanvas;
	}

}
