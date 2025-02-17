#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "KTypelist.hh"
#include "KSurfaceTypes.hh"
#include "KSurface.hh"
#include "KSurfaceContainer.hh"

#include "KDataDisplay.hh"

// definition of integrator types
// see KEMField::KElectrostaticBoundaryIntegratorFactory::Make(std::string)
// for a complete list of options

std::string integratorType{"numeric"};
//std::string integratorType{"analytic"};
//std::string integratorType{"rwg"};

std::string oclIntegratorType{"numeric"};
//std::string oclIntegratorType{"analytic"};
//std::string oclIntegratorType{"rwg"};

#include "KElectrostaticBoundaryIntegratorFactory.hh"
#include "KBoundaryIntegralMatrix.hh"
#include "KBoundaryIntegralVector.hh"
#include "KBoundaryIntegralSolutionVector.hh"

#include "KGaussianElimination.hh"
#include "KGaussSeidel.hh"
#include "KRobinHood.hh"
#include "KMultiElementRobinHood.hh"
#include "KSuccessiveSubspaceCorrection.hh"

#include "KElectrostaticIntegratingFieldSolver.hh"

#include "KEMFieldCanvas.hh"

#ifdef KEMFIELD_USE_ROOT
#include "TGraph.h"
#include "TMultiGraph.h"
#include "KEMRootFieldCanvas.hh"
#endif

#ifdef KEMFIELD_USE_VTK
#include "KEMVTKFieldCanvas.hh"
#endif

#ifdef KEMFIELD_USE_VTK
#include "KEMVTKViewer.hh"
#include "KVTKResidualGraph.hh"
#endif

#include "KEMConstants.hh"

#include "KSADataStreamer.hh"
#include "KBinaryDataStreamer.hh"
#include "KSerializer.hh"

#include "KIterativeStateReader.hh"
#include "KIterativeStateWriter.hh"


#include "KMPIEnvironment.hh"

#ifdef KEMFIELD_USE_MPI
#include "KGaussSeidel_MPI.hh"
#include "KRobinHood_MPI.hh"
#endif

#ifdef KEMFIELD_USE_OPENCL
#include "KOpenCLSurfaceContainer.hh"
#include "KOpenCLElectrostaticBoundaryIntegratorFactory.hh"
#include "KOpenCLBoundaryIntegralMatrix.hh"
#include "KOpenCLBoundaryIntegralVector.hh"
#include "KOpenCLBoundaryIntegralSolutionVector.hh"
#include "KGaussSeidel_OpenCL.hh"
#include "KRobinHood_OpenCL.hh"
#include "KOpenCLElectrostaticIntegratingFieldSolver.hh"
#ifdef KEMFIELD_USE_MPI
// #include "KGaussSeidel_MPI_OpenCL.hh"
#include "KRobinHood_MPI_OpenCL.hh"
#endif
#endif

#include "KSimpleIterativeKrylovSolver.hh"
#include "KBiconjugateGradientStabilized.hh"
#include "KGeneralizedMinimalResidual.hh"

#ifdef KEMFIELD_USE_PETSC
#include "KPETScInterface.hh"
#include "KPETScSolver.hh"
#endif

#ifdef KEMFIELD_USE_KGEOBAG
#include "KGRotatedSurface.hh"
#include "KGMesher.hh"
#include "KGBEM.hh"
#include "KGBEMConverter.hh"
#endif

#ifndef DEFAULT_DATA_DIR
#define DEFAULT_DATA_DIR "."
#endif /* !DEFAULT_DATA_DIR */

#ifndef DEFAULT_OUTPUT_DIR
#define DEFAULT_OUTPUT_DIR "."
#endif /* !DEFAULT_OUTPUT_DIR */

using namespace KEMField;

#ifdef KEMFIELD_USE_KGEOBAG
using namespace KGeoBag;
#endif

void ReadInTriangles(std::string fileName,KSurfaceContainer& surfaceContainer);
std::vector<std::string> Tokenize(std::string separators,std::string input);

void Field_Analytic(double Q,double radius1,double radius2,double radius3,double permittivity1,double permittivity2,double *P,double *F);

clock_t start;

void StartTimer()
{
  MPI_SINGLE_PROCESS
    start = clock();
}

double Time()
{
  MPI_SINGLE_PROCESS
  {
    double end = clock();
    return ((double)(end - start))/CLOCKS_PER_SEC; // time in seconds
  }

  return 0.;
}

int main(int argc, char* argv[])
{
#ifdef KEMFIELD_USE_PETSC
  KPETScInterface::GetInstance()->Initialize(&argc,&argv);
#elif KEMFIELD_USE_MPI
  KMPIInterface::GetInstance()->Initialize(&argc,&argv);
#endif


  std::string usage =
    "\n"
    "Usage: ComputeSphericalCapacitor <options>\n"
    "\n"
    "This program computes the charge densities of elements defined by input files.\n"
    "The program takes as inputs and outputs the names of geometry files to read.\n"
    "\n"
    "\tAvailable options:\n"
    "\t -h, --help               (shows this message and exits)\n"
    "\t -v, --verbose            (0..5; sets the verbosity)\n"
    "\t -a, --accuracy           (accuracy of charge density computation)\n"
    "\t -j, --save_increment     (increment of state saving)\n"
    "\t -t, --time               (bool; time convergence)\n"
    "\t -d, --draw               (bool; draw images)\n"
#ifdef KEMFIELD_USE_KGEOBAG
    "\t -g, --generator          (use Gmsh files or KGeoBag spheres)\n"
#endif
    "\t -c, --cache              (cache matrix elements)\n"
    "\t -s, --scale              (spherical capacitor scale between 1 and 20)\n"
    "\t -m, --method             (0: Gaussian elimination,\n"
    "\t                           1: Gauss-Seidel,\n"
    "\t                           2: Single-Element Robin Hood (default),\n"
    "\t                           3: Multi-Element Robin Hood,\n"
    "\t                           4: BiCGStab,\n"
    "\t                           5: BiCGStab w/Jacobi preconditioner\n"
    "\t                           6: Successive Subspace Correction,\n"
    "\t                           7: GMRES\n"
#ifdef KEMFIELD_USE_PETSC
    ",\n\t                           8: PETSc)\n"
#else
  ")\n"
#endif
    "\t -i, --rh_increment       (increment of RH accuracy check/print/log)\n"
    "\t -n, --rh_dimension       (dimension of subspace)\n"
    "\t -p, --plot_matrix        (plot matrix elements on a grid)\n"
    "\t -r, --residual_graph     (graph residual during iterative solve)\n"
    ;

  int verbose = 3;
  double accuracy = 1.e-8;
  int rh_increment = 0;
  int rh_dimension = 2;
  int saveIncrement = UINT_MAX;
  bool computeTime = false;
  bool draw = false;
  int scale = 1;
  int method = 2;
  bool plotMatrix = false;
  bool residualGraph = false;
  (void) residualGraph;

  bool useGmshFiles = true;
  bool cache = false;

  static struct option longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", required_argument, 0, 'v'},
    {"accuracy", required_argument, 0, 'a'},
    {"save_increment", required_argument, 0, 'j'},
    {"time", no_argument, 0, 't'},
    {"draw", no_argument, 0, 'd'},
#ifdef KEMFIELD_USE_KGEOBAG
    {"generator", required_argument, 0, 'g'},
#endif
    {"cache", no_argument, 0, 'c'},
    {"scale", required_argument, 0, 's'},
    {"method", required_argument, 0, 'm'},
    {"rh_increment", required_argument, 0, 'i'},
    {"rh_dimension", required_argument, 0, 'n'},
    {"plot_matrix", required_argument, 0, 'p'},
    {"residual_graph", required_argument, 0, 'r'},
  };

