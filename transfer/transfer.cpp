#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *pszDirectory = NULL;

char *ultostr(unsigned long value, char *ptr, int base) {
    unsigned long t = 0, res = 0;
    unsigned long tmp = value;
    int count = 0;
    
    if (!ptr)
        return NULL;
    
    if (!tmp)
        count++;
    
    while (tmp > 0) {
        tmp = tmp / base;
        count++;
    }
    
    ptr += count;
    *ptr = 0;
    
    do {
        res = value - base * (t = value / base);
        if (res < 10)
            *(--ptr) = '0' + res;
        else if ((res >= 10) && (res < base))
            *(--ptr) = 'a' - 10 + res;
    } while ((value = t));
    
    return ptr;
}

typedef bool (* PFN_CALLBACK)(char *ipAddress);

bool processEachIP(char *ipAddress) {
    char szRunFtpServer[1024];
    memset(szRunFtpServer, 0, 1024);
    sprintf(szRunFtpServer, "busybox tcpsvd -vE %s 8007 busybox ftpd -w ~/storage/shared", ipAddress);
    
    int nDots = 0, i;
    int ipAddressLen = strlen(ipAddress);
    char *parts[4] = { ipAddress, NULL, NULL, NULL };
    for (i = 0; i < ipAddressLen; i++) {
        if (ipAddress[i] == '.') {
            ipAddress[i] = 0;
            
            if (nDots++ == 3) {
                printf("Invalid IP address\n");
                return false;
            }
            
            parts[nDots] = ipAddress + i + 1;
        }
    }
    
    if (nDots < 3) {
        printf("Invalid IP address\n");
        return false;
    }
    
    int ipBytes[4];
    ipBytes[0] = atoi(parts[0]);
    ipBytes[1] = atoi(parts[1]);
    ipBytes[2] = atoi(parts[2]);
    ipBytes[3] = atoi(parts[3]);
    
    if (ipBytes[0] >= 127)
        return true;
    
    unsigned long rawIPAddress;
    rawIPAddress = ipBytes[0] << 24;
    rawIPAddress |= ipBytes[1] << 16;
    rawIPAddress |= ipBytes[2] << 8;
    rawIPAddress |= ipBytes[3];
    
    char deviceToken[16];
    ultostr(rawIPAddress, deviceToken, 36);
    
    printf("Device Token: %s\n", deviceToken);
    printf("Wish to transfer files? (Y | N)...");
    fflush(stdin);
    char c = getc(stdin);
    if ((c == 'Y') || (c == 'y')) {
        printf("\n");
        system(szRunFtpServer);
    }
    else
        exit(1);
    
    return false;
}

void EnumIPs(char * ipList, PFN_CALLBACK pfnSink) {
    size_t len = strlen(ipList);
    char *listItem = ipList;
    bool mustContinue;
    size_t i;
    
    for (i = 0; i < len; i++) {
        if (ipList[i] == '\n') {
            ipList[i] = '\0';
            mustContinue = pfnSink(listItem);
            if (!mustContinue)
                return;
    
            listItem = &ipList[i + 1];
        }
    }
}

int main(int argc, char *argv[]) {
    char buffer[400];
    
    FILE *fdCommand = popen("ifconfig | grep -w inet | awk '{print $2}'", "r");
    if (!fdCommand) {
        printf("Failed to execute a command to retrieve private IP address\n");
        return 1;
    }
    
    memset(buffer, 0, 400);
    fread(buffer, 1,  400, fdCommand);
    pclose(fdCommand);
    
    pszDirectory = argv[1];
    EnumIPs(buffer, processEachIP);
    return 0;
}