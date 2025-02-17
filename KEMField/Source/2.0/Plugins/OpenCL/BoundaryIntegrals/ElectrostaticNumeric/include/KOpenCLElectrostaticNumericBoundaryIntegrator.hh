#ifndef KOPENCLELECTROSTATICNUMERICBOUNDARYINTEGRATOR_DEF
#define KOPENCLELECTROSTATICNUMERICBOUNDARYINTEGRATOR_DEF

#include "KElectrostaticBoundaryIntegrator.hh"
#include "KOpenCLBoundaryIntegrator.hh"
#include "KSurfaceVisitors.hh"

namespace KEMField
{
  class ElectrostaticOpenCL;

  class KOpenCLElectrostaticNumericBoundaryIntegrator :
    public KOpenCLBoundaryIntegrator<KElectrostaticBasis>,
    public KSelectiveVisitor<KShapeVisitor, KTYPELIST_4(KTriangle,
							KRectangle,
							KLineSegment,
							KConicSection)>
  {
  public:
    typedef KElectrostaticBasis Basis;
    typedef Basis::ValueType ValueType;
    typedef KBoundaryType<Basis,KDirichletBoundary> DirichletBoundary;
    typedef KBoundaryType<Basis,KNeumannBoundary> NeumannBoundary;

    // for selection of the correct KIntegratingFieldSolver template and possibly elsewhere
    typedef ElectrostaticOpenCL Kind;
    typedef KElectrostaticBoundaryIntegrator IntegratorSingleThread;

    using KSelectiveVisitor<KShapeVisitor, KTYPELIST_4(KTriangle,
						       KRectangle,
						       KLineSegment,
						       KConicSection)>::Visit;

    KOpenCLElectrostaticNumericBoundaryIntegrator(KOpenCLSurfaceContainer& c);
    ~KOpenCLElectrostaticNumericBoundaryIntegrator();

    void Visit(KTriangle& t) { ComputeBoundaryIntegral(t); }
    void Visit(KRectangle& r) { ComputeBoundaryIntegral(r); }
    void Visit(KLineSegment& l) { ComputeBoundaryIntegral(l); }
    void Visit(KConicSection& c) { ComputeBoundaryIntegral(c); }

    ValueType  BoundaryIntegral(KSurfacePrimitive* source,
				KSurfacePrimitive* target,
				unsigned int);
    ValueType  BoundaryValue(KSurfacePrimitive* surface,unsigned int);
    ValueType& BasisValue(KSurfacePrimitive* surface,unsigned int);

    template <class SourceShape>
    double Potential(const SourceShape*, const KPosition&) const;
    template <class SourceShape>
    KEMThreeVector ElectricField(const SourceShape*, const KPosition&) const;
    template <class SourceShape>
    std::pair<KEMThreeVector,double> ElectricFieldAndPotential(const SourceShape*, const KPosition&) const;

    std::string OpenCLFile() const { return "kEMField_ElectrostaticNumericBoundaryIntegrals.cl"; }
 
  private:
    class BoundaryVisitor :
      public KSelectiveVisitor<KBoundaryVisitor,
			       KTYPELIST_2(KDirichletBoundary,KNeumannBoundary)>
    {
    public:
      using KSelectiveVisitor<KBoundaryVisitor,KTYPELIST_2(KDirichletBoundary,KNeumannBoundary)>::Visit;

      BoundaryVisitor() {}
      virtual ~BoundaryVisitor() {}

      void Visit(KDirichletBoundary&);
      void Visit(KNeumannBoundary&);

      bool IsDirichlet() const { return fIsDirichlet; }
      ValueType Prefactor() const { return fPrefactor; }
      ValueType GetBoundaryValue() const { return fBoundaryValue; }

    protected:

      bool fIsDirichlet;
      ValueType fPrefactor;
      ValueType fBoundaryValue;
    };

    class BasisVisitor :
      public KSelectiveVisitor<KBasisVisitor,KTYPELIST_1(KElectrostaticBasis)>
    {
    public:
      using KSelectiveVisitor<KBasisVisitor,
			      KTYPELIST_1(KElectrostaticBasis)>::Visit;

      BasisVisitor() : fBasisValue(NULL) {}
      virtual ~BasisVisitor() {}

      void Visit(KElectrostaticBasis&);

      ValueType& GetBasisValue() const { return *fBasisValue; }

    protected:

      ValueType* fBasisValue;
    };

    template <class SourceShape>
    void ComputeBoundaryIntegral(SourceShape& source);

    BoundaryVisitor fBoundaryVisitor;
    BasisVisitor fBasisVisitor;
    KSurfacePrimitive* fTarget;
    ValueType fValue;

  private:

    void ConstructOpenCLKernels() const;
    void AssignBuffers() const;

    mutable cl::Kernel *fPhiKernel;
    mutable cl::Kernel *fEFieldKernel;
    mutable cl::Kernel *fEFieldAndPhiKernel;

    mutable cl::Buffer *fBufferPhi;
    mutable cl::Buffer *fBufferEField;
    mutable cl::Buffer *fBufferEFieldAndPhi;
  };

