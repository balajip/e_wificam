typedef struct ARPPacket
{
// DLC Header
    unsigned char      aucDesMAC[6];             /* destination HW addrress */
    unsigned char      aucSrcMAC[6];             /* source HW addresss */
    unsigned short     usType;                   /* ethernet type */
// ARP Frame
    unsigned short     usHWType;           /* hardware address */
    unsigned short     usProtType;         /* protocol address */
    unsigned char      ucHWAddrLen;        /* length of hardware address */
    unsigned char      ucProtAddrLen;      /* length of protocol address */
	unsigned short     usOpcode;           /* ARP/RARP */
	unsigned char      aucSendHWAddr[6];    /* sender hardware address */
	unsigned char      aucSendProtAddr[4];     /* sender protocol address */
	unsigned char      aucTargHWAddr[6];    /* target hardware address */
	unsigned char      aucTargProtAddr[4];     /* target protocol address */
// ARP Packet = DLC header + ARP Frame + Padding
    unsigned char      aucPadding[18];
} TARPPacket;

typedef struct member_data
{
	int				iFd;
	unsigned long	ulIP;
	TARPPacket		*ptaPck;
	unsigned char	*pucMAC;
	char			acIPAddr[16];
} TMemberData;
