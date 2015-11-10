/*
* author 赵雨
* 2012/05/21
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFSIZE 9 // 缓冲池大小 

struct queue_type     //缓冲池队列 
{
	int buff[BUFFSIZE];
	int front;	//队头，消费者从对头取出"产品” 
	int rear;	//队尾，生产者从队尾放入"产品“ 
}Q={{0},0,0};		

sem_t empty; // 生产者私有信号量，表示空缓冲区数目 。 
sem_t full;   //消费者私有变量，表示有产品的缓冲区数目。 
pthread_mutex_t mutex; // 公有信号量，用于实现对临界区互斥访问 

int producer_id = 0;   //生产者编号，初值为0 
int consumer_id = 0;   //消费者编号，初值为0 

/* 打印缓冲池情况*/
void print()
{
     int i;
     for(i = 0; i < BUFFSIZE; i++)
     printf("%d ", Q.buff[i]);
     printf("\n");
}

/*生产者*/
void *producer(void *)
{
     int id=++producer_id;

     while(1)
      {
             sleep(1);	//		
  
             sem_wait(&empty);	//申请空缓冲区 
             pthread_mutex_lock(&mutex);	//申请队列互斥 

             Q.buff[Q.rear] = 1;   //将产品放入rear所指向的缓冲区 
             printf("producer number<%d> thread idetifier:%lu put into buffer[%d].The buffer is like: \t", id, pthread_self(),Q.rear+1);
             print();  
             Q.rear = (Q.rear+1) % BUFFSIZE;
  
             pthread_mutex_unlock(&mutex);	//释放队列互斥 
             sem_post(&full);  	//释放满缓冲区 
      }
}

/* 消费者*/
void *consumer(void *)
{
     int id=++consumer_id;
     while(1)
      {
             sleep(1);

             sem_wait(&full);  //申请满缓冲区 
             pthread_mutex_lock(&mutex);     //申请队列互斥 
   
             Q.buff[Q.front] = 0;            //从front所指向的缓冲区取产品 
             printf("consumer number<%d> thread idetifier:%lu get from buffer[%d].The buffer is like: \t", id,pthread_self(), Q.front+1);
             print();
             Q.front = (Q.front+1) % BUFFSIZE;
  
             pthread_mutex_unlock(&mutex);    //释放队列互斥 
             sem_post(&empty);                //释放空缓冲区 
      }
}

int main()
{
    int M,N;  //M为生产者者数目，N为消费者数目 
    printf("please input the producers number: ");
    scanf("%d",&M);
    printf("please input the consumers number: ");
    scanf("%d",&N);
    pthread_t id1[M];		//存储生产者线程ID 
    pthread_t id2[N];		//存储消费者者线程ID 
    int i;
    int ret1[M],ret2[N];  //存储创建生产者和消费者线程的返回值。 


/*初始化empty和full私有信号量 */
    int ini1 = sem_init(&empty, 0, BUFFSIZE);//初始化empty信号量，大小为Buffersize ，初始化成功返回零，失败返回-1  
    int ini2 = sem_init(&full, 0, 0);        //初始化full信号量，大小为0                                 
    if((ini1 || ini2)!=0)	//
    {
             printf("sem init failed \n");
             exit(1);
    }

/*初始化公有信号量 mutex*/
    int ini3 = pthread_mutex_init(&mutex, NULL);	//以默认方式创建互斥锁（快速互斥锁） ，初始化为未锁状态，成功返回0 
    if(ini3 != 0)
    {
            printf("mutex init failed \n");
            exit(1);
    }
    
/*创建生产者线程*/
    for(i = 0; i < M; i++)
    {
            ret1[i] = pthread_create(&id1[i], NULL, &producer, (void *)(&i));	//循环创建生产者线程，成功返回0 
            if(ret1[i] != 0)
            {
               printf("producer%d creation failed \n", i);
               exit(1);
            }
    }
    
/*创建消费者线程*/ 
    for(i = 0; i < N; i++)
    {
            ret2[i] = pthread_create(&id2[i], NULL, &consumer, NULL);         //循环创建消费者线程，成功返回0 
            if(ret2[i] != 0)
            {
               printf("consumer%d creation failed \n", i);
               exit(1);
            }
    }
    
/*销毁线程*/
    for(i = 0; i < M; i++)
    {
            pthread_join(id1[i],NULL);   //对创建的生产者线程进行资源回收 
    }
    for(i = 0; i < N; i++)
    { 
            pthread_join(id2[i],NULL);   //对创建的消费者线程进行资源回收 
    }

    exit(0);
}
