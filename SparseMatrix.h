#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include <stdexcept>
#include <string>

namespace custom_sparse_matrix
{
    struct Position
    {
        int row;
        int column;
    };

    struct Cell
    {
        int row;
        int column;
        int value;
    };

    struct CellUpdate
    {
        int row;
        int column;
        int newValue;
    };

    class ZeroValueStorageException : public std::logic_error
    {
    public:
        explicit ZeroValueStorageException(const std::string& message);
    };

    class SparseMatrix
    {
    public:
        SparseMatrix();
        SparseMatrix(int rowCount, int columnCount);
        SparseMatrix(const SparseMatrix& other);
        ~SparseMatrix();

        SparseMatrix& operator=(const SparseMatrix& other);

        SparseMatrix& operator+=(const Cell& cell);
        SparseMatrix& operator-=(const Position& position);
        SparseMatrix& operator*=(const CellUpdate& update);

        int operator[](int value) const;
        void operator!();

        bool operator==(const SparseMatrix& other) const;
        bool operator!=(const SparseMatrix& other) const;
        bool operator<(const SparseMatrix& other) const;
        bool operator<=(const SparseMatrix& other) const;
        bool operator>(const SparseMatrix& other) const;
        bool operator>=(const SparseMatrix& other) const;

        void insert(const Cell& cell);
        int get(const Position& position) const;
        void set(const CellUpdate& update);
        void remove(const Position& position);
        void clear();

        int rowCount() const;
        int columnCount() const;
        int nonZeroCount() const;
        bool isEmpty() const;
        std::string toString() const;

    private:
        class SparseMatrixImpl;
        SparseMatrixImpl* pimpl;
    };
}

#endif
