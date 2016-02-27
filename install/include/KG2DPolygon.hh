#ifndef __KG2DPolygon_H__
#define __KG2DPolygon_H__

#include "KTwoVector.hh"
#include <cmath>

#include "KG2DArea.hh"
#include "KG2DLineSegment.hh"

#include <vector>

#define SMALLNUMBER 1e-9

namespace KGeoBag
{

/**
*
*@file KG2DPolygon.hh
*@class KG2DPolygon
*@brief
*@details
*
*<b>Revision History:<b>
*Date Name Brief Description
*Sat Jul 28 17:34:28 EDT 2012 J. Barrett (barrettj@mit.edu) First Version
*July 2013: AreaMeasure added by Andreas Mueller
*
*/

class KG2DPolygon: public KG2DArea
{
    public:
        KG2DPolygon();
        KG2DPolygon(const KG2DPolygon& copyObject);
        KG2DPolygon(const std::vector< std::vector<double> >* ordered_vertices);
        KG2DPolygon(const std::vector< KTwoVector >* ordered_vertices);

        virtual ~KG2DPolygon();

        ///create the polygon by setting the vertices
        ///sides are created from the vertices in a 'connect the dots' manner.
        ///The last point in the list is connected to the
        ///first by the final edge to close the polygon.
        ///There is no need to repeat the first vertex.
        void SetVertices(const std::vector< std::vector<double> >* ordered_vertices);
        void SetVertices(const std::vector< KTwoVector >* ordered_vertices);
        virtual void Initialize();

        //getters
        void GetVertices(std::vector<KTwoVector>* vertices) const;
        void GetSides( std::vector<KG2DLineSegment>* sides) const;

        //geometry utilities
        virtual void NearestDistance( const KTwoVector& aPoint, double& aDistance ) const;
  virtual KTwoVector Point( const KTwoVector& aPoint ) const;
  virtual KTwoVector Normal( const KTwoVector& aPoint ) const;
        virtual void NearestIntersection( const KTwoVector& aStart, const KTwoVector& anEnd, bool& aResult, KTwoVector& anIntersection ) const;

        ///returns true if point is inside the region enclosed by the polygon
        virtual bool IsInside(const KTwoVector& point) const;

        virtual double Area() const;
  virtual KTwoVector Random()const { return KTwoVector(0.,0.); } //not yet implemented

        ///returns true if polygon has no self intersections
        virtual bool IsSimple() const {return fIsSimple;};

        ///returns true it the polygon has been constructed properly
        ///must be checked before calling other functions otherwise
        ///results cannot be guaranteed to be correct
        virtual bool IsValid() const {return fIsValid;};

        ///use our own modulus function, because c++'s is all screwy
        ///and implementation dependent when negative numbers are involved
        static int Modulus(int arg, int n);

    protected:


        //function used to test if the polygon is simple
        void DetermineIfPolygonIsSimple();

        ///determines whether the interior lies to the left or right
        ///of the sides that make up the (base) polygon
        void DetermineInteriorSide();

        std::vector< KTwoVector > fVertices; //an ordered list of the polygon's vertices
        std::vector< KG2DLineSegment > fSides; //an ordered list of the polygon's sides

        bool fIsSimple;
        bool fIsValid;
        bool fIsLeft;
        int fNVertices; //number of vertices, same as number of sides

        //scratch space for point in polygon test
        mutable std::vector<KTwoVector> fDiff;

};

}//end of namespace


#endif /* __KG2DPolygon_H__ */
