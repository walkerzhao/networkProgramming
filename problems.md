# 几道网络编程题目
## 1. 编程实现IP/TCP/UDP的报文格式,添加头部和解析头部操作
### content:编程实现 IP,TCP,UDP 报文格式。实现报文添加头部、取出头部等基本操作。编程实现网络抓包文件解析软件。网络抓包文件可采用 wireshark 等抓包软件获取。从文件的二进制流中确认网络报文。并解析输出。
### key:TCP/UDP/IP linux 数据结构 位操作
### 我的博客解析:http://blog.csdn.net/zy416548283/article/details/48766873

## 2.linux下实现生产者和消费者
### content:生产者-消费者问题描述:有一群生产者进程在生产消息,并将此 消息提供给消费者进程去消费。为使生产者进程和消费者进程能 并发进行,在他们之间设置了一个具有 N 个缓冲区的缓冲池,生 产者进程可以将它所生产的消息放入一个缓冲区中,消费者进程 可以从一个缓冲区中取得一个消息消费。
### key:linux provider consumer queue
### 我的博客解析：http://blog.csdn.net/zy416548283/article/details/8268061

## 3. 基本的TCP/UDP echo服务器
### content:分别实现ECHO服务器和客户端（客户端发送字符串消息到服务器端，服务器接收到消息之后，返回给客户端同样地消息），基于TCP和UDP。
### key:linux echo Server Client 健壮性
### 我的博客解析：http://blog.csdn.net/zy416548283/article/details/50225633

## 4.编程实现UDP的聊天室
### content:见博客：http://blog.csdn.net/zy416548283/article/details/49760351
### key:udp socket 报文设计 select 测试

## 5. 实现FTP基本的功能


## 6. 使用原始套接字实现Ping基本功能
### content:见博客:http://blog.csdn.net/zy416548283/article/details/50327321
### key:原始套接字 ICMP PING

## 7. 编程实现Traceroute基本功能
### content:见博客:http://blog.csdn.net/zy416548283/article/details/50354530
### key: 原始套接字 ICMP Traceroute TTL

## 8. 多播协议的应用

## 9. select和epoll区别

## 10. 熟悉Linux内核、了解驱动开发基础
