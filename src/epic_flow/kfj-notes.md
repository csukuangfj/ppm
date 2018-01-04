# parameters
## Nadaraya-Watson (NW) estimation
 - kernel: a = 1
 - number of nearest neighbors: 25
 
## Locally-weighted affine (LA) estimation
 - number of nearest neighbors: 100
 

number of input matches: 5000

saliency is determined by the eignevalues of the autocorrelation matrix.

The paper says DM + LA is better.

timing:
 - deep matching: 15s
 - SED: 0.15s
 - dense interpolation: 0.25s
 - variational minimization: 1s
 - total: **16.4s**
 
 
If the following macro is defined, then use OpenMP
```
#if defined(USE_OPENMP)
#pragma omp parallel for num_threads(n_thread)
#endif
```
`n_thread` is a constant variable
