#pragma once

#include "PointMass.h"

class LinkConstraint
{
public:
	LinkConstraint(PointMass* A, PointMass* B);
	~LinkConstraint();

	void solve(unsigned int delta);

private:
	PointMass* pointMassA;
	PointMass* pointMassB;
};

