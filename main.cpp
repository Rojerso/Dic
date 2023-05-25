#include <iostream>
#include <fstream>
#include "Dictionary.h"

using namespace std;

int main() {
    Dictionary dict;
    ifstream file("dictionary.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            int pos = line.find(",");
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);
            dict.insert(key, value);
            Node* tmp_data = dict.search(key);
            cout << tmp_data->numKeys << " " << "" << endl;
        }
        file.close();
    }
    else {
        cout << "Unable to open file" << endl;
        return 0;
    }
    vector<vector<string>> translations;
    Node* tmp_data = dict.search("apple");
    for (int i = 0; i < tmp_data->numKeys; i++) {
        for (size_t j = 0; j < translations[i].size(); j++)
        {
            translations.push_back(tmp_data->values[i]);
            cout << "- " << translations[i][j] << endl;
        }
    }

    if (translations.size() > 0) {
        cout << "Translations of 'apple':" << endl;
        for (int i = 0; i < 2; i++) {
            for (size_t j = 0; j < translations[i].size(); j++)
            {
                cout << "- " << translations[i][j] << endl;
            }
        }
    }
    else {
        cout << "Translation not found" << endl;
    }
    return 0;
}