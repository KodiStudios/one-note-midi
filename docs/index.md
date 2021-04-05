# One Note Midi App 

## Download

[One Note Midi](https://github.com/KodiStudios/one-note-midi/releases/latest)

## Usage

```
Plays one note through Midi

Usage:
OneNoteMidi.exe [FLAGS]

  -i [0-127]          Instrument. Default: 0 (Grand Piano)
  -p [0-127]          Pitch (Note). Default: 60 (Middle C Note)
  -v [0-127]          Velocity (Volume). Default: 90 (70.8661% Loud)
  -l [milliseconds]   Length (Note Length), in Milliseconds. Default: 3000 milliseconds
  -c [0-15]           Channel. Default: 0
  -?                  Prints this help

Examples:

OneNoteMidi.exe -i 24 -p 64
Plays Guitar, E Note

OneNoteMidi.exe -i 13 -p 67 -v 127 -l 1000 -c 1
Plays Xylophone, G Note, at Max Volume, for 2 seconds, on Channel 1
```
