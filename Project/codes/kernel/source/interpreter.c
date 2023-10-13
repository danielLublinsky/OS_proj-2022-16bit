#include "../includes/interpreter.h"


char* substr;
unsigned int istr;
int os_error;
int power(int e, int n)
{
    int res = 1;

    for (; e > 0; e--) {
        res *= n;
    }

    return res;
}

unsigned int extract_number(char* str, unsigned int endstr)
{
    unsigned int decade = power(endstr - 1, 10);
    unsigned int n = 0;
    unsigned int i;

    for (i = 0; i < endstr; i++, decade /= 10) {
        n += ((int)str[i] - 48) * decade;
    }

    return n;
}

int expr(int n1, char symb, int n2)
{

    switch (symb) {
    case '+':
        return n1 + n2;
    case '-':
        return n1 - n2;
    case '*':
        return n1 * n2;
    case '/':
        if (n2 > 0) return n1 / n2;
        
        os_error = ERROR_CALC;
        printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r%s", "[e] cant divide by zero");
        return;
    case '%':
        if (n2 > 0) return n1 % n2;
        os_error = ERROR_CALC;
        printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r%s", "[e] cant divide by zero");
        return;
    default:
        printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r%s", "[e] cant parse symb");
        break;
    }

    return 1;
}

int bcalc(char* str, int res, bool secondary, bool brackets)
{
    char symb = '\0';
    unsigned int isubstr = 0;
    int child;

    if (istr == 0 && str[istr] == '(') {
        istr++;
        child = bcalc(str, 0, false, true);
        res = child;
    }
    else if (!secondary || brackets) {
        while (istr < strlen(str) && str[istr] != ' ') {
            if (str[istr] != '(') {
                substr[isubstr] = str[istr];
                isubstr++;
            }
            istr++;
        }

        res = extract_number(substr, isubstr);
        isubstr = 0;
    }

    istr++;

    for (; istr < strlen(str) + 1; istr++, isubstr++) {
        if (str[istr] == '(') {
            child = bcalc(str, extract_number(substr, isubstr), true, true);
            res = expr(res, symb, child);
            isubstr = -1;
        }
        else if (str[istr] == ')') {
            res = expr(res, symb, extract_number(substr, isubstr));
            if (str[istr + 2] == '*' \
                || str[istr + 2] == '/' \
                || str[istr + 2] == '%') {
                res = bcalc(str, res, true, false);
            }
            break;
        }
        else if (((int)str[istr] > 57 || (int)str[istr] < 48) \
            && str[istr] != ' ' && istr != strlen(str)
            ) {
            symb = str[istr];
            isubstr = -1;
        }
        else if (str[istr] == ' ' || istr == strlen(str)) {
            if ((str[istr + 1] == '*' || str[istr + 1] == '/' || str[istr + 1] == '%'
                ) && !secondary
                ) {
                if (str[istr - 2] == ')') {
                    child = bcalc(str, res, true, brackets);
                }
                else {
                    child = bcalc(str, extract_number(substr, isubstr), true, brackets);
                }
                res = expr(res, symb, child);
            }
            else if (isubstr > 0) {
                res = expr(res, symb, extract_number(substr, isubstr));
                if (secondary && (str[istr + 1] == '+' || str[istr + 1] == '-')) {
                    break;
                }
            }

            isubstr = -1;
        }
        else {
            substr[isubstr] = str[istr];
        }
    }


    return res;
}

int calc(char* str)
{
    os_error = 0;       //no ERROR
    substr = malloc(sizeof(char) * strlen(str));

    istr = 0;
    int retVal = bcalc(str, 0, false, false);
    if(os_error != ERROR_CALC)
        printf_system(DEFAULT_SYSTEMCOlOR, "\n\r%d", retVal);

    free(substr);
}