# Chippy Gonzales
Chippy Gonzales is a tool for directly manipulating the C.H.I.P.'s
GPIO hardware-registers, allowing you to query and set things like
e.g. mux, pin-state and pull-up/-down.

The tool can also print out a prettyish graph of either of the
C.H.I.P.'s two pin-headers:
![Pin-header printout](/img/gonzales.png)

## Warning
Due to directly manipulating the registers it bypasses the
Linux-kernel's pinctrl-scheme and you can very much mess things up
with it. You should only be using the tool, if you know what you
are doing.

**YOU ASSUME ALL RESPONSIBILITY FOR USE OF THIS TOOL.**

## Usage
The tool accepts the following commands: readPin, readPull, readMux,
writePin, writePull, writeMux, header1 and header2. All the
read-commands follow the same format, ie. `gonzales command port pin`
where port is a number ranging from 0 to 6 or a character from a to g,
and pin is 0 to 31. Port 0 corresponds to port A, port 1 to port B
and so on, and if given as a character, the character can be lowercase
or uppercase. Example: `gonzales readPin d 25` to read pin PD25.

The format for the write-commands is the same, except for the value
given:
- `writePin [port] [pin] [low/LOW/high/HIGH/0/1]`
- `writeMux [port] [pin] [in/input/INPUT/out/output/OUTPUT/0-6]`
  - 0 means INPUT, 1 means OUTPUT and the rest depends on the pin,
  and the command takes no human-readable input for those.
- `writePull [port] [pin] [up/UP/down/DOWN/disable/DISABLE/0/1/2]`
  - 0 means disable, 1 means pull-up, 2 means pull-down.

The commands `header1` and `header2` do not take any parameters.
