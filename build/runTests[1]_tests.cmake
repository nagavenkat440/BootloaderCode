add_test([=[PostCRC.BasicTest]=]  E:/Krishna/Qt_CICD_Demo/FTU/BootloaderCode/build/runTests.exe [==[--gtest_filter=PostCRC.BasicTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[PostCRC.BasicTest]=]  PROPERTIES DEF_SOURCE_LINE [==[E:\Krishna\Qt_CICD_Demo\FTU\BootloaderCode\Tests\test_crc.cpp:3]==] WORKING_DIRECTORY E:/Krishna/Qt_CICD_Demo/FTU/BootloaderCode/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  runTests_TESTS PostCRC.BasicTest)
