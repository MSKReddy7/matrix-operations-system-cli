# Matrix Operations System

A C++ console application for working with matrices and performing common linear algebra operations. The program stores matrices in a local JSON file, lets you create or edit saved matrices, and supports a range of operations from simple arithmetic to advanced checks and solving systems.

## Project overview

This project is designed as a lightweight matrix utility for learning and practice. It is implemented as a terminal-based application and does not require a graphical user interface. The app loads and saves matrices to a file named `matrices.json`, so your work can persist between runs.

The project includes:

- Matrix creation and saving
- Arithmetic operations
- Matrix analysis utilities
- Linear algebra problem solving
- Persistent storage using JSON
- Built-in sample matrices

## Main files

- `main.cpp` — complete application source code
- `matrices.json` — saved matrices and generated results
- `matrixapp.exe` — compiled Windows executable (generated from the C++ source)

## Features

The application menu includes the following operations:

1. Exit
2. Subtraction
3. Multiplication
4. Addition
5. Scalar multiplication
6. Transpose
7. Determinant
8. Inverse
9. Symmetric check
10. Identity check
11. Echelon / reduced echelon solver
12. Eigen value and vector
13. Create and save matrix
14. See / edit / delete saved matrices

### Supported matrix operations

- Addition of two matrices with matching dimensions
- Subtraction of two matrices with matching dimensions
- Multiplication of compatible matrices
- Scalar multiplication
- Transpose of a matrix
- Determinant for 2 x 2 and 3 x 3 square matrices
- Inverse for 3 x 3 matrices with non-zero determinant
- Symmetric and skew-symmetric detection
- Identity matrix check
- Gauss elimination and Gauss-Jordan reduction
- Dominant eigenvalue and eigenvector estimation using iteration
- Saving result matrices for later use

## How it works

### Matrix input

When creating or editing a matrix, the program asks for:

- Matrix name
- Number of rows (1 to 10)
- Number of columns (1 to 10)
- Each matrix value as a numeric input

All matrix values are stored as floating-point numbers (`float`), which makes the program suitable for general numerical work.

### Saving data

Matrices are stored in `matrices.json` in the following format:

```json
{
  "matrices": [
    {
      "name": "Example Matrix",
      "rows": 2,
      "cols": 2,
      "values": [
        [1, 2],
        [3, 4]
      ]
    }
  ]
}
```

The program loads this file automatically when launched and appends new matrices or results as needed.

### Default matrices

On first use or if the file does not yet contain matrices, the app can generate default sample matrices with sizes from 1x1 to 3x3. These are useful for testing operations immediately without manually creating data.

## Requirements

### For compiling from source

You need a C++ compiler such as:

- MinGW/`g++` on Windows
- `g++` on Linux/macOS

### For running the compiled executable

- A system capable of executing a Windows `.exe` binary if using `matrixapp.exe`
- Or a C++ runtime environment for compiled source code

## Build and run

### Option 1: Run the existing executable

Double-click `matrixapp.exe` in the project folder or run it from Command Prompt:

```powershell
matrixapp.exe
```

### Option 2: Compile from source

From the project folder:

```powershell
g++ main.cpp -o matrixapp.exe
```

Then run:

```powershell
matrixapp.exe
```

If you are using a different compiler or environment, the equivalent command may vary, but the source file is still `main.cpp` and the output can be named as desired.

## Typical usage flow

1. Run the program
2. Choose an operation from the menu
3. Select an existing matrix or create a new one
4. Enter required values or parameters
5. Review the result
6. Optionally save the result as a named matrix

## Example operations

### Addition

- Select two matrices with the same dimensions
- The program calculates `A + B`
- It can also multiply each matrix by a user-defined scalar before operating

### Multiplication

- Ensure the number of columns in the first matrix equals the number of rows in the second matrix
- The program computes the product matrix

### Gauss-Jordan solver

- Use an augmented matrix where columns = rows + 1
- The app performs reduction steps and prints the solution values

### Eigenvalue estimation

- Requires a square matrix
- Uses the power method to estimate the dominant eigenvalue and corresponding eigenvector

## Program behavior and limitations

- Maximum matrix size is 10 x 10
- Determinant support is limited to 2 x 2 and 3 x 3 matrices
- Inverse is currently implemented for 3 x 3 matrices only
- The application is console-based and not designed for a graphical interface
- It is intended for academic and demonstration use, not production-level numerical analysis software
- The program uses floating-point arithmetic, so results may show rounding behavior for some calculations

## Project purpose

This project demonstrates core matrix operations, file persistence, and numerical computation in C++. It is well suited for:

- Learning matrix algebra
- Testing matrix-handling logic in C++
- Building a simple academic project for coursework
- Expanding into a larger linear algebra toolkit

## Suggested future improvements

Potential upgrades for a more complete version of the project include:

- Support for larger matrix sizes and more robust numerical methods
- Better validation and error handling for malformed JSON
- GUI-based interface
- More advanced decomposition methods such as LU, QR, or SVD
- Better handling of determinant/inverse for arbitrary square matrices
- Improved output formatting and result previews

## License

This project does not appear to declare a formal license. If you plan to distribute or reuse it, check whether the project owner has licensing requirements before publishing or sharing modified versions.

## Summary

The Matrix Operations System is a practical C++ utility for calculating, checking, and saving matrices. It combines common linear algebra tasks with persistent storage and an interactive menu-driven interface, making it useful both as a study project and as a foundation for a more advanced matrix application.
