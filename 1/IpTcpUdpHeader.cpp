#include<stdio.h>
#include<malloc.h>
#define STACK_INIT_SIZE 100
#define STACKINCREMENT 10
#define OVERFLOW -2


typedef struct udp_hdr              //UDP头部 
{
            unsigned short sourceport;
            unsigned short destport;
            unsigned short totallength;
            unsigned short checksum;
}UDP;

typedef struct ip_hdr {                //IP头部 
	         unsigned char ver_hl;		//版本号和头部长度
	         unsigned char tos;			//服务类型
	         unsigned short totallength;	//总长度
	         unsigned short ident;		//16位标识
	         unsigned short frag_and_flags;	//标志位
	         unsigned char ttl;		//生存时间TTL
	         unsigned char proto;	//协议
	         unsigned short checksum;	//IP首部校验和
	         unsigned int sourceIP;	//源IP
	         unsigned int destIP;	//目的IP
}IP;

typedef struct tcp_hdr {			//TCP报文头部 
	         unsigned short sourceport;	//源端口
	         unsigned short destport;		//目的端口
	         unsigned int seq;			//序号
	         unsigned int ack;			//确认号
	         unsigned char hlen;        //首部长度和4位的保留字段 
	         unsigned char control;     //2位保留字段和6位控制字段 
	         unsigned short win;		//窗口大小
	         unsigned short checksum;		//校验和
	         unsigned short urp;		//紧急指针
}TCP;


main()                      
{
      unsigned char buff1[100]={0x45,0x00,0x05,0xdc,
                                0x00,0x14,0x20,0x00,
                                0x80,0x06,0x72,0xb7,
                                0xc0,0xa8,0x0b,0x02,
                                0xc0,0xa8,0x16,0x03,
                                0xcb,0x84,0x00,0x0d,
                                0x00,0x1c,0x00,0x1c};//UDP
      unsigned char buff2[100]={0x45,0x00,0x05,0xdc,
                                0x00,0x14,0x20,0x00,
                                0x80,0x06,0x72,0xb7,
                                0xc0,0xa8,0x0b,0x02,
                                0xc0,0xa8,0x16,0x03,
                                0x00,0x50,0x07,0x45,
                                0x9b,0xd6,0x43,0x3c,
                                0x47,0xfd,0x37,0x50,
                                0x50,0x18,0xff,0x1f,
                                0x05,0xa5,0x00,0x00};//TCP
      IP *ip;UDP *udp;TCP *tcp;
      ip=(IP *)buff1;udp=(UDP *)(buff1+20);tcp=(TCP *)(buff2+20);
      
      
      printf("the version of datagram is: %u\n",(*ip).ver_hl>>4);
      printf("the length of datagram's ip header is: %u bytes\n",(((*ip).ver_hl)&15)*4);
 //     printf("the sourceport of datagram is: %u\n",(*udp).sourceport);
       printf("the sourceport of datagram is: %u\n",(*udp).sourceport);                 //字节存储的问题 
       while(1);
      

}

         

