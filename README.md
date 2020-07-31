# DMC

You will need to have the following libraries installed:
* SDL2
* SDL2_image-2.0.1
* SDL2_ttf

You can install them with `brew` using the following commands:<br/>
`brew install sdl2`<br/>
`brew install sdl2_image`<br/>
`brew install sdl2_ttf`

# Building
Build from the command line with the following:
* `make` - to build the release version
* `make debug` - for the debug version

# In-application commands
* Use the arrow keys to emulate the joypad
* `z` or `a` - A button
* `x` or `b` - B button
* `1` - SELECT button
* `2` - START button
* `q` - quit
* `t` - toggle black&white/original colour modes
* `S` - save emulator state
* `L` - load emulator state (nb. Sometimes freezes after load, in this case try restarting the emulator)
* `m` - view memory location
* `M` - write to memory location
