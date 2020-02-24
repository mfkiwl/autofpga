////////////////////////////////////////////////////////////////////////////////
//
// Filename:	../demo-out/testb.h
//
// Project:	AutoFPGA, a utility for composing FPGA designs from peripherals
//
// DO NOT EDIT THIS FILE!
// Computer Generated: This file is computer generated by AUTOFPGA. DO NOT EDIT.
// DO NOT EDIT THIS FILE!
//
// CmdLine:	./autofpga ./autofpga -d -o ../demo-out -I ../auto-data bkram.txt buserr.txt clkcounter.txt clock.txt enet.txt flash.txt global.txt gpio.txt gps.txt hdmi.txt icape.txt legalgen.txt mdio.txt pic.txt pwrcount.txt rtcdate.txt rtcgps.txt sdram.txt sdspi.txt spio.txt version.txt wbmouse.txt wboledbw.txt wbpmic.txt wbscopc.txt wbscope.txt wbubus.txt xpander.txt zipmaster.txt
//
// Creator:	Dan Gisselquist, Ph.D.
//		Gisselquist Technology, LLC
//
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017-2020, Gisselquist Technology, LLC
//
// This program is free software (firmware): you can redistribute it and/or
// modify it under the terms of  the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTIBILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  (It's in the $(ROOT)/doc directory.  Run make with no
// target there if the PDF file isn't present.)  If not, see
// <http://www.gnu.org/licenses/> for a copy.
//
// License:	GPL, v3, as defined and found on www.gnu.org,
//		http://www.gnu.org/licenses/gpl.html
//
//
////////////////////////////////////////////////////////////////////////////////
//
//
#ifndef	TESTB_H
#define	TESTB_H

// #define TRACE_FST

#include <stdio.h>
#include <stdint.h>
#ifdef	TRACE_FST
#define	TRACECLASS	VerilatedFstC
#include <verilated_fst_c.h>
#else // TRACE_FST
#define	TRACECLASS	VerilatedVcdC
#include <verilated_vcd_c.h>
#endif
#include <tbclock.h>

	//
	// The TESTB class is a useful wrapper for interacting with a Verilator
	// based design.  Key to its capabilities are the tick() method for
	// advancing the simulation timestep, and the opentrace() and
	// closetrace() methods for handling VCD tracefile generation.  To
	// use a non-VCD trace, redefine TRACECLASS before calling this
	// function to the trace class you wish to use.
