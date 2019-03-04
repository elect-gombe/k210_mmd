# MMD test

## how to
Using latest toolchain(development branch) and standalone sdk to compile and build, and then use kflash.py to flash.


## License
MMD model which this project use is under the [CC BY-NC License](https://piapro.net/intl/en_for_creators.html)


You are allowed to use this MMD model only if you follow the guideline
set by Crypton Future Media, INC. for the usage of its characters.
For detail, see: http://piapro.net/en_for_creators.html


**Do not use this model for commercial use**


Other source files distributed under the MIT license.


## performance test result
### environment
2450 triangles, window size is 320x240
### esp32, dual core, 240MHz(on screen, but it's almost cpu limit, reference only)
22fps, 54ktps(K triangles per seconds; 22*2450=54k)
### k210, single core, 400MHz(off screen test)
45fps,110ktps
### k210, dual core, 400MHz(off screen)
71fps, 173ktps
### k210, dual core, 600MHz(Off screen, overvoltage)
107fps, 262ktps
### k210(Onscreen, overvoltage)
100fps, 245ktps