#ifdef KEMFIELD_USE_KGEOBAG
  static const char *optString = "hv:a:j:tdg:cs:m:i:n:pr";
#else
  static const char *optString = "hv:a:j:tdcs:m:i:n:pr";
#endif

  while(1) {
    char optId = getopt_long(argc, argv,optString, longOptions, NULL);
    if(optId == -1) break;
    switch(optId) {
    case('h'): // help
      MPI_SINGLE_PROCESS
    std::cout<<usage<<std::endl;
#ifdef KEMFIELD_USE_MPI
      KMPIInterface::GetInstance()->Finalize();
#endif
      return 0;
    case('v'): // verbose
      verbose = atoi(optarg);
      if (verbose < 0) verbose = 0;
      if (verbose > 5) verbose = 5;
      break;
    case('a'):
      accuracy = atof(optarg);
      break;
#ifdef KEMFIELD_USE_KGEOBAG
    case('g'):
      if (std::string(optarg) == "Gmsh")
      {
    useGmshFiles = true;
    break;
      }
      if (std::string(optarg) == "KGeoBag")
      {
    useGmshFiles = false;
    break;
      }
      MPI_SINGLE_PROCESS
      {
    std::cout<<"generator must be Gmsh or KGeoBag"<<std::endl;
    std::cout<<""<<std::endl;
    std::cout<<usage<<std::endl;
      }
#ifdef KEMFIELD_USE_MPI
      KMPIInterface::GetInstance()->Finalize();
#endif
      return 1;
#endif
    case('j'):
      saveIncrement = atoi(optarg);
      break;
    case('t'):
      computeTime = true;
      break;
    case('d'):
      draw = true;
      break;
    case('m'):
      method = atoi(optarg);
      break;
    case('c'):
      cache = true;
      break;
    case('s'):
      scale = atoi(optarg);
      if (scale < 1) scale = 1;
      if (scale >20) scale = 20;
      break;
    case('i'):
      rh_increment = atoi(optarg);
      break;
    case('n'):
      rh_dimension = atoi(optarg);
      break;
    case('p'):
      plotMatrix = true;
      break;
    case('r'):
      residualGraph = true;
      break;
    default:
        #ifdef KEMFIELD_USE_PETSC
            //we do not exit on an unrecognized option
            //so that we can forward arguments along to PETSc
            break;
        #else
            //unrecognized option and PETSc is not in use
            MPI_SINGLE_PROCESS
            std::cout<<usage<<std::endl;
            #ifdef KEMFIELD_USE_MPI
                KMPIInterface::GetInstance()->Finalize();
            #endif
            return 1;
        #endif
    }
  }

  if (scale < 1)
  {
    MPI_SINGLE_PROCESS
      std::cout<<usage<<std::endl;
#ifdef KEMFIELD_USE_MPI
    KMPIInterface::GetInstance()->Finalize();
#endif
    return 1;
  }

#if defined(KEMFIELD_USE_MPI) && defined(KEMFIELD_USE_OPENCL)
  KOpenCLInterface::GetInstance()->SetGPU(KMPIInterface::GetInstance()->GetProcess() + 1);
#endif

  double radius1 = 1.;
  double radius2 = 2.;
  double radius3 = 3.;

  double potential1 = 1.;

  double permittivity1 = 2.;
  double permittivity2 = 3.;

  KSurfaceContainer surfaceContainer;

  if (useGmshFiles)
  {
    std::stringstream s;
    s << DEFAULT_DATA_DIR << "/sphericalCapacitorFiles/SphericalCapacitor_" << scale << "_triangles.dat";
    ReadInTriangles(s.str(),surfaceContainer);
  }
