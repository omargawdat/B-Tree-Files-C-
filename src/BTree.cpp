#include <iostream>
#include <vector>
#include <fstream>
#include "Node.cpp"

using namespace std;

class BTree {
    fstream wrTree;
    string file = R"(..\Data\BinaryFile.txt)";
    int recordsCount;
    int m;

public:
    BTree(int recordsCount, int m) : recordsCount(recordsCount), m(m) {
        wrTree.open(file, ios::in);
        wrTree.seekg(0, ios::end);
        if (wrTree.tellg() <= 0)
            createIndexFile();
        wrTree.close();
    }

    int insert(int value, int pointer) {
        Node header, root;
        vector<Key> vec;

        wrTree.close();
        wrTree.open(file, ios::in | ios::out);
        wrTree.read((char *) &header, sizeof(Node));
        wrTree.read((char *) &root, sizeof(Node));

        Key key(value, pointer);
        vector<int> route = traverse(value);

        Node node = readNode(route.back());
        for (auto v: node.getKeys()) {
            if (v.getValue() == value) {
                cout << "the key id is already exist" << endl;
                return -1;
            }
        }

        if (isEnoughSpace(vector<int>(route))) {
            if (root.isLeaf() == -1) {
                header.setNextAvailable(root.getNextAvailable());
                root.setLeaf(0);
                writeNode(0, header);
                writeNode(1, root);
            }
            non_full(vector<int>(route), key);
            return route[route.size() - 1];
        } else {
            cout << "their is no enough space to save the value" << endl;
            return -1;
        }
    }

    vector<int> traverse(int value) {
        // this function is used to get the route from the root into the target node
        // target node: the node where the new key should be inserted
        // insertion: we need to keep the parents RRNs to back to them while splitting, we need to check the last element in the vector to see if the key already exist or not
        // Searching: we need to find if the key exist or not

        int nextNodeRRN;
        vector<int> v;
        v.push_back(1);

        Node currentNode = readNode(1);
        while (currentNode.isLeaf() == 1) {
            // 1. iterate until reaching the point which is higher than value
            vector<Key> currentKeys = currentNode.getKeys();

            for (int i = 0; i < currentKeys.size(); ++i) {
                if (currentKeys[i].getValue() >= value or (i == currentKeys.size() - 1)) {
                    v.push_back(currentKeys[i].getPointer());
                    currentNode = readNode(currentKeys[i].getPointer());
                    break;
                }
            }
        }
        return v;
    }


    // return if it's created?
    void non_full(vector<int> routeVector, Key key) {
        // Get the currentNode
        int currentNodeRRn = routeVector.back();
        routeVector.pop_back();
        Node currentNode = readNode(currentNodeRRn);

        // sort the keys
        vector<Key> keys = currentNode.getKeys();
        keys.push_back(key);
        sort(keys.begin(), keys.end());
        // check the size
        if (keys.size() <= 5) {
            currentNode.setKeys(keys);
            writeNode(currentNodeRRn, currentNode);
            // Update the parent value with max value
            if (routeVector.size()) {
                Node parentNode2 = readNode(routeVector.back());
                vector<Key> tempVec = parentNode2.getKeys();
                int newValue = keys[keys.size() - 1].getValue();
                if (tempVec[tempVec.size() - 1].getValue() < newValue) {
                    tempVec[tempVec.size() - 1].setValue(newValue);
                    parentNode2.setKeys(tempVec);
                    writeNode(routeVector.back(), parentNode2);
                }
            }
        } else { // split
            vector<Key> left_keys(keys.begin(), keys.begin() + 3);
            vector<Key> right_keys(keys.begin() + 3, keys.end());

            if (routeVector.empty()) {
                // Code for ROOT
                Node root = readNode(1);
                // Create left child
                Key key1 = createNewNode(left_keys, root.isLeaf());
                // Create right child
                Key key2 = createNewNode(right_keys, root.isLeaf());

                root.setLeaf(1);
                vector<Key> kk = {key1, key2};
                root.setKeys(kk);
                writeNode(1, root);
            } else {
                int parentIndex = routeVector.back();

                // 1. Parent Node edit the last value
                Node parentNode2 = readNode(routeVector.back());
                vector<Key> tempVec = parentNode2.getKeys();

                int newValue = keys[keys.size() - 1].getValue();
                if (tempVec[tempVec.size() - 1].getValue() < newValue) {
                    tempVec[tempVec.size() - 1].setValue(keys[keys.size() - 1].getValue());
                    parentNode2.setKeys(tempVec);
                    writeNode(routeVector.back(), parentNode2);
                }


                // save last 3 (right)
                currentNode.setKeys(right_keys);
                writeNode(currentNodeRRn, currentNode);

                // save first 3 (left) as a new node and save the key in the parent node
                Key keyReferenceInParentNode = createNewNode(left_keys, currentNode.isLeaf());
                non_full(routeVector, keyReferenceInParentNode);
            }
        }
    }

