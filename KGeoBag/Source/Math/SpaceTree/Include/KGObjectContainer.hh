#ifndef KGObjectContainer_HH__
#define KGObjectContainer_HH__


#include <cstddef>
#include <vector>

namespace KGeoBag
{

/*
*
*@file KGObjectContainer.hh
*@class KGObjectContainer
*@brief simple wrapper for a vector
*@details
*
*<b>Revision History:<b>
*Date Name Brief Description
*Fri Aug 23 17:42:44 CEST 2013 J. Barrett (barrettj@mit.edu) First Version
*
*/

template< typename ObjectType >
class KGObjectContainer
{
    public:
        KGObjectContainer(){;}
        virtual ~KGObjectContainer(){;};

        virtual unsigned int GetNObjects() const {return fObjectVector.size();};

        virtual void AddObject(const ObjectType& obj)
        {
            fObjectVector.push_back( new ObjectType(obj) );
        }

        virtual ObjectType* GetObjectWithID(const unsigned int& id)
        {
            if(id < fObjectVector.size())
            {
                return fObjectVector[id];
            }
            else
            {
                return NULL;
            }
        }

        virtual const ObjectType* GetObjectWithID(const unsigned int& id) const
        {
            if(id < fObjectVector.size())
            {
                return fObjectVector[id];
            }
            else
            {
                return NULL;
            }
        }

        virtual void DeleteAllObjects()
        {
            for(unsigned int i=0; i<fObjectVector.size(); i++)
            {
                delete fObjectVector[i];
            }
            fObjectVector.clear();
        }


        //access elements
        inline ObjectType* operator[](unsigned int i);
        inline const ObjectType* operator[](unsigned int i) const;

    private:


        std::vector< ObjectType* > fObjectVector;

};



template< typename ObjectType >
inline ObjectType* KGObjectContainer<ObjectType>::operator[](unsigned int i)
{
    return GetObjectWithID(i);//fObjectVector[i];
}

template< typename ObjectType >
inline const ObjectType* KGObjectContainer<ObjectType>::operator[](unsigned int i) const
{
    return GetObjectWithID(i);//fObjectVector[i];
}



}//end of KGeoBag

#endif /* KGObjectContainer_H__ */