  template <class SourceShape>
  double KOpenCLElectrostaticNumericBoundaryIntegrator::Potential(const SourceShape* source,const KPosition& aPosition) const
  {
    StreamSourceToBuffer(source);

    CL_TYPE P[3] = {aPosition[0],aPosition[1],aPosition[2]};

    KOpenCLInterface::GetInstance()->GetQueue().    
      enqueueWriteBuffer(*fBufferP,
			 CL_TRUE,
			 0,
			 3*sizeof(CL_TYPE),
			 P);

    cl::NDRange global(1);
    cl::NDRange local(1);

    KOpenCLInterface::GetInstance()->GetQueue().    
      enqueueNDRangeKernel(*fPhiKernel,
			   cl::NullRange,
			   global,
			   local);
 
    CL_TYPE phi = 0.;

    KOpenCLInterface::GetInstance()->GetQueue().
      enqueueReadBuffer(*fBufferPhi,
			CL_TRUE,
			0,
			sizeof(CL_TYPE),
			&phi);

    return phi;
  }

  template <class SourceShape>
  KEMThreeVector KOpenCLElectrostaticNumericBoundaryIntegrator::ElectricField(const SourceShape* source, const KPosition& aPosition) const
  {
    StreamSourceToBuffer(source);

    CL_TYPE P[3] = {aPosition[0],aPosition[1],aPosition[2]};

    KOpenCLInterface::GetInstance()->GetQueue().
      enqueueWriteBuffer(*fBufferP,
			 CL_TRUE,
			 0,
			 3*sizeof(CL_TYPE),
			 P);

    cl::NDRange global(1);
    cl::NDRange local(1);

    KOpenCLInterface::GetInstance()->GetQueue().
      enqueueNDRangeKernel(*fEFieldKernel,
			   cl::NullRange,
			   global,
			   local);

    CL_TYPE4 eField;

    KOpenCLInterface::GetInstance()->GetQueue().
      enqueueReadBuffer(*fBufferEField,
			CL_TRUE,
			0,
			sizeof(CL_TYPE4),
			&eField);

    return KEMThreeVector(eField.s[0],eField.s[1],eField.s[2]);
  }

  template <class SourceShape>
  std::pair<KEMThreeVector,double> KOpenCLElectrostaticNumericBoundaryIntegrator::ElectricFieldAndPotential(const SourceShape* source, const KPosition& aPosition) const
  {
	    StreamSourceToBuffer(source);

	    CL_TYPE P[3] = {aPosition[0],aPosition[1],aPosition[2]};

	    KOpenCLInterface::GetInstance()->GetQueue().
	      enqueueWriteBuffer(*fBufferP,
				 CL_TRUE,
				 0,
				 3*sizeof(CL_TYPE),
				 P);

	    cl::NDRange global(1);
	    cl::NDRange local(1);

	    KOpenCLInterface::GetInstance()->GetQueue().
	      enqueueNDRangeKernel(*fEFieldAndPhiKernel,
				   cl::NullRange,
				   global,
				   local);

	    CL_TYPE4 eFieldAndPhi;

	    KOpenCLInterface::GetInstance()->GetQueue().
	      enqueueReadBuffer(*fBufferEFieldAndPhi,
				CL_TRUE,
				0,
				sizeof(CL_TYPE4),
				&eFieldAndPhi);

	    return std::make_pair( KEMThreeVector(eFieldAndPhi.s[0],eFieldAndPhi.s[1],eFieldAndPhi.s[2]), eFieldAndPhi.s[3] );
  }

  template <class SourceShape>
  void KOpenCLElectrostaticNumericBoundaryIntegrator::ComputeBoundaryIntegral(SourceShape& source)
  {
    if (fBoundaryVisitor.IsDirichlet())
    {
      fValue = this->Potential(&source,fTarget->GetShape()->Centroid());
    }
    else
    {
      KEMThreeVector field = this->ElectricField(&source, fTarget->GetShape()->Centroid());
      fValue = field.Dot(fTarget->GetShape()->Normal());
      
      double dist = (source.Centroid() - fTarget->GetShape()->Centroid()).Magnitude();
      
      if (dist<1.e-12)
	    fValue *= fBoundaryVisitor.Prefactor();
    }
  }
}

#endif /* KOPENCLELECTROSTATICNUMERICBOUNDARYINTEGRATOR_DEF */
