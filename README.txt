/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: required information for compiling the project
Language: C++
Platform: Visual Studio 2019, Windows 10
Project: CS350_junki.kim_3
Author: Junki Kim, junki.kim, 180015012
Creation date: 2021-07-24
End Header --------------------------------------------------------*/ 

a. How to use parts of your user interface that is NOT specified in the assignment description.
a-1) How to ImGui
1st line) you can load new model object
2nd line) (new) you can load models in folder
3rd line)  there are radio button to change render mode
(Geometry(Polygon), Position, Normals, Albedo, Specular, Lighting, Each Node Color)
4th line) check box to enable copy depth info. check box for enable vertex / face normal.
5th line) combo box to choose bounding volume type
6th line) (new) combo box to choose structure type
7th line) slider to control hierarchy level
8th line) there is check box to enable lights orbiting
9th line) you can control the number of lights. range is 8-16 based on cs300 assignment

b. Any assumption that you make on how to use the application that, if violated, might cause
the application to fail.
If you try to test exe file in Debug/Release folder,
I left glew32.dll and assimp-vc142-mtd.dll in x64/Debug and x64/Release folder. It shouldn't be deleted.
Also, You need to move Debug/Release folder from x64 folder to the Solution Directory.
So, it can load shader correctly. (or copy Common folder in x64 folder)

Also, recommended resolution for monitor is 1920x1080 (extremely small resolution will cause wrong result)


c. Which part of the assignment has been completed?
1) All stuff about Assignment 1
2) All stuff about Assignment 2
2-1) Not completed part for A2 (Topdown method, loading PowerPlant)
3) Scene Creation 
4) Octree
5) BST Tree
    - Top down creation of the BSP tree
      What I used on BSP Tree is the same method which i used from
      HierarchyTopDownAABB. This method is partitioning a box volume
      to two boxes, so the appearence might be similar to Ocree.
6) Interactivity
    - Toggle Octree display
    - Show all levels of the octree in different colors
7) No Miscellaneous issues

d. Which part of the assignment has NOT been completed (not done, not working, etc.) and
explanation on why those parts are not completed?
1) BSP Tree - Choosing appropriate split planes
2) Serialization of tree
3) Interactivity - Toggle BSP tree display
4) Interactivity - Show all nodes of the BSP tree in a different color

e. Where the relevant source codes (both C++ and shaders) for the assignment are located.
Specify the file path (folder name), file name, function name (or line number).
1) TopDown method in HierarchyBottomUpAABB.cpp/h, HierarchyBottomUpSphere.h
2) PowerPlant loading in Object.cpp/h
3) Scene Creation in SceneSandbox.cpp/h
4) Octree in AdaptiveOctree.cpp/h
5) BST Tree creation with Topdown method in BSPTree.cpp/h
6) Each node color in Deffered_FSQ_Buffer.vs/fs, Deffered_G_Buffer.vs/cs

f. Which machine did you test your application on.
Windows 10, GTX1060, OpenGL 3.3 (330), 1920x1080

g. The number of hours you spent on the assignment, on a weekly basis
48 hours. Detail work is in Independent Study Activity Log.

h. Any other useful information pertaining to the application

Bottom-up hierarchies are not testable with one object, so please test it with a PowerPlant model which has many sub-models.

The level of hight in a hierarchy is reperented as color, red ~ orange ~ light blue).

If you want to see all level of hight in a hierarchy. Please set the hierarchy level to -1.