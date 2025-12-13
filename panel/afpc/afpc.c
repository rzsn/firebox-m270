/*
    Appliance Front Panel Control
    (C) 2025, Daniel Rozsnyo <daniel@rozsnyo.com>

    TODO platform detection
        - eg M270: check if there is a SIO (Super I/O) of the correct model
        - persistent config in /etc/afpc.conf for the platform?

    TODO dynamic controls
        - populated from detected/requested platform

    TODO switch (different tool?):
        https://wiki.freebsd.org/NetworkFirewalls/WatchguardFireboxM270
*/

#include <sys/io.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
    Global options
    - the M270 led is bi-polar and can not support AMBER
*/
#define LED_CAN_DO_AMBER        0
#define ALLOW_DIFF_COLOR        LED_CAN_DO_AMBER

typedef enum {
    LED_BLANK = 0,
    LED_GREEN = 1,
    LED_RED   = 2,
#if LED_CAN_DO_AMBER
    LED_AMBER = 3,
#endif
} led_color_t;


/*
    Nuvoton NCT6776[DF?]
        Publication Release Date: July 12, 2011
        Version: 1.2
*/

#define LPC_SIO_IOPORT_INDEX    0x2E
#define LPC_SIO_IOPORT_VALUE    0x2F

#define LPC_SIO_REG_CHIP_SELECT         0x07
#define LPC_SIO_REG_EXTENDED_INIT       0x87
#define LPC_SIO_REG_EXTENDED_DONE       0xAA

#define LPC_SIO_CHIP_GLOBAL             0x00
#define LPC_SIO_GLOBAL_OPTION                   0x27

#define LPC_SIO_CHIP_GPIO6789           0x07
#define LPC_SIO_GPIO6789_GPIO7X_DIRS            0xE0
#define LPC_SIO_GPIO6789_GPIO7X_DATA            0xE1
#define LPC_SIO_GPIO6789_GPIO7X_MASK_LED_G      0x04
#define LPC_SIO_GPIO6789_GPIO7X_MASK_LED_R      0x08

#define LPC_SIO_CHIP_GPIO2345           0x09
#define LPC_SIO_GPIO2345_CONTROL                0x30


/*
    Grant access to I/O ports
    Note: needs root
*/

static bool io_port_perm( int turn_on ) {
    unsigned short portFrom = LPC_SIO_IOPORT_INDEX;
    unsigned short portSize = LPC_SIO_IOPORT_VALUE-LPC_SIO_IOPORT_INDEX+1;
    int error;

    error = ioperm( portFrom, portSize, turn_on );

    return !error;
}

static bool io_port_init() {
    return io_port_perm( 1 );
}

static bool io_port_done() {
    return io_port_perm( 0 );
}


/*
    Low level access to SIO registers
*/

static void io_port_extended_init() {
    // p.54: 7.1.1 Enter the Extended Function Mode
    unsigned char magic = LPC_SIO_REG_EXTENDED_INIT;
    outb( magic, LPC_SIO_IOPORT_INDEX );
    outb( magic, LPC_SIO_IOPORT_INDEX );
}

static void io_port_extended_done() {
    // p.55: 7.1.3 Exit the Extended Function Mode
    unsigned char magic = LPC_SIO_REG_EXTENDED_DONE;
    outb( magic, LPC_SIO_IOPORT_INDEX );
}

static void io_port_wr( unsigned char index, unsigned char value ) {
    outb( index, LPC_SIO_IOPORT_INDEX );
    outb( value, LPC_SIO_IOPORT_VALUE );
}

static unsigned char io_port_rd( unsigned char index ) {
    outb( index, LPC_SIO_IOPORT_INDEX );
    return inb( LPC_SIO_IOPORT_VALUE );
}


/*
    Mid level access
*/

static void sio_chip_wr( unsigned char chip, unsigned char index, unsigned char value ) {
    io_port_wr( LPC_SIO_REG_CHIP_SELECT, chip );
    io_port_wr( index, value );
}

static unsigned char sio_chip_rd( unsigned char chip, unsigned char index ) {
    io_port_wr( LPC_SIO_REG_CHIP_SELECT, chip );
    return io_port_rd( index );
}


/*
    High level access
*/

