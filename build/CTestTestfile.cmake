# CMake generated Testfile for 
# Source directory: /home/gennart/Documents/MAI/Thesis/spn_sw
# Build directory: /home/gennart/Documents/MAI/Thesis/spn_sw/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test "/home/gennart/Documents/MAI/Thesis/spn_sw/build/exe" "example/example_03.spn")
set_tests_properties(test PROPERTIES  _BACKTRACE_TRIPLES "/home/gennart/Documents/MAI/Thesis/spn_sw/CMakeLists.txt;17;add_test;/home/gennart/Documents/MAI/Thesis/spn_sw/CMakeLists.txt;0;")
subdirs("src")
subdirs("bfp")
