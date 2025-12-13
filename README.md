# WatchGuard Firebox M270

Got an unit for approximately 80 euros by end of 2025.

It did work, but with a proprietary and licensed/paid that I did not like.

## Current status

### System memory

The hardware supports single channel DDR4 SODIMM with ECC, unbuffered.

Original unit had 4GB ADATA ECC module.

Swapped it with 32GB Samsung ECC. I feel the pre-boot post time had increased and the first POST code on serial port appears after 10+ seconds.

The memory speed is limited by SoC to 2133 MT/s.

### mSATA storage media

Original unit had 16GB mSATA SSD from Transcend.

Swapped it with 32GB Samsung for my Gentoo installation.

While the SATA port negotiates at 6Gb/s (SATA-3 speed), the performance is under 300MB/s.

There is an unpopulated 7-pin secondary SATA port on the board, and the BIOS looks like knows about the second port.

### MiniPCIe ports

There are 2 unpopulated miniPCIe positions, and lspci does show empty ports:
```
# lspci -tvnn
-[0000:00]-+-00.0  Intel Corporation Atom Processor C3000 Series System Agent [8086:1980]
           +-04.0  Intel Corporation Atom Processor C3000 Series Error Registers [8086:19a1]
           +-05.0  Intel Corporation Atom Processor C3000 Series Root Complex Event Collector [8086:19a2]
           +-06.0-[01]----00.0  Intel Corporation Atom Processor C3000 Series QuickAssist Technology [8086:19e2]
           +-0e.0-[02]--
           +-0f.0-[03]--
           +-10.0-[04]--
           +-11.0-[05]--
           +-12.0  Intel Corporation Atom Processor C3000 Series SMBus Contoller - Host [8086:19ac]
           +-14.0  Intel Corporation Atom Processor C3000 Series SATA Controller 1 [8086:19c2]
           +-15.0  Intel Corporation Atom Processor C3000 Series USB 3.0 xHCI Controller [8086:19d0]
           +-16.0-[06]--+-00.0  Intel Corporation Ethernet Connection X553 Backplane [8086:15c2]
           |            \-00.1  Intel Corporation Ethernet Connection X553 Backplane [8086:15c2]
           +-18.0  Intel Corporation Atom Processor C3000 Series ME HECI 1 [8086:19d3]
           +-1a.0  Intel Corporation Atom Processor C3000 Series HSUART Controller [8086:19d8]
           +-1a.1  Intel Corporation Atom Processor C3000 Series HSUART Controller [8086:19d8]
           +-1a.2  Intel Corporation Atom Processor C3000 Series HSUART Controller [8086:19d8]
           +-1f.0  Intel Corporation Atom Processor C3000 Series LPC or eSPI [8086:19dc]
           +-1f.2  Intel Corporation Atom Processor C3000 Series Power Management Controller [8086:19de]
           +-1f.4  Intel Corporation Atom Processor C3000 Series SMBus controller [8086:19df]
           \-1f.5  Intel Corporation Atom Processor C3000 Series SPI Controller [8086:19e0]

```

### USB3 ports

There are two USB3.0 ports on the front panel. These were used during initial attepts with a USB flash drive and a USB network adapter.

As the unit has no video output nor a graphic card, the keyboard/mouse usage is not required.

### Serial port console

The unit has a RJ-45 port (Cisco style) for RS-232. The passive switch management serial cable works, and I use a 9-pin to USB adapter with FTDI chip.

The BIOS boots with serial port redirection, 115200-8N1 is a working setting.

### BIOS

The BIOS had a admin password set. The password can be removed by security settings in the BIOS.

BIOS enter code is TAB or DEL. There is a wait period for this, which I reduced from 5 sec to 1 sec in order to have the box boot faster, with less delay.

The BIOS supports both Legacy and UEFI boot. I switched to UEFI mode.

### Linux OS

I am doing my attempts with Gentoo Linux (x86-64 / amd64), as per ease of development in both userspace and for kernel modules.

A portable installation on USB media was used for initial boot, the BOOT order can be set in the BIOS, but the USB boot requires an additional setting explicit to this to be enabled.

The system starts upon AC power being connected as per the BIOS setting to restore operation on power loss.

### Power

The system consumes on average 10W, with peaks to 14W during post and pre-boot.

Both the front and back-side has a ATX style power button, thus the device can not be fully powered down. The system will power down to standby with a long press (4+ sec) on either of these buttons. The front power button will be lit green during ON state, while being red during OFF state.

The AC input goes to a small passive PSU that produces 12V.

The 12V goes by a screw locked DC jack to the mainboard.

### Status indicators

The unit has 3 dedicated LEDs: power, guard, disk activity, while the fourth indicator is hidden in the power button. The Power LED is green when the unit is ON and the Disk activity flashes ORANGE with the mSATA SSD access.

The GUARD logo marked LED is a GPIO controlled bi-color diode that is by default RED upon boot, but can be changed between RED/GREEN/OFF using the tool I made. The nature of the 2-pin bicolor anti-parallel LED connected to two GPIO signals means we can not lit up both diodes to produce orange/amber color. Also a side-effect is that the green setting is darker and red is more intense - since both leds share a single series resistor, and the larger voltage drop on green means less current.

