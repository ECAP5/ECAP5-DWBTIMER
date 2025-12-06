/*           __        _
 *  ________/ /  ___ _(_)__  ___
 * / __/ __/ _ \/ _ `/ / _ \/ -_)
 * \__/\__/_//_/\_,_/_/_//_/\__/
 * 
 * Copyright (C) Clément Chaine
 * This file is part of ECAP5-DWBTIMER <https://github.com/ecap5/ECAP5-DWBTIMER>
 *
 * ECAP5-DWBTIMER is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ECAP5-DWBTIMER is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ECAP5-DWBTIMER.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <svdpi.h>

#include "Vtb_ecap5_dwbtimer.h"
#include "Vtb_ecap5_dwbtimer_ecap5_dwbtimer__Cf4240.h"
#include "Vtb_ecap5_dwbtimer_tb_ecap5_dwbtimer.h"
#include "testbench.h"

enum CondId {
  COND_register,
  COND_precision,
  __CondIdEnd
};

enum TestcaseId {
  T_REGISTERS = 0,
  T_PRECISION = 1
};

enum StateId {
};

class TB_Ecap5_dwbtimer : public Testbench<Vtb_ecap5_dwbtimer> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_ecap5_dwbtimer>::reset();
  }
  
  void _nop() {
    this->core->wb_adr_i = 0;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0;
    this->core->wb_stb_i = 0;
    this->core->wb_cyc_i = 0;

  }

  uint32_t read(uint32_t addr) {
    this->core->wb_adr_i = addr;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0xF;
    this->core->wb_stb_i = 1;
    this->core->wb_cyc_i = 1;

    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;

    uint32_t read_data = this->core->wb_dat_o;

    this->tick();

    this->_nop();
    this->tick();

    return read_data;
  }
};

void tb_ecap5_dwbtimer_registers(TB_Ecap5_dwbtimer * tb) {
  Vtb_ecap5_dwbtimer * core = tb->core;
  core->testcase = T_REGISTERS;

  //=================================
  //      Tick (0)
  
  tb->reset();

  uint32_t timebase_low = tb->read(0);
  uint32_t timebase_high = tb->read(4);

  tb->check(COND_register, ((timebase_low == 0) && (timebase_high == 0)));

  tb->n_tick(1000);

  timebase_low = tb->read(0);
  timebase_high = tb->read(4);
  tb->check(COND_register, ((timebase_low == 1) && (timebase_high == 0)));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbtimer.registers.01",
      tb->conditions[COND_register],
      "Failed to implement the registers", tb->err_cycles[COND_register]);
}

void tb_ecap5_dwbtimer_precision(TB_Ecap5_dwbtimer * tb) {
  Vtb_ecap5_dwbtimer * core = tb->core;
  core->testcase = T_PRECISION;

  //=================================
  //      Tick (0)
  
  tb->reset();

  for(int i = 0; i < 100; i++) {
    uint32_t num_cycles = 0;
    uint64_t current_timebase = core->tb_ecap5_dwbtimer->dut->timebase_q;
    // wait for the timebase to change
    while(current_timebase == core->tb_ecap5_dwbtimer->dut->timebase_q) {
      tb->tick();
      num_cycles += 1; 
    }

    tb->check(COND_precision, (num_cycles == 1000));
  }

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbtimer.precision.01",
      tb->conditions[COND_precision],
      "Failed to implement the required precision", tb->err_cycles[COND_precision]);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Ecap5_dwbtimer * tb = new TB_Ecap5_dwbtimer;
  tb->open_trace("waves/ecap5_dwbtimer.vcd");
  tb->open_testdata("testdata/ecap5_dwbtimer.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  // 1MHz = 1000000ps
  tb->clk_period_in_ps = 1000000;

  /************************************************************/

  tb_ecap5_dwbtimer_registers(tb);
  tb_ecap5_dwbtimer_precision(tb);

  /************************************************************/

  printf("[ECAP5_DWBTIMER]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
