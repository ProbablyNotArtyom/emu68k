//---------------------------------------------------
//
//	emu68k v0.7
//	NotArtyom
//	27/03/21
//
//---------------------------------------------------

#ifndef _HOOKS_HEADER
#define _HOOKS_HEADER

	#include <cstdio>
	#include <cstdlib>

//---------------------------------------------------

class Hooks {
public:
    Hooks();
    virtual ~Hooks();

protected:
	virtual int cpu_irq_ack_callback(int level);
	virtual void cpu_bkpt_ack_callback(u_int data);
	virtual void cpu_reset_callback(void);
	virtual void cpu_fc_callback(u_int fc);
	virtual void cpu_instr_callback(void);
};

typedef int  (*int_ack_callback_t)(int);
typedef void (*bkpt_ack_callback_t)(unsigned int);
typedef void  (*reset_instr_callback_t)(void);
typedef void  (*pc_changed_callback_t)(unsigned int);
typedef void  (*fc_callback_t)(unsigned int);
typedef void  (*instr_hook_callback_t)(void);

//---------------------------------------------------

#endif
