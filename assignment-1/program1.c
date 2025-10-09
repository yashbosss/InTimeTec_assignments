#include <stdio.h>

int isDigit(char c)
{
    return (c >= '0' && c <= '9');
}

long processOperator(long a, long b, char op)
{
    switch (op)
    {
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        if (b == 0)
        {
            printf("Error: Division by zero.\n");
            return 0;
        }
        return a / b;
    default:
        printf("Error: Invalid operator '%c'.\n", op);
        return 0;
    }
}

long evaluateInput()
{
    long num = 0, result = 0, prevVal = 0;
    char currOp = '+', prevOp = '+';
    char ch;

    printf("Enter an expression: ");

    while (scanf(" %c", &ch) == 1)
    {
        if (isDigit(ch))
        {
            num = num * 10 + (ch - '0');
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
        {
            switch (prevOp)
            {
            case '*':
                prevVal = prevVal * num;
                break;
            case '/':
                if (num == 0)
                {
                    printf("Error: Division by zero.\n");
                    return 0;
                }
                prevVal = prevVal / num;
                break;
            default:
                prevVal = num;
            }

            if (ch == '+' || ch == '-')
            {
                result = processOperator(result, prevVal, currOp);
                currOp = ch;
                prevOp = '+';
            }
            else
            {
                prevOp = ch;
            }

            num = 0;
        }
        else if (ch == '=')
        {
            break;
        }
        else if (ch == '-')
        {
            scanf("%ld", &num);
            num = -num;
        }
        else
        {
            printf("Error: Invalid character in expression.\n");
            return 0;
        }
    }

    switch (prevOp)
    {
    case '*':
        prevVal = prevVal * num;
        break;
    case '/':
        if (num == 0)
        {
            printf("Error: Division by zero.\n");
            return 0;
        }
        prevVal = prevVal / num;
        break;
    default:
        prevVal = num;
    }

    result = processOperator(result, prevVal, currOp);
    return result;
}

int main()
{
    long result = evaluateInput();
    printf("Result: %ld\n", result);
    return 0;
}
