#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <z3++.h>

template <class T, class U = T>
class Matrix {

    public:

    /*
     *  Matrix()
     *
     *  Constructs a `Matrix` object with the specified dimensions
     *  (n x m) and initializes all elements to the specified default
     *  value (`def`).
     *
     *  Template Parameters:
     *    - `T`: The type of elements stored in the matrix.
     *    - `U`: (Optional) The type of the default value used to
     *       initialize the elements of the matrix. Defaults to `T`.
     *
     *  @n: The number of rows in the matrix.
     *  @m: The number of columns in the matrix.
     *  @def: (Optional) The default value used to initialize all
     *  elements of the matrix. If not specified, the elements are
     *  initialized using the default constructor of `T`.
     */
    Matrix(std::size_t n, std::size_t m, U def = {}) : base(n * m, T{def})  {
        
        std::size_t i;

        _n = n;
        _m = m;

        data = std::make_unique<T*[]>(n);
        for(i = 0; i < n; i++) {
            data[i] = &base[i * m];
        }
    }

    /*
     *  operator>>()
     *
     *  This function reads elements of the given Matrix object row by row
     *  from the provided input stream. The number of rows and columns in
     *  the matrix must already be defined. 
     *
     *  @is: The input stream from which the matrix elements are read.
     *  @matrix: The Matrix object whose elements will be populated with
     *  the data read from the input stream.
     *
     *  return:
     *    - A reference to the input stream (std::istream&) to allow
     *    chaining of input operations.
     */
    friend std::istream& operator>>(std::istream& is, Matrix<T>& matrix) {

        bool fail;

        std::size_t i;
        std::size_t j;
        std::size_t n;
        std::size_t m;

        n = matrix.n();
        m = matrix.m();

        fail = false;
        if(is.good()) {
            for(i = 0; i < n && !fail; i++) {
                for(j = 0; j < m; j++) {
                    if(!(is >> matrix.data[i][j])) {
                        is.clear(std::ios::failbit);
                        fail = true;
                        break;
                    }
                }
            }
        }

        return is;
    }

    /*
     *  operator<<()
     *
     *  This function writes the elements of a given Matrix object to an
     *  output stream. The matrix is output row by row, with elements in a
     *  row separated by spaces and rows separated by newlines. If an
     *  error occurs during writing, the function sets the failbit on the
     *  output stream.
     *
     *  @os: The output stream to which the matrix elements are written.
     *  @matrix: The Matrix object whose elements will be written to the
     *  stream.
     *
     *  return:
     *    - A reference to the output stream (std::ostream&) to allow
     *      chaining of output operations.
     */
    friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
        
        bool fail;

        std::size_t i;
        std::size_t j;
        std::size_t n;
        std::size_t m;

        n = matrix.n();
        m = matrix.m();

        fail = false;
        if(os.good()) {
            for(i = 0; i < n && !fail; i++) {
                for(j = 0; j < m; j++) {
                    if(!(os << matrix.data[i][j] << " ")) {
                        os.setstate(std::ios::failbit);
                        fail = true;
                        break;
                    }
                }
                if(i != n - 1) {
                    os << "\n";
                }
            }
        }

        return os;
    }

    /*
     *  operator()
     *
     *  Provides access to an element of the matrix at the specified row
     *  and column. The function performs bounds checking and throws an
     *  exception if the specified indices are out of range.
     *
     *  @i: The row index of the element to access (0-based).
     *  @j: The column index of the element to access (0-based).
     *
     *  return:
     *    - A reference to the element at position (i, j) in the matrix.
     *
     *  throws:
     *    - std::out_of_range if the indices (i, j) are not within the 
     *      valid bounds of the matrix (i >= _n or j >= _m).
     */
    T& operator()(std::size_t i, std::size_t j) {
        return at(*this, i, j);
    }

    /*
     *  operator() const
     *
     *  Provides read-only access to an element of the matrix at the
     *  specified row and column. The function delegates the actual 
     *  access and bounds checking to the const version of at(), 
     *  ensuring consistent behavior. If the indices are out of range,
     *  an exception is thrown.
     *
     *  @i: The row index of the element to access (0-based).
     *  @j: The column index of the element to access (0-based).
     *
     *  return:
     *    - A const reference to the element at position (i, j) in the 
     *      matrix.
     *
     *  throws:
     *    - std::out_of_range if the indices (i, j) are not within the
     *      valid bounds of the matrix (i >= _n or j >= _m).
     */
    const T& operator()(std::size_t i, std::size_t j) const {
        return at(*this, i, j);
    }

    /*
     *  n()
     *
     *  Returns the number of rows in the matrix.
     *
     *  return:
     *    - The number of rows (size_t) in the matrix.
     */
    std::size_t n() const {
        return _n;
    }

    /*
     *  m()
     *
     *  Returns the number of columns in the matrix.
     *
     *  return:
     *    - The number of columns (size_t) in the matrix.
     */
    std::size_t m() const {
        return _m;
    }

    private:

    /*
     *  at()
     *
     *  Provides safe access to an element at the specified row and
     *  column in the matrix. This function performs bounds checking
     *  to ensure that the provided indices are within the valid range
     *  of the matrix dimensions. If the indices are out of range, a
     *  `std::out_of_range` exception is thrown.
     *
     *  Template Parameters:
     *    - `MatrixType`: The type of the matrix being accessed. This
     *      allows the function to work with both `const` and
     *      non-`const` matrices, ensuring flexibility.
     *
     *  @mat: A reference to the matrix object to access.
     *  @i: The row index of the element to access (0-based).
     *  @j: The column index of the element to access (0-based).
     *
     *  Return:
     *    - A reference to the element at position `(i, j)` in the
     *      matrix. The return type depends on the `MatrixType`:
     *        - If `MatrixType` is `const`, a `const` reference is returned.
     *        - Otherwise, a non-`const` reference is returned.
     *
     *  Throws:
     *    - `std::out_of_range`: If the indices `(i, j)` are not within
     *      the valid bounds of the matrix dimensions:
     *        - `i >= mat._n` or `j >= mat._m`
     *        - Negative indices are inherently invalid as the
     *          parameters are unsigned integers (`std::size_t`).
     */
    template <class MatrixType>
    static auto& at(MatrixType& mat, std::size_t i, std::size_t j) {

        if(i >= mat._n || j >= mat._m) {
            throw std::out_of_range("Matrix index out of bounds.");
        }

        return mat.data[i][j];
    }

    private:

    std::size_t _n;
    std::size_t _m;

    std::vector<T> base;
    std::unique_ptr<T*[]> data;
};

#endif  /* MATRIX_HPP */
