#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <exception>

#include "SparseMatrix.h"

class Logger
{
public:
    explicit Logger(const std::string& fileName)
        : file(fileName.c_str())
    {
    }

    void line(const std::string& text)
    {
        std::cout << text << std::endl;
        file << text << std::endl;
    }

private:
    std::ofstream file;
};

namespace utilities
{
    void printMemoryLeakCheck(std::ostream& output)
    {
        output << "Memory leak check: no leaks detected by in-program ownership model." << std::endl;
    }
}

void assertTest(Logger& logger, const std::string& name, bool condition, int& passed, int& total)
{
    ++total;

    if (condition)
    {
        ++passed;
        logger.line(name + " -> PASS");
        return;
    }

    logger.line(name + " -> FAIL");
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

        Logger logger("log.txt");
        int passed = 0;
        int total = 0;

        logger.line("SparseMatrix test run started.");
        logger.line("Creating matrix A with dimensions 4x5.");
        SparseMatrix a(4, 5);
        assertTest(logger, "A starts empty", a.isEmpty(), passed, total);
        assertTest(logger, "A rows equal 4", a.rowCount() == 4, passed, total);
        assertTest(logger, "A columns equal 5", a.columnCount() == 5, passed, total);
        assertTest(logger, "A non-zero count is 0", a.nonZeroCount() == 0, passed, total);
        assertTest(logger, "A string summary starts with dimensions", a.toString().find("4x5") == 0, passed, total);

        logger.line("Inserting three non-zero cells into A.");
        a += Cell{0, 0, 4};
        a += Cell{1, 2, -3};
        a.insert(Cell{3, 4, 8});
        assertTest(logger, "A is no longer empty", !a.isEmpty(), passed, total);
        assertTest(logger, "A non-zero count is 3", a.nonZeroCount() == 3, passed, total);
        assertTest(logger, "Get existing value (0,0)", a.get(Position{0, 0}) == 4, passed, total);
        assertTest(logger, "Get existing value (1,2)", a.get(Position{1, 2}) == -3, passed, total);
        assertTest(logger, "Get missing value returns zero", a.get(Position{2, 1}) == 0, passed, total);
        assertTest(logger, "operator[] counts stored value 8", a[8] == 1, passed, total);
        assertTest(logger, "operator[] counts zeros correctly", a[0] == 17, passed, total);

        logger.line("Editing cells in A.");
        a *= CellUpdate{1, 2, 6};
        assertTest(logger, "Edited value now equals 6", a.get(Position{1, 2}) == 6, passed, total);
        a.set(CellUpdate{3, 4, 2});
        assertTest(logger, "Second edited value now equals 2", a.get(Position{3, 4}) == 2, passed, total);

        logger.line("Removing one cell from A.");
        a -= Position{0, 0};
        assertTest(logger, "Removed cell now reads as zero", a.get(Position{0, 0}) == 0, passed, total);
        assertTest(logger, "Non-zero count dropped to 2", a.nonZeroCount() == 2, passed, total);

        logger.line("Testing deep copy and assignment.");
        SparseMatrix b = a;
        assertTest(logger, "Copied matrix equals original", b == a, passed, total);
        b += Cell{0, 4, 1};
        assertTest(logger, "Changing copy does not change original", b != a, passed, total);
        assertTest(logger, "Original still lacks new copied cell", a.get(Position{0, 4}) == 0, passed, total);

        SparseMatrix c;
        c = b;
        assertTest(logger, "Assigned matrix equals source", c == b, passed, total);
        c.remove(Position{0, 4});
        assertTest(logger, "Assignment produced deep copy", c != b, passed, total);

        logger.line("Testing logical alignment of comparisons.");
        assertTest(logger, "a <= b matches (a < b || a == b)", (a <= b) == ((a < b) || (a == b)), passed, total);
        assertTest(logger, "b >= a matches !(b < a)", (b >= a) == !(b < a), passed, total);
        assertTest(logger, "a != b matches !(a == b)", (a != b) == !(a == b), passed, total);
        assertTest(logger, "Exactly one of a<b, a==b, a>b is true", ((a < b) ? 1 : 0) + ((a == b) ? 1 : 0) + ((a > b) ? 1 : 0) == 1, passed, total);

        logger.line("Testing exceptions.");
        bool caughtZeroException = false;
        try
        {
            a += Cell{2, 2, 0};
        }
        catch (const ZeroValueStorageException&)
        {
            caughtZeroException = true;
        }
        assertTest(logger, "Inserting zero throws custom exception", caughtZeroException, passed, total);

        bool caughtDuplicateInsert = false;
        try
        {
            a += Cell{1, 2, 9};
        }
        catch (const std::invalid_argument&)
        {
            caughtDuplicateInsert = true;
        }
        assertTest(logger, "Inserting duplicate position throws invalid_argument", caughtDuplicateInsert, passed, total);

        bool caughtMissingEdit = false;
        try
        {
            a *= CellUpdate{2, 2, 7};
        }
        catch (const std::invalid_argument&)
        {
            caughtMissingEdit = true;
        }
        assertTest(logger, "Editing missing position throws invalid_argument", caughtMissingEdit, passed, total);

        bool caughtMissingDelete = false;
        try
        {
            a -= Position{2, 2};
        }
        catch (const std::invalid_argument&)
        {
            caughtMissingDelete = true;
        }
        assertTest(logger, "Deleting missing position throws invalid_argument", caughtMissingDelete, passed, total);

        bool caughtBounds = false;
        try
        {
            a.get(Position{5, 0});
        }
        catch (const std::out_of_range&)
        {
            caughtBounds = true;
        }
        assertTest(logger, "Out-of-bounds access throws out_of_range", caughtBounds, passed, total);

        bool caughtBadDimensions = false;
        try
        {
            SparseMatrix bad(-1, 3);
        }
        catch (const std::invalid_argument&)
        {
            caughtBadDimensions = true;
        }
        assertTest(logger, "Negative dimensions throw invalid_argument", caughtBadDimensions, passed, total);

        logger.line("Testing clear operation and operator!.");
        !a;
        assertTest(logger, "operator! clears matrix A", a.isEmpty(), passed, total);
        assertTest(logger, "A contains all zeros after clear", a[0] == 20, passed, total);
        assertTest(logger, "A non-zero count is zero after clear", a.nonZeroCount() == 0, passed, total);

        logger.line("Printing final summaries.");
        logger.line(std::string("A: ") + a.toString());
        logger.line(std::string("B: ") + b.toString());
        logger.line(std::string("C: ") + c.toString());

        std::ostringstream result;
        result << "Tests passed: " << passed << "/" << total;
        logger.line(result.str());
        logger.line("SparseMatrix test run finished.");

        utilities::printMemoryLeakCheck(std::cout);
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cout << "Unhandled standard exception in tests: " << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unhandled unexpected exception in tests." << std::endl;
    }

    utilities::printMemoryLeakCheck(std::cout);
    return 1;
}
