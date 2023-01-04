#include <iostream>
#include <vector>
#include <cstring>
# include <string>
#include <cstdlib>

using namespace std;

class Key {
    char value[10]{};
    char pointer[10]{};
public :
    Key() {
        strncpy(value, "-1", sizeof(value) - 1);
        strncpy(pointer, "-1", sizeof(pointer) - 1);
    }

    Key(int v, int p) {
        setValue(v);
        setPointer(p);
    }

    bool operator<(const Key &k) const { return stoi(value) < stoi(k.value); }


    int getValue() {
        return atoi(value);
    }

    void setValue(int v) {
        strncpy(this->value, to_string(v).c_str(), sizeof(value) - 1);}

    void setPointer(int p) { strncpy(this->pointer, to_string(p).c_str(), sizeof(pointer) - 1); }

    int getPointer() { return atoi(pointer); }
};

class Node {
    char is_leaf[3]{};
    Key keys[5]{};
public:
    Node() = default;

    Node(int nextAvailable) {
        setNextAvailable(nextAvailable);
        strncpy(this->is_leaf, to_string(-1).c_str(), sizeof(is_leaf) - 1);
    }

    void setLeaf(int isLeaf) { strncpy(this->is_leaf, to_string(isLeaf).c_str(), sizeof(is_leaf) - 1); }

    int isLeaf() { return atoi(is_leaf); }

    int getNextAvailable() {
        if (atoi(is_leaf) == -1)
            return keys[0].getPointer();
        else {
            return -1;
        }
    }

    void setNextAvailable(int nextAvailable) {
        keys[0].setPointer(nextAvailable);
    }


    vector<Key> getKeys() {
        std::vector<Key> vec(keys, keys + sizeof(keys) / sizeof(Key));
        vec.erase(remove_if(vec.begin(), vec.end(), [](Key &p) { return p.getValue() == -1; }), vec.end());
        return vec;
    }

    void setKeys(vector<Key> vec) {
        for (int i = 0; i < vec.size(); i++)
            keys[i] = vec[i];

        for (int i = vec.size(); i < 5; i++) {
            keys[i] = Key();
        }
    }


    friend ostream &operator<<(ostream &str, Node &node) {
        str << node.is_leaf << " ";

        for (auto &key: node.keys)
            str << "(" << key.getPointer() << " " << key.getValue() << "), ";

        str << endl;
        return str;
    }
};