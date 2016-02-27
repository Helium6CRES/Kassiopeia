#ifndef KGINTERSECTORINITIALIZER_DEF
#define KGINTERSECTORINITIALIZER_DEF

#include "KGCore.hh"

#include "KGIntersectable.hh"

namespace KGeoBag
{
  class KGIntersectorInitializer :
    virtual public KGVisitor,
    virtual public KGExtendedSurface< KGIntersectable >::Visitor
  {
  protected:
    KGIntersectorInitializer() {}
  public:
    virtual ~KGIntersectorInitializer() {}

    void Visit(KGExtendedSurface< KGIntersectable >* intersectableSurface);

  protected:

    void Visit(KGSurface* surface);

    void AssignIntersector(KGAnalyticIntersector* intersector);

    KGExtendedSurface< KGIntersectable >* fIntersectableSurface;
    const KGSurface* fSurface;
  };
}

#endif /* KGINTERSECTORINITIALIZER_DEF */
