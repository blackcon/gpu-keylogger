# gpu-keylogger

GPU Based keylogger
-------------------
* GPU 에서 작동되는 키로거의 PoC 코드이다.
* 2015년에 [You Can Type, but You Can’t Hide: A Stealthy GPU-based Keylogger](https://www3.cs.stonybrook.edu/~mikepo/papers/gpukeylogger.eurosec13.pdf)를 기반으로 제작한 GPU 기반 키로거 입니다.

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


* demo video
[![Video Label](https://play-tv.kakao.com/v/71853580)](https://play-tv.kakao.com/v/71853580)