    Key createNewNode(vector<Key> v, bool isLeaf) {
        // Return a key which contain the RRN of the node and the value of the max value

        Node node;
        sort(v.begin(), v.end());
        node.setKeys(v);
        node.setLeaf(isLeaf);

        // Swap Next Available in the two nodes
        Node header = readNode(0);
        int nextAvail = header.getNextAvailable();
        header.setNextAvailable(readNode(nextAvail).getNextAvailable());
        writeNode(0, header);

        writeNode(nextAvail, node);

        return {v[v.size() - 1].getValue(), nextAvail};
    }

    bool isEnoughSpace(vector<int> routeToLeaf) {
        Node currentNode = readNode(routeToLeaf[routeToLeaf.size() - 1]);
        routeToLeaf.pop_back();

        int requiredSpace = 0;
        while ((currentNode.getKeys().size() >= 5)) {
            requiredSpace += 1;
            if (routeToLeaf.empty()) {
                requiredSpace += 1;
                break;
            }
            currentNode = readNode(routeToLeaf[routeToLeaf.size() - 1]);
            routeToLeaf.pop_back();
        }

        Node header = readNode(0);
        int availableSpace = 0;
        while (header.getNextAvailable() != -1) {
            availableSpace += 1;
            header = readNode(header.getNextAvailable());
        }
        return availableSpace >= requiredSpace;
    }

    Node readNode(int RRN) {
        Node node;
        wrTree.close();
        wrTree.open(file, ios::in | ios::out);
        wrTree.seekg(RRN * sizeof(Node), ios::beg);
        wrTree.read((char *) &node, sizeof(Node));
        return node;
    }

    void writeNode(int RRN, Node node) {
        wrTree.close();
        wrTree.open(file, ios::in | ios::out);
        wrTree.seekg(RRN * sizeof(Node), ios::beg);
        wrTree.seekg(RRN * sizeof(Node), ios::beg);
        wrTree.write((char *) &node, sizeof(Node));
//        wrTree.close();
    }

    int search(int value) {
        Node header, root;

        wrTree.close();
        wrTree.open(file, ios::in | ios::out);
        wrTree.read((char *) &header, sizeof(Node));

        vector<int> route = traverse(value); // 1 3 5

        Node node = readNode(route.back());
        for (auto v: node.getKeys()) {
            if (v.getValue() == value) return v.getPointer();
        }
        return -1;
    }


    void print() {
        Node node;
        wrTree.close();
        wrTree.open(file, ios::in);
        wrTree.read((char *) &node, sizeof(Node));

        vector<Key> v = node.getKeys();
        cout << "Next Available Node: " << v[0].getPointer() << endl;
        for (int i = 0; i < recordsCount - 1; ++i) {
            cout << "Record " << i + 1 << ": ";
            wrTree.read((char *) &node, sizeof(Node));
            cout << node;
        }
    }

    void createIndexFile() {
        wrTree.close();
        wrTree.open(file, ios::out);
        for (int i = 1; i < recordsCount; i++) {
            wrTree.write((char *) new Node(i), sizeof(Node));
        }
        wrTree.write((char *) new Node(-1), sizeof(Node));
        wrTree.close();
    }

