#include <iostream>

struct link
{
    int value;
    link *next;
};

int main()
{
    link finalElement;
    link firstElement;
    link secondElement;

    link *aList = &firstElement;
    firstElement.value = 1;
    firstElement.next = &secondElement;
    secondElement.next = &finalElement;
    secondElement.value = 2;
    finalElement.value = 3;
    finalElement.next = nullptr; //best practice use nullptr not 0

    for (link *p = aList; p != 0; p = p->next)
    {
        std::cout << p->value << " " << std::endl;
    }
};
