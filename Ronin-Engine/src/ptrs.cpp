#include <iostream>

int function_p()
{
    using std::cout;

    int number = 30;

    int* ptr = &number;

    *ptr = 50;

    cout << "Original varaible: " << number << "\n";
    cout << "Dereference through a pointer: " << *ptr << "\n";
    cout << "Memory Adress: " << ptr << "\n";

    return 0;
}