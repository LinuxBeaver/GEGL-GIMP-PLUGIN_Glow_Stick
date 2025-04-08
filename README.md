# GEGL-Glowstick-color-and-lighting-filter
In this Gimp plugin GEGL does an awesome neon glow stick effect. 

![image preview](https://imgur.com/y1kSniY.png)
![image preview](https://imgur.com/hZSpRCq.png )
![image preview](https://imgur.com/DjUub0l.png )
![image preview](https://imgur.com/NgeX7r7.png )



## OS specific location to put GEGL Filter binaries 

Windows
 `C:\\Users\<YOUR NAME>\AppData\Local\gegl-0.4\plug-ins`
 
 Linux 
 `~.local/share/gegl-0.4/plug-ins`
 
 Linux (Flatpak)
`~/.var/app/org.gimp.GIMP/data/gegl-0.4/plug-ins`


## Compiling and Installing

### Linux

To compile and install you will need the GEGL header files (`libgegl-dev` on
Debian based distributions or `gegl` on Arch Linux) and meson (`meson` on
most distributions).

```bash
meson setup --buildtype=release build
ninja -C build

```

If you have an older version of gegl you may need to copy to `~/.local/share/gegl-0.3/plug-ins`
instead (on Ubuntu 18.04 for example).



### Windows

The easiest way to compile this project on Windows is by using msys2.  Download
and install it from here: https://www.msys2.org/
