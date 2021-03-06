#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <npheap.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int i=0,number_of_processes = 1, number_of_objects=1024, max_size_of_objects = 8192 ,j; 
    int a;
    int pid = 1;
    int size = 521;
    char data[8192];
    char filename[256];
    char *mapped_data;
    int devfd;
    unsigned long long msec_time;
    FILE *fp;
    struct timeval current_time;
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s number_of_objects max_size_of_objects number_of_processes\n",argv[0]);
        exit(1);
    }
    number_of_objects = atoi(argv[1]);
    max_size_of_objects = atoi(argv[2]);
    number_of_processes = atoi(argv[3]);
    devfd = open("/dev/npheap",O_RDWR);
    if(devfd < 0)
    {
        fprintf(stderr, "Device open failed");
        exit(1);
    }
    // test code
    /*
    mapped_data = (char *)npheap_alloc(devfd,10,size);
    printf("mapping offset %d to %p\n", i, mapped_data);
    sprintf(mapped_data, "test");
    char *mapped_data2;
    mapped_data2 = (char *)npheap_alloc(devfd,10,size);
    printf("mapping offset %d to %p, content %s\n", i, mapped_data2, mapped_data2);
    
    for (i = 0; i < number_of_objects; i++) {
        npheap_lock(devfd, i);
        //size = npheap_getsize(devfd, i);
        size = 10;
        mapped_data = npheap_alloc(devfd, i, size);
        sprintf(mapped_data, "npheap%d", i);
        //printf("testing lock at %d, size %d, at %s\n", i, size, mapped_data);
        npheap_unlock(devfd,i);
    }
    for (i = 0; i < number_of_objects; i++) {
        npheap_lock(devfd, i);
        //size = npheap_getsize(devfd, i);
        size = 10;
        mapped_data = npheap_alloc(devfd, i, size);
        printf("testing lock at %d, size %d, at %s\n", i, size, mapped_data);
        npheap_unlock(devfd,i);
    }*/
    //npheap_unlock(devfd, 0);
#ifndef ANY
   
    // Writing to objects
    for(i=0;i<(number_of_processes-1) && pid != 0;i++)
    {
        pid=fork();
        srand((int)time(NULL)+(int)getpid());
    }
    sprintf(filename,"npheap.%d.log",(int)getpid());
    fp = fopen(filename,"w");
    for(i = 0; i < number_of_objects; i++)
    {
        npheap_lock(devfd,i);
        size = npheap_getsize(devfd,i);
        
        while(size ==0 || size <= 10)
        {
            size = rand() % max_size_of_objects;
        }
        mapped_data = (char *)npheap_alloc(devfd,i,size);
        //mapped_data = (char *)malloc(size);
        if(!mapped_data)
        {
            fprintf(stderr,"Failed in npheap_alloc()\n");
            exit(1);
        }
        memset(mapped_data, 0, size);
        a = rand()+1;
        gettimeofday(&current_time, NULL);
        //sprintf(mapped_data, "pid %d i %d", (int)getpid(), i);
        //fprintf(fp, "process: %d, write %s\n", (int)getpid(), mapped_data);
        
        for(j = 0; j < size-10; j=strlen(mapped_data))
        {
            sprintf(mapped_data,"%s%d",mapped_data,a);
        }
        //printf("mapped data %s\n", mapped_data);
        fprintf(fp,"S\t%d\t%ld\t%d\t%lu\t%s\n",pid,current_time.tv_sec * 1000000 + current_time.tv_usec,i,strlen(mapped_data),mapped_data);
        npheap_unlock(devfd,i);
        //printf("finish %d pid :%d\n", i, (int)getpid());
    }
    
#endif
/*
    // try delete something
    i = rand()%256;
    npheap_lock(devfd,i);
    npheap_delete(devfd,i);
    fprintf(fp,"D\t%d\t%ld\t%d\t%lu\t%s\n",pid,current_time.tv_sec * 1000000 + current_time.tv_usec,i,strlen(mapped_data),mapped_data);
    npheap_unlock(devfd,i);
    */
    close(devfd);
    if(pid != 0)
        wait(NULL);
    //close(devfd);
    return 0;
}

