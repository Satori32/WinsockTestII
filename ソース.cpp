#define nominmax

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>
#include <vector>
#include <string>
#include <tuple>

#include <algorithm>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable : 4996)

//this code is now incomplete,dont touch.

//i not work to some day to forgot all.lol.

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
	TCP_Client(unsigned long IP, const std::uint16_t& Port) { if (Connect(IP, Port) == SOCKET_ERROR) { closesocket(S); S = INVALID_SOCKET; return; }; }
	virtual ~TCP_Client() {
		if (S != INVALID_SOCKET) { DisConnect(); }
		return;
	}
	
	bool IsConnected() {
		return S != INVALID_SOCKET;
	}
	/** /
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
	/**/
	int Connect(unsigned long IP, std::uint16_t Port) {//IPv4 cliant??
		S = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (S == INVALID_SOCKET) { return SOCKET_ERROR; }
		A.sin_addr.S_un.S_addr = IP;
		A.sin_family = AF_INET;
		A.sin_port = htons(Port);
		int R = connect(S, (SOCKADDR*)&A, sizeof(A));
		if (R == SOCKET_ERROR) { closesocket(S); S = INVALID_SOCKET; return R; }
		return  R;
	}
	int Connect(const SOCKADDR_IN& In) {//IPv4 cliant??
		S = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (S == INVALID_SOCKET) { return SOCKET_ERROR; }
		int R = connect(S, (SOCKADDR*)&In, sizeof(In));
		if (R == SOCKET_ERROR) { closesocket(S); S = INVALID_SOCKET; return R; }
		A = In;
		return R;
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

		static const int L = (1<<15)-1;
		char D[L] = { 0, };

		int S = sizeof(A);

		int R = -1;

		std::vector<char> RR;

		while (R = recv(S, D, L, 0)) {
		//while (R = recvfrom(S, D, L, 0, (SOCKADDR*)&A, &S)) {
			if (R == SOCKET_ERROR) { break; }
			RR.insert(RR.end(), D, D+R);
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
std::vector<std::tuple<std::string, unsigned long>>  GetIPByName(const char* Name) {
	LPHOSTENT LHT = gethostbyname(Name);
	if (LHT == nullptr)return{};
	if (LHT->h_addrtype != AF_INET) { return {}; }

	std::vector<std::tuple<std::string, unsigned long>> IPs;

	SOCKADDR_IN A;

	for (std::size_t i = 0; LHT->h_addr_list[i]; i++) {
		A.sin_addr.S_un.S_addr = *(u_long*)LHT->h_addr_list[i];

		IPs.push_back({ {(LHT->h_addr_list[i])}, A.sin_addr.S_un.S_addr });
	}

	return IPs;
}

int main() {
	WinSockCaller WS;
	if (WS.Call() != 0) { return -1; }

	const char* HN1 = "www.google.co.jp";
	const char* HN2 = "localhost.";

	const char Mess[] = "Disconnect!";
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

	auto A = GetIPByName(HN2);
	u_short Po = 27015;
	std::cout << "Start TCP Client!" << std::endl;
	TCP_Client TC;
	if (TC.Connect(std::get<1>(A[0]), Po) == SOCKET_ERROR) { std::cout << WSAGetLastError() << std::endl; return -1; }
	//if(TC.Connect(Q) == SOCKET_ERROR) { std::cout << TC.LastError() << std::endl; return -1; }
	std::string SS;

	std::cout << "Start Loop!" << std::endl;

	while (TC.IsConnected()) {

		//std::this_thread::sleep_for(std::chrono::seconds(4));
		std::cin >> SS;
		TC.Write({ SS.begin(),SS.end() });

		auto R = TC.Read();
		R.push_back('\0');
		std::string SS = R.data();
		std::cout << "Echo:" << SS << std::endl;

	}

	TC.DisConnect();

	std::cout << WSAGetLastError() << std::endl; 
	
	return 0;

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