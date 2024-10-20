#include <stdio.h>
#include <stdlib.h>

#include "../libdotenv.h"

int main(void) {
    if (loadenv("examples/.env", 0) != 0) return 1;

    printf("This is with whitespaces -> `%s`\n", getenv("EXAMPLE_WITH_WHITESPACES"));
    printf("This is with one word -> `%s`\n", getenv("EXAMPLE_WITH_ONE_WORD"));

    return 0;
}
