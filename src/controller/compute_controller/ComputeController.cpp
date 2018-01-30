#include "ComputeController.h"

void ComputeController::Initialize() {
	fruit->Initialize(true);

	this->flexController = &FlexController::Get();
	this->flexParams = &FlexParams::Get();
	this->buffers = &SimBuffers::Get();
}

void ComputeController::PostInitialize(Scene *scene) {
	this->scene = scene;

	fruit->PostInitialize();
}

void ComputeController::InitializeGPU() {
	fruit->InitializeGPU();
}

void ComputeController::MapBuffers() {
	fruit->MapBuffers();
}

void ComputeController::UnmapBuffers() {
	fruit->UnmapBuffers();
}

int ComputeController::GetActiveCount() const {
	return NvFlexGetActiveCount(flexController->GetSolver());
}

int ComputeController::GetDiffuseParticles() const {
	return NvFlexGetDiffuseParticles(flexController->GetSolver(), nullptr, nullptr, nullptr);
}

void ComputeController::Update() {
	fruit->Update();
}

void ComputeController::SendBuffers(NvFlexSolver *flex) {
	fruit->SendBuffers();
}

void ComputeController::Sync() {
	fruit->Sync();
}