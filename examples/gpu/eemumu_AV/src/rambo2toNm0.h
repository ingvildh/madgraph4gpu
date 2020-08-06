#ifndef RAMBO2TONM0_H
#define RAMBO2TONM0_H 1

#include <cassert>

#include "curand.h"

#include "mgOnGpuConfig.h"

//--------------------------------------------------------------------------

#define checkCurand( code )                     \
  { assertCurand( code, __FILE__, __LINE__ ); }

inline void assertCurand( curandStatus_t code, const char *file, int line, bool abort = true )
{
  if ( code != CURAND_STATUS_SUCCESS )
  {
    printf( "CurandAssert: %s %d\n", file, line );
    if ( abort ) assert( code == CURAND_STATUS_SUCCESS );
  }
}

//--------------------------------------------------------------------------

// Simplified rambo version for 2 to N (with N>=2) processes with massless particles
#ifdef __CUDACC__
namespace grambo2toNm0
#else
namespace rambo2toNm0
#endif
{

  using mgOnGpu::np4;
  using mgOnGpu::npari;
  using mgOnGpu::nparf;
  using mgOnGpu::npar;

  //--------------------------------------------------------------------------

  // Fill in the momenta of the initial particles
  // [NB: the output buffer includes both initial and final momenta, but only initial momenta are filled in]
#ifdef __CUDACC__
  __global__
#endif
  void getMomentaInitial( const double energy,    // input: energy
#if defined MGONGPU_LAYOUT_ASA
                          double momenta1d[],     // output: momenta as AOSOA[npag][npar][4][nepp]
#elif defined MGONGPU_LAYOUT_SOA
                          double momenta1d[],     // output: momenta as SOA[npar][4][nevt]
#elif defined MGONGPU_LAYOUT_AOS
                          double momenta1d[],     // output: momenta as AOS[nevt][npar][4]
#endif
                          const int nevt );       // input: #events

  //--------------------------------------------------------------------------

  // Fill in the momenta of the final particles using the RAMBO algorithm
  // [NB: the output buffer includes both initial and final momenta, but only initial momenta are filled in]
#ifdef __CUDACC__
  __global__
#endif
  void getMomentaFinal( const double energy,      // input: energy
                        const double rnarray1d[], // input: randomnumbers in [0,1] as AOSOA[npag][nparf][4][nepp]
#if defined MGONGPU_LAYOUT_ASA
                        double momenta1d[],       // output: momenta as AOSOA[npag][npar][4][nepp]
#elif defined MGONGPU_LAYOUT_SOA
                        double momenta1d[],       // output: momenta as SOA[npar][4][nevt]
#elif defined MGONGPU_LAYOUT_AOS
                        double momenta1d[],       // output: momenta as AOS[nevt][npar][4]
#endif
                        double wgts[],            // output: weights[nevt]
                        const int nevt );         // input: #events

  //--------------------------------------------------------------------------

  // Create and initialise a curand generator
  void createGenerator( curandGenerator_t* pgen );

  //--------------------------------------------------------------------------
  
  // Seed a curand generator
  void seedGenerator( curandGenerator_t gen, unsigned long long seed );
  
  //--------------------------------------------------------------------------

  // Destroy a curand generator
  void destroyGenerator( curandGenerator_t gen );

  //--------------------------------------------------------------------------

  // Bulk-generate (using curand) the random numbers needed to process nevt events in rambo
  // ** NB: the random numbers are always produced in the same order and are interpreted as an AOSOA
  // AOSOA: rnarray[npag][nparf][np4][nepp] where nevt=npag*nepp
  void generateRnArray( curandGenerator_t gen, // input: curand generator
                        double rnarray1d[],    // output: randomnumbers in [0,1]
                        const int nevt );      // input: #events

  //--------------------------------------------------------------------------

}

#endif // RAMBO2TONM0_H 1