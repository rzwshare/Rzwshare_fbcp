# Introduction
This repository forked from  [juj/fbcp-ili9341](https://github.com/juj/fbcp-ili9341)

This repository implements a driver for certain SPI-based LCD displays for Raspberry Pi A, B, 2, 3, 4 and Zero.

## What I do
1. sorted out the folders (At least it looks better)
2. add the ability to change screen size to fit more screens(Each controller may have different displays)
3. added most of the RzwShare LCD Hat support
## Tested Devices
The driver has been checked to work (at least some point in the past) on the following systems:
* Raspberry Pi 4 Model B with Raspbian Stretch (GCC 8.3.0)
* Raspberry Pi 3 Model B+ with Raspbian Stretch (GCC 6.3.0)
* Raspberry Pi 3 Model B Rev 1.2 with Raspbian Jessie (GCC 4.9.2) and Raspbian Stretch (GCC 6.3.0)
* Raspberry Pi Zero W with Raspbian Jessie (GCC 4.9.2) and Raspbian Stretch (GCC 6.3.0)
* Raspberry Pi 2 Model B
* Raspberry Pi B Rev. 2.0 (old board from Q4 2012, board revision ID 000e)

although not all boards are actively tested on, so ymmv especially on older boards. (Bug fixes welcome, use [https://elinux.org/RPi_HardwareHistory](https://elinux.org/RPi_HardwareHistory) to identify which board you are running on)

## Tested Displays
* [RZWSHARE_1INCH4_LCD]with ST7789V2 controller
* [WAVESHARE_3INCH5_LCD_HAT]with ST7796U controller

## How to use
### Installation
Check the following sections to set up the driver.
#### Boot configuratio
This driver does not utilize the [notro/fbtft](https://github.com/notro/fbtft) framebuffer driver, so that needs to be disabled if active. That is, if your `/boot/config.txt` file has lines that look something like `dtoverlay=pitft28r, ...`, `dtoverlay=rzwshare32b, ...` or `dtoverlay=flexfb, ...`, those should be removed.

This program neither utilizes the default SPI driver, so a line such as `dtparam=spi=on` in `/boot/config.txt` should also be removed so that it will not cause conflicts.

Likewise, if you have any touch controller related dtoverlays active, such as `dtoverlay=ads7846,...` or anything that has a `penirq=` directive, those should be removed as well to avoid conflicts. It would be possible to add touch support to fbcp-ili9341 if someone wants to take a stab at it.


#### Building and running
Run in the console of your Raspberry Pi:
```
cd ~
sudo apt-get install cmake -y
git clone https://github.com/liuqing925/rzwshare.git
cd Rzwshare_fbcp
mkdir build
cd build 
cmake [options] ..
make -j
sudo ./fbcp
```
If you are in China, you can use the following command
```
cd ~
sudo apt-get install cmake -y
sudo apt-get install p7zip-full -y
sudo cp /boot/Rzwshare_fbcp.7z ./
7z x Rzwshare_fbcp.7z
cd Rzwshare_fbcp
mkdir build
cd build
cmake [options] ..
make -j
sudo ./fbcp
```
Note especially the two dots `..` on the CMake line, which denote "up one directory" in this case (instead of referring to "more items go here").

See the next section to see what to input under **[options]**.

If you have been running existing `fbcp` driver, make sure to remove that e.g. via a `sudo pkill fbcp` first (while running in SSH prompt or connected to a HDMI display), these two cannot run at the same time. If `/etc/rc.local` or `/etc/init.d` contains an entry to start up `fbcp` at boot, that directive should be deleted.

##### Configuring build options
If you are using the LCD module of RzwShare, you can use the following commands(I have adapted it for you)
```
#ST7789V2
cmake -DSPI_BUS_CLOCK_DIVISOR=20 -DWAVESHARE_1INCH4_LCD=ON -DBACKLIGHT_CONTROL=ON -DSTATISTICS=0 ..

#ST7796U
cmake -DSPI_BUS_CLOCK_DIVISOR=20 -DWAVESHARE_3INCH15_LCD=ON -DBACKLIGHT_CONTROL=ON -DSTATISTICS=0 ..

#IL9341
cmake -DSPI_BUS_CLOCK_DIVISOR=20 -DWAVESHARE_2INCH4_LCD=ON -DBACKLIGHT_CONTROL=ON -DSTATISTICS=0 ..
```
Other**[options]** You can check out [juj/fbcp-ili9341](https://github.com/juj/fbcp-ili9341) for help.
### License

This driver is licensed under the MIT License. See LICENSE.txt. In nonlegal terms, it's yours for both free and commercial projects, DIY packages, kickstarters, Etsys and Ebays, and you don't owe back a dime. Feel free to apply and derive as you wish.

If you found Rzwshare-fbcp useful, it makes me happy to hear back about the projects it found a home in. If you did a build or a project where Rzwshare-fbcp worked out, it'd be great to see a video or some photos or read about your experiences.

I hope you build something you enjoy!

