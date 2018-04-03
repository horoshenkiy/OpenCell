#ifndef LOGGER_H
#define LOGGER_H

namespace FruitWork {
namespace Utilits {

template <class Class, typename Func, typename ... Args>
void LogArgWithCallFunc(Class object, Func func, Args ... args) {
	object.StartLogging();
	func(args ...);
	object.EndLogging();
}

class Loggable {
public:

	virtual void StartLogging() = 0;

	virtual void EndLogging() = 0;
};

}
}

#endif // LOGGER_H