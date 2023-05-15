/*****************************************************************************

        Parameters.h
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



#if ! defined (Parameters_HEADER_INCLUDED)
#define	Parameters_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<string>
#include	<vector>



class Parameters
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						Parameters ();
	virtual			~Parameters () {}

	int				parse_cmd_line (int argc, char *argv []);
	void				print_usage (const char *progname_0) const;

	const std::string &
						use_ay3_filename () const;

	int				get_song_index () const;
	const std::string &
						use_out_filename () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Opt
	{
		Opt_SONG_INDEX = 0,
		Opt_OUTFILE,

		Opt_NBR_ELT
	};

	struct Truc
	{
		Opt				_opt;
		const char *	_opt_name_0;
		int				_nbr_arg_min;
		int				_nbr_arg_max;
	};

	struct FullOpt
	{
		Opt				_opt;
		std::vector <std::string>
							_arg_list;
	};

	typedef	std::vector <FullOpt>	OptList;

	int				collect_options (OptList &opt_list, char *argv [], long opt_len) const;
	const Truc *	find_option (char *argv [], int arg_index) const;

	static bool		strcmp_ci (const char *s1_0, const char *s2_0);
	static int		conv_int (int &result, const std::string &s);

	std::string		_ay3_filename;
	int				_song_index;			// -1: default song
	std::string		_out_filename;

	static const Truc
						_opt_table [Opt_NBR_ELT];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Parameters (const Parameters &other);
	Parameters &	operator = (const Parameters &other);
	bool				operator == (const Parameters &other);
	bool				operator != (const Parameters &other);

};	// class Parameters



//#include	"Parameters.hpp"



#endif	// Parameters_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
