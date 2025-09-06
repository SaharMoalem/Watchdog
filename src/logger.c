#include <stdio.h>

#include "logger.h"

char* role[2] = {"Client", "Server"};

int UploadMessage(const char* filename, const char* message, int location)
{
    FILE* logger = fopen(filename, "a");

    fprintf(logger, "%s: %s\n" ,role[location], message);
    fclose(logger);

    return 0;
}