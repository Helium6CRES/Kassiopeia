cmake_minimum_required( VERSION 2.8.6 )

project(KEMField)
include(KasperDefaults)

if(POLICY CMP0053)
    cmake_policy(SET CMP0053 OLD)
endif()

# Module version
set(MODULE_VERSION_MAJOR 2)
set(MODULE_VERSION_MINOR 1)
set(MODULE_VERSION_PATCH 0)
set(MODULE_VERSION "${MODULE_VERSION_MAJOR}.${MODULE_VERSION_MINOR}.${MODULE_VERSION_PATCH}")

# paths
kasper_module_paths( KEMField )

#set(MODULE_HEADER_DIRS)
#set(MODULE_CFLAGS)
#set(KASPER_INCLUDE_DIRS)
#set(EXTERNAL_INCLUDE_DIRS)

kasper_find_module(Kommon)

#option (@PROJECT_NAME@_USE_VTK "Enables the use of VTK for visualization" OFF)
set (@PROJECT_NAME@_USE_VTK ${KASPER_USE_VTK})
if (@PROJECT_NAME@_USE_VTK)
  kasper_find_vtk()
endif()

#option (@PROJECT_NAME@_USE_ROOT "Link against ROOT" OFF)
set (@PROJECT_NAME@_USE_ROOT ${KASPER_USE_ROOT})
if (@PROJECT_NAME@_USE_ROOT)
  find_package(ROOT REQUIRED)
endif()

#option (KEMField_USE_GSL "Use GSL" OFF)
set (@PROJECT_NAME@_USE_GSL ${KASPER_USE_GSL})
if (@PROJECT_NAME@_USE_GSL)
  find_package(GSL REQUIRED)
endif ()

# Input files
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data)

# Subdirectories

# NOTE: If files in a subdirectory INCLUDE HEADERS from other subdirectories,
# this subdirectory has to come AFTER those other directories.
# Otherwise you will encounter strange linker or even compiler errors.
set (SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/Source/2.0)
add_subdirectory(${SOURCE}/Core)
add_subdirectory(${SOURCE}/Exceptions)
add_subdirectory(${SOURCE}/IO/HashGenerator)
add_subdirectory(${SOURCE}/IO/StructuredASCII)
add_subdirectory(${SOURCE}/IO/Streamers)
add_subdirectory(${SOURCE}/IO/FileManipulation)
add_subdirectory(${SOURCE}/Math)
add_subdirectory(${SOURCE}/Math/Array)
add_subdirectory(${SOURCE}/Math/Utilities)
add_subdirectory(${SOURCE}/Math/VectorMath)
add_subdirectory(${SOURCE}/Surfaces)
add_subdirectory(${SOURCE}/BoundaryIntegrals/Core)
add_subdirectory(${SOURCE}/BoundaryIntegrals/Electrostatic)
add_subdirectory(${SOURCE}/LinearAlgebra/Core)
add_subdirectory(${SOURCE}/LinearAlgebra/Solvers)
add_subdirectory(${SOURCE}/LinearAlgebra/Preconditioners)
add_subdirectory(${SOURCE}/LinearAlgebra/Visitors)
add_subdirectory(${SOURCE}/FieldSolvers/Core)
add_subdirectory(${SOURCE}/ExternalFields/Electromagnets)
add_subdirectory(${SOURCE}/FieldSolvers/Integrating)
add_subdirectory(${SOURCE}/FieldSolvers/ZonalHarmonic/Generator)
add_subdirectory(${SOURCE}/FieldSolvers/ZonalHarmonic/Solver)
add_subdirectory(${SOURCE}/FastMultipole/Core)
add_subdirectory(${SOURCE}/FastMultipole/Math)
add_subdirectory(${SOURCE}/FastMultipole/Kernel)
add_subdirectory(${SOURCE}/FastMultipole/Tree)
add_subdirectory(${SOURCE}/FastMultipole/Electrostatics)
add_subdirectory(${SOURCE}/FastMultipole/Interface/Extraction)
add_subdirectory(${SOURCE}/FastMultipole/Interface/BoundaryIntegrals)
add_subdirectory(${SOURCE}/FastMultipole/Interface/FieldSolvers)
add_subdirectory(${SOURCE}/Plugins/CUDA)
add_subdirectory(${SOURCE}/Plugins/OpenCL)
add_subdirectory(${SOURCE}/Visualization)
add_subdirectory(${SOURCE}/Plugins/VTK)
add_subdirectory(${SOURCE}/FastMultipole/Utility)
add_subdirectory(${SOURCE}/Plugins/PETSc)
add_subdirectory(${SOURCE}/Interface/BoundaryIntegrators/Electric)
add_subdirectory(${SOURCE}/Interface/ChargeDensitySolvers/Electric)
add_subdirectory(${SOURCE}/Interface/FieldSolvers/Electric)
add_subdirectory(${SOURCE}/Interface/FieldSolvers/Magnetic)
add_subdirectory(${SOURCE}/Interface/Fields/Magnetic)
add_subdirectory(${SOURCE}/Interface/Fields/Electric)
add_subdirectory(${SOURCE}/Plugins/KGeoBag)
add_subdirectory(${SOURCE}/Plugins/VTKPart2)
add_subdirectory(${SOURCE}/Plugins/Bindings)
add_subdirectory(${SOURCE}/Plugins/Root)
add_subdirectory(${SOURCE}/Applications)
add_subdirectory(${SOURCE}/Tests)
add_subdirectory(${SOURCE}/FastMultipole/Test)
add_subdirectory(${SOURCE}/FastMultipole/Applications)
add_subdirectory(${SOURCE}/XML)

kasper_install_module()

# Add documentation targets
add_subdirectory (Documentation)
