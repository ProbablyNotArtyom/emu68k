//---------------------------------------------------
//
//	emu68k v0.7
//	NotArtyom
//	27/03/21
//
//---------------------------------------------------

	#include <cstdbool>
	#include <cctype>
	#include <iostream>
	#include <cstring>

	#include "musashi/m68k.h"
	#include "memspace.hpp"
	#include "emu68k.hpp"
	#include "system.hpp"
	#include "hooks.hpp"
	#include "vinput.hpp"

Hooks::Hooks() {
	m68k_set_int_ack_callback(int_ack_callback_t(&Hooks::cpu_irq_ack_callback));
	m68k_set_bkpt_ack_callback(bkpt_ack_callback_t(&Hooks::cpu_bkpt_ack_callback));
	m68k_set_reset_instr_callback(reset_instr_callback_t(&Hooks::cpu_reset_callback));
	m68k_set_fc_callback(fc_callback_t(&Hooks::cpu_fc_callback));
	m68k_set_instr_hook_callback(instr_hook_callback_t(&Hooks::cpu_instr_callback));
}

Hooks::~Hooks() {

}

//---------------------------------------------------

int Hooks::cpu_irq_ack_callback(int level) {
	std::cout << "[!] IRQ callback triggered. (UNIMPLEMENTED_STUB)\n";
	return M68K_INT_ACK_SPURIOUS;
}

void Hooks::cpu_bkpt_ack_callback(u_int data) {
	std::cout << "[!] Breakpoint callback triggered. (UNIMPLEMENTED_STUB)\n";
}

void Hooks::cpu_reset_callback(void) {
	std::cout << "[!] Reset callback triggered. (UNIMPLEMENTED_STUB)\n";
}

void Hooks::cpu_fc_callback(u_int fc) {
	cpu_fcodes = fc;
}

void Hooks::cpu_instr_callback(void) {
	if (freerun == false) update_ui_regs();
}
