#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

#define maxArrSize 10

struct molecule
{
    double coef;
    bool isLhs;
    std::vector<std::pair<std::string, int>> elem;
};

std::vector<molecule> data;
float M[maxArrSize][maxArrSize] = {};

void printEquation()
{
    unsigned int i, j;
    bool isFt = true;

    for(i = 0; i < data.size(); i++){
        if(data[i].coef > 1)
            std::cout << data[i].coef;
        for(j = 0; j < data[i].elem.size(); j++){
            std::cout << data[i].elem[j].first;
            if(data[i].elem[j].second > 1)
                std::cout << data[i].elem[j].second;
        }
        if(!data[i + 1].isLhs && isFt){
            std::cout << " = ";
            isFt = false;
        }
        else if(i + 1 != data.size())
            std::cout << " + ";
    }
    std::cout << "\n";
}

void calculateMatrix()
{
    int i, j, k, n = data.size() - 1;
    double x[n];

    for(i = 0; i < n; i++){
        for(j = 0; j < n + 1; j++)
            std::cout << M[i][j] << " ";
        std::cout << "\n";
    }

    for(i = 0; i < n; i++)
        for(k = i + 1; k < n; k++)
            if(abs(M[i][i]) < abs(M[k][i]))
                for(j = 0; j <= n; j++){
                    double temp = M[i][j];
                    M[i][j] = M[k][j];
                    M[k][j] = temp;
                }

    for(i = 0; i < n - 1; i++)
        for(k = i + 1; k < n; k++){
            double t = M[k][i] / M[i][i];
            for(j = 0; j <= n; j++)
                M[k][j] = M[k][j] - t * M[i][j];
        }


    for(i = n - 1; i >= 0; i--){
        x[i] = M[i][n];
        for(j = i + 1; j < n; j++)
            if(j!=i)
                x[i] = x[i] - M[i][j] * x[j];
        x[i] = x[i] / M[i][i];
    }


    double tmp, a[n];
    data[0].coef = 1;
    for(i = 0; i < n; i++){
        a[i] = x[i];
    }

    for(i = 0; i < n; i++){
        if(modf(x[i], &tmp) != 0){
            data[0].coef++;
            for(j = 0; j < n; j++)
                x[j] = a[j] * data[0].coef;
            i = 0;
        }
    }

    for(i = 0; i < n; i++)
        data[i + 1].coef = x[i];
}

void generateMatrix()
{
    std::vector<std::string> duplicateElements;
    unsigned int x = 0, i, j, n, m;

    for(i = 0; data[i].isLhs; i++){
        for(j = 0; j < data[i].elem.size(); j++){
            for(n = i; n < data.size(); n++){
                for(m = 0; m < data[n].elem.size(); m++){
                    if(data[i].elem[j].first == data[n].elem[m].first){
                        for(auto k : duplicateElements)
                            if(data[i].elem[j].first == k)
                                goto nextEl;
                        if(i == 0){
                            M[x][data.size() - 1] = data[i].elem[j].second;
                            if(data[n].isLhs)
                                M[x][n - 1] = -data[n].elem[m].second;
                            else
                                M[x][n - 1] = data[n].elem[m].second;
                        }else{
                            M[x][i - 1] = data[i].elem[j].second;
                            //M[x][n - 1] = -data[n].elem[m].second;
                            if(data[n].isLhs)
                                M[x][n - 1] = data[n].elem[m].second;
                            else
                                M[x][n - 1] = -data[n].elem[m].second;
                        }
                    }
                }
            }
            x++;
            duplicateElements.push_back(data[i].elem[j].first);
            nextEl:
                continue;
        }
    }
}

void packInput(std::string eq)
{
    molecule tmp;
    tmp.isLhs = true;

    std::pair<std::string, int> tmppair;
    tmppair.second = 1;

    for(unsigned int i = 0; i < eq.size();){
        if(isspace(eq[i]))
            i++;
        if(isupper(eq[i])){
            tmppair.first = eq[i];
            i++;
            if(islower(eq[i])){
                tmppair.first.push_back(eq[i]);
                i++;
            }
        }
        if(isdigit(eq[i])){
            tmppair.second = eq[i] - 48;
            i++;
            if(isdigit(eq[i])){
                tmppair.second = tmppair.second * 10 + (eq[i] - 48);
                i++;
            }
        }
        if((eq[i] != '+' || eq[i] != '=') && !tmppair.first.empty()){
            tmp.elem.push_back(tmppair);

            tmppair.second = 1;
            tmppair.first.clear();
        }
        if(eq[i] == '('){
            std::string tmpeq = eq.substr(i + 1, eq.find(')') - i - 1);
            for(unsigned int j = 0; j < tmpeq.size();){
                if(isupper(tmpeq[j])){
                    tmppair.first = tmpeq[j];
                    j++;
                    if(islower(tmpeq[j])){
                        tmppair.first.push_back(tmpeq[j]);
                        j++;
                    }
                }
                if(isdigit(tmpeq[j])){
                    tmppair.second = tmpeq[j] - 48;
                    j++;
                    if(isdigit(tmpeq[j])){
                        tmppair.second = tmppair.second * 10 + (tmpeq[j] - 48);
                        j++;
                    }
                }

                tmppair.second *= (int)(eq[tmpeq.size() + i + 2] - 48);

                tmp.elem.push_back(tmppair);

                tmppair.second = 1;
                tmppair.first.clear();

            }
            i += tmpeq.size() + 3;
        }
        if(eq[i] == '+' || eq[i] == '=' || i == eq.size()){
            data.push_back(tmp);

            tmp.elem.clear();
            if(eq[i] == '=')
                tmp.isLhs = false;
            i++;
        }
    }
}

int main()
{
    std::string eq;
    printf("Input chemical formula: ");
    getline(std::cin, eq);

    packInput(eq);

    /*
    for(unsigned int i = 0; i < data.size(); i++){
        std::cout << data[i].isLhs << "\n";
        for(unsigned int j = 0; j < data[i].elem.size(); j++){
            std::cout << data[i].elem[j].first << data[i].elem[j].second << "\n";
        }
        std::cout << "\n";
    }
    */

    generateMatrix();
    calculateMatrix();
    printEquation();

    //data.clear();
    //M = {};
    //main();
}
// C12H26 + O2 = CO2 + H2O
// Al + Cl = AlCl3
// H + O = HO
// O2 + Al = Al2O3
// O2 + SO2 = SO3
// Ca3(PO4)2 + SiO2 + C = CaSiO3 + P4 + CO

// HNO3 + Zn = H2O + NH4NO3 + Zn(NO3)2 || Zn + HNO3 = Zn(NO3)2 + NH4NO3 + H2O
// K2Cr2O7 + H2S + H2SO4 = K2SO4 + Cr2(SO4)3 + S + H2O
