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

module ecap5_dwbtimer #(
  parameter int CLK_FREQ,

  localparam logic TIMER_TIMEBASE_LOW   = 0,
  localparam logic TIMER_TIMEBASE_HIGH  = 1,

  /* 1kHz output frequency */
  localparam int PRESCALER_LOAD = (CLK_FREQ / 1000) - 1
)(
  input   logic         clk_i,
  input   logic         rst_i,

  //=================================
  //    Memory interface

  input   logic[31:0]  wb_adr_i,
  output  logic[31:0]  wb_dat_o,
  input   logic[31:0]  wb_dat_i,
  input   logic        wb_we_i,
  input   logic[3:0]   wb_sel_i,
  input   logic        wb_stb_i,
  output  logic        wb_ack_o,
  input   logic        wb_cyc_i,
  output  logic        wb_stall_o
);

/*****************************************/
/*           Internal signals            */
/*****************************************/

logic[31:0] mem_addr;
logic       mem_read, mem_write;
logic[31:0] mem_read_data_d, mem_read_data_q, 
            mem_write_data;

logic[31:0] prescaler_d, prescaler_q;

/*****************************************/
/*        Memory mapped registers        */
/*****************************************/

logic[63:0] timebase_q;

/*****************************************/

ecap5_dwbmmsc wb_interface_inst (
  .clk_i (clk_i),   .rst_i (rst_i),
  
  .wb_adr_i (wb_adr_i),  .wb_dat_o (wb_dat_o),  .wb_dat_i   (wb_dat_i),
  .wb_we_i  (wb_we_i),   .wb_sel_i (wb_sel_i),  .wb_stb_i   (wb_stb_i),
  .wb_ack_o (wb_ack_o),  .wb_cyc_i (wb_cyc_i),  .wb_stall_o (wb_stall_o),

  .addr_o       (mem_addr),
  .read_o       (mem_read),
  .read_data_i  (mem_read_data_q),
  .write_o      (mem_write),
  .write_data_o (mem_write_data),
  .sel_o          ()
);

always_comb begin : register_access
  // Set the data output for read requests
  mem_read_data_d = 0;
  case(mem_addr[2])
    TIMER_TIMEBASE_LOW:  mem_read_data_d = timebase_q[31:0];
    TIMER_TIMEBASE_HIGH: mem_read_data_d = timebase_q[63:32];
    default:   mem_read_data_d = '0;
  endcase
end

always_comb begin : prescaler
  if(prescaler_q == '0) begin
    prescaler_d = PRESCALER_LOAD;
  end else begin
    prescaler_d = prescaler_q - 1;
  end
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    prescaler_q <= PRESCALER_LOAD;
    timebase_q <= '0;

    mem_read_data_q <= '0;
  end else begin
    prescaler_q <= prescaler_d;
    if(prescaler_q == '0) begin
      timebase_q <= timebase_q + 1;
    end

    mem_read_data_q <= mem_read_data_d;
  end
end

endmodule // ecap5_dwbtimer
