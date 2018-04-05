#ifndef TEST_STATE_H
#define TEST_STATE_H

#include <gtest\gtest.h>
#include <cereal\archives\binary.hpp>
#include <fruit\serialize_types.h>

#ifdef TEST
#define private public
#define protected public 

#include <components\kernel.h>
#endif

std::stringstream ss;
/*
// KERNEL TEST
///////////////////////////////////////////////////////////////////////////////////////

bool operator==(const Kernel &lKernel, const Kernel &rKernel);

TEST(StateCase, StateKernel) {

	Kernel oKernel = Kernel(), iKernel = Kernel();

	// initialise
	oKernel.group = 0;
	oKernel.indexCenter = 10;
	oKernel.positionCenter = Vec4(0.0, 1.0, 1.0, 0.0);
	oKernel.prevPositionCenter = Vec4(0.8, 1.5, 1.0, 0.0);
	oKernel.rateCenter = Vec3(-1.5, 2.1, 4.5);

	// save
	cereal::BinaryOutputArchive oArchive(ss);
	//serialize(Vec3());
	//oKernel.serialize(oArchive);

	cereal::BinaryInputArchive iArchive(ss);
	//iKernel.serialize(iArchive);

//	ASSERT_EQ(iKernel, oKernel);
}

bool operator==(const Kernel &lKernel, const Kernel &rKernel) {
	if (lKernel.group != rKernel.group)
		return false;
	else if (lKernel.indexCenter != rKernel.indexCenter)
		return false;
	else if (lKernel.positionCenter != rKernel.positionCenter)
		return false;
	else if (lKernel.prevPositionCenter != rKernel.prevPositionCenter)
		return false;
	else if (lKernel.rateCenter != rKernel.rateCenter)
		return false;
	else
		return true;
}*/



#endif // TEST_STATE_H