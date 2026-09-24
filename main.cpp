#include <iostream>
#include <conio.h>
#include <math.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

#define SIZE 10
#define MATRIX_FILE "matrices.json"

struct SavedMatrix
{
    string name;
    int rows;
    int cols;
    vector<vector<float>> values;
};

vector<SavedMatrix> savedMatrices;

void saveMatricesToJson()
{
    ofstream file(MATRIX_FILE);
    file << "{\n  \"matrices\": [\n";
    for (size_t index = 0; index < savedMatrices.size(); index++)
    {
        const SavedMatrix &matrix = savedMatrices[index];
        file << "    {\"name\": \"" << matrix.name << "\", \"rows\": "
             << matrix.rows << ", \"cols\": " << matrix.cols << ", \"values\": [\n";
        for (int row = 0; row < matrix.rows; row++)
        {
            file << "      [";
            for (int col = 0; col < matrix.cols; col++)
            {
                if (col > 0)
                    file << ", ";
                file << matrix.values[row][col];
            }
            file << "]" << (row + 1 < matrix.rows ? "," : "") << "\n";
        }
        file << "    ]}" << (index + 1 < savedMatrices.size() ? "," : "") << "\n";
    }
    file << "  ]\n}\n";
}

void loadMatricesFromJson()
{
    ifstream file(MATRIX_FILE);
    if (!file)
        return;

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    size_t position = 0;
    while ((position = content.find("{\"name\": \"", position)) != string::npos)
    {
        position += string("{\"name\": \"").length();
        size_t nameEnd = content.find('"', position);
        if (nameEnd == string::npos)
            break;

        SavedMatrix matrix;
        matrix.name = content.substr(position, nameEnd - position);
        size_t rowsPosition = content.find("\"rows\": ", nameEnd);
        size_t colsPosition = content.find("\"cols\": ", rowsPosition);
        size_t valuesPosition = content.find("\"values\": [", colsPosition);
        if (rowsPosition == string::npos || colsPosition == string::npos || valuesPosition == string::npos)
            break;

        try
        {
            matrix.rows = stoi(content.substr(rowsPosition + string("\"rows\": ").length()));
            matrix.cols = stoi(content.substr(colsPosition + string("\"cols\": ").length()));
        }
        catch (const invalid_argument &)
        {
            position = nameEnd + 1;
            continue;
        }

        if (matrix.rows < 1 || matrix.rows > SIZE || matrix.cols < 1 || matrix.cols > SIZE)
        {
            position = nameEnd + 1;
            continue;
        }

        matrix.values.assign(matrix.rows, vector<float>(matrix.cols));

        size_t valuePosition = content.find('[', valuesPosition + string("\"values\": [").length()) + 1;
        for (int row = 0; row < matrix.rows; row++)
        {
            for (int col = 0; col < matrix.cols; col++)
            {
                while (valuePosition < content.size() && (content[valuePosition] == ' ' || content[valuePosition] == ',' || content[valuePosition] == '\n'))
                    valuePosition++;
                size_t valueEnd = valuePosition;
                while (valueEnd < content.size() && content[valueEnd] != ',' && content[valueEnd] != ']')
                    valueEnd++;
                matrix.values[row][col] = stof(content.substr(valuePosition, valueEnd - valuePosition));
                valuePosition = valueEnd;
            }
            if (row + 1 < matrix.rows)
                valuePosition = content.find('[', valuePosition) + 1;
        }
        savedMatrices.push_back(matrix);
        size_t matrixEnd = content.find("]}", valuePosition);
        position = content.find("{\"name\": \"", matrixEnd == string::npos ? content.size() : matrixEnd + 2);
        if (position == string::npos)
            break;
    }
}

