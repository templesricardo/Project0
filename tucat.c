#include <unistd.h>

int main(void)
{
    write(STDOUT_FILENO, "hello\n", 6);
    return 0;
}