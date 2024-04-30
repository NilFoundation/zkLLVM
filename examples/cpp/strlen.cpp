#include <cstddef>
[[circuit]] size_t strlen(const char *str) {
    size_t counter = 0;
    while (str[counter++] != '\0')
        ;
    return counter - 1;
}
