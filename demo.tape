Output demo.gif

Set Shell "bash"
Set FontSize 32
Set Width 1200
Set Height 600
Set TypingSpeed 130ms
Set Theme {"name": "Whimsy", "black": "#535178", "red": "#ef6487",
    "green": "#5eca89", "yellow": "#d69a01ff", "blue": "#65aef7",
    "magenta": "#aa7ff0", "cyan": "#43c1be", "white": "#ffffff",
    "brightBlack": "#535178", "brightRed": "#ef6487", "brightGreen": "#5eca89",
    "brightYellow": "#fdd877", "brightBlue": "#65aef7", "brightMagenta": "#aa7ff0",
    "brightCyan": "#43c1be", "brightWhite": "#ffffff", "background": "#29283b",
    "foreground": "#b3b0d6", "selection": "#3d3c58", "cursor": "#b3b0d6" }

Require "./build/CalcX"

Hide
Type "echo -ne '\e[5 q'" Enter # change cursor to line
Type "./build/CalcX" Enter
Ctrl+L
Sleep 1ms
Show

Type "5!" Sleep 400ms Enter Sleep 500ms
Type "y = x = sin(3pi / 2" Right@100ms Sleep 400ms Enter Sleep 500ms
Type "x >= 0" Sleep 400ms Enter Sleep 500ms
Type "tau == 2pi" Sleep 400ms Enter Sleep 500ms
Type "y = y + x(25 + tau" Right@100ms Sleep 400ms Enter Sleep 500ms
Type "2^3 + sqrt 9" Sleep 400ms Enter Sleep 500ms
Type "factorial(5" Right@100ms Sleep 400ms Enter Sleep 500ms
Type "min(3, 5, -1" Right@100ms Sleep 400ms Enter Sleep 500ms
Type "3!!" Sleep 400ms Enter Sleep 500ms
Type "arc" Tab@120ms 2 Type "c" Tab@120ms Type "(0" Right@100ms Type " == pi/2" Sleep 400ms Enter Sleep 500ms
Type ":q" Sleep 400ms Enter Sleep 500ms

Sleep 5s

Hide Ctrl+D
