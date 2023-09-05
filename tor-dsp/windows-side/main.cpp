#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

#define GetLastErrorAsHr (HRESULT_FROM_WIN32(GetLastError()))

HRESULT AskDeviceToken(PULONG pulRawIPAddress);
HRESULT LoadScriptlet(IDispatch **ppScriptlet);

int main() {
    SetConsoleTitle(L"TOR-DSP");
	std::cout << "Welcome to Tor DSP (Distributed Socks Proxy)!" << std::endl;

	// Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }
	
	ULONG ipAddress = 0;
	HRESULT hr = AskDeviceToken(&ipAddress);
	if (FAILED(hr)) {
		std::cerr << "Failed to get device token" << std::endl;
		std::cerr << "Error code: 0x" << std::hex << hr << std::endl;
		WSACleanup();
		return 1;
	}

	if (hr == S_FALSE) {
		std::cout << "Operation was canceled by the user" << std::endl;
		WSACleanup();
		return 0;
	}

    // Create socket
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind to localhost and specified port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(9050);
    if (bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket" << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket" << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Listening for incoming connections on 127.0.0.1:9050" << std::endl;

    // Accept incoming connection
    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to accept connection" << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connection accepted" << std::endl;

    // Connect to user-specified IP address and port
    sockaddr_in destinationAddress;
    destinationAddress.sin_family = AF_INET;
    destinationAddress.sin_addr.s_addr = ipAddress;
    destinationAddress.sin_port = htons(9050);
    if (connect(clientSocket, (struct sockaddr*)&destinationAddress, sizeof(destinationAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to destination" << std::endl;
        closesocket(clientSocket);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to destination" << std::endl;

    // Forward traffic from localhost to destination
    char *traffic = new char[4096];
    int bytesRead;

	if (!traffic) {
		std::cerr << "Failed to allocate a buffer for the network traffic" << std::endl;
        closesocket(clientSocket);
        closesocket(listenSocket);
        WSACleanup();
		return 1;
	}

    while ((bytesRead = recv(clientSocket, traffic, 4096, 0)) > 0) {
        send(clientSocket, traffic, bytesRead, 0);
    }

    // Cleanup
	delete[] traffic;
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();

	std::cout << "Tor DSP is about to exit...";
	_getch();
    return 0;
}

HRESULT LoadScriptlet(IDispatch **ppScriptlet) {
	HRESULT hr = S_OK;
	WCHAR wszExecutablePath[MAX_PATH];
	PWSTR pwszScriptMoniker;

	ZeroMemory(wszExecutablePath, sizeof(wszExecutablePath));

	if (GetModuleFileName(NULL, wszExecutablePath, MAX_PATH)) {
		pwszScriptMoniker = (PWSTR) CoTaskMemAlloc((wcslen(wszExecutablePath) + 30) * sizeof(WCHAR));
		if (pwszScriptMoniker) {
			wcscpy(pwszScriptMoniker, L"script:res://");
			wcscat(pwszScriptMoniker, wszExecutablePath);
			wcscat(pwszScriptMoniker, L"/deviceToken.wsc");

			hr = CoGetObject(pwszScriptMoniker, NULL, IID_IDispatch, (void **) ppScriptlet);

			CoTaskMemFree(pwszScriptMoniker);
		}
		else
			hr = E_OUTOFMEMORY;
	}
	else
		hr = GetLastErrorAsHr;

	return hr;
}

HRESULT AskDeviceToken(PULONG pulRawIPAddress) {
	HRESULT hr = S_OK;

	*pulRawIPAddress = 0;

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(hr)) {
		IDispatch *pScriptlet = NULL;
		hr = LoadScriptlet(&pScriptlet);
		if (SUCCEEDED(hr)) {
			VARIANT vIPAddress;
			DISPPARAMS dpNoArgs = { NULL, NULL, 0, 0 };

			VariantInit(&vIPAddress);
			hr = pScriptlet->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dpNoArgs, &vIPAddress, NULL, NULL);
			if (SUCCEEDED(hr)) {
				if (vIPAddress.vt == VT_BSTR) {
					SIZE_T bufSize = SysStringLen(vIPAddress.bstrVal) + 1;
					PSTR pszIPAddress = (PSTR) CoTaskMemAlloc(bufSize);
					if (pszIPAddress) {
						wcstombs(pszIPAddress, vIPAddress.bstrVal, bufSize);
						*pulRawIPAddress = inet_addr(pszIPAddress);
						CoTaskMemFree(pszIPAddress);
					}
					else
						hr = E_OUTOFMEMORY;
				}
				else if (vIPAddress.vt == VT_NULL)
					hr = S_FALSE;
				else
					hr = DISP_E_TYPEMISMATCH;
			}

			VariantClear(&vIPAddress);
			pScriptlet->Release();
		}

		CoUninitialize();
	}

	return hr;
}