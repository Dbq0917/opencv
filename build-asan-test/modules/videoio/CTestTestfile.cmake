# CMake generated Testfile for 
# Source directory: /home/feedback/feedback/uidreczz28aHEdlp76D/modules/videoio
# Build directory: /home/feedback/feedback/uidreczz28aHEdlp76D/build-asan-test/modules/videoio
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_videoio "/home/feedback/feedback/uidreczz28aHEdlp76D/build-asan-test/bin/opencv_test_videoio" "--gtest_output=xml:opencv_test_videoio.xml")
set_tests_properties(opencv_test_videoio PROPERTIES  LABELS "Main;opencv_videoio;Accuracy" WORKING_DIRECTORY "/home/feedback/feedback/uidreczz28aHEdlp76D/build-asan-test/test-reports/accuracy" _BACKTRACE_TRIPLES "/home/feedback/feedback/uidreczz28aHEdlp76D/cmake/OpenCVUtils.cmake;1765;add_test;/home/feedback/feedback/uidreczz28aHEdlp76D/cmake/OpenCVModule.cmake;1370;ocv_add_test_from_target;/home/feedback/feedback/uidreczz28aHEdlp76D/modules/videoio/CMakeLists.txt;278;ocv_add_accuracy_tests;/home/feedback/feedback/uidreczz28aHEdlp76D/modules/videoio/CMakeLists.txt;0;")
