#ifndef _PKT_DEF_H
#define _PKT_DEF_H

// By: Igor Naperkovskiy and Joao Rodrigues
struct Header{
	unsigned char id;
	unsigned char size;
};

struct Commands{
	unsigned char direction;
	unsigned char duration;
};

class PktDef {
private:
	Header head;
	Commands *body;
	unsigned char trailer;
public:
	~PktDef();
	void SetInfo();
	int Listen(SOCKET &, char *, int);
	int Accept(SOCKET &, SOCKET &);
	int Send(SOCKET &);
	void CloseSocket(SOCKET &);
	void WinsockExit();
	void ReceivePkt(SOCKET &ConnectionSocket);
	void PrintInfo();
};

#endif
