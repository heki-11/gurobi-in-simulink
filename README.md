*******************************************************************
   **How to compile C-coded S function that uses Gurobi library**    
            By Heejin Kim (heejink@umich.edu)                      
*******************************************************************

I was so frustrated that there was no clear step-by-step guide on how to use Gurobi libraries in Simulink (for hardware implementation settings, so an M-coded S function nor MATLAB Fcn blocks would not typically work) anywhere on the Internet. After spending three whole days, I just created a guide by myself. So here it is.

**1. Pre-requisites**

Because the Gurobi library doesn't support MinGW compiler, you need to download VS 2017 build tools if you have not already, using this link: https://my.visualstudio.com/Downloads?q=visual%20studio%202017&wt.mc_id=o~msft~vscom~older-downloads. Then, make this the default compiler in MATLAB by commanding:
```
mex -v -setup C  
```
and select VS2017.

**Note:** The compiler used in your hardware should also support VS2017 (or other architectures that MATLAB compiler supports, like CMake). If not, then there is NO way to implement Gurobi in your hardware because it will not be able to link the library. Moreover, some hardwares might not be able to link any commercial library files. In that case, you'll need to have a C source code for the library and compile it using the hardware's own compiler.

**2. Write a C-coded level 2 S function**

A template of S function can be found here: https://www.mathworks.com/help/simulink/sfg/templates-for-c-s-functions.html. Below is what you need to do:
  - change function name in #define S_FUNCTION_NAME
  - include appropriate headers, i.e., #include <stdlib.h>, <stdio.h>, <math.h>, and "gurobi_c.h" (assuming you have gurobi_c.h in the same directory as the S function)
  - in mdlOutputs, that is where you can call Gurobi libraries like GRBenv, GRBemptyenv, etc. Assign output accordingly.

**3. Configure Simulink**

In Simulink, go to "Model Settings" --> "Simulation Target" --> "Additional build information". Then, you need to:
  - in "Include directories", include the path of the Gurobi library inside quotation marks ""
  - in "Libraries", include gurobi_c++mdd2017.lib (or whatever version of VS you are using) and gurobi91.lib

**4. MEX S-function**

Use this command in MATLAB to compile the S function (do not insert space between L/l and the library names):
```
mex -'L{path of gurobi library}' -lgurobi_c++mdd2017 -lgurobi91 {s function name}.c
```
If compiled successfully, you will get the message:
```
Building with 'Microsoft Visual C++ 2017 (C)'.
MEX completed successfully.
```

**5. Generate Simulink model that uses S function**

Finally, you can use the "S function" block in Simulink. Write the S function name from Step 2 in the "Block Parameters" -> "S-function name" and run the model. I recommend having some "printf" functions in S function so that you can check whether S function will actually run on hardware settings. If the Simulink does not print anything in the Diagnostics window but is running, then that means it can only compile in MATLAB setting but will give an error during hardware implementation because of linking error.

