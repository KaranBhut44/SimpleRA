#include "global.h"


/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param matrixName 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}


/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    
    ifstream fin(this->sourceFileName, ios::in);
    
    char ch;
    string numberString = "";
    int number = 0;

    this->nonZeroCount = 0;
    this->zeroCount = 0;

    this->N = 0;
    while (fin.get(ch))
    {
        if(ch == '\n'){
            (this->N)++;    
        }
        
        if(ch == ',' || ch == '\n'){
            number = stoi(numberString);
            if(number == 0){
                (this->zeroCount)++;
            }else{
                (this->nonZeroCount)++;
            }

            numberString = "";
        
        }else if(ch<='9' && ch>='0'){

            numberString.push_back(ch);
        }        
    }

    this->ratio = (this->zeroCount / (this->nonZeroCount + this->zeroCount));
    
    if(this->ratio > 0.6){
        
        blockifySparseMatrix();
        this->isSparse = true;

    }else{

        blockifyNonSparseMatrix();
    }
    return true;
}

string Matrix::getPagePath(int pageIndexRow, int pageIndexCol){
    return "../data/temp/" + this->matrixName + "_Page_{" + to_string(pageIndexRow) + "_" + to_string(pageIndexCol) +"}";
}

void Matrix::appendRowToPage(vector<int> row, int pageIndexRow, int pageIndexCol){

    string pagePath = getPagePath(pageIndexRow, pageIndexCol);
    ofstream fout(pagePath, ios::app);

    int loop = row.size();

    fout << row[0];
    for(int i=1; i < loop; i++)
        fout << " " << row[i];

    fout<<endl;
    fout.close();
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockifyNonSparseMatrix()
{
    logger.log("Matrix::blockifyNonSparseMatrix");

    this->maxR = this->maxC = ceil((float)N/M);
    this->blockCount = this->maxR * this->maxC;

    logger.log("Page::initializePages");
    for(int r = 1; r <= maxR; r++){
        
        for(int c = 1; c <= maxC; c++){
            
            string pagePath = getPagePath(r, c);
            
            ofstream fout(pagePath, ios::trunc);
            fout.close();
        }
    }

    ifstream fin(this->sourceFileName, ios::in);
    
    char ch;
    int number = 0;

    string numberString = "";

    float currRow = 1;
    float currCol = 1;

    vector<int> rowNums;

    logger.log("Page::writePages");
    while (fin.get(ch))
    {
        
        if( ch == '\n' || ch == ','){

            number = stoi(numberString);
            numberString = "";

            rowNums.push_back(number);

            if(ch == '\n' || rowNums.size() == this->M){
              
                int R = ceil(currRow / this->M);
                int C = ceil(currCol / this->M);

                appendRowToPage(rowNums, R, C);
                rowNums.clear();
            }

            if(ch == '\n'){
                currRow++;
                currCol = 0;
            }else{
                currCol++;
            }

        }else if(ch<='9' && ch>='0'){
            numberString.push_back(ch);
        }
    }

    return true;
}


vector<vector<int>> Matrix::fetchMatrixFromPage(int pageIndexRow, int pageIndexCol, int &inPageRowCount, int &inPageColCount){

    vector<vector<int>> subMat(this->M, vector<int>(this->M));

    string pagePath = getPagePath(pageIndexRow, pageIndexCol);
    ifstream fin(pagePath, ios::in);

    for(int mr = 0; mr < this->M; mr++){
        for(int mc = 0; mc <= this->M; mc++){
            
            char ch;
            string numberString = "";

            while(fin.get(ch)){

                if(ch == ' ' || ch == '\n')
                    break;

                numberString.push_back(ch);
            }

            if(numberString == "")
                return subMat;

            int number = stoi(numberString);
            subMat[mr][mc] = number;
            
            inPageColCount = max(inPageColCount, mc);

            if(ch == '\n')
                break;
        }

        inPageRowCount = max(inPageRowCount, mr);
    }

    return subMat;
}

void Matrix::writePage(vector<vector<int>> subMat, int pageIndexRow, int pageIndexCol, int inPageRowCount, int inPageColCount){

    string pagePath = getPagePath(pageIndexRow, pageIndexCol);
    ofstream fout(pagePath, ios::trunc);

    for(int r=0; r<=inPageRowCount; r++){
                
        fout << subMat[r][0];
        for(int c=1; c <= inPageColCount; c++)
            fout << " " << subMat[r][c];

        fout<<endl;
    }

    fout.close();
}

void Matrix::transpose(){

    logger.log("Matrix::transposeMatrix");

    int inPageColCount=0, inPageRowCount=0;
    vector<vector<int>> subMat;

    for(int r = 1; r <= this->maxR; r++){
        for(int c = 1; c <= this->maxC; c++){

            inPageColCount = inPageRowCount = 0;
            subMat = fetchMatrixFromPage(r, c, inPageRowCount, inPageColCount);

            for(int i=0; i<this->M; i++){
                for(int j=0; j<i; j++){
                    swap(subMat[i][j], subMat[j][i]);
                }
            }

            writePage(subMat, r, c, inPageColCount, inPageRowCount);
        }
    }
    cout<<"InPlaceTransposeDone"<<endl;

    int ttt = 0;

    for(int r=1; r<= this->maxR; r++){
        for(int c=1; c<r; c++){

            string pagePath1 = getPagePath(r, c);
            string pagePath2 = getPagePath(c ,r);

            string temp = "temp";

            rename(pagePath1.c_str(), temp.c_str());
            rename(pagePath2.c_str(), pagePath1.c_str());
            rename(temp.c_str(), pagePath2.c_str());
        }
    }
    cout<<"RenameDone"<<endl;

}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockifySparseMatrix()
{
    logger.log("Matrix::blockifySparseMatrix");
    return true;
}

/**
 * @brief Function prints the first few rows of the matrix. If the matrix contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");

    int currRow = 0;

    for(int r=1; r <= this->maxR; r++){

        vector<ifstream *> pagePointers(this->maxC+1, NULL);

        for(int c=1; c <= this->maxC; c++){
            
            string pagePath = getPagePath(r, c);
            pagePointers[c] = new ifstream(pagePath, ios::in);
        }

        for(int rr = 0; rr < this->M; rr++){
            for(int c=1; c <= this->maxC; c++){
                for(int cc = 0; cc < this->M; cc++){
                
                    char ch;
                    string numberString = "";

                    while(pagePointers[c]->get(ch)){

                        if(ch == ' ' || ch == '\n')
                            break;

                        numberString.push_back(ch);
                    }

                    int number = stoi(numberString);
                    cout<<number<< " ";

                    if(ch == '\n')
                        break;
                }
            }
            cout<<endl;

            currRow++;
            if(currRow == this->N)
                break;
        }

        for(int c=1; c <= this->maxC; c++){
            pagePointers[c]->close();
        }

        if(currRow == this->N)
            break;

        return;
    }    
}


/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    // this->writeRow(this->columns, fout);

    Cursor cursor(this->matrixName, 0);
    vector<int> row;
    // for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    // {
    //     row = cursor.getNext();
    //     this->writeRow(row, fout);
    // }
    fout.close();
}

/**
 * @brief Function to check if matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the matrix from the database by deleting
 * all temporary files created as part of this matrix
 *
 */
void Matrix::unload(){
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}
