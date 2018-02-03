#pragma once
#include "component.h"

enum ProteinType
{
	Actin,
	ARP
};

struct Protein
{
	Protein(Vec3 begin_, Vec3 direction_, Quat rotation_)
	{
		length = 1;
		begin_type = Actin;
		end_type = Actin;

		begin = begin_;
		direction = direction_;
		rotation = rotation_;

		forward_protein = nullptr;
		angle_protein = nullptr;
	}
	
	Shape makeShape()
	{
		auto half_len = halfSectionLength*length;
		auto center = begin + Normalize(direction)*half_len;
		//Quat rotation = QuatFromAxisAngle(Vec3(0.0, 1.0, 0.0), -angleBtwVectors(Vec3(1.0,0.0,0.0), direction));
		auto dirShape = AddCapsule(radius, half_len, center, QuatFromAxisAngle(Vec3(0.0, 1.0, 0.0), -M_PI / 4) * rotation);
		return dirShape;
	}

	ProteinType begin_type;
	ProteinType end_type;

	Shape shape;

	Vec3 begin;
	Vec3 direction;
	Quat rotation;

	Protein* forward_protein;
	Protein* angle_protein;

	int length;
	float halfSectionLength = 0.003f;
	float radius = 0.005f;
	
};