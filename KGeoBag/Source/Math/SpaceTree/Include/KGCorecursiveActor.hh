#ifndef KGCorecursiveActor_HH__
#define KGCorecursiveActor_HH__

#include <queue>

#include "KGNodeActor.hh"


namespace KGeoBag
{


/*
*
*@file KGCorecursiveActor.hh
*@class KGCorecursiveActor
*@brief
*@details
*
*<b>Revision History:<b>
*Date Name Brief Description
*Mon Aug 12 10:52:07 EDT 2013 J. Barrett (barrettj@mit.edu) First Version
*
*/

template< typename NodeType>
class KGCorecursiveActor: public KGNodeActor<NodeType>
{
    public:
        KGCorecursiveActor():fOperationalActor(NULL){};
        virtual ~KGCorecursiveActor(){};

        void SetOperationalActor(KGNodeActor<NodeType>* opActor)
        {
            if(opActor != this && opActor != NULL)//avoid a disaster
            {
                fOperationalActor = opActor;
            }
        }

        //corecursively apply the operational visitor to every node
        //below this one
        void ApplyAction(NodeType* node)
        {
            if(node != NULL)
            {
                fNodeQueue = std::queue< NodeType* >();
                fNodeQueue.push(node);
                do
                {
                    fOperationalActor->ApplyAction(fNodeQueue.front());

                    if(fNodeQueue.front()->HasChildren())
                    {
                        fTempNode = fNodeQueue.front();
                        unsigned int n_children = fTempNode->GetNChildren();
                        for(unsigned int i=0; i < n_children; i++)
                        {
                            fNodeQueue.push( fTempNode->GetChild(i) );
                        }
                    }
                    fNodeQueue.pop();
                }
                while(fNodeQueue.size() != 0 );
            }
        }

    private:

        KGNodeActor<NodeType>* fOperationalActor;
        std::queue< NodeType* > fNodeQueue;
        NodeType* fTempNode;

};


}//end of KGeoBag


#endif /* KGCorecursiveActor_H__ */
