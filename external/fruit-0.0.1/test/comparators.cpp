#include "comparators.h"

bool operator==(const NvFlexExtAsset &lAsset, const NvFlexExtAsset &rAsset) {
	// particles
	if (lAsset.numParticles != rAsset.numParticles || lAsset.maxParticles != rAsset.maxParticles)
		return false;

	for (int i = 0; i < lAsset.numParticles; i++)
		if (lAsset.particles[i] != rAsset.particles[i])
			return false;

	// springs
	if (lAsset.numSprings != rAsset.numSprings)
		return false;

	for (int i = 0; i < lAsset.numSprings; i++)
		if (lAsset.springIndices[i] != rAsset.springIndices[i] ||
			lAsset.springCoefficients[i] != rAsset.springCoefficients[i] ||
			lAsset.springRestLengths[i] != rAsset.springRestLengths[i])
			return false;

	// shapes
	if (lAsset.numShapeIndices != rAsset.numShapeIndices)
		return false;

	for (int i = 0; i < lAsset.numShapeIndices; i++)
		if (lAsset.shapeIndices[i] != rAsset.shapeIndices[i])
			return false;

	if (lAsset.numShapes != rAsset.numShapes)
		return false;

	for (int i = 0; i < lAsset.numShapes; i++)
		if (lAsset.shapeOffsets[i] != rAsset.shapeOffsets[i] ||
			lAsset.shapeCoefficients[i] != rAsset.shapeCoefficients[i] ||
			lAsset.shapeCenters[i] != rAsset.shapeCenters[i])
			return false;

	// faces for cloth
	if (lAsset.numTriangles != rAsset.numTriangles)
		return false;

	for (int i = 0; i < lAsset.numTriangles; i++)
		if (lAsset.triangleIndices[i] != rAsset.triangleIndices[i])
			return false;

	// inflatables
	if (lAsset.inflatable != rAsset.inflatable ||
		lAsset.inflatablePressure != rAsset.inflatablePressure ||
		lAsset.inflatableVolume != rAsset.inflatableVolume ||
		lAsset.inflatableStiffness != rAsset.inflatableStiffness)
		return false;

	return true;
}