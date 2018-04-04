#include <scene_cell.h>
#include <fruit/application.h>

int main(int argc, char* argv[]) {

	FruitWork::Application::LoadScene(new SceneCell("Cell motility!"));
	FruitWork::Application::Run(argc, argv);
	
	exit(0);
}
