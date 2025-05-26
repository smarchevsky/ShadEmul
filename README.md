Emulate shader languages on CPU.
Helps you to make CPU do the same things, that GPU does.

Supports GLSL and HLSL.

Supports STL and Unreal environment.

On the current stage of development it does basic math operations(overloading +-*/), swizzling (.xyz .zxxx, .yy - things), some math functions.
It doesn't understand 'in', 'out', 'inout', ternary operations don't work with swizzlers (cond ? vec.xx : vec.xy)
feel free to make it better.

Work in progress.
