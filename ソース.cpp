#define nominmax

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>
#include <vector>
#include <string>

#include <algorithm>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable : 4996)

class WinSockCaller {
public:
	int Call() {
		return WSAStartup(MAKEWORD(2, 2), &WD);
	}
	int Close() {
		return WSACleanup();
	}

	virtual ~WinSockCaller() {
		Close();
	}

	WSADATA GetData() {
		return WD;
	}


protected:
	WSADATA WD = { 0, };
};

class TCP_Client {
public:
	TCP_Client() {}
	TCP_Client(const char* IP, const std::uint16_t& Port) { Connect(IP, Port); }
	virtual ~TCP_Client() {
		if (S != INVALID_SOCKET) { DisConnect(); }
	}

	SOCKET MakeSocket() {
		S = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (S == INVALID_SOCKET) { return INVALID_SOCKET; }
		return S;
	}

	int Connect(const char* IP, std::uint16_t Port) {//IPv4 cliant??
		A.sin_addr.S_un.S_addr = inet_addr(IP);
		A.sin_family = AF_INET;
		A.sin_port = htons(Port);
		return  connect(S, (SOCKADDR*)&A, sizeof(A));
	}

	int Write(const std::vector<char>& In) {
		return sendto(S, In.data(), In.size(), 0, (SOCKADDR*)&A, sizeof(A));
	}
	int Write(const char* In, std::size_t L) {
		return sendto(S, In, L, 0, (SOCKADDR*)&A, sizeof(A));
	}

	std::vector<char> Read() {

		static const int L = 65530;
		char D[L] = { 0, };

		int S = sizeof(A);

		int R = -1;

		std::vector<char> RR;

		while (R = recvfrom(S, D, L, 0, (SOCKADDR*)&A, &S)) {
			RR.insert(RR.end(), D, D+min(L,R));//min provide by windows. yes i know STL Have too.but it is confrict. 
		}

		return RR;
	}
	int Read(char* Buf,std::size_t L) {
		int S = sizeof(A);
		return recvfrom(S, Buf, L, 0, (SOCKADDR*)&A, &S);
	}

	int DisConnect() {//after all conection end. to must call WSAcleanup(). but my case to  auto absove by [WinSockCaller] too it.
		auto X = S;
		S = INVALID_SOCKET;
		A = SOCKADDR_IN();
		return closesocket(X);
	}

protected:
	SOCKET S = INVALID_SOCKET;
	SOCKADDR_IN A = { 0, };
};



int main() {
	WinSockCaller WS;
	WS.Call();

	const char* HN = "www.yahoo.:co.jp";
	LPHOSTENT LHT = gethostbyname(HN);

	/** /

	TCP_Client TC;
	TC.MakeSocket();
	TC.Connect("127.0,0,1", 27015);
	/**/
}

/** /
int main() {
	WS.Call();
	SOCKET S = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (S == INVALID_SOCKET) { return -1;}

	SOCKADDR_IN CS = { 0, };
	CS.sin_family = AF_INET;
	CS.sin_addr.S_un.S_addr = inet_addr("127,0,0,1");
	CS.sin_port = htons(27015);

	int A = connect(S, (SOCKADDR*) &CS, sizeof(CS));
	if (A == SOCKET_ERROR) { closesocket(S); return -1; }

	std::this_thread::sleep_for(std::chrono::seconds(4));

	int B = closesocket(S);
	if (B == SOCKET_ERROR) { return -1; }

	return 0;
}
*/