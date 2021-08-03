#include <stdlib.h> // malloc
#include <stdio.h>  // printf
#include <string.h> // memcpy, strcmp

#include "./patcher_markers.h"
#include "../persistent_data.hpp"

Persistent_Data initial_data;

typedef struct {
    long length;
    char* data;
} String;

String read_entire_file(const char* filename) {
    String ret = {};

    FILE* f = fopen(filename, "rb");

    fseek(f, 0, SEEK_END);
    ret.length = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    ret.data = (char*)malloc(ret.length + 1);
    fread(ret.data, 1, ret.length, f);
    fclose(f);

    ret.data[ret.length] = 0;

    return ret;
}

void write_file(const char* filename, String content) {
    FILE* f = fopen(filename, "wb+");
    fwrite(content.data, content.length, 1, f);
    fclose(f);
}

int main() {
    unsigned long long addr_of_marker1 = 0;
    unsigned long long addr_of_marker_for_addr_of_marker1 = 0;

    String exe_string = read_entire_file(exe_name_pre_patch);

    int i = 0;
    for (char* c = exe_string.data; i < exe_string.length; ++i, ++c) {
        if (strcmp(c, persistent_data_marker) == 0) {
            if (addr_of_marker1) {
                printf("Patcher: Warning: %s was found more than once (%llx)\n", persistent_data_marker, addr_of_marker1);
                printf("    >> maybe compiling with debug info?\n\n");
            }
            addr_of_marker1 = i;
        }
        if (strcmp(c, marker_for_addr_of_persistent_data_marker) == 0) {
            if (addr_of_marker_for_addr_of_marker1) {
                printf("Patcher: Warning: %s was found more than once (%llx)\n", marker_for_addr_of_persistent_data_marker, addr_of_marker_for_addr_of_marker1);
                printf("    >> maybe compiling with debug info?\n\n");
            }
            addr_of_marker_for_addr_of_marker1 = i;
        }
    }

    typedef union {
        unsigned long long ull;
        char bytes[8];
    } long_array_union;

    long_array_union m1;
    m1.ull = addr_of_marker1;

    exe_string.data[addr_of_marker_for_addr_of_marker1+0] = m1.bytes[0];
    exe_string.data[addr_of_marker_for_addr_of_marker1+1] = m1.bytes[1];
    exe_string.data[addr_of_marker_for_addr_of_marker1+2] = m1.bytes[2];
    exe_string.data[addr_of_marker_for_addr_of_marker1+3] = m1.bytes[3];
    exe_string.data[addr_of_marker_for_addr_of_marker1+4] = m1.bytes[4];
    exe_string.data[addr_of_marker_for_addr_of_marker1+5] = m1.bytes[5];
    exe_string.data[addr_of_marker_for_addr_of_marker1+6] = m1.bytes[6];
    exe_string.data[addr_of_marker_for_addr_of_marker1+7] = m1.bytes[7];

    memcpy(exe_string.data+addr_of_marker1, &initial_data, sizeof(initial_data));

    printf("Patcher: found offsets\n"
           "  marker: %33s: %llx\n"
           "  marker_for_addr_of_marker: %14s: %llx\n",
           persistent_data_marker,
           addr_of_marker1,
           marker_for_addr_of_persistent_data_marker,
           addr_of_marker_for_addr_of_marker1
    );
    fflush(stdout);

    write_file(exe_name_post_patch, exe_string);
}
