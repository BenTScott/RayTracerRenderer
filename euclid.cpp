#include <iostream>

using namespace std;

int hcf(int n, int m)
{
    int r = n % m;

    if (r == 0)
    {
        return m;
    }

    return hcf(m, r);
};

int iter_hcf(int n, int m)
{
    int r = n % m;

    while (r != 0)
    {
        n = m;
        m = r;
        r = n % m;
    }

    return m;
};

int main()
{
    int n;
    int m;

    char prompt[] = "Enter an integer: ";
    cout << prompt;
    cin >> n;
    cout << prompt;
    cin >> m;

    cout << "The highest common factor is " << hcf(n, m) << endl;
    cout << "The highest common factor is " << iter_hcf(n, m) << endl;
};