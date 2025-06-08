#include <chrono>
#include <iostream>
#include <vector>

#include "Runtime/Base/Memory/Memory.h"
#include "Runtime/Base/Object/Object.h"
#include "Runtime/Base/Memory/StrongRef.h"

class Dog : public Moyu::Object
{
public:
    Dog()  = default;
    ~Dog()
    {
        std::cout << "Free Dog\n";
    };

    void Print()
    {
        std::cout << "Hello Dog\n";
    }
};

int main()
{
    Moyu::StrongRef<Dog> Dog(Moyu::New<Dog>(), Moyu::Acquire::NORETAIN);

    Dog.Get()->Print();
    Dog.~StrongRef();

    Moyu::ReportLeaks();

    return 0;
}
