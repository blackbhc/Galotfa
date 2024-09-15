/**
 * @file test_recenter.cpp
 * @brief Test the recenter APIs
 */
#include "../include/recenter.hpp"
#ifdef DEBUG
#include "../include/myprompt.hpp"
#endif
#include <cassert>
#include <cstring>
#include <mpi.h>
using namespace std;
using namespace otf;
#define THRESHOLD 1e-6  // the equal threshold of floating numbers

int main( int argc, char* argv[] )
{
    // check whether the double[3] vecctors are equal within the threshold
    auto neq = []( double vec1[ 3 ], double vec2[ 3 ] ) -> bool {
        int eq = 0;
        for ( int i = 0; i < 3; ++i )
            eq += ( int )( abs( vec1[ i ] - vec2[ i ] ) >= THRESHOLD );
        return eq;
    };

    // NOTE: test in rank 4 mpi process program: 10 coordinate in each rank
    int rank = -1, size = 0;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    assert( size == 4 );  // check the mpi size

    // TEST: center of mass calculation
    // TEST: trival case, all==0
    double coordinate[ 120 ] = { 0 };
    double mass[ 40 ]        = { 0 };
    for ( int i = 0; i < 4; ++i )
    {
        if ( rank == i )
        {
            for ( int j = 0; j < 10; ++j )
            {
                for ( int k = 0; k < 3; ++k )
                    coordinate[ 3 * ( i * 10 + j ) + k ] = 0;
                mass[ i * 10 + j ] = 1.25;
            }
        }
    }
    double expected1[ 3 ]    = { 0, 0, 0 };
    double initialGuess[ 3 ] = { 0, 0, 0 };
    auto   res1 = recenter::center_of_mass( mass + 10 * rank, coordinate + 3 * 10 * rank, 10, 100,
                                            initialGuess );
    MPI_Barrier( MPI_COMM_WORLD );
    mpi_print( rank, "Expect: %lf, %lf, %lf", expected1[ 0 ], expected1[ 1 ], expected1[ 2 ] );
    mpi_print( rank, "Get: %lf, %lf, %lf", res1[ 0 ], res1[ 1 ], res1[ 2 ] );
    assert( !neq( expected1, res1.get() ) );

    // TEST: trival case, all==3.14
    memset( coordinate, 0, sizeof( double ) * 120 );
    for ( int i = 0; i < 4; ++i )
    {
        if ( rank == i )
        {
            for ( int j = 0; j < 10; ++j )
            {
                for ( int k = 0; k < 3; ++k )
                    coordinate[ 3 * ( i * 10 + j ) + k ] = 3.14;
                mass[ i * 10 + j ] = 1.25;
            }
        }
    }

    double expected2[ 3 ] = { 3.14, 3.14, 3.14 };
    auto   res2 = recenter::center_of_mass( mass + 10 * rank, coordinate + 3 * 10 * rank, 10, 100,
                                            initialGuess );
    MPI_Barrier( MPI_COMM_WORLD );
    mpi_print( rank, "Expect: %lf, %lf, %lf", expected2[ 0 ], expected2[ 1 ], expected2[ 2 ] );
    mpi_print( rank, "Get: %lf, %lf, %lf", res2[ 0 ], res2[ 1 ], res2[ 2 ] );
    assert( !neq( expected2, res2.get() ) );

    // TEST: equal mass case
    double coordinate2[ 120 ] = {
        0.58801452, 0.69910875, 0.18815196, 0.04380856, 0.20501895, 0.10606287, 0.72724014,
        0.67940052, 0.4738457,  0.44829582, 0.01910695, 0.75259834, 0.60244854, 0.96177758,
        0.66436865, 0.60662962, 0.44915131, 0.22535416, 0.6701743,  0.73576659, 0.25799564,
        0.09554215, 0.96090974, 0.25176729, 0.28216512, 0.76825393, 0.7979234,  0.5440372,
        0.38270763, 0.38165095, 0.28582739, 0.74026815, 0.23898683, 0.4377217,  0.8835387,
        0.28928114, 0.78450686, 0.75895366, 0.41778538, 0.22576877, 0.42009814, 0.06436369,
        0.59643269, 0.83732372, 0.89248639, 0.20052744, 0.50239523, 0.89538184, 0.25592093,
        0.86723234, 0.01648793, 0.55249695, 0.52790539, 0.92335039, 0.24594844, 0.06401838,
        0.9021047,  0.8740398,  0.16366729, 0.99974131, 0.34680397, 0.31287816, 0.84710402,
        0.8802311,  0.67655865, 0.05367515, 0.55921377, 0.69451294, 0.8241973,  0.31142866,
        0.50523054, 0.84900379, 0.29351563, 0.67711955, 0.4209064,  0.68171271, 0.22122799,
        0.5489977,  0.84884672, 0.7365669,  0.49962259, 0.37966499, 0.78752081, 0.16886931,
        0.58635861, 0.43121067, 0.06191019, 0.28945477, 0.7341454,  0.28865545, 0.39039811,
        0.63561732, 0.83114886, 0.319421,   0.15922479, 0.71166422, 0.87270864, 0.59315637,
        0.69471288, 0.17323332, 0.53173259, 0.87095862, 0.84109027, 0.97205554, 0.78225721,
        0.19703051, 0.61062607, 0.47885551, 0.616637,   0.13993324, 0.41123582, 0.77763034,
        0.93972552, 0.10457941, 0.9384822,  0.79738717, 0.33080272, 0.31178575, 0.29015382,
        0.17388959
    };
    /* python code:
    import numpy as np
    np.random.seed(2024)
    mockCoordinate = np.random.rand(120).reshape(40, 3)
    mockCoordinate.reshape(120)
    np.mean(mockCoordinate, axis=0)
    */
    memset( coordinate, 0, sizeof( double ) * 120 );
    for ( int i = 0; i < 4; ++i )
    {
        if ( rank == i )
        {
            for ( int j = 0; j < 10; ++j )
            {
                for ( int k = 0; k < 3; ++k )
                    coordinate[ 3 * ( i * 10 + j ) + k ] = coordinate2[ 3 * ( i * 10 + j ) + k ];
                mass[ i * 10 + j ] = 1;
            }
        }
    }
    double expected3[ 3 ] = { 0.49207988, 0.57682968, 0.49231838 };
    auto   res3 = recenter::center_of_mass( mass + 10 * rank, coordinate + 3 * 10 * rank, 10, 100,
                                            initialGuess );
    MPI_Barrier( MPI_COMM_WORLD );
    mpi_print( rank, "Expect: %lf, %lf, %lf", expected3[ 0 ], expected3[ 1 ], expected3[ 2 ] );
    mpi_print( rank, "Get: %lf, %lf, %lf", res3[ 0 ], res3[ 1 ], res3[ 2 ] );
    assert( !neq( expected3, res3.get() ) );

    // TEST: varing mass case
    double coordinate3[ 120 ] = {
        0.58801452, 0.69910875, 0.18815196, 0.04380856, 0.20501895, 0.10606287, 0.72724014,
        0.67940052, 0.4738457,  0.44829582, 0.01910695, 0.75259834, 0.60244854, 0.96177758,
        0.66436865, 0.60662962, 0.44915131, 0.22535416, 0.6701743,  0.73576659, 0.25799564,
        0.09554215, 0.96090974, 0.25176729, 0.28216512, 0.76825393, 0.7979234,  0.5440372,
        0.38270763, 0.38165095, 0.28582739, 0.74026815, 0.23898683, 0.4377217,  0.8835387,
        0.28928114, 0.78450686, 0.75895366, 0.41778538, 0.22576877, 0.42009814, 0.06436369,
        0.59643269, 0.83732372, 0.89248639, 0.20052744, 0.50239523, 0.89538184, 0.25592093,
        0.86723234, 0.01648793, 0.55249695, 0.52790539, 0.92335039, 0.24594844, 0.06401838,
        0.9021047,  0.8740398,  0.16366729, 0.99974131, 0.34680397, 0.31287816, 0.84710402,
        0.8802311,  0.67655865, 0.05367515, 0.55921377, 0.69451294, 0.8241973,  0.31142866,
        0.50523054, 0.84900379, 0.29351563, 0.67711955, 0.4209064,  0.68171271, 0.22122799,
        0.5489977,  0.84884672, 0.7365669,  0.49962259, 0.37966499, 0.78752081, 0.16886931,
        0.58635861, 0.43121067, 0.06191019, 0.28945477, 0.7341454,  0.28865545, 0.39039811,
        0.63561732, 0.83114886, 0.319421,   0.15922479, 0.71166422, 0.87270864, 0.59315637,
        0.69471288, 0.17323332, 0.53173259, 0.87095862, 0.84109027, 0.97205554, 0.78225721,
        0.19703051, 0.61062607, 0.47885551, 0.616637,   0.13993324, 0.41123582, 0.77763034,
        0.93972552, 0.10457941, 0.9384822,  0.79738717, 0.33080272, 0.31178575, 0.29015382,
        0.17388959
    };
    double mass2[ 40 ] = { 0.88114956, 0.82005649, 0.73665888, 0.84696823, 0.60911562, 0.34423301,
                           0.22966899, 0.94462579, 0.29172571, 0.41004459, 0.33766428, 0.9493286,
                           0.23092893, 0.16947204, 0.49089763, 0.90475345, 0.88399969, 0.19491159,
                           0.52021797, 0.17489932, 0.89954588, 0.37819933, 0.70288128, 0.51174545,
                           0.63485106, 0.93399494, 0.21049022, 0.33620401, 0.65946718, 0.41460336,
                           0.25531344, 0.87560225, 0.40179131, 0.56136793, 0.1716831,  0.00877794,
                           0.40878872, 0.10346889, 0.52504683, 0.45986097 };
    /* python code:
    import numpy as np
    np.random.seed(2024)
    mockCoordinate = np.random.rand(120).reshape(40, 3)
    mockCoordinate.reshape(120)
    mass = np.random.rand(40)
    np.sum(mockCoordinate*np.repeat(mass, 3).reshape(40, 3), axis=0) / np.sum(mass)
    */
    memset( coordinate, 0, sizeof( double ) * 120 );
    for ( int i = 0; i < 4; ++i )
    {
        if ( rank == i )
        {
            for ( int j = 0; j < 10; ++j )
            {
                for ( int k = 0; k < 3; ++k )
                    coordinate[ 3 * ( i * 10 + j ) + k ] = coordinate3[ 3 * ( i * 10 + j ) + k ];
                mass[ i * 10 + j ] = mass2[ i * 10 + j ];
            }
        }
    }
    double expected4[ 3 ] = { 0.44997152, 0.55214703, 0.49108783 };
    auto   res4 = recenter::center_of_mass( mass + 10 * rank, coordinate + 3 * 10 * rank, 10, 100,
                                            initialGuess );
    MPI_Barrier( MPI_COMM_WORLD );
    mpi_print( rank, "Expect: %lf, %lf, %lf", expected4[ 0 ], expected4[ 1 ], expected4[ 2 ] );
    mpi_print( rank, "Get: %lf, %lf, %lf", res4[ 0 ], res4[ 1 ], res4[ 2 ] );
    assert( !neq( expected4, res4.get() ) );

    // TEST: find position of the most bound particle
    double pot[ 40 ]  = { 0 };
    double pot0[ 10 ] = {
        -1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    double pot1[ 10 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    double pot2[ 10 ] = {
        -10, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    double pot3[ 10 ] = { 10, 20, 3, 4, 5, 6, 7, 8, 9, 10 };

    double coordinate4[ 120 ] = {
        5.880145188953979085e-01, 6.991087476815824875e-01, 1.881519600385059832e-01,
        4.380856374686481480e-02, 2.050189523942888004e-01, 1.060628744762657227e-01,
        7.272401436844547762e-01, 6.794005235251415753e-01, 4.738457034082185215e-01,
        4.482958244803045833e-01, 1.910694787246602910e-02, 7.525983372085792711e-01,
        6.024485390081265601e-01, 9.617775753081896362e-01, 6.643686473564756056e-01,
        6.066296193186763164e-01, 4.491513149317164499e-01, 2.253541631926955224e-01,
        6.701742968926955868e-01, 7.357665924519133371e-01, 2.579956380781935898e-01,
        9.554215386036546409e-02, 9.609097422366776886e-01, 2.517672867680110782e-01,
        2.821651194436431975e-01, 7.682539346627851318e-01, 7.979233971149833904e-01,
        5.440371984004160888e-01, 3.827076306466419275e-01, 3.816509502019972411e-01,
        2.858273884771829199e-01, 7.402681531406573034e-01, 2.389868324357290463e-01,
        4.377217046498637076e-01, 8.835387027765857493e-01, 2.892811403327176789e-01,
        7.845068570874209612e-01, 7.589536567735903905e-01, 4.177853849344335124e-01,
        2.257687675960829976e-01, 4.200981389305985525e-01, 6.436369088127136262e-02,
        5.964326869209966020e-01, 8.373237223108302985e-01, 8.924863863290550814e-01,
        2.005274438832268524e-01, 5.023952343652897667e-01, 8.953818444612753336e-01,
        2.559209313882706560e-01, 8.672323429662902594e-01, 1.648793481663091143e-02,
        5.524969543710593900e-01, 5.279053864859400980e-01, 9.233503868700184691e-01,
        2.459484435262107027e-01, 6.401837615878513965e-02, 9.021047045861979585e-01,
        8.740398003743293787e-01, 1.636672905528672173e-01, 9.997413066863959363e-01,
        3.468039703467236112e-01, 3.128781593441584130e-01, 8.471040209904430185e-01,
        8.802311026324961540e-01, 6.765586515466459616e-01, 5.367515427165370223e-02,
        5.592137735196148762e-01, 6.945129418277548039e-01, 8.241973026654624279e-01,
        3.114286588619704643e-01, 5.052305408491463146e-01, 8.490037878830786200e-01,
        2.935156326975796315e-01, 6.771195506548115528e-01, 4.209064021972034331e-01,
        6.817127136293417156e-01, 2.212279894864880303e-01, 5.489976984328670540e-01,
        8.488467194750852762e-01, 7.365669013092747131e-01, 4.996225862809582363e-01,
        3.796649930632635117e-01, 7.875208106001730934e-01, 1.688693075649854158e-01,
        5.863586139898459004e-01, 4.312106707158672725e-01, 6.191018519581947821e-02,
        2.894547746941048549e-01, 7.341453983461350141e-01, 2.886554549444746431e-01,
        3.903981141305007396e-01, 6.356173237123270425e-01, 8.311488567392082416e-01,
        3.194210005732835977e-01, 1.592247876246446481e-01, 7.116642159467059248e-01,
        8.727086430908735926e-01, 5.931563654250515683e-01, 6.947128788862139137e-01,
        1.732333202933027394e-01, 5.317325923046837266e-01, 8.709586206567291322e-01,
        8.410902680536632703e-01, 9.720555400219494935e-01, 7.822572097133597691e-01,
        1.970305063723702954e-01, 6.106260656089501637e-01, 4.788555059117209911e-01,
        6.166369960823228080e-01, 1.399332432916546853e-01, 4.112358162195196565e-01,
        7.776303394377082068e-01, 9.397255196089524532e-01, 1.045794081834356959e-01,
        9.384821973096293402e-01, 7.973871717092873013e-01, 3.308027186729963764e-01,
        3.117857505088535985e-01, 2.901538199940597584e-01, 1.738895906964326166e-01
    };

    memset( coordinate, 0, sizeof( double ) * 120 );
    for ( int i = 0; i < 4; ++i )
    {
        if ( rank == i )
        {
            for ( int j = 0; j < 10; ++j )
            {
                for ( int k = 0; k < 3; ++k )
                {
                    coordinate[ 3 * ( i * 10 + j ) + k ] = coordinate4[ 3 * ( i * 10 + j ) + k ];
                }
            }
        }
    }
    switch ( rank )
    {
    case 0:
        for ( int i = 0; i < 10; ++i )
        {
            pot[ i ] = pot0[ i ];
        }
        break;
    case 1:
        for ( int i = 0; i < 10; ++i )
        {
            pot[ i + 10 ] = pot1[ i ];
        }
        break;
    case 2:
        for ( int i = 0; i < 10; ++i )
        {
            pot[ i + 20 ] = pot2[ i ];
        }
        break;
    case 3:
        for ( int i = 0; i < 10; ++i )
        {
            pot[ i + 30 ] = pot3[ i ];
        }
        break;
    default:
        MPI_ERROR( rank, "Get into an unexpected branch!" );
    }

    double expected5[ 3 ] = { 3.468039703467236112e-01, 3.128781593441584130e-01,
                              8.471040209904430185e-01 };
    auto   res5 = recenter::most_bound_particle( pot + rank * 10, coordinate + 3 * rank * 10, 10 );
    mpi_print( rank, "Expect: %lf, %lf, %lf", expected5[ 0 ], expected5[ 1 ], expected5[ 2 ] );
    mpi_print( rank, "Get: %lf, %lf, %lf", res5[ 0 ], res5[ 1 ], res5[ 2 ] );
    assert( !neq( expected5, res5.get() ) );

    MPI_Finalize();
    return 0;
}
