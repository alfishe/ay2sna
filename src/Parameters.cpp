/*****************************************************************************

        Parameters.cpp
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



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"Parameters.h"

#include	<cassert>
#include	<cctype>
#include	<cstdio>
#include	<cstdlib>
#include	<cstring>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Parameters::Parameters ()
:	_ay3_filename ()
,	_song_index (-1)
,	_out_filename ()
{
	// Nothing
}



int	Parameters::parse_cmd_line (int argc, char *argv [])
{
	int				ret_val = 0;

	if (argc < 2)
	{
		ret_val = -1;
	}

	OptList			opt_list;
	if (ret_val == 0)
	{
		_ay3_filename.erase (_ay3_filename.begin (), _ay3_filename.end ());
		_song_index = -1;
		_out_filename.erase (_out_filename.begin (), _out_filename.end ());

		_ay3_filename = argv [argc - 1];

		ret_val = collect_options (opt_list, argv, argc - 1);
	}

	if (ret_val == 0)
	{
		const int		nbr_opt = opt_list.size ();
		for (int opt_index = 0; opt_index < nbr_opt && ret_val == 0; ++opt_index)
		{
			const FullOpt &	opt = opt_list [opt_index];

			switch (opt._opt)
			{
			case	Opt_SONG_INDEX:
				ret_val = conv_int (_song_index, opt._arg_list [0]);
				++ _song_index;
				if (ret_val == 0 && _song_index < 0)
				{
					ret_val = -1;
					printf ("Error: song index cannot be negative or null.\n");
				}
				break;

			case	Opt_OUTFILE:
				_out_filename = opt._arg_list [0];
				break;

			default:
				assert (false);
				break;
			}
		}
	}

	if (ret_val != 0)
	{
		printf ("Error: syntax.\n");
		print_usage (argv [0]);
	}

	return (ret_val);
}



void	Parameters::print_usage (const char *progname_0) const
{
	assert (progname_0 != 0);

	using namespace std;

	printf (
"ay2sna - A .AY to .SNA file converter\n"
"\n"
"Written by Laurent de Soras, 2007\n"
"\n"
".AY files are music files from Amstrad CPC and Sinclair Spectrum computers.\n"
"They contain music data as well as the code required to play it back on the\n"
"original computer. This program converts these files into .SNA memory snapshot\n"
"files for Amstrad CPC emulators (CPCEmu, WinApe, etc.), in order to be\n"
"executed and debugged. It works only for AY files from Amstrad CPC.\n"
"\n"
"Usage:\n"
"\n"
"   %s [options] <ay_filename>\n"
"\n"
"Options:\n"
"\n"
"   -s <song_number>\n"
"      Selects the song number, ranging from 1 to N, where N is the number of\n"
"      songs stored in the .AY file.\n"
"      Default is the \"main\" song, indicated in the .AY file.\n"
"\n"
"   -o <sna_filename>\n"
"      Selects the snapshot filename.\n"
"      Default is the same filename as input file, with the \".sna\" extension\n"
"      appended.\n"
"\n",
		progname_0
	);
}



const std::string &	Parameters::use_ay3_filename () const
{
	return (_ay3_filename);
}



// Has to be checked for the upper bound
int	Parameters::get_song_index () const
{
	return (_song_index);
}



const std::string &	Parameters::use_out_filename () const
{
	return (_out_filename);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Parameters::collect_options (OptList &opt_list, char *argv [], long opt_len) const
{
	assert (&opt_list != 0);
	assert (argv != 0);
	assert (opt_len >= 0);

	int				ret_val = 0;

	opt_list.clear ();
	std::vector <std::string>	cur_arg_list;

	for (int opt_index = opt_len - 1
	;	opt_index > 0 && ret_val == 0
	;	-- opt_index)
	{
		const Truc *	opt_ptr = find_option (argv, opt_index);
		if (opt_ptr == 0)
		{
			cur_arg_list.insert (cur_arg_list.begin (), argv [opt_index]);
		}
		else
		{
			const int		arg_list_size = cur_arg_list.size ();
			if (   arg_list_size < opt_ptr->_nbr_arg_min
			    || arg_list_size > opt_ptr->_nbr_arg_max)
			{
				ret_val = -1;
			}
			else
			{
				FullOpt			full_opt;
				full_opt._opt = opt_ptr->_opt;
				full_opt._arg_list = cur_arg_list;
				opt_list.insert (opt_list.begin (), full_opt);

				cur_arg_list.clear ();
			}
		}
	}

	return (ret_val);
}



// Returns 0 if not found
const Parameters::Truc *	Parameters::find_option (char *argv [], int arg_index) const
{
	assert (argv != 0);
	assert (arg_index > 0);

	const Truc *	opt_ptr = 0;

	for (int opt_index = 0; opt_index < Opt_NBR_ELT && opt_ptr == 0; ++opt_index)
	{
		using namespace std;

		const Truc &	opt = _opt_table [opt_index];
		const char *	arg_0 = argv [arg_index];
		assert (arg_0 != 0);
		if (strcmp (arg_0, opt._opt_name_0) == 0)
		{
			opt_ptr = &opt;
		}
	}

	return (opt_ptr);
}



bool	Parameters::strcmp_ci (const char *s1_0, const char *s2_0)
{
	using namespace std;

	long				pos = 0;
	bool				ok_flag = true;
	do
	{
		ok_flag = (toupper (s1_0 [pos]) == toupper (s2_0 [pos]));
		++pos;
	}
	while (ok_flag && s1_0 [pos-1] != '\0');

	return (ok_flag);
}



int	Parameters::conv_int (int &result, const std::string &s)
{
	assert (&result != 0);
	assert (&s != 0);

	int				ret_val = 0;

	const char *	s_0 = s.c_str ();
	const char *	test_0 = s_0;
	result = strtol (s_0, const_cast <char **> (&test_0), 10);
	if (test_0 == s_0)
	{
		ret_val = -1;
	}

	return (ret_val);
}



const Parameters::Truc	Parameters::_opt_table [Opt_NBR_ELT] =
{
	{ Opt_SONG_INDEX, "-s", 1, 1 },
	{ Opt_OUTFILE,    "-o", 1, 1 }
};



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
