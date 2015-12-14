#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "arp.h"

#define ETH_ALEN 6
#define IP_ALEN 4
#define IF_NAME  "eth0"

static u_char eth_xmas[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static u_char ip_null[IP_ALEN] = {0x00, 0x00, 0x00, 0x00};

int ShowData(unsigned char* pucData, int len)
{
	int i;
	for(i=0;i<len;++i)
	{
		printf("%02x ", pucData[i]);
		if((i%16) == 15)
			printf("\n");
	}
	if(len%16 != 15)
		printf("\n");
	
	return 0;
}

int GenARP(TMemberData *ptData, int iMode)//mode 0-->probe, mode 1-->announce
{
	memset(ptData->ptaPck, 0, sizeof(TARPPacket));
	
	memcpy(ptData->ptaPck->aucDesMAC, eth_xmas, ETH_ALEN);
	memcpy(ptData->ptaPck->aucSrcMAC, ptData->pucMAC, ETH_ALEN);
	ptData->ptaPck->usType = htons(0x0806);
	ptData->ptaPck->usHWType = htons(0x0001);
	ptData->ptaPck->usProtType = htons(0x0800);
	ptData->ptaPck->ucHWAddrLen = 6;
	ptData->ptaPck->ucProtAddrLen = 4;
	ptData->ptaPck->usOpcode = htons(0x0001);
	memcpy(ptData->ptaPck->aucSendHWAddr, ptData->pucMAC, ETH_ALEN);
	if(iMode == 1)//announce
	{
		memcpy(ptData->ptaPck->aucSendProtAddr, &ptData->ulIP, IP_ALEN);
	}
	memcpy(ptData->ptaPck->aucTargProtAddr, &ptData->ulIP, IP_ALEN);
	
	return 0;
}

int Conflict(TMemberData *ptData, TARPPacket *ptaGet)
{
	TARPPacket *ptaPck = ptData->ptaPck;
	
	if(memcmp(ptaPck->aucSendHWAddr, ptaGet->aucSendHWAddr, ETH_ALEN) != 0)//ptaGet not self
	{
		if(memcmp(ptaGet->aucSendProtAddr, ip_null, IP_ALEN) == 0)//get probe packet
		{
			if(memcmp(ptaPck->aucTargProtAddr, ptaGet->aucTargProtAddr, IP_ALEN) == 0)
			{
				return 1;
			}
		}
		else//announce or ARP request packet
		{
			if(memcmp(ptaPck->aucTargProtAddr, ptaGet->aucSendProtAddr, IP_ALEN) == 0)
			{
				return 1;
			}
		}
	}
	return 0;
}

int Poll(TMemberData *ptData, int iMode)//mode 0-->probe, mode 1->announce
{
	fd_set	fdsRead;
	int i, iTimes, ret;
	struct timeval tvTimeOut;
	unsigned char aucBuf[64];
	
	iTimes = (iMode==0)?3:1;
	tvTimeOut.tv_sec = 1;
	tvTimeOut.tv_usec = 0;
	
	for(i=0;i<3;i++)
	{
		FD_ZERO(&fdsRead);
		FD_SET(ptData->iFd, &fdsRead);
		tvTimeOut.tv_sec=1;
		tvTimeOut.tv_usec=0;

		ret = select(ptData->iFd+1, &fdsRead, NULL, NULL, &tvTimeOut);
		if(ret < 0)
		{
			printf("select error!\n");
		}
		else if(ret == 0)
		{
			//printf("time out!\n");
			tvTimeOut.tv_sec=1;
			tvTimeOut.tv_usec=0;
			if(i<2)
			{
				write(ptData->iFd, ptData->ptaPck, sizeof(TARPPacket));
			}
		}
		else
		{

			memset(aucBuf, 0, 64);
			ret = read(ptData->iFd, aucBuf, 64);
			if(ret == -1)
			{
				printf("recv fail\n");
			}
			//ShowData(aucBuf, 64);
			if(Conflict(ptData, (TARPPacket *)aucBuf))
			{
				return 1;
			}
		}
	}
	return 0;
}

int Zero_Init(TMemberData *ptHandle)
{
	struct ifreq		ifr;
	struct sockaddr_ll  sll;
	unsigned long ulSeed;
	
	TMemberData *ptData = ptHandle;
	
	if(ptData == NULL)
	{
		printf("Get NULL handle\n");
		return -1;
	}
	memset(ptData, 0, sizeof(*ptData));
	//memory allocate
	ptData->pucMAC = (unsigned char*)malloc(ETH_ALEN);
	ptData->ptaPck = (TARPPacket *)malloc(sizeof(TARPPacket));
	if(ptData->ptaPck == NULL || ptData->pucMAC == NULL)
	{
		printf("malloc fail\n");
		return -1;
	}
	//get socket fd, bind, get MAC addr
	ptData->iFd = -1;
    if((ptData->iFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0)
    {
        printf("socket fail\n");
        return -1;
    }
    
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, IF_NAME);
	if(ioctl(ptData->iFd, SIOCGIFINDEX, &ifr) == -1)
	{
		printf("ioctl INDEX fail\n");
		return -1;
	}
	
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = PF_PACKET;
	sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ARP);
	if(bind(ptData->iFd, (struct sockaddr *) &sll, sizeof(sll)) == -1)
	{
		printf("bind fail\n");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, IF_NAME);
	if(ioctl(ptData->iFd, SIOCGIFHWADDR, &ifr) == -1)
	{
		printf("ioctl HWADDR fail\n");
		return -1;
	}
	memcpy(ptData->pucMAC, ifr.ifr_ifru.ifru_hwaddr.sa_data, ETH_ALEN);

	//rand
	memcpy(&ulSeed, ptData->pucMAC, sizeof(ulSeed));
	srand(ulSeed);
	
	return 0;
}

