#pragma once

#include <utilits/Logger.h>

#include <controller/compute_controller/FlexController.h>
#include <controller/compute_controller/SimBuffers.h>

#include <gtest\gtest.h>
/*
void foo(SimBuffers &buffers) {
	printf("ok!\n");
}

TEST(LoggingCase, SimpleLogging) {
	FlexController flexController;
	flexController.InitFlex();

	SimBuffers &buffers = SimBuffers(flexController.GetLib());
	buffers.Initialize();
	buffers.MapBuffers();

	LogArgWithCallFunc(buffers, foo, buffers);

	buffers.UnmapBuffers();
}*/