### Front panel button

The button marked RESET is no resetting the system, it is just a GPIO.

I am able to read the state of the button by a polling method with my userspace tool.

### Network ports

The eight 1GbE RJ-45 ports are connected into a 11-port Marvell 88E6190 switch.

Two 2.5Gb/s serdes connections from the 88E6190 are connected to the Intel SoC NIC. One port is unused.

Linux shall support this architecture with the DSA feature, but we need to instatiate the DSA switch "manually" through a kernel module that finds the MDIO bus of the ixgbe driver that handles the SoC NICs, creates a MDIO device and expect the rest to be handled by Linux DSA.

_This part is WiP at this time. Ports are seen, but setup is slow and no data flows._

Initial development can be done with help of an USB-to-RJ45 dongle (USB NIC).

ref: https://wiki.freebsd.org/NetworkFirewalls/WatchguardFireboxM270

### Fan

The unit has a single 40mm fan that spins at various speeds during the post/pre-boot and boot/runtime.
This way one can tell when the memory training had completed.

I have replaced the noisy fan with a Noctua NF-A4x20 fan.
The two screws that hold the fan are different than for the original fan.
The zip-tie that held the original longer cable was cut and thrown away.

The mainboard has a non-standard 5-pin connector for the fan.
I have used an off the shelf 5-pin housing,
pulled the Noctua fan wires from its housing
and inserted it to the new housing.
The cable length of NF-A4x20 is just right for the spot where the connector is.

The wiring between 4 and 5 pin is simple all wires are in order,
but there is a free spot between the TACHO and PWM signals:

| Pin | Signal | Original fan | Noctua fan |
| --- | ---  | ---    | ---    |
| 1 | GND    | black  | black  |
| 2 | +12V   | red    | yellow |
| 3 | TACHO  | yellow | green  |
| 4 | -      | -      | -      |
| 5 | RPM    | blue   | blue   |

The fan RPM can be seen in the bios, and there is also a setting for manual PWM.

The PWM of the fan can be controlled by lm-sensors and fancontrol.
A quiet fancontrol profile is included in this repo for the Noctua fan.
The temperature sense for this comes from the CPU package temperature.

Overall, with 10W total system power consumption observed the CPU barely gets hot.
But having some airflow helps to keep the temperature below 40°C.

### Intel QAT - QuickAssist Technology

Module is loaded but its use for crypto not verified

Some sources say, the QAT does not accelerate the cipher used in WireGuard VPN?

### RTC

There is a typical replaceable CR2032 coin cell battery located on the mainboard.

My unit still has 3V on it.


## Hardware

The system is basically a 64-bit X86 PC system without a graphic card.

Mainboard looks like a customized solution from external vendor (Lanner):

```
 NCB-WG2511 v.0.5
 NCB-WG2511A v.0.51 - A and 1 are handwritten revision/minor-versions
```

The original system specs are:

```
Intel(R) Atom(TM) CPU C3558 @ 2.20GHz
4C/4T, 64-bit, no AVX2

4GB DDR4 SODIMM ECC unbuffered
Adata 2400 MT/s memory running at 2133 MT/s

MARVELL MV88E6190 switch
2x 2.5G uplink to SoC
8x 1GbE RJ-45 to external connections

Nuvoton NCT6776 Super I/O
(nct6775 driver)
```


## Original software

Original software my device came with was using a heavily patched Linux kernel (4.14.140). There are some custom modules in the **/lib/drivers** directory.

With propietary license
```
alarm.ko
wgapi.ko
wgipc.ko
wglog.ko
```

With GPL / GPLv2 / dual BSD/GPL license:
```
sled_drv.ko
sled_drv-m270.ko
sled_drv-m370-670.ko
sled_drv-m400.ko
sled_drv-m440.ko
sled_drv-m4600.ko
sled_drv-m5600.ko
sled_drv-t55.ko
sled_drv-t70.ko
wg_cryptopost.ko
wg_dsa.ko
wg_ethnull.ko
wg_pkttest.ko
wg_pss.ko
wg_queue.ko
wgrdb.ko
wg_sierra.ko
wg_sierra_net.ko
wgworkqueue.ko
wg_x86-rng.ko
xt_wgaccount.ko
xt_wglog.ko
```

While these have a GPL license according to **modinfo**:
```
$ modinfo wg_dsa.ko
license:        GPL
depends:        dsa_core
retpoline:      Y
intree:         Y
name:           wg_dsa
vermagic:       4.14.140 SMP mod_unload
parm:           split_mode:Split_Mode -1=default (int)
```
 This GPL code was never released by WatchGuard - not for the extra modules nor for the kernel modifications (extra exported functions that the modules then rely on).


# Other links

* FreeBSD / NetworkFirewalls / WatchguardFireboxM270



  * https://wiki.freebsd.org/NetworkFirewalls/WatchguardFireboxM270

* pfSense on Watchguard M270

  * https://forum.netgate.com/topic/154974/pfsense-on-watchguard-m270/3

* Watchguard M270 Ethernet Ports

  * https://www.reddit.com/r/linuxhardware/comments/10ppldo/watchguard_m270_ethernet_ports/