int Zero_Release(TMemberData *ptHandle)
{
	TMemberData *ptData = ptHandle;
	
	if(ptData->iFd != -1)
	{
		close(ptData->iFd);
	}
	
	if(ptData->ptaPck != NULL)
	{
		free(ptData->ptaPck);
	}
	
	if(ptData->pucMAC != NULL)
	{
		free(ptData->pucMAC);
	}
	
	return 0;
}

int Zero_GenIP(TMemberData *ptHandle)
{
	int i,j;
	TMemberData *ptData = ptHandle;
	
	i = rand()%256;
	j = rand()%256;
	
	memset(ptData->acIPAddr, 0, 16);
	sprintf(ptData->acIPAddr, "169.254.%d.%d", i, j);
	//printf("Random addr=%s\n", ptData->acIPAddr);
	
	ptData->ulIP=inet_addr(ptData->acIPAddr);
	
	return 0;
}

int Zero_Probe(TMemberData *ptHandle)
{
	TMemberData *ptData = ptHandle;
	
	GenARP(ptData, 0);
	//ShowData((unsigned char*)ptData->ptaPck, sizeof(TARPPacket));
	
	write(ptData->iFd, ptData->ptaPck, sizeof(TARPPacket));

	return Poll(ptData, 0);
}

int Zero_Announce(TMemberData *ptHandle)
{
	TMemberData *ptData = ptHandle;
	
	GenARP(ptData, 1);
	
	write(ptData->iFd, ptData->ptaPck, sizeof(TARPPacket));
	
	return 0;
}

int main(int argc, char **argv)
{
	int				ret, i;
	FILE			*pfOut;
	struct in_addr	sInAddr;
	TMemberData		tmData;

	if(Zero_Init(&tmData) == -1)
	{
		goto fail;
	}
	
	i=0;
	while(i<3)//only probe 3 times
	{
		Zero_GenIP(&tmData);
		ret = Zero_Probe(&tmData);
		if(ret == 1)
		{
			++i;
			printf("Meet conflict\n");
		}
		else if(ret == -1)
		{
			printf("Probe error\n");
			goto fail;
		}
		else//ret == 0
		{
			//printf("Probe OK\n");
			break;
		}
	}
	
	ret = Zero_Announce(&tmData);
	if(ret == -1)
	{
		printf("Announce error\n");
		goto fail;
	}

	pfOut = fopen("/etc/conf.d/LinkLocalAddr", "wb");
	if(pfOut == NULL)
	{
		printf("Open file fail\n");
		goto fail;
	}
	if(i<3)
	{
		sInAddr.s_addr = tmData.ulIP;
		fprintf(pfOut, "%s\n", inet_ntoa(sInAddr));
	}
	else
	{
		fprintf(pfOut, "0.0.0.0\n");
	}
	fclose(pfOut);
	
	Zero_Release(&tmData);
	return 0;

fail:
	Zero_Release(&tmData);
	return 1;
}

