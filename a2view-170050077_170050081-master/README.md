# a2view-170050077_170050081
    170050077 Nama N V S S Hari Krishna
    170050081 Doddi Sailendra Bathi Babu

# OpenGL 3D viewing tool
## Running the Program

To run this program, type "make" and then execute the a2view file in bin folder using "./bin/a2view"

## Using the Program

The window is initially loaded with the default configuration which is as follows -
window_height = 1024 pixels
window_width = 1024 pixels

The grid of size 30x30x30 is initialized with the scene parsed from "myscene.scn". There are 5 modes available; *WCS*, *VCS*, *CCS*, *NDCS* and *DCS*. By default program will start in *WCS* mode.

Keyboard Functions :-
1. **0** - *WCS* mode
2. **1** - *VCS* mode
3. **2** - *CCS* mode
4. **3** - *NDCS* mode
5. **4** - *DCS* mode
6. **UP/DOWN** - Will rotate the model about X-axis
7. **LEFT/RIGHT** - Will rotate the model about Y-axis
8. **[/]** - Will rotate the model about Z-axis
9. **H/h** - Will hide the grid lines