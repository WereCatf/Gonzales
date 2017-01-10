# Chippy Gonzales
Chippy Gonzales is a tool for directly manipulating the C.H.I.P.'s
GPIO hardware-registers, allowing you to query and set things like
e.g. mux, pin-state and pull-up/-down.

The tool can also print out a prettyish graph of either of the
C.H.I.P.'s two pin-headers:
![Pin-header printout](/images/2017/01/gonzales.png)

## Warning
Due to directly manipulating the registers it bypasses the
Linux-kernel's pinctrl-scheme and you can very much mess things up
with it. You should only be using the tool, if you know what you
are doing.

**YOU ASSUME ALL RESPONSIBILITY FOR USE OF THIS TOOL.**
