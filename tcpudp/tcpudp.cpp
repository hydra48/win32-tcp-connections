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
    DWORD tcp = GetExtendedTcpTable(NULL, &size,TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, NULL);
    if (tcp == NO_ERROR) {
        std::cout << "succes tcp table "<< std::endl;
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
    
   

