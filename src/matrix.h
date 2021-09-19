#include "cursor.h"

enum IndexingStrategy
{
    BTREE,
    HASH,
    NOTHING
};

/**
 * @brief The Matrix class holds all information related to a loaded matrix. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a matrix object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */
class Matrix
{
    vector<unordered_set<int>> distinctValuesInColumns;

    public:
        string sourceFileName = "";
        string matrixName = "";
        
        uint N = 0;
        uint M = 32;

        uint maxR = 0;
        uint maxC = 0;

        uint nonZeroCount = 0;
        uint zeroCount = 0;

        float ratio = 0;
        bool isSparse = false;
       
        uint blockCount = 0;
        
        // bool extractColumnNames(string firstLine);

        bool blockifyNonSparseMatrix();
        bool blockifySparseMatrix();

        Matrix();
        Matrix(string matrixName);
        
        bool load();
        void print();
        void makePermanent();
        bool isPermanent();
        
        void unload();

        string getPagePath(int pageIndexRow, int pageIndexCol);
        void appendRowToPage(vector<int> row, int pageIndexRow, int pageIndexCol);

        void transpose();
        vector<vector<int>> fetchMatrixFromPage(int pageIndexRow, int pageIndexCol, int &inPageRowCount, int &inPageColCount);

        void writePage(vector<vector<int>> subMat, int pageIndexRow, int pageIndexCol, int inPageRowCount, int inPageColCount);
        


    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row 
     */

    template <typename T>
    void writeRow(vector<T> row, ostream &fout)
    {
        logger.log("Matrix::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
        {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row 
     */
    template <typename T>
    void writeRow(vector<T> row)
    {
        logger.log("Matrix::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};