# TCP Monitor

Native Win32 TCP connection enumerator written in C++ using the Windows IP Helper API.

This project lists active IPv4 TCP connections and displays the local address, local port, remote address, remote port, owning process PID, and TCP state.

The implementation is intentionally simple and focuses on understanding how raw Windows networking structures are retrieved and parsed from user mode.

---

## Source Code

```cpp
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <Windows.h>
#include <iostream>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

int main()
{
    DWORD size = 0;
    DWORD tcp = GetExtendedTcpTable(NULL, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, NULL);
    if (tcp == NO_ERROR) {
        std::cout << "succes tcp table " << std::endl;
    }
    else {
        std::cout << GetLastError() << std::endl;
    }

    HANDLE heap = HeapCreate(NULL, 0, size);
    std::cout << size << std::endl;

    PVOID buffer = HeapAlloc(heap, NULL, size);

    DWORD tcp2 = GetExtendedTcpTable(buffer, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, NULL);
    if (tcp2 == NO_ERROR) {
        std::cout << "succes tcp table " << std::endl;
    }
    else {
        std::cout << GetLastError() << std::endl;
    }

    PMIB_TCPTABLE_OWNER_PID TABLEtcp = (PMIB_TCPTABLE_OWNER_PID)buffer;

    int i = 0;
    while (i < TABLEtcp->dwNumEntries) {
        IN_ADDR localaddr;
        localaddr.S_un.S_addr = TABLEtcp->table[i].dwLocalAddr;

        IN_ADDR remoteaddr;
        remoteaddr.S_un.S_addr = TABLEtcp->table[i].dwRemoteAddr;

        char localip[INET_ADDRSTRLEN];
        char remoteip[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &localaddr, localip, sizeof(localip));
        inet_ntop(AF_INET, &remoteaddr, remoteip, sizeof(remoteip));

        std::cout << "ip locale : " << localip << " Port local :" << ntohs((u_short)TABLEtcp->table[i].dwLocalPort) << " IP distante :" << remoteip << " Port distant :" << ntohs((u_short)TABLEtcp->table[i].dwRemotePort) << " PID " << TABLEtcp->table[i].dwOwningPid << " STATE :" << TABLEtcp->table[i].dwState << std::endl;

        i++;
    }
}
```

---

## Features

* Enumerates active TCP connections
* Displays the local IPv4 address
* Displays the local TCP port
* Displays the remote IPv4 address
* Displays the remote TCP port
* Displays the owning process PID
* Displays the raw TCP state value
* Uses native Win32/IP Helper APIs
* No external dependency

---

## Technologies Used

* C++
* Win32 API
* Winsock2
* IP Helper API

Required libraries:

```cpp
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
```

---

## How It Works

### 1. Requesting the Required Buffer Size

The program first calls `GetExtendedTcpTable` with a `NULL` buffer:

```cpp
DWORD tcp = GetExtendedTcpTable(NULL, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, NULL);
```

This is a common Win32 pattern. The first call is used to retrieve the required buffer size through the `size` variable.

The TCP table size is not fixed because the number of active TCP connections changes at runtime.

---

### 2. Allocating Memory

After the required size is known, the program creates a private heap and allocates a buffer:

```cpp
HANDLE heap = HeapCreate(NULL, 0, size);
PVOID buffer = HeapAlloc(heap, NULL, size);
```

This buffer will receive the TCP table returned by the Windows networking stack.

---

### 3. Retrieving the TCP Table

The program calls `GetExtendedTcpTable` a second time, now with a valid buffer:

```cpp
DWORD tcp2 = GetExtendedTcpTable(buffer, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, NULL);
```

If the call succeeds, the buffer contains a `MIB_TCPTABLE_OWNER_PID` structure.

---

### 4. Parsing the Result

The raw buffer is cast to:

```cpp
PMIB_TCPTABLE_OWNER_PID TABLEtcp = (PMIB_TCPTABLE_OWNER_PID)buffer;
```

This structure contains:

```cpp
dwNumEntries
```

which represents the number of TCP entries, and an array of TCP rows:

```cpp
TABLEtcp->table[i]
```

Each row contains information about one TCP connection.

---

### 5. Reading Each TCP Entry

The program loops through all entries:

```cpp
while (i < TABLEtcp->dwNumEntries)
```

For each TCP row, it reads:

```cpp
dwLocalAddr
dwLocalPort
dwRemoteAddr
dwRemotePort
dwOwningPid
dwState
```

These fields represent the local endpoint, remote endpoint, owning process, and TCP state.

---

### 6. Converting IP Addresses

The IP addresses returned by `GetExtendedTcpTable` are raw IPv4 values.

They are converted into readable strings using `inet_ntop`:

```cpp
inet_ntop(AF_INET, &localaddr, localip, sizeof(localip));
inet_ntop(AF_INET, &remoteaddr, remoteip, sizeof(remoteip));
```

This converts values such as raw integers into addresses like:

```txt
127.0.0.1
10.14.22.209
0.0.0.0
```

---

### 7. Converting Ports

TCP ports are stored in network byte order.

The program converts them to host byte order using `ntohs`:

```cpp
ntohs((u_short)TABLEtcp->table[i].dwLocalPort)
ntohs((u_short)TABLEtcp->table[i].dwRemotePort)
```

Without this conversion, the displayed port numbers would be incorrect.

---

## Example Output

```txt
ip locale : 0.0.0.0 Port local :135 IP distante :0.0.0.0 Port distant :0 PID 1772 STATE :2
ip locale : 10.14.22.209 Port local :50915 IP distante :34.125.230.47 Port distant :1119 PID 16788 STATE :5
ip locale : 127.0.0.1 Port local :50765 IP distante :127.0.0.1 Port distant :50804 PID 23384 STATE :5
```

---

## TCP State Values

The program currently prints the raw numeric TCP state.

Common values include:

```txt
2  = LISTEN
5  = ESTABLISHED
6  = FIN_WAIT_1
8  = CLOSE_WAIT
11 = TIME_WAIT
```

A future improvement would be to convert these numeric values into readable strings using a `switch` statement.

---

## Build

This project can be built with Visual Studio on Windows.

Recommended configuration:

```txt
x64 / Debug
x64 / Release
```

The required libraries are linked directly in the source code:

```cpp
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
```

---

## Possible Improvements

* Add TCP state name conversion
* Display process names from PID
* Add UDP support with `GetExtendedUdpTable`
* Add IPv6 support with `AF_INET6`
* Add filtering by PID or port
* Add continuous refresh mode
* Improve error handling
* Free allocated heap memory before exiting

---

## Purpose

This project is a low-level Windows networking exercise.

It demonstrates how to interact with native Win32 networking APIs, parse system structures, handle dynamic buffers, and convert raw network data into human-readable output.

The goal is not to use high-level libraries, but to understand how Windows exposes TCP connection information through the IP Helper API.
