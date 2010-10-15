/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004  J.D. Koftinoff Software, Ltd.
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE GNU GENERAL PUBLIC LICENSE (GPL) April 27, 2004 ***
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

//
// create_midifile.cpp example
//
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com vrm@vmgames.com
//

#ifdef WIN32
#include <windows.h>
#endif

#include "jdksmidi/world.h"
#include "jdksmidi/track.h"
#include "jdksmidi/multitrack.h"
#include "jdksmidi/filereadmultitrack.h"
#include "jdksmidi/fileread.h"
#include "jdksmidi/fileshow.h"
#include "jdksmidi/filewritemultitrack.h"


using namespace jdksmidi;

int main ( int argc, char **argv )
{
  int return_code = -1;

  MIDITimedBigMessage m; // the object for individual midi events
  unsigned char chan, note, velocity, ctrl, val;

  MIDIClockTime t; // time in midi ticks
  MIDIClockTime dt = 100; // time interval (1 second)
  int clks_per_beat = 100; // number of ticks in crotchet (1...32767)

  MIDIMultiTrack tracks;  // the object which will hold all the tracks
  int trk; // track number

  t = 0;
  m.SetTime( t );

  // track 0 is used for tempo and time signature info, and some other stuff

  trk = 0;

/*
  SetTimeSig( numerator, denominator )
  The numerator is specified as a literal value, but the denominator is specified as (get ready!)
  the value to which the power of 2 must be raised to equal the number of subdivisions per whole note.

  For example, a value of 0 means a whole note because 2 to the power of 0 is 1 (whole note),
  a value of 1 means a half-note because 2 to the power of 1 is 2 (half-note), and so on.

  (numerator, denominator) => musical measure conversion
  (1, 1) => 1/2
  (2, 1) => 2/2
  (1, 2) => 1/4
  (2, 2) => 2/4
  (3, 2) => 3/4
  (4, 2) => 4/4
  (1, 3) => 1/8
*/

  m.SetTimeSig( 4, 2 ); // measure 4/4
  tracks.GetTrack( trk )->PutEvent( m );

  // tempo stored as bpm * 32, giving 1/32 bpm resolution
  unsigned short tempo_times_32 = 60*32; // this tempo equivalent: (1 000 000 usec = 1 sec) in crotchet
                                         // with value of clks_per_beat (100) result 10 msec in 1 midi tick

  // m.SetTime( t ); // do'nt need, because previous time is not changed
  m.SetTempo32( tempo_times_32 );
  tracks.GetTrack( trk )->PutEvent( m );

  // META_INSTRUMENT_NAME text in track 0 music notation software like Sibelius uses as headline of the music
  tracks.GetTrack( trk )->PutTextEvent(t, META_INSTRUMENT_NAME, "libjdksmidi create_midifile example");

  // create cannal midi events and add them to a track 1

  trk = 1;

  // META_INSTRUMENT_NAME text in tracks >= 1 Sibelius uses as instrument name (left of staves)
  tracks.GetTrack( trk )->PutTextEvent(t, META_INSTRUMENT_NAME, "Church Organ");

  // we change panorama in channels 0-2

  m.SetControlChange ( chan = 0, ctrl = 0xA, val = 0 ); // channel 0 panorama = 0 at the left
  tracks.GetTrack( trk )->PutEvent( m );

  m.SetControlChange ( chan = 1, ctrl, val = 64 ); // channel 1 panorama = 64 at the centre
  tracks.GetTrack( trk )->PutEvent( m );

  m.SetControlChange ( chan = 2, ctrl, val = 127 ); // channel 2 panorama = 127 at the right
  tracks.GetTrack( trk )->PutEvent( m );

  // we change musical instrument in channels 0-2

  m.SetProgramChange( chan = 0, val = 19 ); // channel 0 instrument 19 - Church Organ
  tracks.GetTrack( trk )->PutEvent( m );

  m.SetProgramChange( chan = 1, val );
  tracks.GetTrack( trk )->PutEvent( m );

  m.SetProgramChange( chan = 2, val );
  tracks.GetTrack( trk )->PutEvent( m );

  // create individual midi events with the MIDITimedBigMessage and add them to a track 1

  t = 0;

  // we add note: press and release in (dt) ticks

  tracks.GetTrack( trk )->PutTextEvent(t, META_LYRIC_TEXT, "Left"); // add words to music in the present situation

  m.SetTime( t );
  m.SetNoteOn( chan = 0, note = 60, velocity = 100 );
  tracks.GetTrack( trk )->PutEvent( m );

  m.SetTime( t += dt );
  m.SetNoteOff( chan, note, velocity );
  tracks.GetTrack( trk )->PutEvent( m );

  // note 2

  tracks.GetTrack( trk )->PutTextEvent(t, META_LYRIC_TEXT, "Centre");

  m.SetNoteOn( chan = 1, note = 64, velocity );
  tracks.GetTrack( trk )->PutEvent( m );

  m.SetTime( t += dt );
  m.SetNoteOff( chan, note, velocity );
  tracks.GetTrack( trk )->PutEvent( m );

  // note 3

  tracks.GetTrack( trk )->PutTextEvent(t, META_LYRIC_TEXT, "Right");

  m.SetNoteOn( chan = 2, note = 67, velocity );
  tracks.GetTrack( trk )->PutEvent( m );

  m.SetTime( t += dt );
  m.SetNoteOff( chan, note, velocity );
  tracks.GetTrack( trk )->PutEvent( m );

  // add pause

  t += dt;

  // add chord: 3 notes simultaneous

  tracks.GetTrack( trk )->PutTextEvent(t, META_LYRIC_TEXT, "Chord");

  // press
  m.SetTime( t );
  m.SetNoteOn( chan = 0, note = 60, velocity );  tracks.GetTrack( trk )->PutEvent( m );
  m.SetNoteOn( chan = 1, note = 64, velocity );  tracks.GetTrack( trk )->PutEvent( m );
  m.SetNoteOn( chan = 2, note = 67, velocity );  tracks.GetTrack( trk )->PutEvent( m );

  // release
  m.SetTime( t += (2*dt) );
  m.SetNoteOff( chan = 0, note = 60, velocity );  tracks.GetTrack( trk )->PutEvent( m );
  m.SetNoteOff( chan = 1, note = 64, velocity );  tracks.GetTrack( trk )->PutEvent( m );
  m.SetNoteOff( chan = 2, note = 67, velocity );  tracks.GetTrack( trk )->PutEvent( m );

  // add pause: press note with velocity = 0 equivalent to simultaneous release it

  t += dt;
  m.SetTime( t );
  m.SetNoteOn( chan = 0, note = 0, velocity = 0 );
  tracks.GetTrack( trk )->PutEvent( m );

  // to write the multi track object out, you need to create an output stream for the output filename

  const char *outfile_name = "create_midifile.mid";
  MIDIFileWriteStreamFileName out_stream( outfile_name );

  // then output the stream like my example does, except setting num_tracks to match your data

  int num_tracks = 2; // tracks 0 and 1

  if( out_stream.IsValid() )
  {
    // the object which takes the midi tracks and writes the midifile to the output stream
    jdksmidi::MIDIFileWriteMultiTrack writer( &tracks, &out_stream );

    // write the output file
    if( writer.Write( num_tracks, clks_per_beat ) ) return_code = 0;
    else fprintf( stderr, "Error writing file '%s'\n", outfile_name );
  }

  return return_code;
}
