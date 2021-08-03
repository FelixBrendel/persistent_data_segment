#include <windows.h>
#include "patcher_markers.h"
#include "..\persistent_data.hpp"

#pragma intrinsic(strcat,strlen)
#pragma function(memset)

void *memset(void* _dst, int _val, size_t len) {
    byte val = *(byte*)&_val;
    byte *at = (byte*)_dst;
    while(len--) {
        *at++ = val;
    }
    return _dst;
}

Persistent_Data persistent_data = *((Persistent_Data*)persistent_data_marker);

static int print_int_into_str(int value, char *ptr) {
    // Function return size of string and convert signed  *
    // * integer to ascii value and store them in array of  *
    // * character with NULL at the end of the array

    int count = 0, temp;

    if (ptr == NULL)
        return 0;

    if (value == 0) {
        *ptr='0';
        return 1;
    }

    if (value < 0) {
        value *= (-1);
        *ptr++ = '-';
        count++;
    }

    for(temp=value;temp>0;temp/=10,ptr++);

    *ptr='\0';

    for(temp = value; temp > 0; temp /= 10) {
        *--ptr = temp % 10 + '0';
        count++;
    }

    return count;
}

void save_and_quit() {
    // this will be patched in
    static byte storage_offset_in_exe[8]  = marker_for_addr_of_persistent_data_marker;

    char* new_file_name = (char*)HeapAlloc(GetProcessHeap(), 0, sizeof(char) * 1000);
    GetTempPathA(1000, new_file_name);
    strcat(new_file_name, "asd");
    char appendix[9] = "_";

    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    int num = (int)li.QuadPart;
    appendix[1] = 'a' +  num        % 26;
    appendix[2] = 'a' + (num /= 26) % 26;
    appendix[3] = 'a' + (num /= 26) % 26;
    appendix[4] = '\0';
    strcat(new_file_name, appendix);


    char current_exe_path[MAX_PATH];
    GetModuleFileName(NULL, current_exe_path, MAX_PATH);
    CopyFile(current_exe_path, new_file_name, FALSE);

    HANDLE hFile = CreateFile(new_file_name,
                              GENERIC_WRITE,
                              0,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);

    SetFilePointer(hFile, *((int *)storage_offset_in_exe), NULL, FILE_BEGIN);
    WriteFile(hFile, &persistent_data, sizeof(persistent_data), NULL, NULL);
    CloseHandle(hFile);


    char delete_cmd[2000] = {0};

    strcat(delete_cmd, "cmd /c taskkill /F /PID ");
    print_int_into_str(GetCurrentProcessId(), delete_cmd+24);
    strcat(delete_cmd, " & move ");
    strcat(delete_cmd, new_file_name);
    strcat(delete_cmd, " ");
    strcat(delete_cmd, current_exe_path);
    strcat(delete_cmd, " || del ");
    strcat(delete_cmd, new_file_name);


    PROCESS_INFORMATION pi = {};
    STARTUPINFO si = {};
    si.cb = sizeof(si);
    CreateProcess(NULL, delete_cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    Sleep(100000);
    ExitProcess(0);
}

int main();

void persistent_main() {
    main();
    save_and_quit();
}
