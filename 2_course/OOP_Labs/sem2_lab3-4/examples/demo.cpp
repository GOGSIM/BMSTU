#include <iostream>

/* Многострочный комментарий:
   ключевое слово class здесь остаётся комментарием. */
int main()
{
    const char* address = "https://example.org"; // // внутри строки — не комментарий
    std::cout << "Привет, Qt! " << address << '\n';
    return 42;
}