#ifdef KEMFIELD_USE_KGEOBAG
  else
  {
    double potential2 = 0.;

    // Construct the shapes
    double p1[2],p2[2];
    double radius = radius1;

    KGRotatedObject* innerhemi1 = new KGRotatedObject(scale*10,10);
    p1[0] = -radius; p1[1] = 0.;
    p2[0] = 0.; p2[1] = radius;
    innerhemi1->AddArc(p2,p1,radius,true);

    KGRotatedObject* innerhemi2 = new KGRotatedObject(scale*10,10);
    p2[0] = radius; p2[1] = 0.;
    p1[0] = 0.; p1[1] = radius;
    innerhemi2->AddArc(p1,p2,radius,false);

    radius = radius2;

    KGRotatedObject* middlehemi1 = new KGRotatedObject(20*scale,10);
    p1[0] = -radius; p1[1] = 0.;
    p2[0] = 0.; p2[1] = radius;
    middlehemi1->AddArc(p2,p1,radius,true);

    KGRotatedObject* middlehemi2 = new KGRotatedObject(20*scale,10);
    p2[0] = radius; p2[1] = 0.;
    p1[0] = 0.; p1[1] = radius;
    middlehemi2->AddArc(p1,p2,radius,false);

    radius = radius3;

    KGRotatedObject* outerhemi1 = new KGRotatedObject(30*scale,10);
    p1[0] = -radius; p1[1] = 0.;
    p2[0] = 0.; p2[1] = radius;
    outerhemi1->AddArc(p2,p1,radius,true);

    KGRotatedObject* outerhemi2 = new KGRotatedObject(30*scale,10);
    p2[0] = radius; p2[1] = 0.;
    p1[0] = 0.; p1[1] = radius;
    outerhemi2->AddArc(p1,p2,radius,false);

    // Construct shape placement
    KGRotatedSurface* ih1 = new KGRotatedSurface(innerhemi1);
    KGSurface* innerhemisphere1 = new KGSurface(ih1);
    innerhemisphere1->SetName( "innerhemisphere1" );
    innerhemisphere1->MakeExtension<KGMesh>();
    innerhemisphere1->MakeExtension<KGElectrostaticDirichlet>()->SetBoundaryValue(potential1);

    KGRotatedSurface* ih2 = new KGRotatedSurface(innerhemi2);
    KGSurface* innerhemisphere2 = new KGSurface(ih2);
    innerhemisphere2->SetName( "innerhemisphere2" );
    innerhemisphere2->MakeExtension<KGMesh>();
    innerhemisphere2->MakeExtension<KGElectrostaticDirichlet>()->SetBoundaryValue(potential1);

    KGRotatedSurface* mh1 = new KGRotatedSurface(middlehemi1);
    KGSurface* middlehemisphere1 = new KGSurface(mh1);
    middlehemisphere1->SetName( "middlehemisphere1" );
    middlehemisphere1->MakeExtension<KGMesh>();
    middlehemisphere1->MakeExtension<KGElectrostaticNeumann>()->SetNormalBoundaryFlux(permittivity2/permittivity1);

    KGRotatedSurface* mh2 = new KGRotatedSurface(middlehemi2);
    KGSurface* middlehemisphere2 = new KGSurface(mh2);
    middlehemisphere2->SetName( "middlehemisphere2" );
    middlehemisphere2->MakeExtension<KGMesh>();
    middlehemisphere2->MakeExtension<KGElectrostaticNeumann>()->SetNormalBoundaryFlux(permittivity1/permittivity2);

    KGRotatedSurface* oh1 = new KGRotatedSurface(outerhemi1);
    KGSurface* outerhemisphere1 = new KGSurface(oh1);
    outerhemisphere1->SetName( "outerhemisphere1" );
    outerhemisphere1->MakeExtension<KGMesh>();
    outerhemisphere1->MakeExtension<KGElectrostaticDirichlet>()->SetBoundaryValue(potential2);
    KGRotatedSurface* oh2 = new KGRotatedSurface(outerhemi2);
    KGSurface* outerhemisphere2 = new KGSurface(oh2);
    outerhemisphere2->SetName( "outerhemisphere2" );
    outerhemisphere2->MakeExtension<KGMesh>();
    outerhemisphere2->MakeExtension<KGElectrostaticDirichlet>()->SetBoundaryValue(potential2);

    // Mesh the elements
    KGMesher* mesher = new KGMesher();
    innerhemisphere1->AcceptNode(mesher);
    innerhemisphere2->AcceptNode(mesher);
    middlehemisphere1->AcceptNode(mesher);
    middlehemisphere2->AcceptNode(mesher);
    outerhemisphere1->AcceptNode(mesher);
    outerhemisphere2->AcceptNode(mesher);

    KGBEMMeshConverter geometryConverter(surfaceContainer);
    geometryConverter.SetMinimumArea(1.e-12);
    innerhemisphere1->AcceptNode(&geometryConverter);
    innerhemisphere2->AcceptNode(&geometryConverter);
    middlehemisphere1->AcceptNode(&geometryConverter);
    middlehemisphere2->AcceptNode(&geometryConverter);
    outerhemisphere1->AcceptNode(&geometryConverter);
    outerhemisphere2->AcceptNode(&geometryConverter);
  }
#endif

  // let's see if we can find this geometry
  bool skipBEM = false;

  /*
    {
    KMD5HashGenerator hashGenerator;
    hashGenerator.Omit(KBasisTypes());

    std::vector<std::string> labels;

    labels.push_back(hashGenerator.GenerateHash(surfaceContainer));
    labels.push_back(KResidualThreshold::Name());

    unsigned int nElements = KEMFileInterface::GetInstance()->NumberWithLabels( labels );

    if (nElements>0)
    {
    KResidualThreshold residualThreshold;
    KResidualThreshold minResidualThreshold;

    for (unsigned int i=0;i<nElements;i++)
    {
    KEMFileInterface::GetInstance()->FindByLabels(residualThreshold,labels,i);
    if (residualThreshold<minResidualThreshold)
    minResidualThreshold = residualThreshold;
    }

    KEMFileInterface::GetInstance()->FindByHash(surfaceContainer,minResidualThreshold.fGeometryHash);

    if (minResidualThreshold.fResidualThreshold<=accuracy)
    skipBEM = true;
    }
    }
  */


#ifdef KEMFIELD_USE_OPENCL
  KOpenCLSurfaceContainer oclSurfaceContainer(surfaceContainer);
  KOpenCLElectrostaticBoundaryIntegrator integrator{KoclEBIFactory::Make(oclIntegratorType,oclSurfaceContainer)};
  KSquareMatrix<double>* A = new KBoundaryIntegralMatrix<KOpenCLBoundaryIntegrator<KElectrostaticBasis> >(oclSurfaceContainer,integrator);

  KBoundaryIntegralVector<KOpenCLBoundaryIntegrator<KElectrostaticBasis> > b(oclSurfaceContainer,integrator);
  KBoundaryIntegralSolutionVector<KOpenCLBoundaryIntegrator<KElectrostaticBasis> > x(oclSurfaceContainer,integrator);
#else
  KElectrostaticBoundaryIntegrator integrator{KEBIFactory::Make(integratorType)};
  KSquareMatrix<double>* A;
  if (cache)
  {
    A = new KBoundaryIntegralMatrix<KElectrostaticBoundaryIntegrator,true>(surfaceContainer,integrator);
// ability to read out matrix elements
//    double tmp;
//    for (unsigned int i=0;i<A->Dimension();i++)
//      for (unsigned int j=0;j<A->Dimension();j++)
//        tmp = (*A)(i,j);
  }
  else
    A = new KBoundaryIntegralMatrix<KElectrostaticBoundaryIntegrator>(surfaceContainer,integrator);

  KBoundaryIntegralSolutionVector<KElectrostaticBoundaryIntegrator> x(surfaceContainer,integrator);
  KBoundaryIntegralVector<KElectrostaticBoundaryIntegrator> b(surfaceContainer,integrator);
