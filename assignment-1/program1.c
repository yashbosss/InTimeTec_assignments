#include <stdio.h>

int numbr(char c)
{
    return (c >= '0' && c <= '9');
}

int main()
{
    char ch;
    long result = 0, num = 0, Preval = 0;
    char opr = '+', Prevopr = '+';

    printf("Enter an expression: ");

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        if (ch == ' ') {
            continue;
        }

        if (numbr(ch))
        {
            num = num * 10 + (ch - '0');
        }

        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
        {
            if (Prevopr == '*') {
                Preval = Preval * num;
            } 
            else if (Prevopr == '/')
            {
                if (num == 0)
                {
                    printf("Error: Division by zero.\n");
                    return 0;
                }
                Preval = Preval / num;
            }
            else
            {
                Preval = num;
            }
            num = 0;

            if (ch == '+' || ch == '-')
            {
                if (opr == '+') {
                    result += Preval;
                }
                else if (opr == '-') {
                    result -= Preval;
                }
                  
                opr = ch;
                Prevopr = '+';
            }
            else
            {
                Prevopr = ch;
            }
        }
        else
        {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    if (Prevopr == '*') {
        Preval = Preval * num;
    }
        
    else if (Prevopr == '/')
    {
        if (num == 0)
        {
            printf("Error: Division by zero.\n");
            return 0;
        }
        Preval = Preval / num;
    }
    else
    {
        Preval = num;
    }

    //for the last plus or minus
    if (opr == '+') {
        result += Preval;
    }
    else if (opr == '-') {
        result -= Preval;
    }

    printf("%ld\n", result);
    return 0;
}
