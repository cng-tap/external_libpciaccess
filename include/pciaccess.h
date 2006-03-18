/*
 * (C) Copyright IBM Corporation 2006
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * IBM AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file pciaccess.h
 * 
 * \author Ian Romanick <idr@us.ibm.com>
 */

#include <inttypes.h>

typedef uint64_t pciaddr_t;

struct pci_device;
struct pci_device_iterator;
struct pci_id_match;

int pci_device_read_rom( struct pci_device * dev, void * buffer );

int pci_device_map_region( struct pci_device * dev, unsigned region,
    int write_enable );

int pci_device_unmap_region( struct pci_device * dev, unsigned region );

int pci_device_probe( struct pci_device * dev );

const struct pci_agp_info * pci_device_get_agp_info( struct pci_device * dev );

int pci_system_init( void );

void pci_system_cleanup( void );

struct pci_device_iterator * pci_iterator_create( const char *regex );

void pci_iterator_destroy( struct pci_device_iterator * iter );

struct pci_device * pci_device_next( struct pci_device_iterator * iter );

const char * pci_get_name( const struct pci_id_match * m );
const char * pci_device_get_device_name( const struct pci_device * dev );
const char * pci_device_get_subdevice_name( const struct pci_device * dev );
const char * pci_device_get_vendor_name( const struct pci_device * dev );
const char * pci_device_get_subvendor_name( const struct pci_device * dev );

int pci_device_cfg_read    ( struct pci_device * dev, void * data,
    pciaddr_t offset, pciaddr_t size, pciaddr_t * bytes_read );
int pci_device_cfg_read_u8 ( struct pci_device * dev, uint8_t  * data,
    pciaddr_t offset );
int pci_device_cfg_read_u16( struct pci_device * dev, uint16_t * data,
    pciaddr_t offset );
int pci_device_cfg_read_u32( struct pci_device * dev, uint32_t * data,
    pciaddr_t offset );

int pci_device_cfg_write    ( struct pci_device * dev, const void * data,
    pciaddr_t offset, pciaddr_t size, pciaddr_t * bytes_written );
int pci_device_cfg_write_u8 ( struct pci_device * dev, const uint8_t  * data,
    pciaddr_t offset );
int pci_device_cfg_write_u16( struct pci_device * dev, const uint16_t * data,
    pciaddr_t offset );
int pci_device_cfg_write_u32( struct pci_device * dev, const uint32_t * data,
    pciaddr_t offset );


#define PCI_MATCH_ANY  (~0)

/**
 */
struct pci_id_match {
    /**
     * \name Device / vendor matching controls
     * 
     * Control the search based on the device, vendor, subdevice, or subvendor
     * IDs.  Setting any of these fields to \c PCI_MATCH_ANY will cause the
     * field to not be used in the comparison.
     */
    /*@{*/
    uint32_t    vendor_id;
    uint32_t    device_id;
    uint32_t    subvendor_id;
    uint32_t    subdevice_id;
    /*@}*/

    
    /**
     * \name Device class matching controls
     * 
     */
    /*@{*/
    uint32_t    device_class;
    uint32_t    device_class_mask;
    /*@}*/
    
    intptr_t    match_data;
};


/**
 * BAR descriptor for a PCI device.
 */
struct pci_mem_region {
    /**
     * When the region is mapped, this is the pointer to the memory.
     */
    void * memory;

    pciaddr_t bus_addr;
    pciaddr_t base_addr;


    /**
     * Size, in bytes, of the region.
     */
    pciaddr_t size;
    

    /**
     * Is the region I/O ports or memory?
     */
    unsigned is_IO:1;
    
    /**
     * Is the memory region prefetchable?
     *
     * \note
     * This can only be set if \c is_IO is not set.
     */
    unsigned is_prefetchable:1;


    /**
     * Is the memory at a 64-bit address?
     *
     * \note
     * This can only be set if \c is_IO is not set.
     */
    unsigned is_64:1;
};


/**
 * PCI device.
 * 
 * Contains all of the information about a particular PCI device.
 */
struct pci_device {
    /**
     * \name Device bus identification.
     *
     * Complete bus identification, including domain, of the device.  On
     * platforms that do not support PCI domains (e.g., 32-bit x86 hardware),
     * the domain will always be zero.
     */
    /*@{*/
    uint16_t    domain;
    uint8_t     bus;
    uint8_t     dev;
    uint8_t     func;
    /*@}*/


    /**
     * \name Vendor / device ID
     * 
     * The vendor ID, device ID, and sub-IDs for the device.
     */
    /*@{*/
    uint16_t    vendor_id;
    uint16_t    device_id;
    uint16_t    subvendor_id;
    uint16_t    subdevice_id;
    /*@}*/

    /**
     * Device's class and subclass packed into a single 32-bit value.
     */
    uint32_t    device_class;


    /**
     * Device revision number, as read from the configuration header.
     */
    uint8_t     revision;


    /**
     * BAR descriptors for the device.
     */
    struct pci_mem_region regions[6];


    /**
     * Size, in bytes, of the device's expansion ROM.
     */
    pciaddr_t   rom_size;


    /**
     * IRQ associated with the device.  If there is no IRQ, this value will
     * be -1.
     */
    int irq;


    /**
     * Storage for user data.  Users of the library can store arbitrary
     * data in this pointer.  The library will not use it for any purpose.
     * It is the user's responsability to free this memory before destroying
     * the \c pci_device structure.
     */
    void * user_data;
};


/**
 * Description of the AGP capability of the device.
 * 
 * \sa pci_device_get_agp_info
 */
struct pci_agp_info {
    /**
     * Offset of the AGP registers in the devices configuration register
     * space.  This is generally used so that the offset of the AGP command
     * register can be determined.
     */
    unsigned    config_offset;

   
    /**
     * \name AGP major / minor version.
     */
    /*@{*/
    uint8_t	major_version;
    uint8_t     minor_version;
    /*@}*/

    /**
     * Logical OR of the supported AGP rates.  For example, a value of 0x07
     * means that the device can support 1x, 2x, and 4x.  A value of 0x0c
     * means that the device can support 8x and 4x.
     */
    uint8_t    rates;

    uint8_t    fast_writes:1;       /**< Are fast-writes supported? */
    uint8_t    addr64:1;
    uint8_t    htrans:1;
    uint8_t    gart64:1;
    uint8_t    coherent:1;
    uint8_t    sideband:1;          /**< Is side-band addressing supported? */
    uint8_t    isochronus:1;

    uint8_t    async_req_size;
    uint8_t    calibration_cycle_timing;
    uint8_t    max_requests;
};
