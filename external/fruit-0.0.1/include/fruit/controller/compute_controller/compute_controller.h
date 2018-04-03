#ifndef ABST_COMPUTE_CONTROLLER_H
#define ABST_COMPUTE_CONTROLLER_H

#include <fruit/fruit.h>

namespace FruitWork {
namespace Compute {

class ComputeController : public Fruit {

public:

	virtual void Initialize(bool debug) = 0;

	virtual void PostInitialize() = 0;

	virtual void InitializeGPU() = 0;

	virtual bool IsInitialize() = 0;

	virtual void Reset() = 0;

	virtual void Destroy() = 0;

	virtual void Update() = 0;

	virtual void MapBuffers() = 0;

	virtual void UnmapBuffers() = 0;

	virtual void Sync() = 0;

	/////////////////////////////////////////////
	virtual int GetActiveCount() const = 0;

	virtual int GetDiffuseParticles() const = 0;
};

}
}

#endif // ABST_COMPUTE_CONTROLLER_H
