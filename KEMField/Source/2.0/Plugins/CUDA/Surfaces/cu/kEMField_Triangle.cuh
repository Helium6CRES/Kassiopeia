#ifndef KEMFIELD_TRIANGLE_CUH
#define KEMFIELD_TRIANGLE_CUH

#include "kEMField_cuda_defines.h"

// Triangle geometry definition (as defined by the streamers in KTriangle.hh):
//
// data[0]:     A
// data[1]:     B
// data[2..4]:  P0[0..2]
// data[5..7]:  N1[0..2]
// data[8..10]: N2[0..2]

//______________________________________________________________________________

__forceinline__ __device__ void Tri_Centroid( CU_TYPE* cen, const CU_TYPE* data )
{
    cen[0] = data[2] + (data[0]*data[5] + data[1]*data[8])/3.;
    cen[1] = data[3] + (data[0]*data[6] + data[1]*data[9])/3.;
    cen[2] = data[4] + (data[0]*data[7] + data[1]*data[10])/3.;
}

//______________________________________________________________________________

__forceinline__ __device__ void Tri_Normal( CU_TYPE* norm, const CU_TYPE* data )
{
    norm[0] = data[6]*data[10] - data[7]*data[9];
    norm[1] = data[7]*data[8]  - data[5]*data[10];
    norm[2] = data[5]*data[9]  - data[6]*data[8];
    CU_TYPE mag = SQRT(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]);
    norm[0] = norm[0]/mag;
    norm[1] = norm[1]/mag;
    norm[2] = norm[2]/mag;
}

#endif /* KEMFIELD_TRIANGLE_CUH */
