/*
    -- MAGMA (version 1.4.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       August 2013

       @generated s Tue Aug 13 16:46:03 2013
*/
// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cuda_runtime_api.h>
#include <cublas.h>

// includes, project
#include "flops.h"
#include "magma.h"
#include "magma_lapack.h"
#include "testings.h"

#include <assert.h>
#include "dataio.h"

// #define N_REPEATS 1
#define N_REPEATS 10


/* ////////////////////////////////////////////////////////////////////////////
   -- Testing sgeqrf
*/
int main( int argc, char** argv)
{
    TESTING_INIT();

    real_Double_t    gflops, gpu_perf, gpu_time, cpu_perf=0, cpu_time=0;
    float           error, work[1];
    float  c_neg_one = MAGMA_S_NEG_ONE;
    float *h_A, *d_A, *h_R, *tau, *dT, *h_work, tmp[1];
    magma_int_t M, N, n2, lda, ldda, lwork, info, min_mn, nb, size;
    magma_int_t ione     = 1;
    magma_int_t ISEED[4] = {0,0,0,1}, ISEED2[4];

    // hack the arguments
    char *input_name = argv[argc-2];
    char *output_name = argv[argc-1];
    assert(argc >= 7);
    argv[argc-1] = NULL;
    argv[argc-2] = NULL;
    argc -= 2;
    
    magma_opts opts;
    parse_opts( argc, argv, &opts );
 
    magma_int_t status = 0;
    float tol;
    opts.lapack |= (opts.version == 2 && opts.check == 2);  // check (-c2) implies lapack (-l)

    if ( opts.version != 2 && opts.check == 1 ) {
        printf( "  ===================================================================\n"
                "  NOTE: -c check for this version will be wrong\n"
                "  because tester ignores the special structure of MAGMA sgeqrf resuls.\n"
                "  We reset it to -c2.\n" 
                "  ===================================================================\n\n");
        opts.check = 2;
    }
    /*
    if( opts.version == 2 ) {
        if ( opts.check == 1 ) {
            printf("  M     N     CPU GFlop/s (sec)   GPU GFlop/s (sec)   ||R-Q'A||_1 / (M*||A||_1*eps) ||I-Q'Q||_1 / (M*eps)\n");
            printf("=========================================================================================================\n");
        } else {
            printf("  M     N     CPU GFlop/s (sec)   GPU GFlop/s (sec)   ||R||_F / ||A||_F\n");
            printf("=======================================================================\n");
        }
        tol = 1.0;
    } else {
        printf("  M     N     CPU GFlop/s (sec)   GPU GFlop/s (sec)   ||Ax-b||_F/(N*||A||_F*||x||_F)\n");
        printf("====================================================================================\n");
        tol = opts.tolerance * lapackf77_slamch("E");
    }
    */

    FILE *infile = fopen(input_name, "r");
    assert(infile != NULL);
    FILE *outfile = fopen(output_name, "w");
    assert(outfile != NULL);

    for( int i = 0; i < 1 ; ++i ) {
        for( int iter = 0; iter < 1 ; ++iter ) {
	  /*
    for( int i = 0; i < opts.ntest; ++i ) {
        for( int iter = 0; iter < opts.niter; ++iter ) {
	  */
            M = opts.msize[i];
            N = opts.nsize[i];
            min_mn = min(M, N);
            lda    = M;
            n2     = lda*N;
            ldda   = ((M+31)/32)*32;
            gflops = FLOPS_SGEQRF( M, N ) / 1e9;
            
            lwork = -1;
            lapackf77_sgeqrf(&M, &N, h_A, &M, tau, tmp, &lwork, &info);
            lwork = (magma_int_t)MAGMA_S_REAL( tmp[0] );
            
            TESTING_MALLOC(    tau, float, min_mn );
            TESTING_MALLOC(    h_A, float, n2     );
            TESTING_HOSTALLOC( h_R, float, n2     );
            TESTING_DEVALLOC(  d_A, float, ldda*N );
            TESTING_MALLOC( h_work, float, lwork  );
            
            /* Initialize the matrix */
            for ( int j=0; j<4; j++ ) ISEED2[j] = ISEED[j]; // saving seeds
            lapackf77_slarnv( &ione, ISEED, &n2, h_A );

	    // buck test 
	    for (unsigned int bi = 0 ; bi < N_REPEATS ; bi++) { 

	    // overwrite 
	    fillInputs32to32(infile, h_A, n2);

            lapackf77_slacpy( MagmaUpperLowerStr, &M, &N, h_A, &lda, h_R, &lda );
            magma_ssetmatrix( M, N, h_R, lda, d_A, ldda );
            
            /* ====================================================================
               Performs operation using MAGMA
               =================================================================== */
            gpu_time = magma_wtime();
            if ( opts.version == 2 ) {
                magma_sgeqrf2_gpu( M, N, d_A, ldda, tau, &info);
            }
            else {
                nb = magma_get_sgeqrf_nb( M );
                size = (2*min(M, N) + (N+31)/32*32 )*nb;
                magma_smalloc( &dT, size );
                if ( opts.version == 3 ) {
                    magma_sgeqrf3_gpu( M, N, d_A, ldda, tau, dT, &info);
                }
                else {
                    magma_sgeqrf_gpu( M, N, d_A, ldda, tau, dT, &info);
                }
            }
            gpu_time = magma_wtime() - gpu_time;
            gpu_perf = gflops / gpu_time;
            if (info != 0)
                printf("magma_sgeqrf returned error %d: %s.\n",
                       (int) info, magma_strerror( info ));
	                
            // if ( opts.lapack ) {
	    if (false) {
                /* =====================================================================
                   Performs operation using LAPACK
                   =================================================================== */
                float *tau;
                TESTING_MALLOC( tau, float, min_mn );
                cpu_time = magma_wtime();
                lapackf77_sgeqrf(&M, &N, h_A, &lda, tau, h_work, &lwork, &info);
                cpu_time = magma_wtime() - cpu_time;
                cpu_perf = gflops / cpu_time;
                if (info != 0)
                    printf("lapackf77_sgeqrf returned error %d: %s.\n",
                           (int) info, magma_strerror( info ));
                TESTING_FREE( tau );
            }

	    /*
            if ( opts.check == 1 ) {
	    //  =====================================================================
                   Check the result 
                   =================================================================== 

                magma_int_t lwork = n2+N;
                float *h_W1, *h_W2, *h_W3;
                float *h_RW, results[2];
                magma_sgetmatrix( M, N, d_A, ldda, h_R, M );

                TESTING_MALLOC( h_W1, float, n2 ); // Q
                TESTING_MALLOC( h_W2, float, n2 ); // R
                TESTING_MALLOC( h_W3, float, lwork ); // WORK
                TESTING_MALLOC( h_RW, float, M );  // RWORK
                lapackf77_slarnv( &ione, ISEED2, &n2, h_A );
                lapackf77_sqrt02( &M, &N, &min_mn, h_A, h_R, h_W1, h_W2, &lda, tau, h_W3, &lwork,
                                  h_RW, results );

                if ( opts.lapack ) {
                    printf("%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)   %8.2e                      %8.2e",
                           (int) M, (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time, results[0],results[1] );
                } else {
                    printf("%5d %5d     ---   (  ---  )   %7.2f (%7.2f)    %8.2e                      %8.2e",
                           (int) M, (int) N, gpu_perf, gpu_time, results[0],results[1] );
                } 
                printf("%s\n", (results[0] < tol ? "" : "  failed"));
                status |= ! (results[0] < tol);
            
                TESTING_FREE( h_W1 );
                TESTING_FREE( h_W2 );
                TESTING_FREE( h_W3 );
                TESTING_FREE( h_RW );

            } else if ( opts.check == 2 ) {
                if ( opts.version == 2 ) {
                    // =====================================================================
                       Check the result compared to LAPACK
                       =================================================================== 
                    magma_sgetmatrix( M, N, d_A, ldda, h_R, M );
                    error = lapackf77_slange("f", &M, &N, h_A, &lda, work);
                    blasf77_saxpy(&n2, &c_neg_one, h_A, &ione, h_R, &ione);
                    error = lapackf77_slange("f", &M, &N, h_R, &lda, work) / error;

                    if ( opts.lapack ) {
                        printf("%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)   %8.2e",
                               (int) M, (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time, error );
                    } else {
                        printf("%5d %5d     ---   (  ---  )   %7.2f (%7.2f)   %8.2e",
                               (int) M, (int) N, gpu_perf, gpu_time, error );
                    }
                    printf("%s\n", (error < tol ? "" : "  failed"));
                    status |= ! (error < tol);
                } else if(M >= N) {
                    magma_int_t lwork;
                    float *x, *b, *d_B, *hwork;
                    const float c_zero    = MAGMA_S_ZERO;
                    const float c_one     = MAGMA_S_ONE;
                    const float c_neg_one = MAGMA_S_NEG_ONE;
                    const magma_int_t ione = 1;

                    // initialize RHS, b = A*random
                    TESTING_MALLOC( x, float, N );
                    TESTING_MALLOC( b, float, M );
                    lapackf77_slarnv( &ione, ISEED, &N, x );
                    blasf77_sgemv( "Notrans", &M, &N, &c_one, h_A, &lda, x, &ione, &c_zero, b, &ione );
                    // copy to GPU
                    TESTING_DEVALLOC( d_B, float, M );
                    magma_ssetvector( M, b, 1, d_B, 1 );

                    if ( opts.version == 1 ) {
                        // allocate hwork
                        magma_sgeqrs_gpu( M, N, 1,
                                          d_A, ldda, tau, dT,
                                          d_B, M, tmp, -1, &info );
                        lwork = (magma_int_t)MAGMA_S_REAL( tmp[0] );
                        TESTING_MALLOC( hwork, float, lwork );

                        // solve linear system
                        magma_sgeqrs_gpu( M, N, 1,
                                          d_A, ldda, tau, dT,
                                          d_B, M, hwork, lwork, &info );
                       if (info != 0)
                           printf("magma_sgeqrs returned error %d: %s.\n",
                                  (int) info, magma_strerror( info ));
                        TESTING_FREE( hwork );
                    } else {
                        // allocate hwork
                        magma_sgeqrs3_gpu( M, N, 1,
                                           d_A, ldda, tau, dT,
                                           d_B, M, tmp, -1, &info );
                        lwork = (magma_int_t)MAGMA_S_REAL( tmp[0] );
                        TESTING_MALLOC( hwork, float, lwork );

                        // solve linear system
                        magma_sgeqrs3_gpu( M, N, 1,
                                           d_A, ldda, tau, dT,
                                           d_B, M, hwork, lwork, &info );
                       if (info != 0)
                           printf("magma_sgeqrs3 returned error %d: %s.\n",
                                  (int) info, magma_strerror( info ));
                        TESTING_FREE( hwork );
                    }
                    magma_sgetvector( N, d_B, 1, x, 1 );

                    // compute r = Ax - b, saved in b
                    lapackf77_slarnv( &ione, ISEED2, &n2, h_A );
                    blasf77_sgemv( "Notrans", &M, &N, &c_one, h_A, &lda, x, &ione, &c_neg_one, b, &ione );

                    // compute residual |Ax - b| / (n*|A|*|x|)
                    float norm_x, norm_A, norm_r, work[1];
                    norm_A = lapackf77_slange( "F", &M, &N, h_A, &lda, work );
                    norm_r = lapackf77_slange( "F", &M, &ione, b, &M, work );
                    norm_x = lapackf77_slange( "F", &N, &ione, x, &N, work );

                    TESTING_FREE( x );
                    TESTING_FREE( b );
                    TESTING_DEVFREE( d_B );

                    error = norm_r / (N * norm_A * norm_x);
                    if ( opts.lapack ) {
                        printf("%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)   %8.2e",
                               (int) M, (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time, error );
                    } else {
                        printf("%5d %5d     ---   (  ---  )   %7.2f (%7.2f)   %8.2e",
                               (int) M, (int) N, gpu_perf, gpu_time, error );
                    }
                    printf("%s\n", (error < tol ? "" : "  failed"));
                    status |= ! (error < tol);
                } else {
                    if ( opts.lapack ) {
                        printf("%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)   --- ",
                               (int) M, (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time );
                    } else {
                        printf("%5d %5d     ---   (  ---  )   %7.2f (%7.2f)     --- ",
                               (int) M, (int) N, gpu_perf, gpu_time);
                    }
                    printf("%s ", (opts.check != 0 ? "  (error check only for M>=N)" : ""));
                }
            }
            else {
                if ( opts.lapack ) {
                    printf("%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)   ---\n",
                           (int) M, (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time );
                } else {
                    printf("%5d %5d     ---   (  ---  )   %7.2f (%7.2f)     ---  \n",
                           (int) M, (int) N, gpu_perf, gpu_time);
                }

            }
	    */

	    magma_sgetmatrix( M, N, d_A, ldda, h_R, M );
	    // writeOutput32(outfile, h_R[n2-2]);
	    writeOutput32to128(outfile, h_R[n2-1]);
	    // writeOutput32(outfile, h_R[(n2-lda)-1]);
	    // printf("sgeqrf output (%d) %11.10f\n", h_R[n2-1]); 
	    /*
	    for (int mi = 0 ; mi < M ; mi++) {
	      for (int ni = 0 ; ni < N ; ni++) {
		printf("h_R[%d][%d] %11.10f \n", mi, ni, h_R[mi * N + ni]);
	      }
	    }
	    printf("============ \n");
	    */
	    } // the end of the buck test 
            
            if(opts.version != 2) magma_free( dT );
            TESTING_FREE( tau );
            TESTING_FREE( h_A );
            TESTING_FREE( h_work );
            TESTING_HOSTFREE( h_R );
            TESTING_DEVFREE( d_A );
        }
        if ( opts.niter > 1 ) {
            printf( "\n" );
        }
    }

    fclose(infile);
    fclose(outfile);
    
    TESTING_FINALIZE();
    return status;
}
