#include "SparseMatrix.h"

#include <sstream>

namespace custom_sparse_matrix
{
    ZeroValueStorageException::ZeroValueStorageException(const std::string& message)
        : std::logic_error(message)
    {
    }

    class SparseMatrix::SparseMatrixImpl
    {
    public:
        struct Node
        {
            int row;
            int column;
            int value;
            Node* next;

            Node(int row, int column, int value)
                : row(row), column(column), value(value), next(nullptr)
            {
            }
        };

        struct SearchResult
        {
            Node* previous;
            Node* current;
        };

        Node* head;
        int rows;
        int columns;
        int nonZero;

        SparseMatrixImpl()
            : head(nullptr), rows(0), columns(0), nonZero(0)
        {
        }

        SparseMatrixImpl(const SparseMatrixImpl& other)
            : head(nullptr), rows(other.rows), columns(other.columns), nonZero(0)
        {
            copyFrom(other);
        }

        ~SparseMatrixImpl()
        {
            destroyAll();
        }

        static bool isBefore(int leftRow, int leftColumn, int rightRow, int rightColumn)
        {
            if (leftRow != rightRow)
            {
                return leftRow < rightRow;
            }

            return leftColumn < rightColumn;
        }

        void validateDimensions(int rowCount, int columnCount) const
        {
            if (rowCount < 0 || columnCount < 0)
            {
                throw std::invalid_argument("Matrix dimensions must be non-negative.");
            }
        }

        void validatePosition(int row, int column) const
        {
            if (row < 0 || row >= rows || column < 0 || column >= columns)
            {
                throw std::out_of_range("Matrix position is out of bounds.");
            }
        }

        SearchResult locate(int row, int column) const
        {
            Node* previous = nullptr;
            Node* current = head;

            while (current != nullptr && isBefore(current->row, current->column, row, column))
            {
                previous = current;
                current = current->next;
            }

            return SearchResult{previous, current};
        }

        bool matches(Node* node, int row, int column) const
        {
            return node != nullptr && node->row == row && node->column == column;
        }

        Node* findNode(int row, int column) const
        {
            SearchResult result = locate(row, column);
            return matches(result.current, row, column) ? result.current : nullptr;
        }

        void copyFrom(const SparseMatrixImpl& other)
        {
            Node* source = other.head;
            Node* tail = nullptr;

            while (source != nullptr)
            {
                Node* node = new Node(source->row, source->column, source->value);

                if (head == nullptr)
                {
                    head = node;
                }
                else
                {
                    tail->next = node;
                }

                tail = node;
                source = source->next;
                ++nonZero;
            }
        }

        void destroyAll()
        {
            Node* current = head;

            while (current != nullptr)
            {
                Node* next = current->next;
                delete current;
                current = next;
            }

            head = nullptr;
            nonZero = 0;
        }

        void insertNode(int row, int column, int value)
        {
            validatePosition(row, column);

            if (value == 0)
            {
                throw ZeroValueStorageException("Sparse matrix stores only non-zero values.");
            }

            SearchResult result = locate(row, column);

            if (matches(result.current, row, column))
            {
                throw std::invalid_argument("Cell already exists. Use set for editing.");
            }

            Node* node = new Node(row, column, value);
            node->next = result.current;

            if (result.previous == nullptr)
            {
                head = node;
            }
            else
            {
                result.previous->next = node;
            }

            ++nonZero;
        }

        int getValue(int row, int column) const
        {
            validatePosition(row, column);
            Node* node = findNode(row, column);
            return node == nullptr ? 0 : node->value;
        }

        void updateNode(int row, int column, int newValue)
        {
            validatePosition(row, column);
            Node* node = findNode(row, column);

            if (node == nullptr)
            {
                throw std::invalid_argument(newValue == 0
                    ? "Cannot edit a missing cell to zero."
                    : "Cannot edit a missing cell. Use insert first.");
            }

            if (newValue == 0)
            {
                removeNode(row, column);
                return;
            }

            node->value = newValue;
        }

        void removeNode(int row, int column)
        {
            validatePosition(row, column);
            SearchResult result = locate(row, column);

            if (!matches(result.current, row, column))
            {
                throw std::invalid_argument("Cannot remove a cell that is not stored.");
            }

            Node* target = result.current;

            if (result.previous == nullptr)
            {
                head = target->next;
            }
            else
            {
                result.previous->next = target->next;
            }

            delete target;
            --nonZero;
        }

        int countValue(int value) const
        {
            if (value == 0)
            {
                return rows * columns - nonZero;
            }

            int count = 0;
            Node* current = head;

            while (current != nullptr)
            {
                if (current->value == value)
                {
                    ++count;
                }

                current = current->next;
            }

            return count;
        }

