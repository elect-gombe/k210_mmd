# MMD test for k210

## how to
Using latest toolchain(development branch) and standalone sdk to compile and build, and then use kflash.py to flash.


**Makefile is written for PC testing, not for K210**


model file name is `model.pmd`, make sure that texture images must be `RGB565 format` bmp file.


motion file name is `motion.vmd`.

All these file are placed in root of your SD card.


### test files.
[初音ミク@七葉1052式 Ver20090309(MMD model file)](https://bowlroll.net/file/1568)
each texture file is converted by using ffmpeg.
[wavefile (MMD motion file)](https://bowlroll.net/file/5983)


### convert your texture bmp file to `RGB565` format
BMP file must be RGB565 color format, you can convert bmp file by using ffmpeg.
```
$ ffmpeg -vcodec bmp -i <input>.bmp -vcodec bmp -pix_fmt rgb565 <texturename>.bmp
```


## License
My source code is distributed under the MIT license.
Other file is distributed under each license, see each source code for detail.

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

note that performance is also depends on your model's ik bone count.