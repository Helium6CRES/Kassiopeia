#ifndef KFMElementLocator_HH__
#define KFMElementLocator_HH__

#include "KFMNode.hh"
#include "KFMNodeActor.hh"
#include "KFMObjectRetriever.hh"

#include "KFMIdentitySet.hh"

namespace KEMField
{


/*
*
*@file KFMElementLocator.hh
*@class KFMElementLocator
*@brief
*@details
*
*<b>Revision History:<b>
*Date Name Brief Description
*Tue Mar  4 12:19:34 EST 2014 J. Barrett (barrettj@mit.edu) First Version
*
*/
template< typename ObjectTypeList>
class KFMElementLocator: public KFMNodeActor< KFMNode<ObjectTypeList> >
{
    public:
        KFMElementLocator(){};
        virtual ~KFMElementLocator(){};

        void SetElementID(unsigned int id)
        {
            fID = id;
        }

        virtual void ApplyAction(KFMNode<ObjectTypeList>* node)
        {
            if(node != NULL)
            {
                KFMIdentitySet* set = KFMObjectRetriever<ObjectTypeList, KFMIdentitySet>::GetNodeObject(node);
                if(set != NULL)
                {
                    if(set->IsPresent(fID))
                    {
                        std::cout<<"found element w/ id: "<<fID<<" in node: "<<node->GetID()<<std::endl;
                    }
                }
            }
        }

    private:


        unsigned int fID;

};


}



#endif /* KFMElementLocator_H__ */