#endif

  double computationTime = 0.;
  int dimension = b.Dimension();

  if (!skipBEM)
  {
    if (method == 0)
    {
      KGaussianElimination<KElectrostaticBoundaryIntegrator::ValueType> gaussianElimination;
      if (computeTime) StartTimer();
      gaussianElimination.Solve(*A,x,b);
      if (computeTime) computationTime = Time();
    }
    else if (method == 1)
    {
      // #if defined(KEMFIELD_USE_MPI) && defined(KEMFIELD_USE_OPENCL)
      //       KGaussSeidel<KElectrostaticBoundaryIntegrator::ValueType,
      //         KGaussSeidel_MPI_OpenCL> gaussSeidel;
      // #ifndef KEMFIELD_USE_DOUBLE_PRECISION
      //       gaussSeidel.SetTolerance((accuracy > 1.e-5 ? accuracy : 1.e-5));
      // #else
      //       gaussSeidel.SetTolerance(accuracy);
      // #endif
      // #elif defined(KEMFIELD_USE_MPI)
#if defined(KEMFIELD_USE_MPI)
      KGaussSeidel<KElectrostaticBoundaryIntegrator::ValueType,
           KGaussSeidel_MPI> gaussSeidel;
#elif defined(KEMFIELD_USE_OPENCL)
      KGaussSeidel<KElectrostaticBoundaryIntegrator::ValueType,
           KGaussSeidel_OpenCL> gaussSeidel;
#ifndef KEMFIELD_USE_DOUBLE_PRECISION
      gaussSeidel.SetTolerance((accuracy > 1.e-5 ? accuracy : 1.e-5));
#else
      gaussSeidel.SetTolerance(accuracy);
#endif
#else
      KGaussSeidel<KElectrostaticBoundaryIntegrator::ValueType> gaussSeidel;
      gaussSeidel.SetTolerance(accuracy);
#endif

      MPI_SINGLE_PROCESS
      {
    gaussSeidel.AddVisitor(new KIterationDisplay<KElectrostaticBoundaryIntegrator::ValueType>());
      }

#ifdef KEMFIELD_USE_VTK
      if (residualGraph)
    MPI_SINGLE_PROCESS
      gaussSeidel.AddVisitor(new KVTKResidualGraph<double>());
#endif

      KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>* stateReader = new KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      gaussSeidel.AddVisitor(stateReader);

      KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>* stateWriter = new KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      stateWriter->Interval(saveIncrement);
      gaussSeidel.AddVisitor(stateWriter);

      if (computeTime) StartTimer();
      gaussSeidel.Solve(*A,x,b);
      if (computeTime) computationTime = Time();
    }
    else if (method == 2)
    {
#if defined(KEMFIELD_USE_MPI) && defined(KEMFIELD_USE_OPENCL)
      KRobinHood<KElectrostaticBoundaryIntegrator::ValueType,
         KRobinHood_MPI_OpenCL> robinHood;
#ifndef KEMFIELD_USE_DOUBLE_PRECISION
      robinHood.SetTolerance((accuracy > 1.e-5 ? accuracy : 1.e-5));
#else
      robinHood.SetTolerance(accuracy);
#endif
#elif defined(KEMFIELD_USE_MPI)
      KRobinHood<KElectrostaticBoundaryIntegrator::ValueType,
         KRobinHood_MPI> robinHood;
#elif defined(KEMFIELD_USE_OPENCL)
      KRobinHood<KElectrostaticBoundaryIntegrator::ValueType,
         KRobinHood_OpenCL> robinHood;
#ifndef KEMFIELD_USE_DOUBLE_PRECISION
      robinHood.SetTolerance((accuracy > 1.e-5 ? accuracy : 1.e-5));
#else
      robinHood.SetTolerance(accuracy);
#endif
#else
      KRobinHood<KElectrostaticBoundaryIntegrator::ValueType> robinHood;
      robinHood.SetTolerance(accuracy);
#endif

      MPI_SINGLE_PROCESS
      {
    robinHood.AddVisitor(new KIterationDisplay<KElectrostaticBoundaryIntegrator::ValueType>());
      }

      KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>* stateReader = new KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      robinHood.AddVisitor(stateReader);

      KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>* stateWriter = new KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      stateWriter->Interval(saveIncrement);
      robinHood.AddVisitor(stateWriter);

      robinHood.SetResidualCheckInterval((rh_increment ? rh_increment : b.Dimension()));

#ifdef KEMFIELD_USE_VTK
      if (residualGraph)
      {
    robinHood.SetResidualCheckInterval(1);
    MPI_SINGLE_PROCESS
      robinHood.AddVisitor(new KVTKResidualGraph<double>());
      }
#endif

      if (computeTime) StartTimer();
      robinHood.Solve(*A,x,b);
      if (computeTime) computationTime = Time();
    }
    else if (method == 3)
    {
      KMultiElementRobinHood<KElectrostaticBoundaryIntegrator::ValueType,
                 KMultiElementRobinHood_SingleThread> robinHood;
      robinHood.SetSubspaceDimension(rh_dimension);
      robinHood.SetTolerance(accuracy);

      MPI_SINGLE_PROCESS
      {
    robinHood.AddVisitor(new KIterationDisplay<KElectrostaticBoundaryIntegrator::ValueType>());
      }

      KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>* stateReader = new KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      robinHood.AddVisitor(stateReader);

      KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>* stateWriter = new KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      stateWriter->Interval(saveIncrement);
      robinHood.AddVisitor(stateWriter);

      robinHood.SetResidualCheckInterval((rh_increment ? rh_increment : b.Dimension()/2));

#ifdef KEMFIELD_USE_VTK
      if (residualGraph)
      {
    robinHood.SetResidualCheckInterval(1);
    MPI_SINGLE_PROCESS
      robinHood.AddVisitor(new KVTKResidualGraph<double>());
      }
#endif

      if (computeTime) StartTimer();
      robinHood.Solve(*A,x,b);
      if (computeTime) computationTime = Time();
    }
    else if (method == 4)
    {
      KSimpleIterativeKrylovSolver<KElectrostaticBoundaryIntegrator::ValueType, KBiconjugateGradientStabilized> biCGStab;
      biCGStab.SetTolerance(accuracy);

      MPI_SINGLE_PROCESS
      {
    biCGStab.AddVisitor(new KIterationDisplay<KElectrostaticBoundaryIntegrator::ValueType>());
      }

      KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>* stateReader = new KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      biCGStab.AddVisitor(stateReader);

      KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>* stateWriter = new KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      stateWriter->Interval(saveIncrement);
      biCGStab.AddVisitor(stateWriter);

#ifdef KEMFIELD_USE_VTK
      if (residualGraph)
      {
    MPI_SINGLE_PROCESS
      biCGStab.AddVisitor(new KVTKResidualGraph<double>());
      }
#endif

      if (computeTime) StartTimer();
      biCGStab.Solve(*A,x,b);
      if (computeTime) computationTime = Time();
    }
    else if (method == 5)
    {
        //TODO, make this preconditioned!!
      KSimpleIterativeKrylovSolver<KElectrostaticBoundaryIntegrator::ValueType, KBiconjugateGradientStabilized> biCGStab;
      biCGStab.SetTolerance(accuracy);

      MPI_SINGLE_PROCESS
      {
    biCGStab.AddVisitor(new KIterationDisplay<KElectrostaticBoundaryIntegrator::ValueType>());
      }

      KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>* stateReader = new KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      biCGStab.AddVisitor(stateReader);

      KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>* stateWriter = new KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      stateWriter->Interval(saveIncrement);
      biCGStab.AddVisitor(stateWriter);

#ifdef KEMFIELD_USE_VTK
      if (residualGraph)
      {
    MPI_SINGLE_PROCESS
      biCGStab.AddVisitor(new KVTKResidualGraph<double>());
      }
#endif

      if (computeTime) StartTimer();
      biCGStab.Solve(*A,x,b);
      if (computeTime) computationTime = Time();
    }
    else if (method == 6)
    {
      KSuccessiveSubspaceCorrection<KElectrostaticBoundaryIntegrator::ValueType,
                 KSuccessiveSubspaceCorrection_SingleThread> ssc;
      ssc.SetSubspaceDimension(rh_dimension);
      ssc.SetTolerance(accuracy);

      MPI_SINGLE_PROCESS
      {
    ssc.AddVisitor(new KIterationDisplay<KElectrostaticBoundaryIntegrator::ValueType>());
      }

      KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>* stateReader = new KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      ssc.AddVisitor(stateReader);

      KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>* stateWriter = new KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      stateWriter->Interval(saveIncrement);
      ssc.AddVisitor(stateWriter);

      ssc.SetResidualCheckInterval((rh_increment ? rh_increment : b.Dimension()/2));

#ifdef KEMFIELD_USE_VTK
      if (residualGraph)
      {
    ssc.SetResidualCheckInterval(1);

    MPI_SINGLE_PROCESS
      ssc.AddVisitor(new KVTKResidualGraph<double>());
      }
#endif

      if (computeTime) StartTimer();
      ssc.Solve(*A,x,b);
      if (computeTime) computationTime = Time();
    }
    else if (method == 7)
    {

      MPI_SINGLE_PROCESS
      {
      KSimpleIterativeKrylovSolver< KElectrostaticBoundaryIntegrator::ValueType, KGeneralizedMinimalResidual> gmres;
      gmres.SetTolerance(accuracy);
      //gmres.SetRestartParameter(30);

      gmres.AddVisitor(new KIterationDisplay<KElectrostaticBoundaryIntegrator::ValueType>());

      KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>* stateReader = new KIterativeStateReader<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      gmres.AddVisitor(stateReader);

      KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>* stateWriter = new KIterativeStateWriter<KElectrostaticBoundaryIntegrator::ValueType>(surfaceContainer);
      stateWriter->Interval(saveIncrement);
      gmres.AddVisitor(stateWriter);


      if (computeTime) StartTimer();
      gmres.Solve(*A,x,b);
      if (computeTime) computationTime = Time();
      }

    }
#ifdef KEMFIELD_USE_PETSC
    else if (method == 8)
    {
      KPETScSolver<KElectrostaticBoundaryIntegrator::ValueType> petscSolver;
      petscSolver.CacheMatrixElements(cache);
      if (computeTime) StartTimer();
      petscSolver.SetTolerance(accuracy);
      petscSolver.Solve(*A,x,b);
      if (computeTime) computationTime = Time();
    }
#endif
  }

  if (plotMatrix)
  {
    KEMFieldCanvas* fieldCanvas = NULL;

#if defined(KEMFIELD_USE_VTK)
    fieldCanvas = new KEMVTKFieldCanvas(0.,double(A->Dimension()),0.,double(A->Dimension()),1.e30,true);
#elif defined(KEMFIELD_USE_ROOT)
    fieldCanvas = new KEMRootFieldCanvas(0.,A->Dimension(),0,A->Dimension(),1.e30,true);
#endif

    if (fieldCanvas)
    {
      std::vector<double> x_;
      std::vector<double> y_;
      std::vector<double> V_;

      for (unsigned int i=0;i<A->Dimension();i++)
      {
    x_.push_back(i);
    y_.push_back(i);

    for (unsigned int j=0;j<A->Dimension();j++)
    {
      double value = fabs(A->operator()(i,j));

      if (value > 1.e-16)
          V_.push_back(log(value));
      else
        V_.push_back(-16.);
    }
      }

      fieldCanvas->DrawFieldMap(x_,y_,V_,false,0.);
      fieldCanvas->LabelAxes("i","j","log (A_{ij})");
      std::string fieldCanvasName = DEFAULT_OUTPUT_DIR;
      fieldCanvas->SaveAs(fieldCanvasName + "/Matrix.gif");
    }
  }

  delete A;

  MPI_SINGLE_PROCESS
  {
    if (computeTime)
    {
      std::cout<<""<<std::endl;
      std::cout<<"Solution found in "<<computationTime<<" seconds."<<std::endl;
      std::cout<<""<<std::endl;

      bool newFile = true;
      if (KEMFileInterface::GetInstance()->FileExists("SphericalCapacitorTimes.txt"))
    newFile = false;

      std::ofstream file;
      file.open("SphericalCapacitorTimes.txt",std::ios::app);

      if (newFile)
      {
    file<<"Processors\tMethod\t";
      if (method == 3)
    file<<"RH_Dimension\t";
      file<<"Cached\tScale\tAccuracy\tDimension\tTime\n";
}

#ifdef KEMFIELD_USE_MPI
      file<<KMPIInterface::GetInstance()->GetNProcesses()<<"\t";
#else
      file<<"1\t";
#endif
      file<<std::setprecision(16)<<method<<"\t";
      if (method == 3)
    file<<rh_dimension<<"\t";
      file<<cache<<"\t"<<scale<<"\t"<<accuracy<<"\t"<<dimension<<"\t"<<computationTime<<"\n";
      file.close();
    }

    double P[3] = {0,0,0};

    double field_numeric[4];
    double field_analytic[4];

    double Q;
    double Q_b1;
    double Q_b2;
    double Q_b3;
    double Q_b4;
    double Q_computed;
    double Q_1;
    double Q_2;
    double Q_3;

    Q = potential1*4.*KEMConstants::Pi/
      (-1./(KEMConstants::Eps0*permittivity2*radius3) +
       1./(KEMConstants::Eps0*permittivity2*radius2) -
       1./(KEMConstants::Eps0*permittivity1*radius2) +
       1./(KEMConstants::Eps0*permittivity1*radius1));

    Q_b1 = -((permittivity1-1.)*Q/
         (permittivity1));

    Q_b2 = ((permittivity1-1.)*Q/
        (permittivity1));

    Q_b3 = -((permittivity2-1.)*Q/
         (permittivity2));

    Q_b4 = ((permittivity2-1.)*Q/
        (permittivity2));

    std::cout<<"analytic charges: "<<std::endl;
    std::cout<<"Q: "<<Q<<std::endl;
    std::cout<<"Q_b1: "<<Q_b1<<std::endl;
    std::cout<<"Q_b2: "<<Q_b2<<std::endl;
    std::cout<<"Q_b3: "<<Q_b3<<std::endl;
    std::cout<<"Q_b4: "<<Q_b4<<std::endl;
    std::cout<<""<<std::endl;

    std::cout<<"analytic charges: "<<std::endl;
    std::cout<<"Q_1 analytic: "<<Q+Q_b1<<std::endl;
    std::cout<<"Q_2 analytic: "<<Q_b2+Q_b3<<std::endl;
    std::cout<<"Q_3 analytic: "<<Q_b4-Q<<std::endl;
    std::cout<<""<<std::endl;

    Q_computed = 0.;
    Q_1 = 0.;
    Q_2 = 0.;
    Q_3 = 0.;

    unsigned int i=0;
    for (KSurfaceContainer::iterator it=surfaceContainer.begin();
     it!=surfaceContainer.end();it++)
    {
      if ((*it)->GetShape()->Centroid().Magnitude()<.5*(radius1+radius2))
    Q_1 += ((*it)->GetShape()->Area() *
        dynamic_cast<KElectrostaticBasis*>(*it)->GetSolution());
      else if ((*it)->GetShape()->Centroid().Magnitude()<.5*(radius3+radius2))
    Q_2 += ((*it)->GetShape()->Area() *
        dynamic_cast<KElectrostaticBasis*>(*it)->GetSolution());
      else
    Q_3 += ((*it)->GetShape()->Area() *
        dynamic_cast<KElectrostaticBasis*>(*it)->GetSolution());
      i++;
    }

    MPI_SINGLE_PROCESS
    {
      double rel_max[4] = {0,0,0,0};
      double rel_min[4] = {1.e10,1.e10,1.e10,1.e10};
      double rel_average[4] = {0,0,0,0};

      double abs_max[4] = {0,0,0,0};
      double abs_min[4] = {1.e10,1.e10,1.e10,1.e10};
      double abs_average[4] = {0,0,0,0};

      std::cout<<"total computed charge: "<<Q_computed<<std::endl;

      std::cout<<"Q_1: "<<Q_1<<std::endl;
      std::cout<<"Q_2: "<<Q_2<<std::endl;
      std::cout<<"Q_3: "<<Q_3<<std::endl;

      std::cout<<""<<std::endl;
      std::cout<<"comparisons:"<<std::endl;
      std::cout<<std::setprecision(16)<<"Q_1 vs (Q+Q_b1): "<<(Q_1-(Q+Q_b1))/(Q+Q_b1)*100.<<" %"<<std::endl;
      std::cout<<std::setprecision(16)<<"Q_2 vs (Q_b2+Q_b3): "<<(Q_2 - (Q_b2+Q_b3))/(Q_b2+Q_b3)*100.<<" %"<<std::endl;
      std::cout<<std::setprecision(16)<<"Q_3 vs (-Q+Q_b4): "<<(Q_3 - (-Q+Q_b4))/(-Q+Q_b4)*100.<<" %"<<std::endl;

      if (draw)
      {
#ifdef KEMFIELD_USE_VTK
    KEMVTKViewer viewer(surfaceContainer);
    std::cout<<"generating spherical capacitor file"<<std::endl;
    viewer.GenerateGeometryFile(std::string(DEFAULT_OUTPUT_DIR) + std::string("/SphericalCapacitor.vtp"));
    std::cout<<"saved to "<<std::string(DEFAULT_OUTPUT_DIR) + std::string("/SphericalCapacitor.vtp")<<std::endl;
    viewer.ViewGeometry();
#endif

#ifdef KEMFIELD_USE_OPENCL
    KIntegratingFieldSolver<KOpenCLElectrostaticBoundaryIntegrator> field(oclSurfaceContainer,integrator);
    field.Initialize();
#else
    KIntegratingFieldSolver<KElectrostaticBoundaryIntegrator> field(surfaceContainer,integrator);
#endif

    srand((unsigned)time(0));

    int nTest = 100;

    // we reject sample points that are sufficiently close to one of the
    // boundaries that polygonization of the spheres becomes an issue
    double tolerance = 1.e-1;

    for (int i=0;i<nTest;i++)
    {
      P[0] = -4. + 8.*((double)rand())/RAND_MAX;
      P[1] = -4. + 8.*((double)rand())/RAND_MAX;
      P[2] = -4. + 8.*((double)rand())/RAND_MAX;

      double r = sqrt(P[0]*P[0] + P[1]*P[1] + P[2]*P[2]);

      if (fabs(r-radius1)<tolerance*radius1 ||
          fabs(r-radius2)<tolerance*radius2 ||
          fabs(r-radius3)<tolerance*radius3)
      {
        i--;
        continue;
      }

      Field_Analytic(Q,
             radius1,
             radius2,
             radius3,
             permittivity1,
             permittivity2,
             P,
             field_analytic);

      field_numeric[0] = field.Potential(P);

      KEMThreeVector f = field.ElectricField(P);

      for (int j=0;j<3;j++)
        field_numeric[j+1] = f[j];

      for (int i=0;i<4;i++)
      {
        double tmp = fabs(field_analytic[i]-field_numeric[i]);

        abs_average[i] += tmp;
        if (abs_max[i]<tmp) abs_max[i] = tmp;
        if (abs_min[i]>tmp) abs_min[i] = tmp;

        if (fabs(field_analytic[i])>1.e-8)
          tmp = tmp/fabs(field_analytic[i])*100.;
        else
          tmp = tmp*100.;

        rel_average[i] += tmp;
        if (rel_max[i]<tmp) rel_max[i] = tmp;
        if (rel_min[i]>tmp) rel_min[i] = tmp;

        // if (tmp>50. && verbose)
        // {
        //  std::cout<<std::setprecision(8)<<"P: ("<<P[0]<<","<<P[1]<<","<<P[2]<<")"<<std::endl;
        //  std::cout<<"r: "<<r<<std::endl;
        //  std::cout<<"analytic: "<<field_analytic[0]<<"\t"<<field_analytic[1]<<"\t"<<field_analytic[2]<<"\t"<<field_analytic[3]<<std::endl;
        //  std::cout<<"numeric:  "<<field_numeric[0]<<"\t"<<field_numeric[1]<<"\t"<<field_numeric[2]<<"\t"<<field_numeric[3]<<std::endl;
        //  std::cout<<"dimension "<<i<<" error: "<<tmp<<std::endl;
        //  std::cout<<""<<std::endl;
        // }
      }
    }

    for (int i=0;i<4;i++)
    {
      rel_average[i]/=nTest;
      abs_average[i]/=nTest;
    }

    if (verbose)
    {
      std::cout<<""<<std::endl;
      std::cout<<"Relative Accuracy Summary (analytic vs numeric): "<<std::endl;
      std::cout<<"\t Average \t\t Max \t\t\t Min"<<std::endl;
      std::cout<<"Phi:\t "<<rel_average[0]<<" % \t "<<rel_max[0]<<" % \t "<<rel_min[0]<<" %"<<std::endl;
      std::cout<<"Ex: \t "<<rel_average[1]<<" % \t "<<rel_max[1]<<" % \t "<<rel_min[1]<<" %"<<std::endl;
      std::cout<<"Ey: \t "<<rel_average[2]<<" % \t "<<rel_max[2]<<" % \t "<<rel_min[2]<<" %"<<std::endl;
      std::cout<<"Ez: \t "<<rel_average[3]<<" % \t "<<rel_max[3]<<" % \t "<<rel_min[3]<<" %"<<std::endl;
      std::cout<<""<<std::endl;

      std::cout<<"Absolute Accuracy Summary (analytic vs numeric): "<<std::endl;
      std::cout<<"\t Average \t\t Max \t\t\t Min"<<std::endl;
      std::cout<<"Phi:\t "<<abs_average[0]<<" \t "<<abs_max[0]<<" \t "<<abs_min[0]<<std::endl;
      std::cout<<"Ex: \t "<<abs_average[1]<<" \t "<<abs_max[1]<<" \t "<<abs_min[1]<<std::endl;
      std::cout<<"Ey: \t "<<abs_average[2]<<" \t "<<abs_max[2]<<" \t "<<abs_min[2]<<std::endl;
      std::cout<<"Ez: \t "<<abs_average[3]<<" \t "<<abs_max[3]<<" \t "<<abs_min[3]<<std::endl;
      std::cout<<""<<std::endl;
    }

#ifdef KEMFIELD_USE_ROOT

    // sample the potentials and fields along z
    unsigned int nSamples = 1000.;

    std::vector<double> phiA_points;
    std::vector<double> phiN_points;
    std::vector<double> EA_points;
    std::vector<double> EN_points;
    std::vector<double> Z_points;

    P[0] = P[1] = 0.;

    for (unsigned int i=0;i<nSamples;i++)
    {
      P[2] = 2.*radius3*((double)i)/nSamples;

      Field_Analytic(Q,
             radius1,
             radius2,
             radius3,
             permittivity1,
             permittivity2,
             P,
             field_analytic);

      field_numeric[0] = field.Potential(P);

      KEMThreeVector f = field.ElectricField(P);

      for (int j=0;j<3;j++)
        field_numeric[j+1] = f[j];

      Z_points.push_back(P[2]);
      phiA_points.push_back(field_analytic[0]);
      phiN_points.push_back(field_numeric[0]);
      EA_points.push_back(sqrt(field_analytic[1]*field_analytic[1] +
                   field_analytic[2]*field_analytic[2] +
                   field_analytic[3]*field_analytic[3]));
      EN_points.push_back(sqrt(field_numeric[1]*field_numeric[1] +
                   field_numeric[2]*field_numeric[2] +
                   field_numeric[3]*field_numeric[3]));

      // std::cout<<"Phi, |E| at z = "<<P[2]<<":"<<std::endl;
      // std::cout<<"Phi:\t"<<field_analytic[0]<<"\t"<<field_numeric[0]<<std::endl;
      // std::cout<<"|E|:\t"
      //         <<sqrt(field_analytic[1]*field_analytic[1] +
      //            field_analytic[2]*field_analytic[2] +
      //            field_analytic[3]*field_analytic[3])<<"\t"
      //         <<sqrt(field_numeric[1]*field_numeric[1] +
      //            field_numeric[2]*field_numeric[2] +
      //            field_numeric[3]*field_numeric[3])<<std::endl;

      // std::cout<<""<<std::endl;
    }

    TCanvas *C = new TCanvas("C","Canvas",5,5,900,450);
    C->Divide(2);
    C->cd(1);
    C->SetBorderMode(0);
    C->SetFillColor(kWhite);
    gStyle->SetOptStat(0000000);
    gStyle->SetOptFit(0111);

    TMultiGraph* mg1 = new TMultiGraph();

    TGraph* g1 = new TGraph(nSamples,&Z_points.at(0),&phiA_points.at(0));
    g1->SetLineColor(kBlue);
    mg1->Add(g1);
    TGraph* g2 = new TGraph(nSamples,&Z_points.at(0),&phiN_points.at(0));
    g2->SetLineColor(kGreen);
    mg1->Add(g2);

    mg1->Draw("AL");
    std::stringstream s2;s2<<"Dielectric Phi: analytic vs Numeric (Accuracy = 1.e-";
    s2<<fabs(log10(accuracy))<<")";
    // mg1->SetTitle(s2.str().c_str());
    mg1->SetTitle("");
    mg1->GetXaxis()->SetTitle("r (m)");
    mg1->GetXaxis()->CenterTitle();
    // mg1->GetXaxis()->SetLimits(4.e-9,1.1e-5);
    mg1->GetXaxis()->SetTitleOffset(1.25);
    mg1->GetYaxis()->SetTitle("Phi (V)");
    mg1->GetYaxis()->CenterTitle();
    mg1->GetYaxis()->SetTitleOffset(1.25);
    mg1->Draw("AL");

    C->cd(2);
    C->SetBorderMode(0);
    C->SetFillColor(kWhite);
    gStyle->SetOptStat(0000000);
    gStyle->SetOptFit(0111);

    TMultiGraph* mg2 = new TMultiGraph();

    TGraph* g3 = new TGraph(nSamples,&Z_points.at(0),&EA_points.at(0));
    g3->SetLineColor(kBlue);
    mg2->Add(g3);
    TGraph* g4 = new TGraph(nSamples,&Z_points.at(0),&EN_points.at(0));
    g4->SetLineColor(kGreen);
    mg2->Add(g4);

    mg2->Draw("AL");
    s2.str("");s2<<"Dielectric |E|: analytic vs Numeric (Accuracy = 1.e-";
    s2<<fabs(log10(accuracy))<<")";
    // mg2->SetTitle(s2.str().c_str());
    mg2->SetTitle("");
    mg2->GetXaxis()->SetTitle("r (m)");
    mg2->GetXaxis()->CenterTitle();
    // mg2->GetXaxis()->SetLimits(4.e-9,1.1e-5);
    mg2->GetXaxis()->SetTitleOffset(1.25);
    mg2->GetYaxis()->SetTitle("|E| (V/m)");
    mg2->GetYaxis()->CenterTitle();
    mg2->GetYaxis()->SetTitleOffset(1.25);
    mg2->Draw("AL");

    s2.str("");s2<<DEFAULT_OUTPUT_DIR<<"/dielectricPhi_"<<fabs(log10(accuracy))<<".pdf";
    C->SaveAs(s2.str().c_str());

#endif

    double z1 = -2.;
    double z2 = 6.;
    double x1 = 0.;
    double x2 = 4.;

    double dx = .05;
    double dz = .05;

    KEMFieldCanvas* fieldCanvas = NULL;

#if defined(KEMFIELD_USE_ROOT)
    fieldCanvas = new KEMRootFieldCanvas(z1,z2,x1,x2,1.e30,true);
#elif defined(KEMFIELD_USE_VTK)
    fieldCanvas = new KEMVTKFieldCanvas(z1,z2,x1,x2,1.e30,true);
#endif

    if (fieldCanvas)
    {
      int N_x = (int)((x2-x1)/dx);
      int N_z = (int)((z2-z1)/dz);

      int counter = 0;
      int countermax = N_z*N_x;

      std::vector<double> x_;
      std::vector<double> y_;
      std::vector<double> V_;

      double spacing[2] = {dz,dx};

      double phi = 0.;

      std::cout<<"Computing potential field on a "<<N_x<<" by "<<N_z<<" grid"<<std::endl;

      for (int g=0;g<N_z;g++)
        x_.push_back(z1+g*spacing[0]+spacing[0]/2);

      for (int h=0;h<N_x;h++)
        y_.push_back(x1+h*spacing[1]+spacing[1]/2);

      for (int g=0;g<N_z;g++)
      {
        for (int h=0;h<N_x;h++)
        {
          double P[3] = {y_[h],0.,x_[g]};

          phi = field.Potential(P);

          counter++;
          if (counter*100%countermax==0)
          {
        std::cout<<"\r";
        std::cout<<int((float)counter/countermax*100)<<" %";
        std::cout.flush();
          }

          V_.push_back(phi);
        }
      }
      std::cout<<"\r";
      std::cout.flush();

      fieldCanvas->DrawFieldMap(x_,y_,V_,false,.5);
      fieldCanvas->LabelAxes("z (m)","r (m)","#Phi (V)");
      std::string fieldCanvasName = DEFAULT_OUTPUT_DIR;
      fieldCanvas->SaveAs(fieldCanvasName + "/VFieldMap_rz.gif");
    }
      }
    }
  }

