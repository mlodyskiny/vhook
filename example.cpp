#include <iostream>
#include <print>

#include "VHook.h"

class Tset
{
public:
	Tset() {};

	virtual void Dog()
	{
		std::print("Dog\n");
	}
	virtual void Cat()
	{
		std::print("Cat\n");
	}
};

void Detour(void* this_)
{
	std::print("Lolz\n");
}

int main()
{
	try
	{
		Tset TsetObj{};

		std::print("Before hooking:\n");
		TsetObj.Dog();
		TsetObj.Cat();

		std::print("After hooking:\n");
		{
			VHook TsetHook(&TsetObj);
			TsetHook.Add(0, Detour);
			TsetHook.Add(1, Detour);
			TsetObj.Dog();
			TsetObj.Cat();
		}

		std::print("VHook object gone (unhooked):\n");
		TsetObj.Dog();
		TsetObj.Cat();

		std::print("Test out of bounds hook:\n");
		VHook TsetHook(&TsetObj);
		TsetHook.Add(2, Detour);
	}
	catch (const std::runtime_error& RuntimeError)
	{
		std::print("Runtime error: {}\n", RuntimeError.what());
		return 1;
	}
	catch (const std::exception& Exception)
	{
		std::print("Exception: {}\n", Exception.what());
		return 2;
	}
	catch (...)
	{
		std::print("Unknown exception :(\n");
		return 3;
	}

	return 0;
}