static void io_gpio_setup() {
    unsigned char cfg;

    // p.315 (p.335): enable GPIO's GP[75:70] pins
    cfg = sio_chip_rd( LPC_SIO_CHIP_GLOBAL, LPC_SIO_GLOBAL_OPTION );
    cfg |= 0x80;
    sio_chip_wr( LPC_SIO_CHIP_GLOBAL, LPC_SIO_GLOBAL_OPTION, cfg );

    // p.356 (p.376): enable GPIO7X
    cfg = sio_chip_rd( LPC_SIO_CHIP_GPIO2345, LPC_SIO_GPIO2345_CONTROL );
    cfg |= 0x80;
    sio_chip_wr( LPC_SIO_CHIP_GPIO2345, LPC_SIO_GPIO2345_CONTROL, cfg );

    // p.343 (p.363): change GPIO[74] in, GPIO[73:72] out
    cfg = sio_chip_rd( LPC_SIO_CHIP_GPIO6789, LPC_SIO_GPIO6789_GPIO7X_DIRS );
    cfg &= 0xE3; // xxx0.00xx
    cfg |= 0x10; // xxx1.xxxx
    sio_chip_wr( LPC_SIO_CHIP_GPIO6789, LPC_SIO_GPIO6789_GPIO7X_DIRS, cfg );

}

static bool get_button_state() {
    unsigned char cfg;

    // p.343 (p.363): sense GPIO[74] value
    cfg = sio_chip_rd( LPC_SIO_CHIP_GPIO6789, LPC_SIO_GPIO6789_GPIO7X_DATA );

    // button at GPIO[74] to ground (active low)
    return !((cfg >> 4) & 1);
}

static led_color_t get_led_color() {
    unsigned char cfg;

    // p.343 (p.363): sense GPIO[73:72] value
    cfg = sio_chip_rd( LPC_SIO_CHIP_GPIO6789, LPC_SIO_GPIO6789_GPIO7X_DATA );

    // led between GPIO[73] and GPIO[72]
    cfg &= LPC_SIO_GPIO6789_GPIO7X_MASK_LED_R | LPC_SIO_GPIO6789_GPIO7X_MASK_LED_G;

    if (cfg==LPC_SIO_GPIO6789_GPIO7X_MASK_LED_R) return LED_RED;
    if (cfg==LPC_SIO_GPIO6789_GPIO7X_MASK_LED_G) return LED_GREEN;

    #if LED_CAN_DO_AMBER
    if (cfg==(LPC_SIO_GPIO6789_GPIO7X_MASK_LED_R|LPC_SIO_GPIO6789_GPIO7X_MASK_LED_G)) return LED_AMBER;
    #endif

    return LED_BLANK;
}


#if ALLOW_DIFF_COLOR

static void set_led1_value( unsigned char mask, unsigned char on ) {
    unsigned char cfg;

    // p.343 (p.363): change GPIO[73:72] value (73:red, 72:green)
    cfg = sio_chip_rd( LPC_SIO_CHIP_GPIO6789, LPC_SIO_GPIO6789_GPIO7X_DATA );
    if (on) {
        cfg |= mask;
    } else {
        cfg &= ~mask;
    }
    sio_chip_wr( LPC_SIO_CHIP_GPIO6789, LPC_SIO_GPIO6789_GPIO7X_DATA, cfg );
}

static void set_led_red( unsigned char on ) {
    set_led1_value( LPC_SIO_GPIO6789_GPIO7X_MASK_LED_R, on );
}

static void set_led_green( unsigned char on ) {
    set_led1_value( LPC_SIO_GPIO6789_GPIO7X_MASK_LED_G, on );
}

#endif


static void set_led2_value( unsigned char red, unsigned char green ) {
    unsigned char cfg;

    // p.343 (p.363): change GPIO[73:72] value (73:red, 72:green)
    cfg = sio_chip_rd( LPC_SIO_CHIP_GPIO6789, LPC_SIO_GPIO6789_GPIO7X_DATA );
    if (red) {
        cfg |= LPC_SIO_GPIO6789_GPIO7X_MASK_LED_R;
    } else {
        cfg &= ~LPC_SIO_GPIO6789_GPIO7X_MASK_LED_R;
    }
    if (green) {
        cfg |= LPC_SIO_GPIO6789_GPIO7X_MASK_LED_G;
    } else {
        cfg &= ~LPC_SIO_GPIO6789_GPIO7X_MASK_LED_G;
    }
    sio_chip_wr( LPC_SIO_CHIP_GPIO6789, LPC_SIO_GPIO6789_GPIO7X_DATA, cfg );
}

