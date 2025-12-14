# Circuit Simulator

A C++ circuit simulation project for modeling and analyzing electrical circuits by constructing and solving system equations programmatically.

This project is focused on learning circuit theory, numerical methods, and software architecture by building a simulator from scratch.

## How It’s Made
This Circuit Simulator project was made in C++. It simulates transient circuits with linear components. The simulator represents electrical components as objects and assembles them into a circuit model. 
The model is then passed to the solver which uses Modified Nodal Analysis (MNA) to create and solve a system of equations that represents the voltage and current constraints of each component.
A matrix is formed, then solved using a matrix solving function. The node voltage data is read from the matrix and cycled back into the component objects where it can be solved again for every time step.
The simulator features an oscilloscope tool that allows the user to view the voltage waveform of a component of their choice. 

### Current Limitations
- Only linear components are supported
- Only transient analysis is implemented
- No nonlinear devices or AC/DC sweep analysis yet
  
### Technologies Used
- C++
- Object-Oriented Design
- Numerical Linear Algebra


### Implementation Details
Circuit components (such as resistors and sources) are modeled as individual classes. These components contribute to a system that is assembled and solved numerically to determine voltages and currents.

The codebase is being extensively refactored to improve:
- Data flow and readability
- Separation between simulation logic and control
- Long-term maintainability and extensibility


## Optimizations

- Refactored tightly coupled logic into clearer abstractions
- Improved code organization to support future features

---

## Lessons Learned

- Early design decisions heavily impact maintainability
- New transient circuit simulation methods
- Git branches are critical for safely restructuring large codebases

---

## Project Status

This project is under active development and serves as a learning-focused circuit simulation framework.
