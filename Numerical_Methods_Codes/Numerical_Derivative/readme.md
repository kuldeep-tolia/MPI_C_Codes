Problem Description:  

-> Consider an equation $u(x) = x tan(x)$ that governs the transient heat conduction in a plane wall for $x = [-1,1]$.  
-> The analytical expression for the first derivative of the given equation is  
$$\frac{du}{dx} = tan(x) + x sec^2x$$  
-> This is a MPI program to compute the first derivative using second-order accurate central-difference formulae.  
-> The grid size is varied as $\Delta x = 0.01, 0.001$  
-> The following is the central-difference formulae to compute the first derivative, along with the corresponding truncation errors:

- $2^{nd}$ order accurate:  
$$\left. \frac{du}{dx} \right|_{1} = \frac{u_2 - u_0}{2 \Delta x}, TE \sim (\Delta x^2)$$,

where $TE$ denotes the truncation error.  
-> The results are compared with the analytical solution and plotted.  

NOTE: The corresponding $2^{nd}$ order accurate one-sided finite-difference formulae is used to compute the first derivative near the boundary location nodes.  
