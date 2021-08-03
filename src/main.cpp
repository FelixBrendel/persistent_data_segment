#include <windows.h>

#pragma intrinsic(strcat,strlen)
#pragma function(memset)

#include "persistent_data.hpp"

int print_int_to_str(int value, char* buffer) {
    int count = 0, temp;
    char* ptr = buffer;

    if (value == 0) {
        *ptr='0';
        count = 1;
    } else {
        if (value < 0) {
            value *= (-1);
            *ptr++ = '-';
            count++;
        }

        for(temp=value;temp>0;temp/=10)
            ptr++;

        *ptr='\0';

        for(temp = value; temp > 0; temp /= 10) {
            *--ptr = temp % 10 + '0';
            count++;
        }
    }
    return count;
}

auto main() -> int {
    char text[200] = "You ran this program ";

    char* ptr = text+21;
    ptr += print_int_to_str(persistent_data.counter, ptr);
    strcat(ptr, " times.");

    MessageBoxA(0, text, "I remember", MB_ICONINFORMATION);

    persistent_data.counter++;
}
