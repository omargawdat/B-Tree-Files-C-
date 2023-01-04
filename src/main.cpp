#include <iostream>
#include "BTree.cpp"

using namespace std;


int main() {
    int records_count = 10, m = 5;

//    cout << "Records Count:";
//    cin >> records_count;

    BTree bTree = BTree(records_count, m);

    // Doctor Examples
    bTree.createIndexFile();
    bTree.insert(3, 12);
    bTree.insert(7, 24);
    bTree.insert(10, 48);
    bTree.insert(24, 60);
    bTree.insert(14, 72);

    bTree.insert(19, 84);

    bTree.insert(30, 96);
    bTree.insert(15, 108);
    bTree.insert(1, 120);
    bTree.insert(5, 132);

    bTree.insert(2, 144);

    bTree.insert(8, 156);
    bTree.insert(9, 168);
    bTree.insert(6, 180);
    bTree.insert(11, 192);
    bTree.insert(12, 204);
    bTree.insert(17, 216);
    bTree.insert(18, 228);

    bTree.insert(32, 240);

    cout << bTree.search(32) << endl ;
    cout << bTree.search(50) << endl;
    bTree.deleteKey(10);
    bTree.deleteKey(9);
    bTree.deleteKey(8);

    bTree.print();

//    bTree.print();

//    char c;
//    do {
//        cout << "---MENU--" << endl;
//        cout << "1. Add New Key" << endl;
//        cout << "2. Search for Key" << endl;
//        cout << "3. Delete Key" << endl;
//        cout << "4. Print Binary File" << endl;
//        cout << "5. initialize Binary file" << endl;
//        cout << "press 'E' for Exit.." << endl;
//
//        cout << "choice:";
//        cin >> c;
//        switch (c) {
//            int value, key;
//
//            case '1': {
//                cout << "value, key:";
//                cin >> value >> key;
//                int pos = bTree.insert(value, key);
//                cout << "Position of the record: " << pos << endl;
//                break;
//            }
//            case '2': {
//                cout << "key:";
//                cin >> key;
//                int index = bTree.search(key);
//
//                if (index == -1) cout << "this ID doesn't exist" << endl;
//                else cout << "ID index:" << index << endl;
//
//                break;
//            }
//            case '3': {
//
//                break;
//            }
//            case '4': {
//                bTree.print();
//                break;
//            }
//            case '5': {
//                bTree.createIndexFile();
//                break;
//            }
//            default:
//                break;
//        }
//        cout << "\n";
//    } while (toupper(c) != 'E');
    return 0;
}