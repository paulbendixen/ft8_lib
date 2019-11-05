cmake_minimum_required( VERSION 3.10..3.15 )
project( FT8 )

add_subdirectory( ft8 )
add_subdirectory( common )
add_subdirectory( fft )

add_executable( gen_ft8 gen_ft8.cpp )
target_link_libraries( gen_ft8 PRIVATE ft8Encode ft8Common )

add_executable( decode_ft8 decode_ft8.cpp )
target_link_libraries( decode_ft8 PRIVATE ft8Decode ft8Common kissFFT )

enable_testing()
add_executable( baseTest test.cpp )
target_link_libraries( baseTest ft8Encode kissFFT )
add_test( repositoryTest baseTest )