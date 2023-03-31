# CHIP-8

## TR

Bu proje 8 bitlik bir işlemci öykünücünün nasıl çalıştığını öğrenmek için yapılmıştır ve olabildiğince sade, basit şekilde oluşturulmaya özen gösterilmiştir.

Muadillerinin aksine herhangi bir ekstra kurulması gereken kütüphane kullanılmamaya çalışılmıştır bu sebepten doğrudan xlib'i kullanan bir grafik kütüphanesi tercih edilmiştir. 

Grafik kütüphanesinin orijinal deposuna aşağıdaki linkten ulaşılabilir.

https://github.com/dthain/gfx


Proje boyunca takip edilen ana rehber: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

Yararlanılan çeşitli kaynaklar:

* https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
* https://github.com/arnsa/Chip-8-Emulator/blob/master/chip8.c
* https://cs.baylor.edu/~donahoo/tools/gdb/tutorial.html
* https://chip-8.github.io/links/
* http://chip8emulator.azurewebsites.net/CHIP8.html
* https://github.com/loktar00/chip8/tree/master/roms
* https://chipgr8.io/index

### Derleme Adımları

`make all` ile proje derlenebilir ve `./chip8` ile çalıştırılabilir.

Proje, geliştirilmeye devam edilmektedir ve çizim buyruğu ile klavye girdileri konusunda hatalara sahiptir.

***

## EN

This project was created to learn how an 8-bit processor emulator works and was designed to be as simple as possible. Unlike its counterparts, an attempt was made not to use any extra libraries that need to be installed, so a graphics library that directly uses ,xlib was preferred.

The original repository of the graphics library can be found at the link below:

https://github.com/dthain/gfx

The main guide followed throughout the project: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

Various resources used:

* https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
* https://github.com/arnsa/Chip-8-Emulator/blob/master/chip8.c
* https://cs.baylor.edu/~donahoo/tools/gdb/tutorial.html
* https://chip-8.github.io/links/
* http://chip8emulator.azurewebsites.net/CHIP8.html
* https://github.com/loktar00/chip8/tree/master/roms
* https://chipgr8.io/index

Compilation Steps:
The project can be compiled with `make all` and can be run with `./chip8`.

The project is still being developed and has errors regarding drawing commands and keyboard inputs.