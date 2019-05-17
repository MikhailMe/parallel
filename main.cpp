#include <iostream>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <ctime>
#include <Windows.h>
#include <mpi.h>

using namespace std;

int seed;

double RandDouble(int max) {
    srand(time(0) + seed);
    seed = rand();
    return  (double)(rand() % max) + (double)(rand() % max) / max;
}

class Matrix {

public:
    int rowsCount;
    int columnsCount;
    double **elems;

    void GenerateMatrix(int randMax) {
        for (int i = 0; i < rowsCount; i++) {
            for (int j = 0; j < columnsCount; j++)
                elems[i][j] = RandDouble(randMax);
        }
    }

    void Print() {
        for (int i = 0; i < rowsCount; i++) {
            for (int j = 0; j < columnsCount; j++)
                std::cout << " " << elems[i][j];
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    Matrix(int _rowsCount, int _columnsCount) {
        rowsCount = _rowsCount;
        columnsCount = _columnsCount;
        elems = new double*[rowsCount];
        for (int i = 0; i < rowsCount; i++)
            elems[i] = new double[columnsCount];
    }

    ~Matrix() {
        for (int i = 0; i < rowsCount; i++)
            delete elems[i];
        delete elems;
    }
};

//Ôóíêöèÿ, îïèñûâàþùàÿ èñïîëíåíèå ïîäçàäà÷è îäíèì èç íåêîðíåâûõ ïðîöåññîâ 
void ProcTask(int rank) {

    //Áóôåð äëÿ ñîõðàíåíèÿ ðàçìåðîâ ìàòðèöû B
    int bsizeBuff[2];
    //Áóôåð äëÿ ñîõðàíåíèÿ ïàðàìåòðîâ ïîäçàäà÷è
    int taskParams[4];

    double **matrixARows;
    double **matrixB;
    double *result;
    
    MPI_Status status;

    //Ïîëó÷àåì broadcast-ñîîáùåíèå ñ ðàçìåðàìè ìàòðèöû B
    MPI_Bcast(&bsizeBuff, 2, MPI_INT, 0, MPI_COMM_WORLD);

    int bRowsCount = bsizeBuff[0];
    int bColumnsCount = bsizeBuff[1];
    matrixB = new double*[bRowsCount];
    //Çàïîëíÿåì ìàòðèöó B èñõîäíûìè ýëåìåíòàìè
    for (int i = 0; i < bRowsCount; i++) {
        matrixB[i] = new double[bColumnsCount];
        //Ïîëó÷àåì broadcast-ñîîáùåíèå, ñîäåðæàùåå i-óþ ñòðîêó ìàòðèöû B
        MPI_Bcast(matrixB[i], bColumnsCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    //Ïîëó÷àåì broadcast-ñîîáùåíèå ñ ïàðàìåòðàìè ïîäçàäà÷è
    MPI_Recv(&taskParams, 4, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    //Êîëè÷åñòâî ñòðîê ìàòðèöû A, íåîáõîäèìûå äëÿ âûïîëíåíèÿ òåêóùåé ïîäçàäà÷è
    int aRowsCount = taskParams[0]; 
    //Êîëè÷åñòâî ñòîëáöîâ ìàòðèöû A
    int aColumnsCount = taskParams[1];
    //Èíäåêñ ñòîëáöà, ñ êîòîðîãî íåîáõîäèìî íà÷àòü âû÷èñëåíèÿ
    int startColumnIndex = taskParams[2];
    //×èñëî ýëåìåíòîâ, êîòîðûå íåîáõîäèìî âû÷èñëèòü â ðàìêàõ òåêóùåé ïîäçàäà÷è
    int elemsToCalcByProc = taskParams[3];

    matrixARows = new double*[aRowsCount];
    result = new double[elemsToCalcByProc];
    //Ïîëó÷àåì âñå ñòðîêè ìàòðèöû A, íåîáõîäèìûå äëÿ ðàñ÷åòà òåêóùåé ïîäçàäà÷è
    for (int i = 0; i < aRowsCount; i++) {
        matrixARows[i] = new double[aColumnsCount];
        MPI_Recv(matrixARows[i], aColumnsCount, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    }
    int j = startColumnIndex;
    //Ðàññ÷èòûâàåì òðåáóþùååñÿ êîëè÷åñòâî ýëåìåíòîâ è çàïèñûâàåì èõ â åäèíûé áóôåð
    for (int i = 0, m = 0; i < aRowsCount; i++) {
        for (; j < bColumnsCount && m < elemsToCalcByProc; j++, m++) {
            double tmp = 0;
            for (int k = 0; k < aColumnsCount; k++) {
                tmp += matrixARows[i][k] * matrixB[k][j];
            }
            result[m] = tmp;
        }
        j = 0;
    }
    //Îòïðàâëÿåì ðåçóëüòàòû êîðíåâîìó ïðîöåññó
    MPI_Send(result, elemsToCalcByProc, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);

    for (int i = 0; i < bRowsCount; i++)
        delete matrixB[i];
    for (int i = 0; i < aRowsCount; i++)
        delete matrixARows[i];

    delete matrixARows;
    delete matrixB;
    delete result;
}


Matrix* Multiply(Matrix& a, Matrix& b, int procCount) {

    int bsizeBuff[2] = { b.rowsCount, b.columnsCount };

    int *awaitElemsCount = new int[procCount];

    if (a.columnsCount == b.rowsCount) {
        
        Matrix *result = new Matrix(a.rowsCount, b.columnsCount);
        int elemsToCalc = a.rowsCount * b.columnsCount;
        int avCountToCalcByProc = elemsToCalc / procCount;
        int remElemsCount = elemsToCalc % procCount;

        //Ðàññûëàåì âñåì ïðîöåññàì èíôîðìàöèþ î ðàçìåðå ìàòðèöû B
        MPI_Bcast(&bsizeBuff, 2, MPI_INT, 0, MPI_COMM_WORLD);

        //Ïîñòðî÷íî ðàññûëàåì ìàòðèöó B âñåì ïðîöåññàì 
        for (int i = 0; i < b.rowsCount; i++)
            MPI_Bcast(b.elems[i], b.columnsCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        int resultRowsCount = result->rowsCount;
        int resultColumnsCount = result->columnsCount;

        for (int tmpCounter = 0, i = 1; tmpCounter < elemsToCalc; i++) {

            int startRowIndex = tmpCounter / resultColumnsCount;
            int startColumnIndex = tmpCounter % resultColumnsCount;
            int elemsToCalcByProc = avCountToCalcByProc;

            if (remElemsCount > 0) {
                elemsToCalcByProc += 1;
                remElemsCount -= 1;
            }

            tmpCounter += elemsToCalcByProc;

            int tmp = tmpCounter / resultColumnsCount - startRowIndex;
            //Îïðåäåëÿåì ÷èñëî ñòðîê ìàòðèöû A, íåîáõîäèìûõ äëÿ âûïîëíåíèÿ ôîðìèðóåìîé ïîäçàäà÷è
            int rowsCount = (tmpCounter % resultColumnsCount > 0) ? tmp + 1 : tmp;
            awaitElemsCount[i - 1] = elemsToCalcByProc;

            //Ôîðìèðóåì è îòïðàâëÿåì ïðîöåññó ïàðàìåòðû íàçíà÷åííîé åìó ïîäçàäà÷è
            int calcParams[4] = { rowsCount, a.columnsCount, startColumnIndex, elemsToCalcByProc };
            MPI_Send(&calcParams, 4, MPI_INT, i, 0, MPI_COMM_WORLD);

            double **aElems = a.elems;
            int aColumnsCount = a.columnsCount;
            //Îòïðàâëÿåì ïðîöåññó íåîáõîäèìûå ñòðîêè ìàòðèöû A
            for (int j = startRowIndex, k = 0; k < rowsCount; j++, k++) {
                MPI_Send(aElems[j], aColumnsCount, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            }

        }

        MPI_Status status;
        int resRowPos = 0, resColumnPos = 0;
        double *resBuff = new double[awaitElemsCount[0]];
        double **resultElems = result->elems;

        //Ïîëó÷àåì îò êàæäîãî ïðîöåññà, íå ÿâëÿþùåãîñÿ êîðíåâûì, ðåçóëüòàòû ðàñ÷åòîâ è ôîðìèðóåì ðåçóëüòèðóþùóþ ìàòðèöó
        for (int otherProc = 1; otherProc <= procCount; otherProc++)
        {
            int resSize = awaitElemsCount[otherProc - 1];
            MPI_Recv(resBuff, resSize, MPI_DOUBLE, otherProc, otherProc, MPI_COMM_WORLD, &status);
            for (int k = 0; resRowPos < resultRowsCount; ++resRowPos) {
                for (; resColumnPos < resultColumnsCount && k < resSize; ++resColumnPos, ++k) {
                    resultElems[resRowPos][resColumnPos] = resBuff[k];
                }
                if (k >= resSize)
                    break;
                else 
                    resColumnPos = 0;
            }
        }

        delete awaitElemsCount;
        delete resBuff;

        return result;
    }
    else
        return NULL;
}


int main(int argc, char *argv[]) {

    int rank, procCount;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    if (rank == 0) {
        //Âûïîëíÿåòñÿ â êîðíåâîì ïðîöåññå
        double *buff;
        Matrix a(1500, 500);
        Matrix b(500, 1000);
        a.GenerateMatrix(100);
        b.GenerateMatrix(100);

        unsigned int startTime = clock();

        Matrix *c = Multiply(a, b, procCount - 1);

        unsigned int endTime = clock();
        unsigned int multiplicationTime = endTime - startTime;
        std::cout << multiplicationTime << endl;

        delete c;
    }
    else
        ProcTask(rank); //Âûïîëíÿåòñÿ â ïðîöåññàõ, îáåñïå÷èâàþùèõ ðåøåíèå ïîäçàäà÷

    MPI_Finalize();
    return 0;
}