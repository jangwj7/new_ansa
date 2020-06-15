# About new_ansa
This program takes an ABAQUS input text file and displays the mesh. ABAQUS is a commercial FE-solver, and this program displays some of the thousands of available element types  in ABAQUS.

The name is a play on the charachter Yin Yan from the TV-show Silicon Valley, who has the app idea New Netflix.

ANSA is a commercial software for viewing and editing FE-mesh files and this small project emulates some of its features and borrows some color schemes.. Please don't sue me.

## Features
- Reads a text file following the ABAQUS syntax and displays a visual representation of the mesh
- Color elements based on section
- Drag and rotate model (arcball-style, as is typical in CAD-programs)

## Usage and demo
To launch program and read the included simple example input file "tubes.inp", run:
``new_ansa abaqus_examples/tubes.inp``

![](demo.gif)



