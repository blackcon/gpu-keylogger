GPU Based keylogger
-------------------
* GPU 메모리에서 실행되는 키로거의 PoC 코드이다.
* PDF: [GPU_based_keylogger.pdf](/gpu-based-keylogger.pdf)

DEMO
----
* enviroment:

        blackcon@bk:~$ lspci | grep -i nvidia
        01:00.0 VGA compatible controller: NVIDIA Corporation GF108M [GeForce GT 635M] (rev a1)

        blackcon@bk:~$ uname -a
        Linux bk 3.19.0-15-generic #15-Ubuntu SMP Thu Apr 16 23:32:37 UTC 2015 x86_64 x86_64 x86_64 GNU/Linux

        blackcon@bk:~$ lsb_release -a
        No LSB modules are available.
        Distributor ID: Ubuntu
        Description:    Ubuntu 15.04
        Release:        15.04
        Codename:       vivid

* demo (click img):  
[![Video Label](https://t1.daumcdn.net/thumb/C640x360.q50.fjpg/?fname=http://t1.daumcdn.net/tvpot/thumb/v0310aGWPaaqtdtaAEidUAW/thumb.png)](https://blackcon.tistory.com/136)
