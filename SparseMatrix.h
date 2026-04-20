#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include <stdexcept>
#include <string>
#include <utility>

namespace Custom_sparse_matrix
{
    struct Position
    {
        int row;
        int column;
    };

    class ZeroValueStorageException : public std::logic_error
    {
    public:
        explicit ZeroValueStorageException(const std::string& message);
    };

    class SparseMatrix
    {
    private:
        class SparseMatrixImpl;
        SparseMatrixImpl* pimpl;

    public:
        SparseMatrix();
        SparseMatrix(int rowCount, int columnCount);
        SparseMatrix(const SparseMatrix& other);
        ~SparseMatrix();

        SparseMatrix& operator=(const SparseMatrix& other);

        SparseMatrix& operator+=(const std::pair<Position, int>& cell);
        SparseMatrix& operator-=(const Position& position);
        SparseMatrix& operator*=(const std::pair<Position, int>& update);

        int operator[](int value) const;
        void operator!();

        bool operator==(const SparseMatrix& other) const;
        bool operator!=(const SparseMatrix& other) const;
        bool operator<(const SparseMatrix& other) const;
        bool operator<=(const SparseMatrix& other) const;
        bool operator>(const SparseMatrix& other) const;
        bool operator>=(const SparseMatrix& other) const;

        void insert(const std::pair<Position, int>& cell);
        int get(const Position& position) const;
        void set(const std::pair<Position, int>& update);
        void remove(const Position& position);
        void clear();

        int rowCount() const;
        int columnCount() const;
        int nonZeroCount() const;
        bool isEmpty() const;
        std::string toString() const;
    };
}

#endif
