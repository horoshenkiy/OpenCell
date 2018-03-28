#include <gtest\gtest.h>

#include "TestState.h"
//#include "TestLogging.h"

//FlexController flexController;

int main(int argc, char **argv) {
	//flexController.InitFlex();
	//SimBuffers::Instance(flexController.GetLib());

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
