#include "KSFieldElectrostaticBuilder.h"
#include "KSRootBuilder.h"

using namespace Kassiopeia;
namespace katrin
{

    template< >
    KSKEMFieldVTKViewerBuilder::~KComplexElement()
    {
    }

    template< >
    KSCachedBEMSolverBuilder::~KComplexElement()
    {
    }

    template< >
    KSExplicitSuperpositionSolutionComponentBuilder::~KComplexElement()
    {
    }

    template< >
    KSExplicitSuperpositionCachedBEMSolverBuilder::~KComplexElement()
    {
    }

    template< >
    KSGaussianEliminationBEMSolverBuilder::~KComplexElement()
    {
    }

    template< >
    KSRobinHoodBEMSolverBuilder::~KComplexElement()
    {
    }

    template< >
    KSElectrostaticZonalHarmonicFieldSolverBuilder::~KComplexElement()
    {
    }

    template< >
    KSElectrostaticIntegratingFieldSolverBuilder::~KComplexElement()
    {
    }

    template< >
    KSFieldElectrostaticBuilder::~KComplexElement()
    {
    }

    STATICINT sKSElectrostaticVTKViewerStructure =
        KSKEMFieldVTKViewerBuilder::Attribute< string >( "file" ) +
        KSKEMFieldVTKViewerBuilder::Attribute< bool >( "view" ) +
        KSKEMFieldVTKViewerBuilder::Attribute< bool >( "save" ) +
        KSKEMFieldVTKViewerBuilder::Attribute< bool >( "preprocessing" ) +
        KSKEMFieldVTKViewerBuilder::Attribute< bool >( "postprocessing" );

    STATICINT sKSElectrostaticCachedBEMSolverStructure =
        KSCachedBEMSolverBuilder::Attribute< string >( "name" ) +
        KSCachedBEMSolverBuilder::Attribute< string >( "hash" );


    STATICINT sKSExplicitSuperpositionSolutionComponentStructure =
        KSExplicitSuperpositionSolutionComponentBuilder::Attribute< string >( "name" ) +
        KSExplicitSuperpositionSolutionComponentBuilder::Attribute< double >( "scale" ) +
        KSExplicitSuperpositionSolutionComponentBuilder::Attribute< string >( "hash" );

    STATICINT sKSElectrostaticExplicitSuperpositionCachedBEMSolverStructure =
        KSExplicitSuperpositionCachedBEMSolverBuilder::Attribute< string >( "name" ) +
        KSExplicitSuperpositionCachedBEMSolverBuilder::ComplexElement< KSFieldElectrostatic::ExplicitSuperpositionSolutionComponent >( "component" );

    STATICINT sKSElectrostaticRobinHoodBEMSolverStructure =
        KSRobinHoodBEMSolverBuilder::Attribute< double >( "tolerance" ) +
        KSRobinHoodBEMSolverBuilder::Attribute< unsigned int >( "check_sub_interval" ) +
        KSRobinHoodBEMSolverBuilder::Attribute< unsigned int >( "display_interval" ) +
        KSRobinHoodBEMSolverBuilder::Attribute< unsigned int >( "write_interval" ) +
        KSRobinHoodBEMSolverBuilder::Attribute< unsigned int >( "plot_interval" ) +
        KSRobinHoodBEMSolverBuilder::Attribute< bool >( "cache_matrix_elements" ) +
        KSRobinHoodBEMSolverBuilder::Attribute< bool >( "use_opencl" ) +
        KSRobinHoodBEMSolverBuilder::Attribute< bool >( "use_vtk" );

    STATICINT sKSElectrostaticIntegratingFieldSolverStructure =
        KSElectrostaticIntegratingFieldSolverBuilder::Attribute< bool >( "use_opencl" );

