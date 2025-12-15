# WatchGuard Firebox M270

Got an unit for approximately 80 euros by end of 2025.

It did work, but with a proprietary and licensed/paid SW that I did not like at all.


## Current status

### System memory

The hardware supports single channel DDR4 SODIMM with ECC, unbuffered.

Original unit had 4GB ADATA ECC module (2400 MT/s).

Swapped it with 32GB Samsung ECC. I feel the pre-boot post time had increased and the first POST code on serial port appears after 10+ seconds.

The memory speed is limited by SoC to 2133 MT/s.

State of the system with my RAM upgrade using `dmidecode -t 16`:
```
# dmidecode 3.6
Getting SMBIOS data from sysfs.
SMBIOS 3.0.0 present.

Handle 0x0025, DMI type 16, 23 bytes
Physical Memory Array
        Location: System Board Or Motherboard
        Use: System Memory
        Error Correction Type: Single-bit ECC
        Maximum Capacity: 128 GB
        Error Information Handle: Not Provided
        Number Of Devices: 4
```

And the one (and only possible) SO-DIMM socket populated visible at `dmidecode -t 17` (other's output was removed for clarity):
```
# dmidecode 3.6
Getting SMBIOS data from sysfs.
SMBIOS 3.0.0 present.

Handle 0x0027, DMI type 17, 40 bytes
Memory Device
        Array Handle: 0x0025
        Error Information Handle: Not Provided
        Total Width: 72 bits
        Data Width: 72 bits
        Size: 32 GB
        Form Factor: DIMM
        Set: None
        Locator: DIMM0
        Bank Locator: BANK 0
        Type: DDR4
        Type Detail: Synchronous Unbuffered (Unregistered)
        Speed: 2666 MT/s
        Manufacturer: SK Hynix
        Serial Number: 53A1D52E
        Asset Tag: BANK 0 DIMM0 AssetTag
        Part Number: HMAA4GS7AJR8N-WM
        Rank: 2
        Configured Memory Speed: 2133 MT/s
        Minimum Voltage: 1.2 V
        Maximum Voltage: 1.2 V
        Configured Voltage: 1.2 V

Handle 0x0029, DMI type 17, 40 bytes
Memory Device
        Array Handle: 0x0025
        Size: No Module Installed

Handle 0x002A, DMI type 17, 40 bytes
Memory Device
        Array Handle: 0x0025
        Size: No Module Installed

Handle 0x002B, DMI type 17, 40 bytes
Memory Device
        Array Handle: 0x0025
        Size: No Module Installed
```


### mSATA storage media

Original unit had 16GB mSATA SSD from Transcend.

Swapped it with 32GB Samsung for my Gentoo installation,
which was deemed OK for the development/porting phase.

While the SATA port negotiates at 6Gb/s (SATA-3 speed), the performance is under 300MB/s.

There is an unpopulated 7-pin secondary SATA port on the board, and the BIOS looks like knows about the second port.


### MiniPCIe ports

There are 2 unpopulated miniPCIe positions, and `lspci -tvnn` does show empty ports:
```
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

Using the USB NIC, I can see in `lsusb`:
```
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 002 Device 002: ID 0b95:1790 ASIX Electronics Corp. AX88179 Gigabit Ethernet
```
and as a tree with `lsusb -t`:
```
/:  Bus 001.Port 001: Dev 001, Class=root_hub, Driver=xhci_hcd/4p, 480M
/:  Bus 002.Port 001: Dev 001, Class=root_hub, Driver=xhci_hcd/4p, 5000M
    |__ Port 003: Dev 002, If 0, Class=Vendor Specific Class, Driver=ax88179_178a, 5000M
```


### Serial port console

The unit has a RJ-45 port (Cisco style) for RS-232. The passive switch management serial cable works, and I use a 9-pin to USB adapter with FTDI chip.

The BIOS boots with serial port redirection, 115200-8N1 is a working setting.

I use **minicom** as a terminal client, and using the USB adapter, it can be run as:
```
$ minicom -D /dev/ttyUSB0 -b 115200
```

Further configuration aspects are shown in detail within the
[Serial port](serial/serial-port.md) documenation.

### BIOS

The BIOS had a admin password set. The password can be removed by security settings in the BIOS.

BIOS enter code is TAB or DEL. There is a wait period for this, which I reduced from 5 sec to 1 sec in order to have the box boot faster, with less delay.

The BIOS supports both Legacy and UEFI boot. I switched to UEFI mode.

The ACPI tables and DMI is very poor (I can hardly imagine it being worse).
The BIOS was not customized for the board at all.
Using **dmidecode** we are unable to discover the machine/board model, and the physical port/connector list is way off, either a generic one or made for other, desktop like, mainboards.

Brief output from `dmidecode -t 0 -t 1`:
```
# dmidecode 3.6
Getting SMBIOS data from sysfs.
SMBIOS 3.0.0 present.

Handle 0x0000, DMI type 0, 24 bytes
BIOS Information
        Vendor: American Megatrends Inc.
        Version: 5.13
        Release Date: 01/16/2019
        Address: 0xF0000
        Runtime Size: 64 kB
        ROM Size: 8 MB
        Characteristics:
                PCI is supported
                BIOS is upgradeable
                BIOS shadowing is allowed
                Boot from CD is supported
                Selectable boot is supported
                BIOS ROM is socketed
                EDD is supported
                5.25"/1.2 MB floppy services are supported (int 13h)
                3.5"/720 kB floppy services are supported (int 13h)
                3.5"/2.88 MB floppy services are supported (int 13h)
                Print screen service is supported (int 5h)
                Serial services are supported (int 14h)
                Printer services are supported (int 17h)
                ACPI is supported
                USB legacy is supported
                BIOS boot specification is supported
                Targeted content distribution is supported
                UEFI is supported
        BIOS Revision: 5.13

Handle 0x0001, DMI type 1, 27 bytes
System Information
        Manufacturer: Default string
        Product Name: Default string
        Version: Default string
        Serial Number: Default string
        UUID: 03000200-0400-0500-0006-000700080009
        Wake-up Type: Power Switch
        SKU Number: Default string
        Family: Default string
```


### Linux OS

I am doing my attempts with Gentoo Linux (x86-64 / amd64), as per ease of development in both userspace and for kernel modules.

A portable installation on USB media was used for initial boot, the BOOT order can be set in the BIOS, but the USB boot requires an additional setting explicit to this to be enabled.


### Power

The system consumes on average 10W, with peaks to 14W during post and pre-boot.

Both the front and back-side has a ATX style power button, thus the device can not be fully powered down. The system will power down to standby with a long press (4+ sec) on either of these buttons. The front power button will be lit green during ON state, while being red during OFF state.

The AC input goes to a small passive PSU that produces 12V.

The 12V goes by a screw locked DC jack to the mainboard.

The power button does work for graceful shutdown of the machine when ACPI daemon is used:
```
# emerge -av sys-power/acpid
# rc-update add acpid default
# /etc/init.d/acpid start
```

The system starts upon AC power being connected as per the BIOS setting to restore operation on power loss.


### Status indicators

The unit has 3 dedicated LEDs: power, guard, disk activity, while the fourth indicator is hidden in the power button. The Power LED is green when the unit is ON and the Disk activity flashes ORANGE with the mSATA SSD access.

The GUARD logo marked LED is a GPIO controlled bi-color diode that is by default RED upon boot, but can be changed between RED/GREEN/OFF using the tool I made. The nature of the 2-pin bicolor anti-parallel LED connected to two GPIO signals means we can not lit up both diodes to produce orange/amber color. Also a side-effect is that the green setting is darker and red is more intense - since both leds share a single series resistor, and the larger voltage drop on green means less current.

```
# afpc red
button.reset=0
led.guard=red

# afpc green
button.reset=0
led.guard=green

# afpc off
button.reset=0
led.guard=off
```


### Front panel button

The button marked RESET is no resetting the system, it is just a GPIO.

I am able to read the state of the button by a polling method with my userspace tool.

```
# afpc
button.reset=0
led.guard=green

# afpc
button.reset=1
led.guard=green
```


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

After installing the [config](fan/fancontrol/noctua-nf-a4x20-pwm) as **/etc/fancontrol** file, the daemon can be activated:
```
# rc-update add fancontrol default
# /etc/init.d/fancontrol start
```

Another option is to go through customization using the **pwmconfig** tool.

Overall, with 10W total system power consumption observed the CPU barely gets hot.
But having some airflow helps to keep the temperature below 40°C.


### Intel QAT - QuickAssist Technology

Module is loaded but its use for crypto not verified.

Some sources say, the QAT does not accelerate the cipher used in WireGuard VPN?

ref: https://forum.netgate.com/topic/173606/solved-intel-qat-driver-with-wireguard-support/10


### RTC

There is a typical replaceable CR2032 coin cell battery located on the mainboard.

My unit still has 3V on it.


### Sensors

The **lm-sensors** package does work and after detection of the Super I/O
we can obtain further information.

Some channels have missing names in `sensors`:

```
nct6776-isa-0a10
Adapter: ISA adapter
Vcore:           1.01 V  (min =  +0.00 V, max =  +1.74 V)
in1:             1.02 V  (min =  +0.00 V, max =  +0.00 V)  ALARM
AVCC:            3.34 V  (min =  +0.00 V, max =  +0.00 V)  ALARM
+3.3V:           3.34 V  (min =  +0.00 V, max =  +0.00 V)  ALARM
in4:             1.01 V  (min =  +0.00 V, max =  +0.00 V)  ALARM
in5:             1.21 V  (min =  +0.00 V, max =  +0.00 V)  ALARM
in6:           264.00 mV (min =  +0.00 V, max =  +0.00 V)  ALARM
3VSB:            3.34 V  (min =  +0.00 V, max =  +0.00 V)  ALARM
Vbat:            3.10 V  (min =  +0.00 V, max =  +0.00 V)  ALARM
fan1:          2571 RPM  (min =    0 RPM)
fan2:             0 RPM  (min =    0 RPM)
SYSTIN:         +26.0°C  (high =  +0.0°C, hyst =  +0.0°C)  ALARM  sensor = CPU diode
CPUTIN:         +34.0°C  (high = +80.0°C, hyst = +75.0°C)  sensor = CPU diode
AUXTIN:         +29.0°C  (high = +80.0°C, hyst = +75.0°C)  sensor = CPU diode
PCH_CHIP_TEMP:   +0.0°C
PCH_CPU_TEMP:    +0.0°C
PCH_MCH_TEMP:    +0.0°C
pwm1:                0%  (mode = dc)  MANUAL CONTROL
pwm2:               57%  (mode = pwm)  MANUAL CONTROL
cpu0_vid:      +0.000 V
intrusion0:    ALARM
intrusion1:    ALARM
beep_enable:   enabled

acpitz-acpi-0
Adapter: ACPI interface
temp1:         +0.0°C

coretemp-isa-0000
Adapter: ISA adapter
Package id 0:  +42.0°C  (high = +71.0°C, crit = +91.0°C)
Core 2:        +40.0°C  (high = +71.0°C, crit = +91.0°C)
Core 6:        +39.0°C  (high = +71.0°C, crit = +91.0°C)
Core 8:        +39.0°C  (high = +71.0°C, crit = +91.0°C)
Core 12:       +40.0°C  (high = +71.0°C, crit = +91.0°C)
```


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

### Processor

With **lscpu** we can see CPU details:
```
Architecture:                x86_64
  CPU op-mode(s):            32-bit, 64-bit
  Address sizes:             39 bits physical, 48 bits virtual
  Byte Order:                Little Endian
CPU(s):                      4
  On-line CPU(s) list:       0-3
Vendor ID:                   GenuineIntel
  Model name:                Intel(R) Atom(TM) CPU C3558 @ 2.20GHz
    CPU family:              6
    Model:                   95
    Thread(s) per core:      1
    Core(s) per socket:      4
    Socket(s):               1
    Stepping:                1
    BogoMIPS:                4400.00
    Flags:                   fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pg
                             e mca cmov pat pse36 clflush dts acpi mmx fxsr sse
                             sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm cons
                             tant_tsc art arch_perfmon pebs bts rep_good nopl xt
                             opology nonstop_tsc cpuid aperfmperf tsc_known_freq
                              pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ss
                             se3 sdbg cx16 xtpr pdcm sse4_1 sse4_2 x2apic movbe
                             popcnt tsc_deadline_timer aes xsave rdrand lahf_lm
                             3dnowprefetch cpuid_fault epb cat_l2 ssbd ibrs ibpb
                              stibp tpr_shadow flexpriority ept vpid ept_ad fsgs
                             base tsc_adjust smep erms mpx rdt_a rdseed smap clf
                             lushopt intel_pt sha_ni xsaveopt xsavec xgetbv1 xsa
                             ves dtherm arat pln pts vnmi arch_capabilities
Virtualization features:
  Virtualization:            VT-x
Caches (sum of all):
  L1d:                       96 KiB (4 instances)
  L1i:                       128 KiB (4 instances)
  L2:                        8 MiB (4 instances)
Vulnerabilities:
  Gather data sampling:      Not affected
  Ghostwrite:                Not affected
  Indirect target selection: Not affected
  Itlb multihit:             Not affected
  L1tf:                      Not affected
  Mds:                       Not affected
  Meltdown:                  Not affected
  Mmio stale data:           Not affected
  Old microcode:             Vulnerable
  Reg file data sampling:    Vulnerable: No microcode
  Retbleed:                  Not affected
  Spec rstack overflow:      Not affected
  Spec store bypass:         Mitigation; Speculative Store Bypass disabled via p
                             rctl
  Spectre v1:                Mitigation; usercopy/swapgs barriers and __user poi
                             nter sanitization
  Spectre v2:                Mitigation; Retpolines; IBPB conditional; IBRS_FW;
                             STIBP disabled; RSB filling; PBRSB-eIBRS Not affect
                             ed; BHI Not affected
  Srbds:                     Not affected
  Tsa:                       Not affected
  Tsx async abort:           Not affected
  Vmscape:                   Not affected
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

* Intel®Atom™ Processor C3000 Product Family Integrated 10 GbE LAN Controller Programmer's Reference Manual (PRM)

  * https://www.intel.com/content/www/us/en/content-details/338653/intel-atom-processor-c3000-product-family-integrated-10-gbe-lan-controller-programmer-s-reference-manual-prm.html


* Distributed Switch Architecture, A.K.A. DSA

  * https://netdevconf.info/2.1/papers/distributed-switch-architecture.pdf
  * https://netdevconf.org/2.1/slides/apr6/lunn-dsa-netdev-2.1.pdf


* FreeBSD / NetworkFirewalls / WatchguardFireboxM270

  * https://wiki.freebsd.org/NetworkFirewalls/WatchguardFireboxM270
  * https://lists.freebsd.org/archives/freebsd-net/2025-April/006812.html


* pfSense on Watchguard M270

  * https://forum.netgate.com/topic/154974/pfsense-on-watchguard-m270/3


* OPNsense Forum - Support for Intel X553 NIC and Marvell 88E6190 switch (WatchGuard Firebox M270)

  * https://forum.opnsense.org/index.php?topic=32991.0
  * https://github.com/opnsense/src/issues/232

* Watchguard M270 Ethernet Ports

  * https://www.reddit.com/r/linuxhardware/comments/10ppldo/watchguard_m270_ethernet_ports/


* #openwrt-devel / WatchGuard XTM330 - also uses DSA and violates GPL

  * https://oftc.catirclogs.org/openwrt-devel/2021-09-20#


* mdio-tools - ERROR: Unable to read status (-110) with Marvell 88e6190x on x86

  * https://github.com/wkz/mdio-tools/issues/35

