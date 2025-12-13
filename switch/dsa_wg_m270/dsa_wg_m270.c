/*
    DSA activator for Watchguard Firebox M270
    ------------------------------------------------------------------------
    (C) 2025, Daniel Rozsnyo <daniel@rozsnyo.com>

    The Atom C3558 has a dual port 10GbE NIC that connects to
    an external Marvell 88E6190 switch chip over 2x 2.5Gb/s links.
    ( The 88E6190X would be able to handle 10Gb/s, but its not used )

    Based on info at:
    https://wiki.freebsd.org/NetworkFirewalls/WatchguardFireboxM270

    This module finds the MDIO bus that is created by the IXGBE driver
    and places there a platform driver device entry in order to make
    the switch work via the DSA subsystem.

    Note: the proper way would be to auto-probe this setup from either
    end (from the switch driver, or maybe rather from the upstream device
    which is made by the ixgbe driver, probing for switch when ixgbe makes
    its MDIO bus would be ideal). The only issue with that is, that the
    hardware is configured specifically for the M270 appliance, so extra
    checks would be required (eg. DMI data, if they would not be useless
    crap - the BIOS reports very generic information, its not properly
    customized).


    TODO 0:
        The module dependencies do not work as expected.
        We need to load the mv88e6xxx driver, otherwise the registerd
        MDIO device will just wait for it to be loaded in driverless
        state.


    TODO 1:
        The mv88e6xxx driver can not accept multiple conduits?
            -- the conduit is detected by "cpu" name and then a device
               is assigned, followed by BREAK 
               [FILE] drivers/net/dsa/mv88e6xxx/chip.c
               [LINE] 7312

        for (port = 0; port < DSA_MAX_PORTS; port++) {
            if (!(pdata->enabled_ports & (1 << port)))
                continue;
            if (strcmp(pdata->cd.port_names[port], "cpu"))
                continue;
            pdata->cd.netdev[port] = &pdata->netdev->dev;
            break; <------------------------------------------ PROBLEM!!
        }


    TODO 2:
        There is some error - link registration ?? after some minutes!!
            Dec 13 03:06:57 [kernel] dsa_wg_m270: starting
            Dec 13 03:06:57 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00: switch 0x1900 detected: Marvell 88E6190, revision 1
            Dec 13 03:10:36 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00: skipping link registration for CPU port 10

        Also the individual ports are initialized very slowly:
            Dec 13 03:10:38 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00 gbe0 (uninitialized): PHY [mv88e6xxx-0:01] driver [Generic PHY] (irq=POLL)
            Dec 13 03:10:43 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00 gbe1 (uninitialized): PHY [mv88e6xxx-0:02] driver [Generic PHY] (irq=POLL)
            Dec 13 03:10:46 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00 gbe2 (uninitialized): PHY [mv88e6xxx-0:03] driver [Generic PHY] (irq=POLL)
            Dec 13 03:10:50 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00 gbe3 (uninitialized): PHY [mv88e6xxx-0:04] driver [Generic PHY] (irq=POLL)
            Dec 13 03:10:54 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00 gbe4 (uninitialized): PHY [mv88e6xxx-0:05] driver [Generic PHY] (irq=POLL)
            Dec 13 03:10:57 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00 gbe5 (uninitialized): PHY [mv88e6xxx-0:06] driver [Generic PHY] (irq=POLL)
            Dec 13 03:11:00 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00 gbe6 (uninitialized): PHY [mv88e6xxx-0:07] driver [Generic PHY] (irq=POLL)
            Dec 13 03:11:04 [kernel] mv88e6085 ixgbe-mdio-0000:06:00.0:00 gbe7 (uninitialized): PHY [mv88e6xxx-0:08] driver [Generic PHY] (irq=POLL)
            Dec 13 03:11:06 [kernel] dsa_wg_m270: started


    TODO 3:
        The 8 ethernet ports appear.
        They have a link LED on with cable.
        They appear 1G negotiated after ifconfig up.

        But packets do not flow.
        ------------------------

        # ethtool cpu (was dsa0 before)
        # ethtool dsa1

        Settings for cpu:
        Settings for dsa1: (same)

                Supported ports: [ FIBRE ]
                Supported link modes:   2500baseT/Full
                Supported pause frame use: Symmetric
                Supports auto-negotiation: Yes
                Supported FEC modes: Not reported
                Advertised link modes:  Not reported
                Advertised pause frame use: Symmetric
                Advertised auto-negotiation: Yes
                Advertised FEC modes: Not reported
                Speed: Unknown!
                Duplex: Unknown! (255)
                Auto-negotiation: on
                Port: Other
                PHYAD: 0
                Transceiver: internal
                Supports Wake-on: umbg
                Wake-on: g
                Current message level: 0x00000007 (7)
                                       drv probe link
                Link detected: no  <------------------------- PROBLEM !



    TODO 4:
        the mvls command from mdio-tool package does finally work, but its very slow!

            # mvls
             VID   FID  SID  P  Q  F  0  1  2  3  4  5  6  7  8  9  a
            DEV:0 Marvell 88E6190
               0     0    0  y  -  -  .  =  =  =  =  =  =  =  =  .  =
            4095     1    0  -  -  -  .  =  =  =  =  =  =  =  =  .  =

            SID  0  1  2  3  4  5  6  7  8  9  a
            DEV:0 Marvell 88E6190
              0  -  -  -  -  -  -  -  -  -  -  -

            ADDRESS             FID  STATE      Q  F  0  1  2  3  4  5  6  7  8  9  a
            DEV:0 Marvell 88E6190
            ff:ff:ff:ff:ff:ff     1  static     -  -  .  1  2  3  4  5  6  7  8  .  a

            NETDEV    P  LINK  MO  FL  S  L  .1Q  PVID   FID
            DEV:0 Marvell 88E6190
            -         0     -  n=  um  -  -  -ut     0     0!map
            gbe0      1     -  n=  um  f  -  -ut     0     0!map
            gbe1      2     -  n=  um  f  -  -ut     0     0!map
            gbe2      3     -  n=  um  f  -  -ut     0     0!map
            gbe3      4     -  n=  um  f  -  -ut     0     0!map
            gbe4      5     -  n=  um  f  -  -ut     0     0!map
            gbe5      6     -  n=  um  f  -  -ut     0     0!map
            gbe6      7     -  n=  um  f  -  -ut     0     0!map
            gbe7      8    1G  n=  um  f  -  -ut     0     0!map
            -         9     -  D=  um  -  -  sut     0     0
            (cpu)     a     -  D=  um  f  -  sut     0     0

        real    0m24.349s
        user    0m0.000s
        sys     0m0.048s

*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mdio.h>
#include <linux/phy.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <net/dsa.h>

 // debug prints
#define PREFIX "dsa_wg_m270: "

// mdio driver
const char* dsa_driver_name = "mv88e6085";
const char* dsa_module_name = "mv88e6xxx";

// this is the MDIO bus name of the 10GbE NIC by IXGBE driver
// ( as seen in /sys/class/mdio_bus/ixgbe-mdio-0000:06:00.0/ )
const char* mdio_class_name = "ixgbe-mdio-0000:06:00.0";
const int mdio_address = 0;

// the MDIO bus object
static struct mii_bus *ixgbe_mdio_bus;

// switch identification
#define MARVELL_PHY_ID_88E6190  0x01411900

// FAKE platform data
#include <linux/platform_data/mv88e6xxx.h>

static struct dsa_mv88e6xxx_pdata switch_pdata = {
    .cd = {
        .port_names[ 0] = NULL, // not enabled
        .port_names[ 1] = "gbe0",
        .port_names[ 2] = "gbe1",
        .port_names[ 3] = "gbe2",
        .port_names[ 4] = "gbe3",
        .port_names[ 5] = "gbe4",
        .port_names[ 6] = "gbe5",
        .port_names[ 7] = "gbe6",
        .port_names[ 8] = "gbe7",

        /*
            This is problematic, we want 2 conduits, like the HW is designed
            - not sure how it should be done on CPU side though. When both
            are enabled, then there is a kernel panic, as we break internal
            expectations between drivers/net/dsa/mv88e6xxx/chip.c and the
            net/dsa/dsa.c which then crashes on the second "cpu" named item.
        */

        // .port_names[ 9] = "cpu", // 2.5G uplink
        // .port_names[10] = NULL,  // 2.5G uplink (disabled temporarily)

        // .port_names[ 9] = NULL,   // 2.5G uplink (disabled temporarily)
        // .port_names[10] = "cpu",  // 2.5G uplink

        .port_names[ 9] = "cpu",  // 2.5G uplink
        .port_names[10] = "two",  // 2.5G ..link (but not as uplink)
        /*
            results in:

            mv88e6085 ixgbe-mdio-0000:06:00.0:00 two (uninitialized): validation of  with support 0000000,00000000,00000000,00006000 and advertisement 0000000,00000000,00000000,00000000 failed: -EINVAL
            mv88e6085 ixgbe-mdio-0000:06:00.0:00 two (uninitialized): failed to connect to PHY: -EINVAL
            mv88e6085 ixgbe-mdio-0000:06:00.0:00 two (uninitialized): error -22 setting up PHY for tree 0, switch 0, port 10

            and its NOT in the /proc/net/dev, nor ethtool works on this "two" interface
        */

    },
    .compatible = "marvell,mv88e6190", // want MV88E6190 (0x01411900)

    // .enabled_ports = 0x07FE, // 00000.11.1111.1111.0 (2 upper 2.5G, 8x RJ45, 1x disabled)
    // .enabled_ports = 0x03FE, // 00000.01.1111.1111.0 (0/1 upper 2.5G, 8x RJ45, 1x disabled)
    // .enabled_ports = 0x05FE, // 00000.10.1111.1111.0 (1/0 upper 2.5G, 8x RJ45, 1x disabled)

    .enabled_ports = 0x07FE, // 00000.11.1111.1111.0 (2 upper 2.5G, 8x RJ45, 1x disabled)

    // TODO: struct net_device * .netdev
    .eeprom_len = 0, // idk if we got EEPROM
    // TODO: int .irq
};

