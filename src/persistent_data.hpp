#pragma once

struct Persistent_Data {
    char text[13]              = "Run Number: ";
    unsigned long long counter = 0;
};

extern Persistent_Data persistent_data;
