//
// Created by Anton on 05.01.2023.
//

#include "Interpreter.h"
#include "addr_ops.h"
#include "p5_errors.h"

int Interpreter::ms_fret_off = 0;
int Interpreter::ms_st_link_off = 8;
int Interpreter::ms_dyn_link_off = 12;
int Interpreter::ms_ep_off = 16;
int Interpreter::ms_stack_bottom_off = 20;
int Interpreter::ms_ep_cur_off = 24;
int Interpreter::ms_ret_addr_off = 28;

void Interpreter::run() {
	pc = 0;
	ep = 5;

	auto interpreting = true;

	while (interpreting) {
		auto op_code = get_pc<P5::ins_t>();
		switch (op_code) {
			//lod
			case 0: lod_instr<int>(); break;
			case 105: lod_instr<P5::addr_t>(); break;
			case 106: lod_instr<P5::real_t>(); break;
			case 108: lod_instr<P5::bool_t>(); break;
			case 109: lod_instr<P5::char_t>(); break;
			case 107: {
				auto p = get_pc<P5::lvl_t>();
				auto q = get_pc<P5::addr_t>();
				auto addr = get_base_addr(p)+q;
				P5::set_el_t set[P5::set_size];
				get_addr_by_ptr(store, addr, set, P5::set_size);
				put_addr_by_ptr(store, sp, set, P5::set_size);
				break;
			}
			case 58: interpreting = false; break;
			default: {
				P5_ERR("unexpected op code %d", op_code);
			}
		}
	}
}


P5::addr_t Interpreter::get_base_addr(P5::ins_t p) {
	auto addr = mp;
	while (p > 0) {
		addr = get_addr<P5::addr_t>(store, addr+ms_st_link_off);
		p--;
	}
	return addr;
}

template<typename T>
void Interpreter::push_stack(T val) {
	put_addr(store, sp, val);
	sp += sizeof(T);
}

template<typename T>
void Interpreter::lod_instr() {
	auto p = get_pc<P5::lvl_t>();
	auto q = get_pc<P5::addr_t>();
	auto val = get_addr<T>(store, get_base_addr(p)+q);
	push_stack(val);
}

template<typename T>
T Interpreter::get_pc() {
	auto val = get_addr<T>(store, pc);
	pc += sizeof(T);
	return val;
}