# Serial port

## BIOS

The BIOS will use a **115200-8N1** setting.

TAB or DEL enters the BIOS.

There is a password, but it can be removed.


## GRUB2

Will use the BIOS settings with the console module:

```
GRUB_TERMINAL=console
```

Found no need to use a specific serial config, which would be about this:

```
#GRUB_TERMINAL=serial
#GRUB_SERIAL_COMMAND="serial --unit=0 --speed=115200 --word=8 --parity=no --stop=1"
```


## KERNEL

The command line has to be extended with:

```
console=ttyS0,115200
```


## SHELL

The terminal session can be enabled in **/etc/inittab** with:
```
s0:12345:respawn:/sbin/agetty -L 115200 ttyS0 vt100
```