// REAL switch device on mdio
static struct mdio_device *switch_mdiodev;

// TODO: no longer required - probe happens when device exists and module is loaded
// static struct device_driver *drv;
// static struct mdio_driver *mdiodrv;

static int __init dsa_wg_m270_init(void)
{
    int ret;

    pr_info( PREFIX "starting\n");

    ret = -ENODEV;
    ixgbe_mdio_bus = mdio_find_bus( mdio_class_name );

    if (!ixgbe_mdio_bus) {
        pr_err( PREFIX "Failed to find MDIO bus, class:%s\n", mdio_class_name);
        return ret;
    }

    if (ixgbe_mdio_bus->name) {
        pr_info( PREFIX "Found MDIO bus, name:%s\n", ixgbe_mdio_bus->name );
    }

    switch_mdiodev = mdio_device_create(ixgbe_mdio_bus, mdio_address);
    if (IS_ERR(switch_mdiodev)) {
        pr_err( PREFIX "Failed to create MDIO device #%d\n", mdio_address );
        goto out;
    }

    strscpy( switch_mdiodev->modalias, dsa_driver_name );

    switch_pdata.netdev = to_net_dev( ixgbe_mdio_bus->parent );
    if (IS_ERR(switch_pdata.netdev)) {
        pr_err( PREFIX "Failed to determine NETDEV from MDIO bus parent\n" );
        goto out;
    } else {
        pr_info( PREFIX "Found NET dev, name:%s\n", switch_pdata.netdev->name );
    }

    switch_mdiodev->dev.platform_data = &switch_pdata;

    // activate!
    ret = mdio_device_register(switch_mdiodev);
    if (ret) {
        pr_err( PREFIX "Failed to register MDIO device\n" );
        mdio_device_free(switch_mdiodev);
        goto out;
    }

/*
    // TODO: no longer required - probe happens when device exists and module is loaded

    drv = switch_mdiodev->dev.driver;
    if (drv) {
        mdiodrv = to_mdio_driver(drv);

        if (mdiodrv->probe) {
            ret = mdiodrv->probe(switch_mdiodev);
            if (ret) {
                pr_err( PREFIX "Err %d ? Assert the reset signal\n", ret );
                // Assert the reset signal
                // mdio_device_reset(mdiodev, 1);
            }
        } else {
            pr_err( PREFIX "No driver->probe\n" );
        }

    } else {
        pr_err( PREFIX "No driver\n" );
    }
*/

    pr_info( PREFIX "started\n");

out:
    put_device(&ixgbe_mdio_bus->dev);
    return ret;
}

static void __exit dsa_wg_m270_exit(void)
{
    pr_info( PREFIX "removing\n");

    // release
    if (switch_mdiodev) {
        mdio_device_remove( switch_mdiodev );
        mdio_device_free( switch_mdiodev );
    }

    pr_info( PREFIX "removed\n");
}


MODULE_AUTHOR("Daniel Rozsnyo <daniel@rozsnyo.com>");
MODULE_DESCRIPTION("DSA activator for Watchguard Firebox M270");
MODULE_LICENSE("GPL");

// TODO: either of these two does not work as intended - module is not loaded before this one
MODULE_INFO(depends, "mv88e6xxx");
//MODULE_SOFTDEP("pre: mv88e6xxx");

module_init(dsa_wg_m270_init);
module_exit(dsa_wg_m270_exit);

