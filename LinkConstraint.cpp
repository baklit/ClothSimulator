#include "stdafx.h"
#include "LinkConstraint.h"

#include <algorithm>

#define RESTINGDISTANCE 1.0f
#define BREAKINGDISTANCE 300
#define ELASTICITY 0.01f
#define PUSHY false

LinkConstraint::LinkConstraint(PointMass* A, PointMass* B)
{
	pointMassA = A;
	pointMassB = B;
}


LinkConstraint::~LinkConstraint()
{
}

void LinkConstraint::solve(unsigned int delta)
{
	glm::vec3 difference = pointMassA->position - pointMassB->position;
	float distance = glm::length(difference);

	if (distance > RESTINGDISTANCE || PUSHY) {
		float moveFactor = (RESTINGDISTANCE - distance) / distance;
		glm::vec3 translation = difference * 0.5f * moveFactor * std::min(delta/ELASTICITY, 1.0f);

		if (pointMassA->anchored && !pointMassB->anchored) {
			pointMassB->position -= translation * 2.0f;
		}
		else if (pointMassB->anchored && !pointMassA->anchored) {
			pointMassA->position += translation * 2.0f;
		}
		else if (!pointMassA->anchored && !pointMassB->anchored) {
			pointMassA->position += translation;
			pointMassB->position -= translation;
		}
	}
}
