# CMake generated Testfile for 
# Source directory: /home/feedback/feedback/uidreczz28aHEdlribS/modules/highgui
# Build directory: /home/feedback/feedback/uidreczz28aHEdlribS/build-test/modules/highgui
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_highgui "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/bin/opencv_test_highgui" "--gtest_output=xml:opencv_test_highgui.xml")
set_tests_properties(opencv_test_highgui PROPERTIES  LABELS "Main;opencv_highgui;Accuracy" WORKING_DIRECTORY "/home/feedback/feedback/uidreczz28aHEdlribS/build-test/test-reports/accuracy" _BACKTRACE_TRIPLES "/home/feedback/feedback/uidreczz28aHEdlribS/cmake/OpenCVUtils.cmake;1765;add_test;/home/feedback/feedback/uidreczz28aHEdlribS/cmake/OpenCVModule.cmake;1370;ocv_add_test_from_target;/home/feedback/feedback/uidreczz28aHEdlribS/modules/highgui/CMakeLists.txt;269;ocv_add_accuracy_tests;/home/feedback/feedback/uidreczz28aHEdlribS/modules/highgui/CMakeLists.txt;0;")
