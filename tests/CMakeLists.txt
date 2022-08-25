add_test(NAME "Run and show help"
	COMMAND sh -c "$<TARGET_FILE:afc> --help")

add_test(NAME "Generate correct CRC file"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_correct_crc_file.sh)

add_test(NAME "Check correct CRC file"
	COMMAND sh -c "$<TARGET_FILE:afc> crc_testfile_*.dat")
set_property(TEST "Check correct CRC file"
	PROPERTY PASS_REGULAR_EXPRESSION "1 files checked, 1 files ok, 0 files could not be read\.")

add_test(NAME "Test non-existing file, exit code == 1"
	COMMAND sh -c "$<TARGET_FILE:afc> doesnotexist.dat; test $? -eq 1")

add_test(NAME "Test non-existing file, test output"
	COMMAND sh -c "$<TARGET_FILE:afc> doesnotexist.dat")
set_property(TEST "Test non-existing file, test output"
	PROPERTY PASS_REGULAR_EXPRESSION "1 files checked, 0 files ok, 1 files could not be read\.")

add_test(NAME "Create empty file for testing"
	COMMAND cmake -E touch emptyfile.dat)

add_test(NAME "Test empty file, exit code == 2"
	COMMAND sh -c "$<TARGET_FILE:afc> emptyfile.dat; test $? -eq 2")

add_test(NAME "Test empty file, test output"
	COMMAND sh -c "$<TARGET_FILE:afc> emptyfile.dat")
set_property(TEST "Test empty file, test output"
	PROPERTY PASS_REGULAR_EXPRESSION "1 files checked, 0 files ok, 0 files could not be read\.")
