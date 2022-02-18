# R5 CMake BSP Build

This project contains the cmake infrastructure to build the R5 bsp using CMake, the project makes assumption about what libraries are generated as part of the BSP so it should be modified on a case by case basis

## How to integrate with your CMake projects

Add these to your project `CMakeLists.txt`:

```cmake
# set variables that this project needs
set(XSA_PATH path-to-your-xsa)
set(CUSTOM_VITIS_REPO_PATH path-to-your-custom-vitis-repo)

add_subdirectory(path_to_this_project "r5_bsp_build")

# after this, these variables are available for usage by subsequent CMake commands:
# R5_BSP_EXPORT_LIB_DIR: directories where the build libraries are
# R5_BSP_EXPORT_INCLUDE_DIR: directories of the bsp include
```

## How to customize the generated BSP

For every build, if the libraries need to be built(or rebuilt) the `build_bsp.tcl` is used to create the platform project and then build from there so any permanent changes to the BSP must be added in the script

When customizing the BSP, you should do it interactively first before adding the changes to the `build_bsp.tcl` script, to launch a Vitis IDE GUI windows with the platform project that's being used by the CMake to build the libraries:

```shell
cd <path-to-cmake-build-dir>
cmake --build . --target show_r5_bsp_in_vitis
```

Once the IDE has been launched, you can customize the BSP normally and then build inside the IDE, to link with the newly created libraries, simply run the normal CMake build(you can run this build in a separate terminal session from the one that was used to run `show_r5_bsp_in_vitis` target so you don't have to close the Vitis GUI for every build), the updated libraries should be propagated to all targets that use it in your CMake files

Once you are satisfied with the changes, modify the `build_bsp.tcl` accordingly. If you don't, the changes you have made in the Vitis GUI will be lost on the next clean build
