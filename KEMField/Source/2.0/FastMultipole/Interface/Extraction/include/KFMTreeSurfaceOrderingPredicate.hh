#ifndef KFMTreeSurfaceOrderingPredicate_HH__
#define KFMTreeSurfaceOrderingPredicate_HH__

#include "KSurfaceVisitors.hh"
#include "KSurfaceTypes.hh"
#include "KSurfaceContainer.hh"

#include "KFMCubicSpaceTree.hh"
#include "KFMCubicSpaceTreeNavigator.hh"
#include "KFMCubicSpaceTreeProperties.hh"

namespace KEMField
{

/*
*
*@file KFMTreeSurfaceOrderingPredicate.hh
*@class KFMTreeSurfaceOrderingPredicate
*@brief
*@details
*
*<b>Revision History:<b>
*Date Name Brief Description
*Fri Apr 11 10:23:57 EDT 2014 J. Barrett (barrettj@mit.edu) First Version
*
*/

template<unsigned int NDIM, typename ObjectTypeList>
class KFMTreeSurfaceOrderingPredicate:
public KSurfaceOrderingPredicate<KShapeVisitor, KTYPELIST_3(KTriangle, KRectangle, KLineSegment)>
{
    public:

        typedef KFMCubicSpaceTree<NDIM, NodeObjectTypeList> TreeType;
        typedef KFMCubicSpaceTreeNavigator<NodeObjectTypeList, NDIM> NavigatorType
        typedef KFMCubicSpaceTreeProperties<NDIM> TreePropertyType;

        KFMTreeSurfaceOrderingPredicate(){};
        virtual ~KFMTreeSurfaceOrderingPredicate(){};

        virtual void SetTree(TreeType* tree);
        virtual void Initialize(){;}

        //the ordering operator
        virtual bool operator() (int i,int j)
        {
            KEMThreeVector center_i = fSurfaceContainer[i]->GetShape()->Centroid();
            KEMThreeVector center_j = fSurfaceContainer[j]->GetShape()->Centroid();


        }

    protected:

        NavigatorType fNavigator;
        TreePropertyType fTreeProperties;




};


}


#endif /* KFMTreeSurfaceOrderingPredicate_H__ */
