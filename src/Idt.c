#include "../include/Idt.h"
#include "../include/ACOREFuncs.h"
#include "../include/Isr.h"
#include "../include/Memtools.h"
#include "../include/Pic.h"
#include "../include/Other.h"
#include "../include/Irq.h"
#include <stdint.h>

struct IDT_pointer idtp;
struct IDT_descriptor idtd[256];
irq_handler_t irqs[15];

extern void isr_handler(struct Interrupt_parameters iparam);
extern void irq_handler(struct Interrupt_parameters iparam);
static void encode_idt(uint8_t index, uint32_t offset, uint16_t selector, uint8_t type);
void int50stub(void);
extern void int50(void);

//int test

void init_idt(void){
  encode_idt(0, (unsigned)isr0, 0x08, 0x8F);
  encode_idt(1, (unsigned)isr1, 0x08, 0x8F);
  encode_idt(2, (unsigned)isr2, 0x08, 0x8F);
  encode_idt(3, (unsigned)isr3, 0x08, 0x8F);
  encode_idt(4, (unsigned)isr4, 0x08, 0x8F);
  encode_idt(5, (unsigned)isr5, 0x08, 0x8F);
  encode_idt(6, (unsigned)isr6, 0x08, 0x8F);
  encode_idt(7, (unsigned)isr7, 0x08, 0x8F);
  encode_idt(8, (unsigned)isr8, 0x08, 0x8F);
  encode_idt(9, (unsigned)isr9, 0x08, 0x8F);
  encode_idt(10, (unsigned)isr10, 0x08, 0x8F);
  encode_idt(11, (unsigned)isr11, 0x08, 0x8F);
  encode_idt(12, (unsigned)isr12, 0x08, 0x8F);
  encode_idt(13, (unsigned)isr13, 0x08, 0x8F);
  encode_idt(14, (unsigned)isr14, 0x08, 0x8F);
  encode_idt(15, (unsigned)isr15, 0x08, 0x8F);
  encode_idt(16, (unsigned)isr16, 0x08, 0x8F);
  encode_idt(17, (unsigned)isr17, 0x08, 0x8F);
  encode_idt(18, (unsigned)isr18, 0x08, 0x8F);
  encode_idt(19, (unsigned)isr19, 0x08, 0x8F);
  encode_idt(20, (unsigned)isr20, 0x08, 0x8F);
  encode_idt(21, (unsigned)isr21, 0x08, 0x8F);
  encode_idt(22, (unsigned)isr22, 0x08, 0x8F);
  encode_idt(23, (unsigned)isr23, 0x08, 0x8F);
  encode_idt(24, (unsigned)isr24, 0x08, 0x8F);
  encode_idt(25, (unsigned)isr25, 0x08, 0x8F);
  encode_idt(26, (unsigned)isr26, 0x08, 0x8F);
  encode_idt(27, (unsigned)isr27, 0x08, 0x8F);
  encode_idt(28, (unsigned)isr28, 0x08, 0x8F);
  encode_idt(29, (unsigned)isr29, 0x08, 0x8F);
  encode_idt(30, (unsigned)isr30, 0x08, 0x8F);
  encode_idt(31, (unsigned)isr31, 0x08, 0x8F);
  encode_idt(32, (unsigned)irq0, 0x08, 0x8F);
  encode_idt(33, (unsigned)irq1, 0x08, 0x8F);
  encode_idt(34, (unsigned)irq2, 0x08, 0x8F);
  encode_idt(35, (unsigned)irq3, 0x08, 0x8F);
  encode_idt(36, (unsigned)irq4, 0x08, 0x8F);
  encode_idt(37, (unsigned)irq5, 0x08, 0x8F);
  encode_idt(38, (unsigned)irq6, 0x08, 0x8F);
  encode_idt(39, (unsigned)irq7, 0x08, 0x8F);
  encode_idt(40, (unsigned)irq8, 0x08, 0x8F);
  encode_idt(41, (unsigned)irq9, 0x08, 0x8F);
  encode_idt(42, (unsigned)irq10, 0x08, 0x8F);
  encode_idt(43, (unsigned)irq11, 0x08, 0x8F);
  encode_idt(44, (unsigned)irq12, 0x08, 0x8F);
  encode_idt(45, (unsigned)irq13, 0x08, 0x8F);
  encode_idt(46, (unsigned)irq14, 0x08, 0x8F);
  encode_idt(47, (unsigned)isr15, 0x08, 0x8F);
  
  encode_idt(80, (unsigned)int50stub, 0x08, 0x8E);
  
  idtp.size = sizeof(idtd) - 1;
  idtp.offset = (uint32_t)&idtd[0];
  
  
  flush_idt();
  return;
}

static void encode_idt(uint8_t index, uint32_t offset, uint16_t selector, uint8_t type){
  idtd[index].offset_low = (offset & 0xFFFF);
  idtd[index].selector = selector;
  idtd[index].zero = 0;
  idtd[index].type = type;
  idtd[index].offset_high = ((offset >> 16) & 0xFFFF);
  return;
}


extern void isr_handler(struct Interrupt_parameters iparam){
	//for now
	os_cli();
	os_halt();
}

extern void irq_handler(struct Interrupt_parameters iparam){
	//for now
	
	memcpy((char*)0, &iparam, sizeof(iparam));
	
	uint8_t interrupt = (uint8_t)iparam.vendor_code - 32;
	
	if ((pic_irr() & interrupt) != interrupt){
		*(volatile char*)0xB8002 = 0x39;//
		return;
	}
	
	irq_handler_t handler = irqs[interrupt];
	
	if (handler){
		*(volatile char*)0xB8000 = 0x31;
		handler();
	}
	
	pic_EOI(interrupt);
	return;
}

extern void int50(void){	
	*(volatile char*)0xA0 = 0xF9;
	return;
}

void install_irq(uint8_t index, irq_handler_t handler){
	irqs[index] = handler;
	return;
}

void remove_irq(uint8_t index){
	irqs[index] = 0;
	return;
}

