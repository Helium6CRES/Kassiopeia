#include "KFMNamedScalarDataCollection.hh"

namespace KEMField
{

const KFMNamedScalarData*
KFMNamedScalarDataCollection::GetDataWithName(std::string name) const
{
    unsigned int n = fData.size();
    for(unsigned int i=0; i<n; i++)
    {
        if(fData[i].GetName() == name)
        {
            return &(fData[i]);
        }
    }

    return NULL;
}

KFMNamedScalarData*
KFMNamedScalarDataCollection::GetDataWithName(std::string name)
{
    unsigned int n = fData.size();
    for(unsigned int i=0; i<n; i++)
    {
        if(fData[i].GetName() == name)
        {
            return &(fData[i]);
        }
    }

    return NULL;
}

void
KFMNamedScalarDataCollection::AddData(const KFMNamedScalarData& data)
{
    fData.push_back(data);
}

void KFMNamedScalarDataCollection::DefineOutputNode(KSAOutputNode* node) const
{
    if(node != NULL)
    {
        AddKSAOutputFor(KFMNamedScalarDataCollection,CollectionName,std::string);
        node->AddChild(new KSAObjectOutputNode< std::vector< KFMNamedScalarData > >( KSAClassName< std::vector< KFMNamedScalarData > >::name() , &fData) );
    }
}

void KFMNamedScalarDataCollection::DefineInputNode(KSAInputNode* node)
{
    if(node != NULL)
    {
        AddKSAInputFor(KFMNamedScalarDataCollection,CollectionName,std::string);
        node->AddChild(new KSAObjectInputNode< std::vector< KFMNamedScalarData > >( KSAClassName< std::vector< KFMNamedScalarData > >::name() , &fData) );
    }
}


}
