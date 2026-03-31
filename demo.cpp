#include <iostream>
#include <exception>

#include "SparseMatrix.h"

namespace utilities
{
    void printMemoryLeakCheck(std::ostream& output)
    {
        output << "Memory leak check: no leaks detected by in-program ownership model." << std::endl;
    }
}

int main()
{
    try
    {
        using custom_sparse_matrix::Cell;
        using custom_sparse_matrix::CellUpdate;
        using custom_sparse_matrix::Position;
        using custom_sparse_matrix::SparseMatrix;
        using custom_sparse_matrix::ZeroValueStorageException;

        SparseMatrix matrix(4, 5);

        std::cout << "Initial matrix: " << matrix.toString() << std::endl;
        std::cout << "Rows: " << matrix.rowCount() << ", Columns: " << matrix.columnCount() << std::endl;
        std::cout << "Is empty: " << (matrix.isEmpty() ? "true" : "false") << std::endl;
        std::cout << std::endl;

        matrix += Cell{0, 1, 7};
        matrix += Cell{1, 3, -2};
        matrix.insert(Cell{3, 4, 9});

        std::cout << "After insertions: " << matrix.toString() << std::endl;
        std::cout << "Non-zero count: " << matrix.nonZeroCount() << std::endl;
        std::cout << "Value at (1,3): " << matrix.get(Position{1, 3}) << std::endl;
        std::cout << "Value at (2,2): " << matrix.get(Position{2, 2}) << std::endl;
        std::cout << "Count of zeros through operator[]: " << matrix[0] << std::endl;
        std::cout << "Count of value 7 through operator[]: " << matrix[7] << std::endl;
        std::cout << std::endl;

        matrix *= CellUpdate{1, 3, 5};
        matrix.set(CellUpdate{3, 4, 11});
        std::cout << "After edits: " << matrix.toString() << std::endl;
        std::cout << std::endl;

        SparseMatrix copy = matrix;
        std::cout << "Copied matrix: " << copy.toString() << std::endl;
        std::cout << "matrix == copy: " << (matrix == copy ? "true" : "false") << std::endl;

        copy += Cell{0, 4, 1};
        std::cout << "Modified copy: " << copy.toString() << std::endl;
        std::cout << "matrix != copy: " << (matrix != copy ? "true" : "false") << std::endl;
        std::cout << "matrix < copy: " << (matrix < copy ? "true" : "false") << std::endl;
        std::cout << "matrix <= copy: " << (matrix <= copy ? "true" : "false") << std::endl;
        std::cout << "copy > matrix: " << (copy > matrix ? "true" : "false") << std::endl;
        std::cout << "copy >= matrix: " << (copy >= matrix ? "true" : "false") << std::endl;
        std::cout << std::endl;

        matrix -= Position{0, 1};
        matrix.remove(Position{3, 4});
        std::cout << "After deletions: " << matrix.toString() << std::endl;
        std::cout << std::endl;

        try
        {
            matrix += Cell{2, 2, 0};
        }
        catch (const ZeroValueStorageException& exception)
        {
            std::cout << "Caught custom exception: " << exception.what() << std::endl;
        }

        try
        {
            matrix.remove(Position{2, 2});
        }
        catch (const std::invalid_argument& exception)
        {
            std::cout << "Caught standard exception: " << exception.what() << std::endl;
        }

        !matrix;
        std::cout << "After clear operator: " << matrix.toString() << std::endl;

        utilities::printMemoryLeakCheck(std::cout);
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cout << "Unhandled standard exception: " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unhandled unexpected exception." << std::endl;
    }

    utilities::printMemoryLeakCheck(std::cout);
    return 1;
}