//
template <class VA>	class TESTB {
public:
	VA	*m_core;
	bool		m_changed;
	TRACECLASS*	m_trace;
	bool		m_done, m_paused_trace;
	uint64_t	m_time_ps;
	// TBCLOCK is a clock support class, enabling multiclock simulation
	// operation.
	TBCLOCK	m_clk;
	TBCLOCK	m_net_rx_clk;
	TBCLOCK	m_hdmi_in_clk;
	TBCLOCK	m_hdmi_in_hsclk;
	TBCLOCK	m_hdmi_out_clk;

	TESTB(void) {
		m_core = new VA;
		m_time_ps  = 0ul;
		m_trace    = NULL;
		m_done     = false;
		m_paused_trace = false;
		Verilated::traceEverOn(true);
// Set the initial clock periods
		m_clk.init(10000);	//  100.00 MHz
		m_net_rx_clk.init(8000);	//  125.00 MHz
		m_hdmi_in_clk.init(6734);	//  148.50 MHz
		m_hdmi_in_hsclk.init(673);	// 1485.88 MHz
		m_hdmi_out_clk.init(6734);	//  148.50 MHz
	}
	virtual ~TESTB(void) {
		if (m_trace) m_trace->close();
		delete m_core;
		m_core = NULL;
	}

	//
	// opentrace()
	//
	// Useful for beginning a (VCD) trace.  To open such a trace, just call
	// opentrace() with the name of the VCD file you'd like to trace
	// everything into
	virtual	void	opentrace(const char *vcdname, int depth=99) {
		if (!m_trace) {
			m_trace = new TRACECLASS;
			m_core->trace(m_trace, 99);
			m_trace->spTrace()->set_time_resolution("ps");
			m_trace->spTrace()->set_time_unit("ps");
			m_trace->open(vcdname);
			m_paused_trace = false;
		}
	}

	//
	// trace()
	//
	// A synonym for opentrace() above.
	//
	void	trace(const char *vcdname) {
		opentrace(vcdname);
	}

	//
	// pausetrace(pause)
	//
	// Set/clear a flag telling us whether or not to write to the VCD trace
	// file.  The default is to write to the file, but this can be changed
	// by calling pausetrace.  pausetrace(false) will resume tracing,
	// whereas pausetrace(true) will stop all calls to Verilator's trace()
	// function
	//
	virtual	bool	pausetrace(bool pausetrace) {
		m_paused_trace = pausetrace;
		return m_paused_trace;
	}

	//
	// pausetrace()
	//
	// Like pausetrace(bool) above, except that pausetrace() will return
	// the current status of the pausetrace flag above.  Specifically, it
	// will return true if the trace has been paused or false otherwise.
	virtual	bool	pausetrace(void) {
		return m_paused_trace;
	}

	//
	// closetrace()
	//
	// Closes the open trace file.  No more information will be written
	// to it
	virtual	void	closetrace(void) {
		if (m_trace) {
			m_trace->close();
			delete m_trace;
			m_trace = NULL;
		}
	}

	//
	// eval()
	//
	// This is a synonym for Verilator's eval() function.  It evaluates all
	// of the logic within the design.  AutoFPGA based designs shouldn't
	// need to be calling this, they should call tick() instead.  However,
	// in the off chance that your design inputs depend upon combinatorial
	// expressions that would be output based upon other input expressions,
	// you might need to call this function.
	virtual	void	eval(void) {
		m_core->eval();
	}

	//
	// tick()
	//
	// tick() is the main entry point into this helper core.  In general,
	// tick() will advance the clock by one clock tick.  In a multiple clock
	// design, this will advance the clocks up until the nearest clock
	// transition.
	virtual	void	tick(void) {
		unsigned	mintime = m_clk.time_to_edge();

		if (m_net_rx_clk.time_to_edge() < mintime)
			mintime = m_net_rx_clk.time_to_edge();

		if (m_hdmi_in_clk.time_to_edge() < mintime)
			mintime = m_hdmi_in_clk.time_to_edge();

		if (m_hdmi_in_hsclk.time_to_edge() < mintime)
			mintime = m_hdmi_in_hsclk.time_to_edge();

		if (m_hdmi_out_clk.time_to_edge() < mintime)
			mintime = m_hdmi_out_clk.time_to_edge();

		assert(mintime > 1);

		// Pre-evaluate, to give verilator a chance to settle any
		// combinatorial logic thatthat may have changed since the
		// last clockevaluation, and then record that in the trace.
		eval();
		if (m_trace && !m_paused_trace) m_trace->dump(m_time_ps+1);

		// Advance each clock
		m_core->i_clk = m_clk.advance(mintime);
		m_core->i_net_rx_clk = m_net_rx_clk.advance(mintime);
		m_core->i_hdmi_in_clk = m_hdmi_in_clk.advance(mintime);
		m_core->i_hdmi_in_hsclk = m_hdmi_in_hsclk.advance(mintime);
		m_core->i_hdmi_out_clk = m_hdmi_out_clk.advance(mintime);

		m_time_ps += mintime;
		eval();
		// If we are keeping a trace, dump the current state to that
		// trace now
		if (m_trace && !m_paused_trace) {
			m_trace->dump(m_time_ps);
			m_trace->flush();
		}

		if (m_clk.falling_edge()) {
			m_changed = true;
			sim_clk_tick();
		}
		if (m_net_rx_clk.falling_edge()) {
			m_changed = true;
			sim_net_rx_clk_tick();
		}
		if (m_hdmi_in_clk.falling_edge()) {
			m_changed = true;
			sim_hdmi_in_clk_tick();
		}
		if (m_hdmi_in_hsclk.falling_edge()) {
			m_changed = true;
			sim_hdmi_in_hsclk_tick();
		}
		if (m_hdmi_out_clk.falling_edge()) {
			m_changed = true;
			sim_hdmi_out_clk_tick();
		}
	}

	virtual	void	sim_clk_tick(void) {
		// AutoFPGA will override this method within main_tb.cpp if any
		// @SIM.TICK key is present within a design component also
		// containing a @SIM.CLOCK key identifying this clock.  That
		// component must also set m_changed to true.
		m_changed = false;
	}
	virtual	void	sim_net_rx_clk_tick(void) {
		// AutoFPGA will override this method within main_tb.cpp if any
		// @SIM.TICK key is present within a design component also
		// containing a @SIM.CLOCK key identifying this clock.  That
		// component must also set m_changed to true.
		m_changed = false;
	}
	virtual	void	sim_hdmi_in_clk_tick(void) {
		// AutoFPGA will override this method within main_tb.cpp if any
		// @SIM.TICK key is present within a design component also
		// containing a @SIM.CLOCK key identifying this clock.  That
		// component must also set m_changed to true.
		m_changed = false;
	}
	virtual	void	sim_hdmi_in_hsclk_tick(void) {
		// AutoFPGA will override this method within main_tb.cpp if any
		// @SIM.TICK key is present within a design component also
		// containing a @SIM.CLOCK key identifying this clock.  That
		// component must also set m_changed to true.
		m_changed = false;
	}
	virtual	void	sim_hdmi_out_clk_tick(void) {
		// AutoFPGA will override this method within main_tb.cpp if any
		// @SIM.TICK key is present within a design component also
		// containing a @SIM.CLOCK key identifying this clock.  That
		// component must also set m_changed to true.
		m_changed = false;
	}
	virtual bool	done(void) {
		if (m_done)
			return true;

		if (Verilated::gotFinish())
			m_done = true;

		return m_done;
	}

	//
	// reset()
	//
	// Sets the i_reset input for one clock tick.  It's really just a
	// function for the capabilies shown below.  You'll want to reset any
	// external input values before calling this though.
	virtual	void	reset(void) {
		m_core->i_reset = 1;
		tick();
		m_core->i_reset = 0;
		// printf("RESET\n");
	}
};

#endif	// TESTB