    STATICINT sKSElectrostaticZonalHarmonicFieldSolverStructure =
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< int >( "number_of_bifurcations" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< double >( "convergence_ratio" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< double >( "proximity_to_sourcepoint" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< double >( "convergence_parameter" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< int >( "number_of_central_coefficients" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< bool >( "use_fractional_central_sourcepoint_spacing" ) +
KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< double >( "central_sourcepoint_fractional_distance" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< double >( "central_sourcepoint_spacing" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< double >( "central_sourcepoint_start" ) +
	KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< double >( "central_sourcepoint_end" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< int >( "number_of_remote_coefficients" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< double >( "remote_sourcepoint_start" ) +
        KSElectrostaticZonalHarmonicFieldSolverBuilder::Attribute< double >( "remote_sourcepoint_end" );

    STATICINT sKSElectrostaticFastMultipoleFieldSolverStructure =
    	KSFastMultipoleFieldSolverBuilder::Attribute< unsigned int >( "top_level_divisions" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< unsigned int >( "tree_level_divisions" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< unsigned int >( "expansion_degree" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< unsigned int >( "neighbor_order" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< unsigned int >( "maximum_tree_depth" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< double >( "region_expansion_factor" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< bool >( "use_region_size_estimation" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< double >( "world_cube_center_x" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< double >( "world_cube_center_y" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< double >( "world_cube_center_z" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< double >( "world_cube_length" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< bool >( "use_caching" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute<unsigned int>("verbosity") +
        KSFastMultipoleFieldSolverBuilder::Attribute< double >( "insertion_ratio" ) +
        KSFastMultipoleFieldSolverBuilder::Attribute< bool >( "use_opencl" );

    STATICINT sKSFieldElectrostaticStructure =
        KSFieldElectrostaticBuilder::Attribute< string >( "name" ) +
        KSFieldElectrostaticBuilder::Attribute< string >( "directory" ) +
        KSFieldElectrostaticBuilder::Attribute< string >( "file" ) +
        KSFieldElectrostaticBuilder::Attribute< string >( "system" ) +
        KSFieldElectrostaticBuilder::Attribute< string >( "surfaces" ) +
        KSFieldElectrostaticBuilder::Attribute< string >( "spaces" ) +
        KSFieldElectrostaticBuilder::Attribute< string >( "symmetry" ) +
        KSFieldElectrostaticBuilder::Attribute< unsigned int >( "hash_masked_bits" ) +
        KSFieldElectrostaticBuilder::Attribute< double >( "hash_threshold" ) +
        KSFieldElectrostaticBuilder::Attribute< double >( "minimum_element_area" ) +
        KSFieldElectrostaticBuilder::Attribute< double >( "maximum_element_aspect_ratio" ) +
        KSFieldElectrostaticBuilder::ComplexElement< KSFieldElectrostatic::VTKViewer >( "viewer" ) +
        KSFieldElectrostaticBuilder::ComplexElement< KSFieldElectrostatic::CachedBEMSolver >( "cached_bem_solver" ) +
        KSFieldElectrostaticBuilder::ComplexElement< KSFieldElectrostatic::ExplicitSuperpositionCachedBEMSolver >( "explicit_superposition_cached_bem_solver" ) +
        KSFieldElectrostaticBuilder::ComplexElement< KSFieldElectrostatic::GaussianEliminationBEMSolver >( "gaussian_elimination_bem_solver" ) +
        KSFieldElectrostaticBuilder::ComplexElement< KSFieldElectrostatic::RobinHoodBEMSolver >( "robin_hood_bem_solver" ) +
        KSFieldElectrostaticBuilder::ComplexElement< KrylovBEMSolver >( "krylov_bem_solver" ) +
        KSFieldElectrostaticBuilder::ComplexElement< KSFieldElectrostatic::IntegratingFieldSolver >( "integrating_field_solver" ) +
        KSFieldElectrostaticBuilder::ComplexElement< KSFieldElectrostatic::ZonalHarmonicFieldSolver >( "zonal_harmonic_field_solver" ) +
        KSFieldElectrostaticBuilder::ComplexElement< KSFieldElectrostatic::FastMultipoleFieldSolver >( "fast_multipole_field_solver" );

    STATICINT sKSFieldElectrostatic =
        KSRootBuilder::ComplexElement< KSFieldElectrostatic >( "ksfield_electrostatic" );

}
