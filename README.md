# Overview
This is just a basic Vulkan configuration project.
It can be useful as a base for a simple Vulkan application, or as a 
begginer-level Vulkan Tutorial.

## Tools
**Visual Studio 2022**

**CMake 4.0.1**


## Third party libraries
**GLFW 3.4** [[Github]](https://github.com/glfw/glfw) or [[Website]](https://www.glfw.org/)

**glm 1.0.1** aka OpenGL Mathematics [[Github]](https://github.com/g-truc/glm)

## Setup
This is only **one** way to configure the project! I preferred to do it this way.


1) Create a ***libraries*** folder inside the project folder
  
    - Paste in the ***libraries*** folder the **GLFW** and **GLM** **files**.
  
    - Not all the libraries files needs to be pasted, just the following:
      - **GLFW**: *include* and *lib-vc2022* folders
      - **GLM**: *glm* folder
   
    - (But if you don't want to make errors, just paste directly all the files :) )

<br>

2) Open the project Properties

    - Make sure that ***All Configurations*** is selected at the top of the window

    - Go to ***C++ -> General -> Additional Include Directories*** and click **<Edit...>**
      - Add the header directories for
        - **Vulkan** (C:\VulkanSDK\1.4.309.0\Include) or wherever you installed Vulkan
        - **GLFW** (C:\ ... \project folder\libraries\glfw-3.4.bin.WIN64\include)
        - **GLM** (C:\ ... \project folder\libraries\glm-1.0.1)

    - Go to ***Linker -> General -> Additional Library Directories*** and click **<Edit...>**
      - Add the object files for
        - **Vulkan** (C:\VulkanSDK\1.4.309.0\Lib)
        - **GLFW** (C:\ ... \project folder\libraries\glfw-3.4.bin.WIN64\lib-vc2022)

    - Go to ***Linker -> Input -> Additional Dependencies*** and click **<Edit...>**
      - Write the names of the Vulkan and GLFW object files
        - **Vulkan**: vulkan-1.lib
        - **GLFW**: glfw3.lib

    - Go to ***C++ -> Language -> C++ Language Standard*** and set it to C++17


    - Finally, click Apply and close the Properties window