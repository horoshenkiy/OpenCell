#ifndef PROTEIN_H
#define PROTEIN_H

#include <fruit/controller/compute_controller/flex_params.h>
#include "component.h"
#include "fruit/utilits/utilits.h"

using namespace FruitWork::Primitives;
using namespace FruitWork::Utilits;

enum ProteinType
{
	Actin,
	ARP
};

struct Protein
{
	Protein(Vec3 begin_, Vec3 direction_, Quat rotation_, Vec3 streamDirection_)
	{
		length = 1;
		begin_type = Actin;
		end_type = Actin;

		begin = begin_; // begin position
		direction = direction_; // direction
		rotation = rotation_; // rotation over main direction
		streamDirection = streamDirection_;  // main direction of actin grow

		forward_protein = nullptr;
		angle_protein = nullptr;
	}
	
	Shape makeShape()
	{
		Compute::FlexParams &flexParams = Compute::FlexParams::Get();

		Vec3 x_ax = Vec3(1.0, 0.0, 0.0);
		Vec3 y_ax = Vec3(0.0, 1.0, 0.0);

		auto half_len = flexParams.sectionLength*0.5f*length;
		auto center = begin + Normalize(direction)*half_len;
		auto dirShape = AddCapsule(flexParams.sectionRadius, half_len, center, QuatFromAxisAngle(y_ax, -angleBtwVectors(streamDirection, x_ax)) * rotation); 
		return dirShape;
	}

	ProteinType begin_type;
	ProteinType end_type;

	Shape shape;

	Vec3 begin;
	Vec3 direction;
	
	Vec3 streamDirection; // this parameter needs to compute angle btw old stream direciton and own current direction of branch

	Quat rotation;

	Protein* forward_protein;
	Protein* angle_protein;

	int length;	
};

#endif // PROTEIN_H