#ifdef KEMFIELD_USE_PETSC
  KPETScInterface::GetInstance()->Finalize();
#elif KEMFIELD_USE_MPI
  KMPIInterface::GetInstance()->Finalize();
#endif

  return 0;
}

void ReadInTriangles(std::string fileName,KSurfaceContainer& surfaceContainer)
{
  typedef KSurface<KElectrostaticBasis,KDirichletBoundary,KTriangle> KDirichletTriangle;
  typedef KSurface<KElectrostaticBasis,KNeumannBoundary,KTriangle> KNeumannTriangle;

  std::string inBuf;
  std::vector<std::string> token;
  std::fstream file;

  file.open(fileName.c_str(), std::fstream::in);

  getline(file,inBuf);
  token = Tokenize(" \t",inBuf);

  int lineNum = 0;

//  bool dir = false;

  int counter = 0;

  while (!file.eof())
  {
    lineNum++;
    if (token.size()>0)
    {
      if (token.at(0).at(0)=='#')
      {
    getline(file,inBuf);
    token = Tokenize(" \t",inBuf);
    continue;
      }
    }

    if (token.size()!=0)
    {
      if (token.size()==1)
      {
    MPI_SINGLE_PROCESS
      KEMField::cout<<"Reading in "<<token.at(0)<<" triangles."<<KEMField::endl;
      }
      else
      {
    std::stringstream s;
    int n[4];
    double d[15];

    for (int i=0;i<4;i++)
    { s.str(token.at(i)); s >> n[i]; s.clear(); }
    for (int i=0;i<15;i++)
    { s.str(token.at(i+4)); s >> d[i]; s.clear(); }

    if (fabs(d[13]/d[12])<1.e-10)
    {
//      dir = true;
      // if (dir_ == false)
      //   std::cout<<"switch to dirichlet at "<<counter<<std::endl;

      KDirichletTriangle* t = new KDirichletTriangle();

      t->SetA(d[9]);
      t->SetB(d[10]);
      t->SetP0(KPosition(d[0],d[1],d[2]));
      t->SetN1(KPosition(d[3],d[4],d[5]));
      t->SetN2(KPosition(d[6],d[7],d[8]));
      t->SetBoundaryValue(d[11]);

      surfaceContainer.push_back(t);
    }
    else
    {
//      dir = false;
      // if (dir_ == true)
      //   std::cout<<"switch to neumann at "<<counter<<std::endl;

      KNeumannTriangle* t = new KNeumannTriangle();

      t->SetA(d[9]);
      t->SetB(d[10]);
      t->SetP0(KPosition(d[0],d[1],d[2]));
      t->SetN1(KPosition(d[3],d[4],d[5]));
      t->SetN2(KPosition(d[6],d[7],d[8]));
      t->SetNormalBoundaryFlux(d[13]/d[12]);

      surfaceContainer.push_back(t);
    }
//    dir_ = dir;
    counter++;
      }
    }
    getline(file,inBuf);
    token = Tokenize(" \t",inBuf);
  }
  file.close();
}