        int compareTo(const SparseMatrixImpl& other) const
        {
            if (rows != other.rows)
            {
                return rows < other.rows ? -1 : 1;
            }

            if (columns != other.columns)
            {
                return columns < other.columns ? -1 : 1;
            }

            Node* left = head;
            Node* right = other.head;

            while (left != nullptr && right != nullptr)
            {
                if (left->row != right->row)
                {
                    return left->row < right->row ? -1 : 1;
                }

                if (left->column != right->column)
                {
                    return left->column < right->column ? -1 : 1;
                }

                if (left->value != right->value)
                {
                    return left->value < right->value ? -1 : 1;
                }

                left = left->next;
                right = right->next;
            }

            if (left == nullptr && right == nullptr)
            {
                return 0;
            }

            return left == nullptr ? -1 : 1;
        }

        std::string summary() const
        {
            std::ostringstream output;
            output << rows << "x" << columns << " nz=" << nonZero << " entries=[";

            Node* current = head;
            bool first = true;

            while (current != nullptr)
            {
                if (!first)
                {
                    output << "; ";
                }

                output << "(" << current->row << "," << current->column << ")=" << current->value;
                first = false;
                current = current->next;
            }

            output << "]";
            return output.str();
        }
    };

    SparseMatrix::SparseMatrix()
        : pimpl(new SparseMatrixImpl())
    {
    }

    SparseMatrix::SparseMatrix(int rowCount, int columnCount)
        : pimpl(new SparseMatrixImpl())
    {
        pimpl->validateDimensions(rowCount, columnCount);
        pimpl->rows = rowCount;
        pimpl->columns = columnCount;
    }

    SparseMatrix::SparseMatrix(const SparseMatrix& other)
        : pimpl(new SparseMatrixImpl(*other.pimpl))
    {
    }

    SparseMatrix::~SparseMatrix()
    {
        delete pimpl;
    }

    SparseMatrix& SparseMatrix::operator=(const SparseMatrix& other)
    {
        if (this == &other)
        {
            return *this;
        }

        SparseMatrixImpl* newData = new SparseMatrixImpl(*other.pimpl);
        delete pimpl;
        pimpl = newData;
        return *this;
    }

    SparseMatrix& SparseMatrix::operator+=(const Cell& cell)
    {
        insert(cell);
        return *this;
    }

    SparseMatrix& SparseMatrix::operator-=(const Position& position)
    {
        remove(position);
        return *this;
    }

    SparseMatrix& SparseMatrix::operator*=(const CellUpdate& update)
    {
        set(update);
        return *this;
    }

    int SparseMatrix::operator[](int value) const
    {
        return pimpl->countValue(value);
    }

    void SparseMatrix::operator!()
    {
        clear();
    }

    bool SparseMatrix::operator==(const SparseMatrix& other) const
    {
        return pimpl->compareTo(*other.pimpl) == 0;
    }

    bool SparseMatrix::operator!=(const SparseMatrix& other) const
    {
        return !(*this == other);
    }

    bool SparseMatrix::operator<(const SparseMatrix& other) const
    {
        return pimpl->compareTo(*other.pimpl) < 0;
    }

    bool SparseMatrix::operator<=(const SparseMatrix& other) const
    {
        return (*this < other) || (*this == other);
    }

    bool SparseMatrix::operator>(const SparseMatrix& other) const
    {
        return !(*this <= other);
    }

    bool SparseMatrix::operator>=(const SparseMatrix& other) const
    {
        return !(*this < other);
    }

    void SparseMatrix::insert(const Cell& cell)
    {
        pimpl->insertNode(cell.row, cell.column, cell.value);
    }

    int SparseMatrix::get(const Position& position) const
    {
        return pimpl->getValue(position.row, position.column);
    }

    void SparseMatrix::set(const CellUpdate& update)
    {
        pimpl->updateNode(update.row, update.column, update.newValue);
    }

    void SparseMatrix::remove(const Position& position)
    {
        pimpl->removeNode(position.row, position.column);
    }

    void SparseMatrix::clear()
    {
        pimpl->destroyAll();
    }

    int SparseMatrix::rowCount() const
    {
        return pimpl->rows;
    }

    int SparseMatrix::columnCount() const
    {
        return pimpl->columns;
    }

    int SparseMatrix::nonZeroCount() const
    {
        return pimpl->nonZero;
    }

    bool SparseMatrix::isEmpty() const
    {
        return pimpl->nonZero == 0;
    }

    std::string SparseMatrix::toString() const
    {
        return pimpl->summary();
    }
}
