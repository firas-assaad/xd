#include <iostream>
#include <fstream>
#include <string>
#include <xd/common.h>
#include <xd/utf8.h>
#include "test.h"

int main(int argc, char *argv[])
{
	/*
	try
	{
		xd::lua::vm vm;
		vm.load_library();
		xd::lua::script_handle script = vm.load_file("test.lua");
		script->run();
	} catch (xd::lua::script_load_failed& e) {
		std::cout << "failed to load script:\n" << e.error_message() << std::endl;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	} catch (...) {
	}
	return 0;
	*/

	try {
		test my_app;
		my_app.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "unknown exception" << std::endl;
	}
	return 0;
}