std::vector<std::string> Tokenize(std::string separators,std::string input)
{
  unsigned int startToken = 0, endToken; // Pointers to the token pos
  std::vector<std::string> tokens;       // Vector to keep the tokens
  unsigned int commentPos = input.size()+1;

  if( separators.size() > 0 && input.size() > 0 )
  {
    // Check for comment
    for (unsigned int i=0;i<input.size();i++)
    {
      if (input[i]=='#' || (i<input.size()-1&&(input[i]=='/'&&input[i+1]=='/')))
      {
    commentPos=i;
    break;
      }
    }

    while( startToken < input.size() )
    {
      // Find the start of token
      startToken = input.find_first_not_of( separators, startToken );

      // Stop parsing when comment symbol is reached
      if (startToken == commentPos)
      {
    if (tokens.size()==0)
      tokens.push_back("#");
    return tokens;
      }

      // If found...
      if( startToken != (unsigned int)std::string::npos)
      {
    // Find end of token
    endToken = input.find_first_of( separators, startToken );

    if( endToken == (unsigned int)std::string::npos )
      // If there was no end of token, assign it to the end of string
      endToken = input.size();

    // Extract token
    tokens.push_back( input.substr( startToken, endToken - startToken ) );

    // Update startToken
    startToken = endToken;
      }
    }
  }

  return tokens;
}

