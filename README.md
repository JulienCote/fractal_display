# fractal_display
Personal projet to play around with an interactive fractal display. This project is pretty basic as of now but it works :)

Controls:
* arrows     -> move around(translation)
* numpad0    -> Toggle CPU/GPU rendering
* numpad1    -> Doubles current precision, needed for bigger zooms
* numpad4    -> Halves current precesion
* numpad +   -> zoom in
* numpad -   -> zoom out
* left click -> recenter to mouse position

Technologies used (dependencies): 
* SFML
* Boost/Multiprecision
* CUDA (I'll try to make this one optional or support OpenCL too)

Currently looks like that: ![alt tag](https://github.com/JulienCote/fractal_display/blob/master/sample%20pictures/fractal5.PNG)