    void deleteKey(int value) {
        if (checkIfExist(value) != -1) {
            vector<int> route = traverse(value);
            int position = route.back();
            route.pop_back();

            Node node = readNode(position);

            vector<Key> myKeys = node.getKeys();

            myKeys.erase(remove_if(myKeys.begin(), myKeys.end(), [value](Key p) { return p.getValue() == value; }),
                         myKeys.end());

            // is three a valid number?
            node.setKeys(myKeys);
            writeNode(position, node);

            // Update parent
            Node parentNode2 = readNode(route.back());
            vector<Key> tempKeys = parentNode2.getKeys();
            tempKeys[tempKeys.size() - 1].setValue(myKeys[myKeys.size() - 1].getValue());
            parentNode2.setKeys(tempKeys);
            writeNode(route.back(), parentNode2);


            if (myKeys.size() < 2) {
                if (!redistribute(value)) {
                    merge(value);
                }
            }
        } else cout << "Not found " << endl;
    }


    bool redistribute(int value) {
        vector<int> route = traverse(value);
        int currentRecordNumber = route.back();
        Node currentNode = readNode(currentRecordNumber);
        route.pop_back();
        int parentPosition = route.back();
        Node parent = readNode(parentPosition);

        vector<Key> parentKeys = parent.getKeys();

        if (parentKeys[0].getPointer() == currentRecordNumber) return false;

        for (int i = 1; i < parentKeys.size(); ++i) {
            if (parentKeys[i].getPointer() == currentRecordNumber) {
                int siblingRecordNumber = parentKeys[i - 1].getPointer();
                Node sibilingNode = readNode(siblingRecordNumber);

                vector<Key> sibilingKeys = sibilingNode.getKeys();

                if (sibilingKeys.size() != 2) {
                    // Edit Sibling
                    Key lastKey = sibilingKeys[sibilingKeys.size() - 1];
                    sibilingKeys.pop_back();
                    sibilingNode.setKeys(sibilingKeys);
                    writeNode(siblingRecordNumber, sibilingNode);

                    // edit parent max key value
                    parentKeys[i - 1].setValue(sibilingKeys[sibilingKeys.size() - 1].getValue());
                    parent.setKeys(parentKeys);
                    writeNode(parentPosition, parent);

                    Node root = readNode(1);
                    vector<Key> rootKeys = root.getKeys();
                    rootKeys[0].setValue(8);
                    root.setKeys(rootKeys);
                    writeNode(1, root);


                    // Edit Current Node
                    vector<Key> currentKeys = currentNode.getKeys();
                    currentKeys.push_back(lastKey);
                    sort(currentKeys.begin(), currentKeys.end());
                    currentNode.setKeys(currentKeys);
                    writeNode(currentRecordNumber, currentNode);
                    return true;
                } else return false;
            }
        }
        return false;
    }

    void merge(int value) {
        vector<int> route = traverse(value);

        int currentRecordNumber = route.back();
        Node currentNode = readNode(currentRecordNumber);
        Key currentKey = currentNode.getKeys()[0];
        route.pop_back();
        int parentPosition = route.back();
        Node parentNode = readNode(parentPosition);
        vector<Key> parentKeys = parentNode.getKeys();

        int v = currentKey.getValue();
        parentKeys.erase(
                remove_if(parentKeys.begin(), parentKeys.end(), [v](Key p) { return p.getValue() == v; }),
                parentKeys.end());

        // Delete Current Node
        writeNode(currentRecordNumber, Node(-1));

        for (int i = 1; i < parentKeys.size(); ++i) {
            if (parentKeys[i].getPointer() == currentRecordNumber) {
                // add the key into the sibiling
                int siblingRecordNumber = parentKeys[i - 1].getPointer();
                Node sibilingNode = readNode(siblingRecordNumber);
                vector<Key> sibilingKeys = sibilingNode.getKeys();
                sibilingKeys.push_back(currentKey);
                sibilingNode.setKeys(sibilingKeys);
                writeNode(siblingRecordNumber, sibilingNode);

                // update the parent sibling value
                parentKeys[i - 1].setValue(sibilingKeys[sibilingKeys.size() - 1].getValue());
                parentNode.setKeys(parentKeys);
                writeNode(parentPosition, parentNode);

                break;
            }
        }
    }

    int checkIfExist(int value) {
        Node header;

        wrTree.close();
        wrTree.open(file, ios::in | ios::out);
        wrTree.read((char *) &header, sizeof(Node));

        vector<int> route = traverse(value);
        Node node = readNode(route.back());
        for (auto v: node.getKeys()) {
            if (v.getValue() == value) {
                // the key has been found
                // back of the vector is the position of the target node
                return route.back();
            }
        }
        return -1;
    }

    // todo update avail

};