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
#include "Vtb_ecap5_dwbtimer_tb_ecap5_dwbtimer.h"
#include "Vtb_ecap5_dwbtimer_ecap5_dwbtimer.h"
#include "testbench.h"

enum CondId {
  __CondIdEnd
};

enum TestcaseId {
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

  void read(uint32_t addr) {
    this->core->wb_adr_i = addr;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0xF;
    this->core->wb_stb_i = 1;
    this->core->wb_cyc_i = 1;
  }

  void write(uint32_t addr, uint32_t data) {
    this->core->wb_adr_i = addr;
    this->core->wb_dat_i = data;
    this->core->wb_we_i = 1;
    this->core->wb_sel_i = 0xF;
    this->core->wb_stb_i = 1;
    this->core->wb_cyc_i = 1;
  }

};

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Ecap5_dwbtimer * tb = new TB_Ecap5_dwbtimer;
  tb->open_trace("waves/ecap5_dwbtimer.vcd");
  tb->open_testdata("testdata/ecap5_dwbtimer.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/


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