static void set_led_color( led_color_t color ) {
    unsigned char red = 0;
    unsigned char green = 0;

    switch(color) {
        case LED_RED:
            red = 1;
            break;
        case LED_GREEN:
            green = 1;
            break;
#if LED_CAN_DO_AMBER
        case LED_AMBER:
            red = 1;
            green = 1;
            break;
#endif
        case LED_BLANK:
            break;
    }

    set_led2_value( red, green );
}

static char* led_color_to_string( led_color_t color ) {
    switch(color) {
        case LED_RED:   return "red";   break;
        case LED_GREEN: return "green"; break;
#if LED_CAN_DO_AMBER
        case LED_AMBER: return "amber"; break;
#endif
        case LED_BLANK: return "off";   break;
    }

    return "unknown";
}


/*
    M270 LEDs tool
*/

int main( int argc, char* argv[] ) {
    bool button;
    led_color_t color;

    if (argc==2) {
        if ( (strcmp(argv[1],"-h")==0) || (strcmp(argv[1],"--help")==0) ) {
            printf("Appliance Front Panel Control\n");
            printf("(C) 2025, Daniel Rozsnyo <daniel@rozsnyo.com>\n");
            printf("\n");
            printf("Supported devices:\n");
            printf("\n");
            printf("    Watchguard Firebox M270     NCB-WG2511(A) V0.5(1)       Intel Atom C3558\n");
            printf("                                                            Marvell 88E6190\n");
            printf("                                                            Nuvoton NCT6776(?)\n");
            printf("\n");
            printf("Usage:\n");
            printf("\n");
            printf("    afpc                        report current state\n");
#if ALLOW_DIFF_COLOR
            printf("    afpc[+-](red|green)         change one color\n");
#endif
#if LED_CAN_DO_AMBER
            printf("    afpc red|green|amber|off    set direct color\n");
#else
            printf("    afpc red|green|off          set direct color\n");
#endif
            printf("\n");
            printf("Output:\n");
            printf("\n");
            printf("    Will always report the state (after operations) as multiple lines of:\n");
            printf("\n");
            printf("        TYPE.CHANNEL=VALUE\n");
            printf("\n");
            printf("    eg:\n");
            printf("\n");
            printf("        button.reset=0\n");
            printf("        led.guard=red\n");
            printf("\n");

            return 0;
        }
    }

    // init
    if (!io_port_init()) {
        perror("ioperm failed");
        return -1;
    }

    // for GPIO configuration we need extended mode
    io_port_extended_init();

    // configure gpio
    io_gpio_setup();

    if (argc==2) {

        #if ALLOW_DIFF_COLOR
        /*
            color difference operations
             - will not throw error when AMBER is not supported
             - will change to "different" OFF when both are on but diode is 2 wire bi-polar
        */
        if (strcmp(argv[1],"+red")==0) set_led_red( 1 );
        if (strcmp(argv[1],"-red")==0) set_led_red( 0 );
        if (strcmp(argv[1],"+green" )==0) set_led_green( 1 );
        if (strcmp(argv[1],"-green" )==0) set_led_green( 0 );
        #endif

        /*
            direct color operations
        */
        if (strcmp(argv[1],"red"  )==0) set_led_color( LED_RED   );
        if (strcmp(argv[1],"green")==0) set_led_color( LED_GREEN );
#if LED_CAN_DO_AMBER
        if (strcmp(argv[1],"amber")==0) set_led_color( LED_AMBER );
#else
        if (strcmp(argv[1],"amber")==0) {
            fprintf(stderr,"amber: not supported\n");
        }
#endif
        if (strcmp(argv[1],"off"  )==0) set_led_color( LED_BLANK );

    }

    button = get_button_state();
    printf( "button.reset=%d\n", button ? 1 : 0 );

    color = get_led_color();
    printf( "led.guard=%s\n", led_color_to_string( color ) );

    // reconfig done
    io_port_extended_done();

    // done
    io_port_done();
    return 0;
}

