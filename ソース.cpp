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

//this code is now incomplete,dont touch.

class WinSockCaller {
public:
	int Call() {
		return WSAStartup(MAKEWORD(2, 2), &WD);
	}
	virtual ~WinSockCaller() {
		Close();
	}

	int Close() {
		return WSACleanup();
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
	TCP_Client(const char* hostname, const std::uint16_t& Port) { Connect(hostname, Port); }
	virtual ~TCP_Client() {
		if (S != INVALID_SOCKET) { DisConnect(); }
	}

	SOCKET Connect(const char* HOSTNAME, const std::uint16_t& Port) {
		ADDRINFO AF = { 0, };

		//AF.ai_family = AF_UNSPEC;
		AF.ai_family = AF_INET;
		AF.ai_socktype = SOCK_STREAM;
		AF.ai_protocol = IPPROTO_TCP;

		ADDRINFO* AI = nullptr;
		std::string P = std::to_string(Port);

		if (getaddrinfo(HOSTNAME, P.c_str(), &AF, &AI) != 0) { return INVALID_SOCKET; }

		SOCKET Z = INVALID_SOCKET;

		for (addrinfo* X = AI; X; X = X->ai_next) {
			Z = socket(X->ai_family, X->ai_socktype, X->ai_protocol);
			if (Z == INVALID_SOCKET) { freeaddrinfo(AI); return Z; }
			if (bind(Z, X->ai_addr, X->ai_addrlen) == SOCKET_ERROR) {
				closesocket(Z);
				Z = INVALID_SOCKET;
				continue;
			}
			if (connect(Z, X->ai_addr, X->ai_addrlen) == SOCKET_ERROR) {
				closesocket(Z);
				Z = INVALID_SOCKET;
				continue;
			}
			break;
		}
		freeaddrinfo(AI);
		S = Z;
		return Z;
	}
	/** /
	int Connect(const char* IP, std::uint16_t Port) {//IPv4 cliant??
		A.sin_addr.S_un.S_addr = inet_addr(IP);
		A.sin_family = AF_INET;
		A.sin_port = htons(Port);
		return  connect(S, (SOCKADDR*)&A, sizeof(A));
	}
	int Connect(const SOCKADDR_IN& In) {//IPv4 cliant??
		int X = connect(S, (SOCKADDR*)&In, sizeof(In));
		if (X == SOCKET_ERROR) { if (LastError() != WSAEWOULDBLOCK) { return SOCKET_ERROR; } }
		A = In;
		return X;
	}
	/**/
	int Write(const std::vector<char>& In) {
		return send(S, In.data(), In.size(), 0);
		//return send(S, In.data(), In.size(), 0, (SOCKADDR*)&A, sizeof(A));
	}
	int Write(const char* In, std::size_t L) {
		return send(S, In, L, 0 );
		//return send(S, In, L, 0, (SOCKADDR*)&A, sizeof(A));
	}

	std::vector<char> Read() {

		static const int L = 65000;
		char D[L] = { 0, };

		int S = sizeof(A);

		int R = -1;

		std::vector<char> RR;

		while (R = recv(S, D, L, 0)) {
		//while (R = recvfrom(S, D, L, 0, (SOCKADDR*)&A, &S)) {
			RR.insert(RR.end(), D, D+R);//min provide by windows. yes i know STL Have too.but it is confrict. 
		}

		return RR;
	}
	int Read(char* Buf,std::size_t L) {
		int S = sizeof(A);
		return recv(S, Buf, L, 0);
		//return recvfrom(S, Buf, L, 0, (SOCKADDR*)&A, &S);
	}

	int DisConnect() {//after all conection end. to must call WSAcleanup(). but my case to  auto absove by [WinSockCaller] too it.
		
		if (S == INVALID_SOCKET) { return SOCKET_ERROR; }

		auto X = S;
		S = INVALID_SOCKET;
		A = SOCKADDR_IN();
		return closesocket(X);
	}

	int LastError() {
		return WSAGetLastError();
	}

	int FormatError() {
		return FormatError();
	}

protected:
	SOCKET S = INVALID_SOCKET;
	SOCKADDR_IN A = { 0, };
};

unsigned long IPByNumber(unsigned char A, unsigned char B, unsigned char C, unsigned char D) {
	return D << 24 | C << 16 | B << 8 | A;
}

SOCKADDR_IN MakeSOCKADDER(short F, unsigned char A, unsigned char B, unsigned char C, unsigned char D, unsigned short Port) {
	SOCKADDR_IN R;
	R.sin_family = F;
	R.sin_port = htons(Port);
	R.sin_addr.S_un.S_addr = IPByNumber(A, B, C, D);

	return R;
}


int main() {
	WinSockCaller WS;
	if (WS.Call() != 0) { return -1; }

	const char* HN = "www.google.co.jp";
/** /
	LPHOSTENT LHT = gethostbyname(HN);

	SOCKADDR_IN Q = MakeSOCKADDER(AF_INET,(std::uint8_t)(*LHT->h_addr_list)[0],(std::uint8_t)(*LHT->h_addr_list)[1],(std::uint8_t)(*LHT->h_addr_list)[2],(std::uint8_t)(*LHT->h_addr_list)[3],27915);

	const int L = 1128;
	char A[L] = { 0, };
	char B[L] = { 0, };
	getnameinfo((SOCKADDR*) & Q, sizeof(Q), A, L,B,L,0);

	LHT = gethostbyname(A);
	Q = MakeSOCKADDER(AF_INET,(std::uint8_t)(*LHT->h_addr_list)[0],(std::uint8_t)(*LHT->h_addr_list)[1],(std::uint8_t)(*LHT->h_addr_list)[2],(std::uint8_t)(*LHT->h_addr_list)[3],27915);

/**/
	
	TCP_Client TC;
	if (TC.Connect(HN, 27015) == INVALID_SOCKET) { std::cout << TC.LastError() << std::endl; return -1; }
	//if(TC.Connect(Q) == SOCKET_ERROR) { std::cout << TC.LastError() << std::endl; return -1; }

	std::this_thread::sleep_for(std::chrono::seconds(4));

	auto R = TC.Read();
	auto W = TC.Write("DISCONNECT!", sizeof("DISCONNECT!"));


	TC.DisConnect();

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