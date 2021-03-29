# One Note Midi App 

## Download

https://github.com/KodiStudios/one-note-midi/releases/latest

## Usage

```
OneNoteMidi.exe [FLAGS]

  -c [0-15]           Channel. Default: 0
  -i [0-127]          Instrument. Default: 0 (Grand Piano)
  -p [0-127]          Pitch (Note). Default: 60 (Middle C Note)
  -v [0-127]          Velocity (Volume). Default: 90 (70.8661% Loud)
  -l [milliseconds]   Length (Note Length), in Milliseconds. Default: 3000 milliseconds
  -?                  Prints this help

Examples:

OneNoteMidi.exe -i 24 -p 80
Play Guitar Note

OneNoteMidi.exe -c 1 -i 24 -p 81 -v 120 -l 2000
Sets Channel 1 to Guitar, Plays G Note, at Volume 120, for 2 seconds
```
