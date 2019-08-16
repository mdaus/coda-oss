#include <iostream>

#include "TestCase.h"


TEST_CASE_ARGS(readWords) {

	std::string word1;
	if(argc != 2) {
		std::cerr << "Incorrect number of command line args\n";
		throw except::Exception(Ctxt(
			"Incorrect number of cmd line args"));
	} else {
		word1 = argv[1];
		std::cout << "Parsed cmd line argument: " << word1 << std::endl;
	}
}

int main(int argc, char** argv) {
	try {
		TEST_CHECK_ARGS(readWords);	
	} catch (const except::Exception& ex) {
		std::cerr << ex.toString() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception\n";
	}
	return 1;

}
