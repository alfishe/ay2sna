/*****************************************************************************

        main.cpp
        Copyright (c) 2007 Laurent de Soras

--- Legal stuff ---

This file is part of ay2sna.

AY2Wav is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

AY2Wav is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with AY2Wav; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130) // "'operator' : logical operation on address of string constant"
	#pragma warning (1 : 4223) // "nonstandard extension used : non-lvalue array converted to pointer"
	#pragma warning (1 : 4705) // "statement has no effect"
	#pragma warning (1 : 4706) // "assignment within conditional expression"
	#pragma warning (4 : 4786) // "identifier was truncated to '255' characters in the debug information"
	#pragma warning (4 : 4800) // "forcing value to bool 'true' or 'false' (performance warning)"
	#pragma warning (4 : 4355) // "'this' : used in base member initializer list"
#endif



#include	"AyFileReader.h"
#include	"def.h"
#include	"Parameters.h"

#include	<string>

#include	<cassert>

namespace std { }



typedef	UByte	MemZone [65536L];



static void	init_memory (MemZone &mem_zone, const AyFileReader &ay_data, int song);
static void	write_block_to_mem (MemZone &mem_zone, const UByte data_ptr [], UInt16 addr, long len);
static void	write_word_to_mem (MemZone &mem_zone, UInt16 addr, UInt16 data);
static int	write_sna_file (MemZone &mem_zone, const AyFileReader &ay_data, int song, const char *filename_0);



int main (int argc, char *argv [])
{
	int				ret_val = 0;

	Parameters		param;
	if (ret_val == 0)
	{
		ret_val = param.parse_cmd_line (argc, argv);
	}

	AyFileReader	ay_data;
	if (ret_val == 0)
	{
		const std::string &	filename = param.use_ay3_filename ();
		ret_val = ay_data.read_file (filename.c_str ());
		if (ret_val != 0)
		{
			printf ("Error: cannot read AY file %s.\n", filename.c_str ());
		}
	}

	if (ret_val == 0)
	{
		MemZone			mem_zone;
		int				song = param.get_song_index ();
		if (song < 0 || song >= ay_data.get_nbr_songs ())
		{
			song = ay_data.get_def_song ();
		}

		init_memory (mem_zone, ay_data, song);

		std::string		filename_sna = param.use_out_filename ();
		if (filename_sna.length () == 0)
		{
			filename_sna = param.use_ay3_filename () + ".sna";
		}

		ret_val = write_sna_file (
			mem_zone,
			ay_data,
			song,
			filename_sna.c_str ()
		);
		if (ret_val != 0)
		{
			printf ("Error: cannot write SNA file %s.\n", filename_sna.c_str ());
		}
	}

	return (ret_val);
}



static void	init_memory (MemZone &mem_zone, const AyFileReader &ay_data, int song)
{
	assert (&mem_zone != 0);
	assert (&ay_data != 0);

	using namespace std;

	// Fill #0000-#00FF range with #C9 value
	memset (&mem_zone [0x0000], 0xC9, 0x0100);	// #C9: RET

	// Fill #0100-#3FFF range with #FF value
	memset (&mem_zone [0x0100], 0xFF, 0x3F00);	// #FF: RST #38

	// Fill #4000-#FFFF range with #00 value
	memset (&mem_zone [0x4000], 0x00, 0xC000);

	// Place to #0038 address #FB value
	mem_zone [0x0038] = 0xFB;						// #FB: EI

	// If INTERRUPT equal to ZERO then place at ZERO address next player:
	const UInt16	interrupt = ay_data.get_interrupt (song);
	if (interrupt == 0)
	{
		static const UByte	code [] =
		{
			0xF3,					// DI
			0xCD,	0x00, 0x00,	// CALL INIT
			0xED, 0x5E,			// LOOP: IM 2
			0xFB,					// EI
			0x76,					// HALT
			0x18, 0xFA			// JR LOOP
		};
		const long		len = sizeof (code) / sizeof (code [0]);
		write_block_to_mem (mem_zone, code, 0, len);
	}

	// If INTERRUPT not equal to ZERO then place at ZERO address next player:
	else
	{
		static const UByte	code [] =
		{
			0xF3,					// DI
			0xCD,	0x00, 0x00,	// CALL INIT
			0xED, 0x56,			// LOOP: IM 1
			0xFB,					// EI
			0x76,					// HALT
			0xCD, 0x00, 0x00,	// CALL INTERRUPT
			0x18, 0xF7			// JR LOOP
		};
		const long		len = sizeof (code) / sizeof (code [0]);
		write_block_to_mem (mem_zone, code, 0, len);
		write_word_to_mem (mem_zone, 9, interrupt);
	}

	const UInt16	init = ay_data.get_init (song);
	write_word_to_mem (mem_zone, 2, init);

	// Load all blocks for this song
	const int		nbr_blocks = ay_data.get_nbr_blocks (song);
	for (int block = 0; block < nbr_blocks; ++block)
	{
		UInt16			block_addr = ay_data.get_block_addr (song, block);
		const int		block_len = ay_data.get_block_len (song, block);
		const UByte *	block_ptr = ay_data.get_block_ptr (song, block);
		write_block_to_mem (mem_zone, block_ptr, block_addr, block_len);
	}

}



static void	write_block_to_mem (MemZone &mem_zone, const UByte data_ptr [], UInt16 addr, long len)
{
	assert (data_ptr != 0);
	assert (len > 0);

	if (addr + len > 65536L)
	{
		len = 65536L - addr;
	}

	for (long pos = 0; pos < len; ++pos)
	{
		mem_zone [addr + pos] = data_ptr [pos];
	}
}



static void	write_word_to_mem (MemZone &mem_zone, UInt16 addr, UInt16 data)
{
	mem_zone [addr + 0] = data & 0xFF;
	if (addr <= 0xFFFEL)
	{
		mem_zone [addr + 1] = data >> 8;
	}
}



static int	write_sna_file (MemZone &mem_zone, const AyFileReader &ay_data, int song, const char *filename_0)
{
	assert (&mem_zone != 0);
	assert (&ay_data != 0);
	assert (song >= 0);
	assert (song < ay_data.get_nbr_songs ());
	assert (filename_0 != 0);

	using namespace std;

	int				ret_val = 0;

	UByte				header [0x100] = { 0 };
	memcpy (header, "MV - SNA", 8);
	header [0x10] = 1;	// Version

	const UInt16	reg_init = ay_data.get_init_reg_val (song);
	for (int k = 0; k < 8; ++k)
	{
		header [0x11 + k * 2] =  reg_init       & 0xFF;
		header [0x12 + k * 2] = (reg_init >> 8) & 0xFF;
	}
	header [0x1A] = 3;		// I register
	header [0x1B] = 1;		// IFF0
	header [0x1C] = 1;		// IFF1
	const UInt16	sp = ay_data.get_sp (song);
	header [0x21] =  sp       & 0xFF;	// SP (low)
	header [0x22] = (sp >> 8) & 0xFF;	// SP (high)
	header [0x23] = 0;		// PC (low)
	header [0x24] = 0;		// PC (high)
	header [0x25] = 1;		// IM 1
	header [0x40] = 0x8C;	// RAM/ROM configuration
	header [0x41] = 0xC0;	// RAM selection
	header [0x55] = 0;		// ROM selection
	header [0x6B] = 64;		// Dump size in KB (low byte)
	header [0x6C] = 0;		// Dump size in KB (high byte)

	FILE *			f_ptr = fopen (filename_0, "wb");
	if (f_ptr == 0)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		const long	wlen = fwrite (&header [0], sizeof (header), 1, f_ptr);
		if (wlen != 1)
		{
			ret_val = -1;
		}
	}

	if (ret_val == 0)
	{
		const long	wlen = fwrite (&mem_zone [0], sizeof (mem_zone), 1, f_ptr);
		if (wlen != 1)
		{
			ret_val = -1;
		}
	}

	if (f_ptr != 0)
	{
		fclose (f_ptr);
		f_ptr = 0;
	}

	return (ret_val);
}