void createMatrix()
{
    SavedMatrix matrix;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\nEnter matrix name: ";
    getline(cin, matrix.name);
    if (matrix.name.empty())
    {
        cout << "\nMatrix name cannot be empty.\n";
        return;
    }

    cout << "Enter number of rows (1-10): ";
    while (!(cin >> matrix.rows) || matrix.rows < 1 || matrix.rows > SIZE)
    {
        cout << "Please enter a whole number from 1 to 10 for rows: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Enter number of columns (1-10): ";
    while (!(cin >> matrix.cols) || matrix.cols < 1 || matrix.cols > SIZE)
    {
        cout << "Please enter a whole number from 1 to 10 for columns: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    matrix.values.assign(matrix.rows, vector<float>(matrix.cols));
    cout << "\nEnter matrix values:\n";
    for (int row = 0; row < matrix.rows; row++)
    {
        for (int col = 0; col < matrix.cols; col++)
        {
            cout << "[" << row << "][" << col << "] = ";
            while (!(cin >> matrix.values[row][col]))
            {
                cout << "Please enter a numeric value for [" << row << "][" << col << "]: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
    }

    savedMatrices.push_back(matrix);
    saveMatricesToJson();
    cout << "\nMatrix saved to " << MATRIX_FILE << ".\n";
}

bool readMenuInteger(const string &prompt, int &value)
{
    cout << prompt;
    if (cin >> value)
        return true;

    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\nInvalid input. Please enter a number from the displayed menu.\n";
    getch();
    return false;
}

bool chooseMatrix(const string &label, SavedMatrix &selected)
{
    if (savedMatrices.empty())
    {
        cout << "\nNo saved matrices found. Choose 'Create / save matrix' first.\n";
        return false;
    }

    cout << "\nSaved matrices:\n";
    for (size_t index = 0; index < savedMatrices.size(); index++)
        cout << index + 1 << ". " << savedMatrices[index].name << " ("
             << savedMatrices[index].rows << " x " << savedMatrices[index].cols << ")\n";

    int selection;
    if (!readMenuInteger("Choose " + label + " matrix: ", selection))
        return false;
    if (selection < 1 || selection > static_cast<int>(savedMatrices.size()))
    {
        cout << "\nInvalid matrix selection.\n";
        return false;
    }

    selected = savedMatrices[selection - 1];
    return true;
}

void printSavedMatrix(const SavedMatrix &matrix)
{
    cout << "\n"
         << matrix.name << " (" << matrix.rows << " x " << matrix.cols << ")\n";
    for (int row = 0; row < matrix.rows; row++)
    {
        for (int col = 0; col < matrix.cols; col++)
            cout << matrix.values[row][col] << "   ";
        cout << "\n";
    }
}

void presentAndSaveResult(const string &title, const vector<vector<float>> &values)
{
    SavedMatrix result;
    result.rows = static_cast<int>(values.size());
    result.cols = result.rows == 0 ? 0 : static_cast<int>(values[0].size());
    result.values = values;

    cout << "\n"
         << title << " (" << result.rows << " x " << result.cols << ")\n";
    for (int row = 0; row < result.rows; row++)
    {
        for (int col = 0; col < result.cols; col++)
            cout << setw(10) << result.values[row][col] << " ";
        cout << "\n";
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\nEnter a name to save this result (press Enter for Result "
         << savedMatrices.size() + 1 << "): ";
    getline(cin, result.name);
    if (result.name.empty())
        result.name = "Result " + to_string(savedMatrices.size() + 1);

    savedMatrices.push_back(result);
    saveMatricesToJson();
    cout << "Result saved as \"" << result.name << "\".\n";
}

void printBoxTitle(const string &title);
void printOperationRules(int choice);

void editSavedMatrix(SavedMatrix &matrix)
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\nEnter new matrix name: ";
    getline(cin, matrix.name);
    while (matrix.name.empty())
    {
        cout << "Matrix name cannot be empty. Enter a new matrix name: ";
        getline(cin, matrix.name);
    }

    cout << "Enter new number of rows (1-10): ";
    while (!(cin >> matrix.rows) || matrix.rows < 1 || matrix.rows > SIZE)
    {
        cout << "Please enter a whole number from 1 to 10 for rows: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Enter new number of columns (1-10): ";
    while (!(cin >> matrix.cols) || matrix.cols < 1 || matrix.cols > SIZE)
    {
        cout << "Please enter a whole number from 1 to 10 for columns: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    matrix.values.assign(matrix.rows, vector<float>(matrix.cols));
    cout << "\nEnter the new matrix values:\n";
    for (int row = 0; row < matrix.rows; row++)
    {
        for (int col = 0; col < matrix.cols; col++)
        {
            cout << "[" << row << "][" << col << "] = ";
            while (!(cin >> matrix.values[row][col]))
            {
                cout << "Please enter a numeric value for [" << row << "][" << col << "]: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
    }

    saveMatricesToJson();
    cout << "\nMatrix updated and saved to " << MATRIX_FILE << ".\n";
}

void viewSavedMatrix()
{
    while (true)
    {
        system("cls");
        printBoxTitle("SEE SAVED MATRICES");
        printOperationRules(14);
        cout << "\n1. Exit to main menu\n";

        if (savedMatrices.empty())
        {
            cout << "\nNo saved matrices found.\n";
        }
        else
        {
            for (size_t index = 0; index < savedMatrices.size(); index++)
            {
                cout << index + 2 << ". " << savedMatrices[index].name << " ("
                     << savedMatrices[index].rows << " x " << savedMatrices[index].cols << ")\n";
            }
        }

        int selection;
        if (!readMenuInteger("\nSelect an option: ", selection))
        {
            continue;
        }

        if (selection == 1)
            return;

        int matrixIndex = selection - 2;
        if (matrixIndex < 0 || matrixIndex >= static_cast<int>(savedMatrices.size()))
        {
            cout << "\nInvalid matrix selection.";
            getch();
            continue;
        }

        system("cls");
        printBoxTitle("VIEW / EDIT MATRIX");
        printSavedMatrix(savedMatrices[matrixIndex]);

        int editChoice;
        if (!readMenuInteger("\n1. Edit this matrix\n2. Return to matrix list\n\nSelect an option: ", editChoice))
            continue;

        if (editChoice == 1)
        {
            system("cls");
            printBoxTitle("EDIT MATRIX");
            editSavedMatrix(savedMatrices[matrixIndex]);
            printSavedMatrix(savedMatrices[matrixIndex]);
            cout << "\nPress any key to return to the matrix list...";
            getch();
        }
        else if (editChoice == 2)
        {
            continue;
        }
        else
        {
            cout << "\nInvalid option. Returning to the matrix list.";
            getch();
        }
    }
}

class matrix
{
private:
    int i, j, m, n, row, col, choice;
    float a[10][10], b[10][10], mt[50][50];

public:
    void addition();
    void subtraction();
    void multplication();
    void s_matrix();
    void Transpose();
    int Determinant();
    void Inverse();
    void eigen();
    void accept();
    void display();
    void gauss();
    void EnterJordan();
    void credits();
    void identity();
    void symmtreic();
    int getSolverSize() const { return n; }
    float getSolverValue(int row, int col) const { return mt[row][col]; }
};

bool canAddSubtract(int r1, int c1, int r2, int c2)
{
    return (r1 == r2) && (c1 == c2);
}

bool canMultiply(int r1, int c1, int r2, int c2)
{
    return (c1 == r2);
}

void matrix::eigen()
{
    SavedMatrix selected;
    if (!chooseMatrix("for eigenvalue calculation", selected))
        return;
    if (selected.rows != selected.cols)
    {
        cout << "\nEigenvalue calculation requires a square saved matrix.\n";
        return;
    }

    int n = selected.rows;
    float x[SIZE] = {}, next[SIZE] = {};
    float lambdaNew = 0, lambdaOld = 0, error = 0.0001f;
    int step = 1;

    cout << setprecision(3) << fixed;
    printSavedMatrix(selected);
    for (int index = 0; index < n; index++)
        x[index] = 1;

    do
    {
        float previousLambda = lambdaNew;
        for (int i = 0; i < n; i++)
        {
            next[i] = 0;
            for (int j = 0; j < n; j++)
                next[i] += selected.values[i][j] * x[j];
        }

        lambdaNew = fabs(next[0]);
        for (int i = 1; i < n; i++)
            lambdaNew = max(lambdaNew, fabs(next[i]));
        if (lambdaNew == 0)
        {
            cout << "\nThe selected matrix has no non-zero dominant eigenvalue.\n";
            return;
        }
        for (int i = 0; i < n; i++)
            x[i] = next[i] / lambdaNew;

        cout << "\nStep " << step++ << " eigenvalue = " << lambdaNew << "\nEigenvector: [";
        for (int i = 0; i < n; i++)
            cout << x[i] << (i + 1 == n ? "]\n" : ", ");
        if (step > 1000)
            break;
        lambdaOld = previousLambda;
    } while (step == 1 || (fabs(lambdaNew - lambdaOld) > error && step <= 1000));

    vector<vector<float>> eigenvector(n, vector<float>(1));
    for (int index = 0; index < n; index++)
        eigenvector[index][0] = x[index];
    presentAndSaveResult("DOMINANT EIGENVECTOR RESULT", eigenvector);
}

void matrix::addition()
{
    SavedMatrix first, second;
    if (!chooseMatrix("first", first) || !chooseMatrix("second", second))
        return;

    i = first.rows;
    j = first.cols;
    m = second.rows;
    n = second.cols;

    if (!canAddSubtract(i, j, m, n))
    {
        cout << "\nMATRIX CANNOT BE ADDED (rows and columns are not equal)";
        return;
    }

    printSavedMatrix(first);
    printSavedMatrix(second);

    int x, y;
    cout << "\nEnter multiplication value for 1st matrix\nvalue = ";
    cin >> x;
    cout << "\nEnter multiplication value for 2nd matrix\nvalue = ";
    cin >> y;

    vector<vector<float>> result(i, vector<float>(j));
    for (row = 0; row < i; row++)
        for (col = 0; col < j; col++)
            result[row][col] = x * first.values[row][col] + y * second.values[row][col];
    presentAndSaveResult("ADDITION RESULT", result);
}

void matrix::subtraction()
{
    SavedMatrix first, second;
    if (!chooseMatrix("first", first) || !chooseMatrix("second", second))
        return;

    i = first.rows;
    j = first.cols;
    m = second.rows;
    n = second.cols;

    if (!canAddSubtract(i, j, m, n))
    {
        cout << "\nMATRIX CANNOT BE SUBTRACTED (rows and columns are not equal)";
        return;
    }

    printSavedMatrix(first);
    printSavedMatrix(second);

    int x, y;
    cout << "\nEnter multiplication value for 1st matrix\nvalue = ";
    cin >> x;
    cout << "\nEnter multiplication value for 2nd matrix\nvalue = ";
    cin >> y;

    vector<vector<float>> result(i, vector<float>(j));
    for (row = 0; row < i; row++)
        for (col = 0; col < j; col++)
            result[row][col] = x * first.values[row][col] - y * second.values[row][col];
    presentAndSaveResult("SUBTRACTION RESULT", result);
}

void matrix::multplication()
{
    SavedMatrix first, second;
    if (!chooseMatrix("first", first) || !chooseMatrix("second", second))
        return;

    i = first.rows;
    j = first.cols;
    m = second.rows;
    n = second.cols;

    if (!canMultiply(i, j, m, n))
    {
        cout << "\nMATRIX CANNOT BE MULTIPLIED (column of 1st matrix must equal row of 2nd matrix)";
        return;
    }

    printSavedMatrix(first);
    printSavedMatrix(second);

    vector<vector<float>> result(i, vector<float>(n, 0));
    for (int r = 0; r < i; r++)
        for (int c = 0; c < n; c++)
            for (int k = 0; k < j; k++)
                result[r][c] += first.values[r][k] * second.values[k][c];
    presentAndSaveResult("MULTIPLICATION RESULT", result);
}

void matrix::s_matrix()
{
    SavedMatrix first;
    if (!chooseMatrix("", first))
        return;

    printSavedMatrix(first);
    int k, choiceMatrix;
    cout << "\nEnter scalar value: ";
    while (!(cin >> k))
    {
        cout << "Please enter a numeric scalar value: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (!readMenuInteger("\nPress 1 to multiply the selected matrix: ", choiceMatrix))
        return;

    if (choiceMatrix == 1)
    {
        vector<vector<float>> result(first.rows, vector<float>(first.cols));
        for (row = 0; row < first.rows; row++)
            for (col = 0; col < first.cols; col++)
                result[row][col] = k * first.values[row][col];
        presentAndSaveResult("SCALAR MULTIPLICATION RESULT", result);
    }
    else
    {
        cout << "\nInvalid matrix selection.";
    }
}

void matrix::Transpose()
{
    SavedMatrix selected;
    if (!chooseMatrix("to transpose", selected))
        return;

    printSavedMatrix(selected);
    vector<vector<float>> result(selected.cols, vector<float>(selected.rows));
    for (int col = 0; col < selected.cols; col++)
        for (int row = 0; row < selected.rows; row++)
            result[col][row] = selected.values[row][col];
    presentAndSaveResult("TRANSPOSE RESULT", result);
}

int matrix::Determinant()
{
    SavedMatrix selected;
    if (!chooseMatrix("for determinant", selected))
        return 0;
    if (selected.rows != selected.cols || (selected.rows != 2 && selected.rows != 3))
    {
        cout << "\nDeterminant requires a saved 2 x 2 or 3 x 3 square matrix.\n";
        return 0;
    }

    printSavedMatrix(selected);
    float determinant;
    if (selected.rows == 2)
    {
        determinant = selected.values[0][0] * selected.values[1][1] - selected.values[1][0] * selected.values[0][1];
    }
    else
    {
        determinant = 0;
        for (int index = 0; index < 3; index++)
            determinant += selected.values[0][index] * (selected.values[1][(index + 1) % 3] * selected.values[2][(index + 2) % 3] - selected.values[1][(index + 2) % 3] * selected.values[2][(index + 1) % 3]);
    }
    cout << "\nYour determinant is: " << determinant << "\n";
    return static_cast<int>(determinant);
}

void matrix::Inverse()
{
    SavedMatrix selected;
    if (!chooseMatrix("for inverse", selected))
        return;
    if (selected.rows != 3 || selected.cols != 3)
    {
        cout << "\nInverse currently requires a saved 3 x 3 matrix.\n";
        return;
    }

    float determinant = 0;
    for (int index = 0; index < 3; index++)
        determinant += selected.values[0][index] * (selected.values[1][(index + 1) % 3] * selected.values[2][(index + 2) % 3] - selected.values[1][(index + 2) % 3] * selected.values[2][(index + 1) % 3]);
    if (determinant == 0)
    {
        cout << "\nThis matrix has no inverse because its determinant is zero.\n";
        return;
    }

    printSavedMatrix(selected);
    vector<vector<float>> result(3, vector<float>(3));
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            float cofactor = selected.values[(col + 1) % 3][(row + 1) % 3] * selected.values[(col + 2) % 3][(row + 2) % 3] - selected.values[(col + 1) % 3][(row + 2) % 3] * selected.values[(col + 2) % 3][(row + 1) % 3];
            result[row][col] = cofactor / determinant;
        }
    }
    presentAndSaveResult("INVERSE RESULT", result);
}

void matrix::accept()
{
    SavedMatrix selected;
    if (!chooseMatrix("for the augmented solver", selected))
    {
        n = 0;
        return;
    }
    if (selected.cols != selected.rows + 1 || selected.rows > 49)
    {
        cout << "\nThe solver requires a saved augmented matrix where columns = rows + 1.\n";
        n = 0;
        return;
    }

    n = selected.rows;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n + 1; j++)
            mt[i][j] = selected.values[i][j];
    }
    cout << "\nUsing saved augmented matrix: " << selected.name << "\n";
}

void matrix::display()
{
    for (int i = 0; i < n; i++)
    {
        cout << "\n";
        for (int j = 0; j < n + 1; j++)
        {
            if (j == n)
                cout << " ";
            cout << mt[i][j] << "\t";
        }
    }
}

void matrix::gauss() // converting augmented matrix to row echelon form
{
    float temp; // Line 1
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            temp = mt[j][i] / mt[i][i]; // Line 2
            for (int k = i; k < n + 1; k++)
            {
                mt[j][k] -= temp * mt[i][k]; // Line 3
                // a[j][k]-=a[j][i]*a[i][k]/a[i][i];//Line 4
            }
        }
    }
}

void matrix::EnterJordan() // converting to reduced row echelon form
{
    float temp;
    for (int i = n - 1; i >= 0; i--)
    {

        for (int j = i - 1; j >= 0; j--)
        {
            temp = mt[j][i] / mt[i][i];
            for (int k = n; k >= i; k--)
            {
                mt[j][k] -= temp * mt[i][k];
            }
        }
    }

    float x[n];
    for (int i = 0; i < n; i++) // making leading coefficients zero
        x[i] = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            if (x[i] == 0 && j != n)
                x[i] = mt[i][j];
            if (x[i] != 0)
                mt[i][j] /= x[i];
        }
    }
}

void matrix::credits()
{
    for (int i = 0; i < n; i++)
    {
        cout << "\nx" << i + 1 << " = " << mt[i][n] << endl;
    }
}

void matrix::identity()
{
    SavedMatrix selected;
    if (!chooseMatrix("for identity check", selected))
        return;
    if (selected.rows != selected.cols)
    {
        cout << "\nIdentity check requires a square saved matrix.\n";
        return;
    }
    bool isIdentity = true;
    for (int row = 0; row < selected.rows; row++)
        for (int col = 0; col < selected.cols; col++)
            if ((row == col && selected.values[row][col] != 1) || (row != col && selected.values[row][col] != 0))
                isIdentity = false;

    printSavedMatrix(selected);
    if (isIdentity)
        cout << "The matrix is an identity matrix.\n";
    else
        cout << "The matrix is not an identity matrix.\n";
}

void matrix::symmtreic()
{
    SavedMatrix selected;
    if (!chooseMatrix("for symmetry check", selected))
        return;
    if (selected.rows != selected.cols)
    {
        cout << "\nSymmetric checks require a square saved matrix.\n";
        return;
    }

    bool symmetric = true;
    bool skewSymmetric = true;
    for (int row = 0; row < selected.rows; row++)
    {
        for (int col = 0; col < selected.cols; col++)
        {
            if (selected.values[row][col] != selected.values[col][row])
                symmetric = false;
            if (selected.values[row][col] != -selected.values[col][row])
                skewSymmetric = false;
        }
    }

    printSavedMatrix(selected);
    if (symmetric)
        cout << "The matrix is symmetric.\n";
    else if (skewSymmetric)
        cout << "The matrix is skew-symmetric.\n";
    else
        cout << "The matrix is neither symmetric nor skew-symmetric.\n";
}

void printBoxTitle(const string &title)
{
    const int width = 60;
    cout << '\n';
    cout << string(width, '=') << '\n';
    cout << setw((width + title.length()) / 2) << title << '\n';
    cout << string(width, '=') << '\n';
}

void printMenu()
{
    cout << '\n';
    cout << setw(38) << "SELECT AN OPTION" << '\n'
         << '\n';
    cout << left << setw(6) << "" << setw(36) << "1. Exit" << '\n';
    cout << left << setw(6) << "" << setw(36) << "2. Subtraction" << '\n';
    cout << left << setw(6) << "" << setw(36) << "3. Multiplication" << '\n';
    cout << left << setw(6) << "" << setw(36) << "4. Addition" << '\n';
    cout << left << setw(6) << "" << setw(36) << "5. Scalar multiplication" << '\n';
    cout << left << setw(6) << "" << setw(36) << "6. Transpose" << '\n';
    cout << left << setw(6) << "" << setw(36) << "7. Determinant" << '\n';
    cout << left << setw(6) << "" << setw(36) << "8. Inverse" << '\n';
    cout << left << setw(6) << "" << setw(36) << "9. Symmetric" << '\n';
    cout << left << setw(6) << "" << setw(36) << "10. Identity" << '\n';
    cout << left << setw(6) << "" << setw(36) << "11. Echelon / reduced echelon" << '\n';
    cout << left << setw(6) << "" << setw(36) << "12. Eigen value and vector" << '\n'
         << left << setw(6) << "" << setw(36) << "13. Create and save matrix" << '\n'
         << left << setw(6) << "" << setw(36) << "14. See / Edit saved matrices" << '\n'
         << '\n';
    cout << "   Enter your choice: ";
}

void printOperationRules(int choice)
{
    cout << "\nRULES / REQUIREMENTS\n";
    switch (choice)
    {
    case 2:
        cout << "- Select two saved matrices.\n"
             << "- Both matrices must have the same rows and columns.\n";
        break;
    case 3:
        cout << "- Select two saved matrices.\n"
             << "- Columns of the first matrix (C1) must equal rows of the second matrix (R2).\n"
             << "- Result size is rows of the first matrix x columns of the second matrix.\n";
        break;
    case 4:
        cout << "- Select two saved matrices.\n"
             << "- Both matrices must have the same rows and columns.\n";
        break;
    case 5:
        cout << "- Select one saved matrix.\n"
             << "- Enter a numeric scalar value.\n";
        break;
    case 6:
        cout << "- Select one saved matrix.\n"
             << "- No square-matrix condition is required.\n"
             << "- Rows and columns are exchanged in the result.\n";
        break;
    case 7:
        cout << "- Select one saved matrix.\n"
             << "- The matrix must be 2 x 2 or 3 x 3.\n";
        break;
    case 8:
        cout << "- Select one saved 3 x 3 matrix.\n"
             << "- Its determinant must not be zero.\n";
        break;
    case 9:
        cout << "- Select one saved square matrix.\n"
             << "- Symmetric: A[i][j] = A[j][i].\n"
             << "- Skew-symmetric: A[i][j] = -A[j][i].\n";
        break;
    case 10:
        cout << "- Select one saved square matrix.\n"
             << "- Diagonal values must be 1.\n"
             << "- All non-diagonal values must be 0.\n";
        break;
    case 11:
        cout << "- Select one saved augmented matrix.\n"
             << "- Columns must equal rows + 1.\n"
             << "- The final column contains the constants.\n";
        break;
    case 12:
        cout << "- Select one saved square matrix.\n"
             << "- The calculation uses the power method.\n"
             << "- It estimates the dominant eigenvalue and eigenvector.\n";
        break;
    case 13:
        cout << "- Matrix names cannot be empty.\n"
             << "- Rows and columns must be between 1 and 10.\n"
             << "- Enter numeric values for every element.\n";
        break;
    case 14:
        cout << "- Select a saved matrix to view its values.\n"
             << "- You can edit its name, dimensions, and values.\n"
             << "- Changes are saved to matrices.json.\n";
        break;
    }
    cout << '\n';
}

int main()
{
    int choice;
    matrix mt;
    loadMatricesFromJson();

    while (true)
    {
        system("cls");
        printBoxTitle("MATRIX OPERATIONS SYSTEM");
        printMenu();
        if (!readMenuInteger("", choice))
            continue;
        system("cls");

        switch (choice)
        {
        case 1:
            printBoxTitle("GOODBYE");
            cout << "\nThank you for using Matrix Operations System!\n\n";
            return 0;
        case 2:
            printBoxTitle("SUBTRACTION");
            printOperationRules(choice);
            mt.subtraction();
            getch();
            break;
        case 3:
            printBoxTitle("MULTIPLICATION");
            printOperationRules(choice);
            mt.multplication();
            getch();
            break;
        case 4:
            printBoxTitle("ADDITION");
            printOperationRules(choice);
            mt.addition();
            getch();
            break;
        case 5:
            printBoxTitle("SCALAR MULTIPLICATION");
            printOperationRules(choice);
            mt.s_matrix();
            getch();
            break;
        case 6:
            printBoxTitle("TRANSPOSE");
            printOperationRules(choice);
            mt.Transpose();
            getch();
            break;
        case 7:
            printBoxTitle("DETERMINANT");
            printOperationRules(choice);
            mt.Determinant();
            getch();
            break;
        case 8:
            printBoxTitle("INVERSE");
            printOperationRules(choice);
            mt.Inverse();
            getch();
            break;
        case 9:
            printBoxTitle("SYMMETRIC CHECK");
            printOperationRules(choice);
            mt.symmtreic();
            getch();
            break;
        case 10:
            printBoxTitle("IDENTITY CHECK");
            printOperationRules(choice);
            mt.identity();
            getch();
            break;
        case 11:
        {
            printBoxTitle("GAUSS-JORDAN SOLVER");
            printOperationRules(choice);
            mt.accept();
            if (mt.getSolverSize() == 0)
            {
                getch();
                break;
            }
            cout << "\n\nOriginal matrix:\n\n";
            mt.display();
            mt.gauss();
            cout << "\n\nRow Echelon form:\n\n";
            mt.display();
            mt.EnterJordan();
            cout << "\n\nReduced row echelon form:\n\n";
            mt.display();
            cout << "\n\nSolution:\n\n";
            mt.credits();
            vector<vector<float>> solverResult(mt.getSolverSize(), vector<float>(mt.getSolverSize() + 1));
            for (int resultRow = 0; resultRow < mt.getSolverSize(); resultRow++)
                for (int resultCol = 0; resultCol < mt.getSolverSize() + 1; resultCol++)
                    solverResult[resultRow][resultCol] = mt.getSolverValue(resultRow, resultCol);
            presentAndSaveResult("REDUCED ECHELON RESULT", solverResult);
            getch();
            break;
        }
        case 12:
            printBoxTitle("EIGEN VALUE & VECTOR");
            printOperationRules(choice);
            mt.eigen();
            getch();
            break;
        case 13:
            printBoxTitle("CREATE / SAVE MATRIX");
            printOperationRules(choice);
            createMatrix();
            getch();
            break;
        case 14:
            printBoxTitle("SEE / EDIT MATRICES");
            printOperationRules(choice);
            viewSavedMatrix();
            getch();
            break;
        default:
            printBoxTitle("INVALID CHOICE");
            cout << "\nPlease select a valid option.\n";
            getch();
            break;
        }
    }

    return 0;
}