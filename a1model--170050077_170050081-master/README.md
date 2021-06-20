# a1model--170050077_170050081
    170050077 Nama N V S S Hari Krishna
    170050081 Doddi Sailendra Bathi Babu

# OpenGL 3D modelling tool
## Running the Program

To run this program, type "make" and then execute the a1model file in bin folder using "./bin/a1model"

## Using the Program

The window is initially loaded with the default configuration which is as follows -
window_height = 1024 pixels
window_width = 1024 pixels

The grid of size 20x20x20 is initialized with a GREEN cursor cube of unit cell size at the origin. There are two modes of operation, *INSPECTION* and *MODELLING* modes. By default program will start in *INSPECTION* mode.

Keyboard Functions :-
1. **i/I** - Activate *INSPECTION* mode
2. **m/M** - Activate *MODELLING* mode
3. **h/H** - Will hide/display the grid
4. **UP/DOWN** - Will rotate the model about X-axis
5. **LEFT/RIGHT** - Will rotate the model about Y-axis
6. **[/]** - Will rotate the model about Z-axis

The following functionalities are available only in *MODELLING* mode
1. **x/X** - Move the cursor cube one grid cell to the left/right along the X-axis
2. **y/Y** - Move the cursor cube one grid cell to the left/right along the Y-axis
3. **z/Z** - Move the cursor cube one grid cell to the left/right along the Z-axis
4. **p/P** - Fills that grid cell by placing a cube there with current drawing colour
5. **d/D** - Deletes the grid cell
6. **c/C** - Changes current drawing color by prompting to enter R/G/B values on the terminal. R,G,B values are float values from 0.0 to 1.0. Everything that is drawn subsequently is drawn with this color
7. **r/R** - Resets the grid by clearing all grid cells
8. **s/S** - Saves the current state of the grid cells(with filename given in prompt)
9. **k/K** - Clears the current grid, loads a file into the grid(filename given in prompt)
