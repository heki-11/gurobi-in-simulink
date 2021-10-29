#define S_FUNCTION_NAME wraps_gurobi
#define S_FUNCTION_LEVEL 2
#include "simstruc.h"
#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"

// extern real_T gurobi_source(void);
 
static void mdlInitializeSizes(SimStruct *S){
    ssSetNumSFcnParams(S, 0);  
    ssSetNumContStates(S, 0);   
    ssSetNumDiscStates(S, 0); 
    
    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
        
    if (!ssSetNumOutputPorts(S,1)) return;
    ssSetOutputPortWidth(S, 0, 1); 
    
    ssSetNumSampleTimes(S, 1);   
}

static void mdlInitializeSampleTimes(SimStruct *S){
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0);
}


static void mdlOutputs(SimStruct *S, int tid){
//       InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);
      const real_T *u = (const real_T*) ssGetInputPortSignal(S,0);
      real_T            *y    = ssGetOutputPortRealSignal(S,0);

     // variables
      GRBenv   *env   = NULL;
      GRBmodel *model = NULL;
      int       error = 0;
      double    sol[3];
      int       ind[3];
      double    val[3];
      double    obj[3];
      char      vtype[3];
      int       optimstatus;
      double    objval;

    // error environment
      error = GRBemptyenv(&env);
      if (error) goto QUIT;
      error = GRBsetstrparam(env, "LogFile", "mip1.log");
      if (error) goto QUIT;
      error = GRBstartenv(env);
      if (error) goto QUIT;
    
     // model
      error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
      if (error) goto QUIT;
    
     // add objective
      obj[0] = 1; obj[1] = 1; obj[2] = 2;
      vtype[0] = GRB_BINARY; vtype[1] = GRB_BINARY; vtype[2] = GRB_BINARY;
      error = GRBaddvars(model, 3, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype,
                         NULL);
      if (error) goto QUIT;
      error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
      if (error) goto QUIT;
    
     // add constraint 1
      ind[0] = 0; ind[1] = 1; ind[2] = 2;
      val[0] = 1; val[1] = 2; val[2] = 3;
      error = GRBaddconstr(model, 3, ind, val, GRB_LESS_EQUAL, 4.0, "c0");
      if (error) goto QUIT;

    // add constraint 2
      ind[0] = 0; ind[1] = 1;
      val[0] = 1; val[1] = 1;
      error = GRBaddconstr(model, 2, ind, val, GRB_GREATER_EQUAL, 1.0, "c1");
      if (error) goto QUIT;
    
     // optimize
      error = GRBoptimize(model);
      if (error) goto QUIT;
    
     // write model
      error = GRBwrite(model, "mip1.lp");
      if (error) goto QUIT;
    
     // capture solution
      error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
      if (error) goto QUIT;
      error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
      if (error) goto QUIT;
      error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, 3, sol);
      if (error) goto QUIT;
      printf("\nOptimization complete\n");
      if (optimstatus == GRB_OPTIMAL) {
        printf("Optimal objective: %.4e\n", objval);
        printf("  x=%.0f, y=%.0f, z=%.0f\n", sol[0], sol[1], sol[2]);
      } else if (optimstatus == GRB_INF_OR_UNBD) {
        printf("Model is infeasible or unbounded\n");
      } else {
        printf("Optimization was stopped early\n");
      }
    QUIT:
    
     // error reporting
      if (error) {
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        exit(1);
      }
    
     // free model
      GRBfreemodel(model);
    
     // free environment
      GRBfreeenv(env);

     // return output
      *y = objval + *u;
}

static void mdlTerminate(SimStruct *S){
    UNUSED_ARG(S); 
}

#ifdef	MATLAB_MEX_FILE    
#include "simulink.c"   
#else
#include "cg_sfun.h"      
#endif
