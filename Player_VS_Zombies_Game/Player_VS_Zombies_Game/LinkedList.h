#include<iostream>
using namespace std;

template<class T>
class DNode
{
public:
    T Data;
    DNode* Previous;
    DNode* Next;
    DNode(const T& _Data)
    {
        Data = _Data;
        Previous = nullptr;
        Next = nullptr;
    }
};

template<class T>
class LinkedList
{
private:
    DNode<T>* Head;
    DNode<T>* Tail;
public:
    LinkedList()
    {
        Head = nullptr;
        Tail = nullptr;
    }
    LinkedList(const LinkedList& other)
    {
        Head = nullptr;
        Tail = nullptr;

        DNode<T>* current = other.Head;
        while (current != nullptr) {
            insertAtEnd(current->Data);
            current = current->Next;
        }
    }

    LinkedList& operator=(const LinkedList& other)
    {
        if (this != &other)
        {
            DNode<T>* current = Head;
            while (current != nullptr)
            {
                DNode<T>* next = current->Next;
                delete current;
                current = next;
            }
            Head = nullptr;
            Tail = nullptr;

            current = other.Head;
            while (current != nullptr)
            {
                insertAtEnd(current->Data);
                current = current->Next;
            }
        }
        return *this;
    }


    DNode<T>* getHead() const { return Head; };
    DNode<T>* getTail() const { return Tail; };

    bool isEmpty()
    {
        return Head == nullptr && Tail == nullptr;
    }
    void insertAtEnd(const T& Value)
    {
        DNode<T>* NewNode = new DNode<T>(Value);
        if (isEmpty())
        {
            Head = NewNode;
            Tail = NewNode;
        }
        else
        {
            Tail->Next = NewNode;
            NewNode->Previous = Tail;
            Tail = NewNode;
        }
    }

    void deleteAtEnd()
    {
        if (isEmpty())
        {
            cout << "LinkedList is already Empty!" << endl;
            return;
        }
        DNode<T>* temp;
        if (Head->Next == nullptr)
        {
            temp = Head;
            delete temp;
            Head = nullptr;
            Tail = nullptr;
            return;
        }
        temp = Tail;
        Tail = Tail->Previous;
        Tail->Next = nullptr;
        delete temp;
    }

    DNode<T>* searchData(const T& Value)
    {
        DNode<T>* curr = Head;
        while (curr != nullptr)
        {
            if (curr->Data == Value)
            {
                return curr;
            }
            curr = curr->Next;
        }
        return nullptr;
    }



    void deleteValue(const T& Value)
    {
        DNode<T>* curr = searchData(Value);
        if (curr == nullptr)
        {
            cout << "Data not found!" << endl;
            return;
        }
        else
        {
            DNode<T>* temp;
            if (curr == Head && curr == Tail)
            {
                temp = Head;
                Head = nullptr;
                Tail = nullptr;
                delete temp;
            }
            else if (curr == Head)
            {
                temp = Head;
                Head = Head->Next;
                if (Head != nullptr)
                    Head->Previous = nullptr;
                delete temp;
            }
            else if (curr == Tail)
            {
                temp = Tail;
                Tail = Tail->Previous;
                if (Tail != nullptr)
                    Tail->Next = nullptr;
                delete temp;
            }
            else
            {
                temp = curr;
                curr->Previous->Next = curr->Next;
                curr->Next->Previous = curr->Previous;
                delete temp;
            }
        }
    }

    void print()
    {
        DNode<T>* curr = Head;
        while (curr != nullptr)
        {
            cout << curr->Data << " ";
            curr = curr->Next;
        }
        cout << endl;
    }

};
