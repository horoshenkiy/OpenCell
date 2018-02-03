#pragma once

#include <vector>
#include <set>
#include <random>

#include "component.h"
#include "shell.h"

class Receptors : public Component {
private:
	int group;

	std::vector<int> indParticlesShell;

	std::random_device gen;
	std::uniform_int_distribution<> urd;

public:

	Receptors() {}
	Receptors(int _group) : group(_group) {}

	void Initialize(SimBuffers *buffers, Shell *shell) {
		this->buffers = buffers;

		size_t indBeginBufferShell = shell->GetIndBeginPosition();
		size_t indEndBufferShell = shell->GetIndEndPosition();

		// TODO: add exception
		if (indEndBufferShell < indBeginBufferShell)
			printf("error!");

		urd = std::uniform_int_distribution<>(indBeginBufferShell, indEndBufferShell);

		std::set<int> setInd;
		int countReceptors = indEndBufferShell - indBeginBufferShell / 10;
		for (int i = 0; i < countReceptors; i++) {
			setInd.insert(urd(gen));
		}

		indParticlesShell = std::vector<int>(setInd.begin(), setInd.end());
	}
};