void Field_Analytic(double Q,double radius1,double radius2,double radius3,double permittivity1,double permittivity2,double *P,double *F)
{
  // This function computes the electric potential and electric field due to a
  // charge <Q> on a sphere of radius <radius1>, surrounded by two dielectrics.

  double r = sqrt(P[0]*P[0]+P[1]*P[1]+P[2]*P[2]);
  double fEps0 = 8.85418782e-12;

  if (r<radius1)
  {
    F[0] = Q/(4.*M_PI)*(-1./(fEps0*permittivity2*radius3) +
            1./(fEps0*permittivity2*radius2) -
            1./(fEps0*permittivity1*radius2) +
            1./(fEps0*permittivity1*radius1));
    F[1] = F[2] = F[3] = 0.;
  }
  else if (r<radius2)
  {
    F[0] = Q/(4.*M_PI)*(-1./(fEps0*permittivity2*radius3) +
            1./(fEps0*permittivity2*radius2) -
            1./(fEps0*permittivity1*radius2) +
            1./(fEps0*permittivity1*r));
    F[1] = Q/(4.*M_PI*fEps0*permittivity1*r*r)*P[0]/r;
    F[2] = Q/(4.*M_PI*fEps0*permittivity1*r*r)*P[1]/r;
    F[3] = Q/(4.*M_PI*fEps0*permittivity1*r*r)*P[2]/r;
  }
  else if (r<radius3)
  {
    F[0] = Q/(4.*M_PI)*(-1./(fEps0*permittivity2*radius3) +
            1./(fEps0*permittivity2*r));
    F[1] = Q/(4.*M_PI*fEps0*permittivity2*r*r)*P[0]/r;
    F[2] = Q/(4.*M_PI*fEps0*permittivity2*r*r)*P[1]/r;
    F[3] = Q/(4.*M_PI*fEps0*permittivity2*r*r)*P[2]/r;
  }
  else
  {
    F[0] = 0.;
    F[1] = 0.;
    F[2] = 0.;
    F[3] = 0.;
  }

}
