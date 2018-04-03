#include <controller/compute_controller/flex_controller.h>

#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <flex/NvFlexDevice.h>
#include <application.h>

namespace FruitWork {
namespace Compute {

void ErrorCallback(NvFlexErrorSeverity, const char* msg, const char* file, int line) {
	printf("Flex: %s - %s:%d\n", msg, file, line);
}

FlexController::FlexController() {
	// use the PhysX GPU selected from the NVIDIA control panel
	if (device == -1)
		device = NvFlexDeviceGetSuggestedOrdinal();

	// Create an optimized CUDA context for Flex and set it on the 
	// calling thread. This is an optional call, it is fine to use 
	// a regular CUDA context, although creating one through this API
	// is recommended for best performance.
	bool success = NvFlexDeviceCreateCudaContext(device);

	if (!success){
		printf("Error creating CUDA context.\n");
		exit(-1);
	}

	NvFlexInitDesc desc;
	desc.deviceIndex = device;
	desc.enableExtensions = AppParams::g_extensions;
	desc.renderDevice = 0;
	desc.renderContext = 0;
	desc.computeType = eNvFlexCUDA;

	// Init Flex library, note that no CUDA methods should be called before this 
	// point to ensure we get the device context we want
	lib = NvFlexInit(NV_FLEX_VERSION, ErrorCallback, &desc);

	if (error || lib == NULL)
	{
		printf("Could not initialize Flex, exiting.\n");
		exit(-1);
	}

	// store device name
	strcpy(deviceName, NvFlexGetDeviceName(lib));
}

}
}