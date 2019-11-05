add_library( kissFFT kiss_fft.c kiss_fftr.c )
target_include_directories( kissFFT PUBLIC ${CMAKE_CURRENT_LIST_DIR} )