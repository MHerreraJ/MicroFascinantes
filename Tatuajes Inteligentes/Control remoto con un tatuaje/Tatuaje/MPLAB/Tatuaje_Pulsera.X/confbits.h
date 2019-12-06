
// ATtiny1614 Configuration Bit Settings

// 'C' source line config statements

#include <avr/io.h>

FUSES = 
{
	.WDTCFG = (FUSE_WDTCFG_DEFAULT),
	.BODCFG = (FUSE_BODCFG_DEFAULT),
    .OSCCFG = (FUSE_OSCCFG_DEFAULT),
	.TCD0CFG = (FUSE_TCD0CFG_DEFAULT),
    .SYSCFG0 = (0xc4),
	.SYSCFG1 = (FUSE_SYSCFG1_DEFAULT),
	.APPEND = (FUSE_BOOTEND_DEFAULT),
    .BOOTEND = (FUSE_APPEND_DEFAULT)
};


