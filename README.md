This github project is meant to give any Game Engine Design students from TUM working solutions for past tasks.

## How to use

Simply clone and pull the branches you need.

You still need some files like meshes or converter tools from external. This project expects the git project directory and the external directory have the same parent directory.

As all compiled stuff isn't commited you should perform a git clean ignored files when switching branches.

```
git clean -f -X -d
```

add -n if you want to see whats going to be removed.

### Solution02

This branch offers a solution for Assignment 02 and can be used as a template for Assignment 03.

Remember to set TerrainGenerator as startup project. Use TerrainViewer tool in order heightmap, colormap, normalmap to view your heightmap.

### Solution03

This branch offers a solution for Assignment 03 and can be used as a template for Assignment 04.

Use TerrainViewer tool in order heightmap, colormap, normalmap to view your heightmap.
You need to use the 3 .tiff files though only Heightmap is in resources folder. You can find the other in the Debug/Release folder of the RessourceGenerator.

### Solution04

This branch offers a solution for Assignment 04 and can be used as a template for Assignment 05.

### Solution05

This branch offers a solution for Assignment 05 and can be used as a template for Assignment 06.

Simply run in debug or release mode to see your rendered terrain :). 
Some little black fields are bugs due to high slope in normal calculation and probably float inaccuracies.

### Solution06
This branch offers a solution for Assignment 06 and can be used as a template for Assignment 07.

The nmake line can be found in nmake_text.txt for better readability. You still have to set it in VisualStudio though.