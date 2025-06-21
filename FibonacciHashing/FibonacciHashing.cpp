#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <list>
#include <windows.h>
#include <psapi.h>
#include <functional>

using namespace std;

struct Value {
    string key;
    string val;
};

class FibonacciHashing {
private:
    vector<list<Value>> table; //Bảng băm
    size_t m = 0; // kích thước bảng băm có giá trị là 2^n
    int n = 0; // lũy thừa của cơ số 2
    const int w = 32; // kích thước của biến int (32 bit)
    const uint32_t FIBO_RATIO = 2654435769; // Tỉ lệ vàng * 2^32
    size_t count = 0; // số lượng khóa đã được phân bố vào bảng băm

    bool isSpaceOnly(string& s) {
        return all_of(s.begin(), s.end(), [](char ch) {
            return isspace(static_cast<unsigned char>(ch));
            });
    }

    size_t findTableSize(int size) { // Tìm kích thước bảng băm phù hợp
        size_t res = 1;
        n = 0;
        while (res <= size) {
            res = res << 1;
            n++;
        }
        return res;
    }

    int hashFunction(const string& key) {
        uint32_t k = 0;
        for (int i = 0; i < key.size(); ++i) { //Băm từng kí tự
            int val = static_cast<int>(key[i]);
            if (val < 0 || val > 127) return -1;
            k = k * 31 + val; // 31 là số nguyên tố thường dùng để tránh các va chạm như "AB" và "BA"
        }
        return ((k * FIBO_RATIO) >> (w - n));
    }

    bool rawInsert(const Value& data) { // Hàm phụ trợ cho việc tái băm
        int index = hashFunction(data.key);
        if (index < 0 || index >= m) { // index không hợp lệ
            return false;
        }
        table[index].push_back(data); // Không cần kiểm tra key đã tồn tại hay chưa vì trong bảng băm cũ, các key là duy nhất
        count++;
        return true;
    }

public:
    FibonacciHashing() {}

    void build(const string& path) {
        vector<Value> data;
        readFile(path, data);
        m = findTableSize((int)data.size()); // Tìm kích thước của bảng băm, đồng thời gán lại n là bậc lũy thừa cơ số 2 với 2^n = m
        table.assign(m, list<Value>());

        for (const auto& cur : data) {
            insert(cur);
        }
    }

    void readFile(const string& path, vector<Value>& vec) {
        ifstream ifs(path);
        if (!ifs.is_open()) {
            cerr << "Can't open file: " << path << endl;
            return;
        }
        string temp;
        Value cur;
        while (getline(ifs, temp)) {
            if (temp.empty() || isSpaceOnly(temp) || temp.size() < 3) continue;
            stringstream ss(temp);
            ss >> cur.key;
            cur.val = temp;
            vec.push_back(cur);
        }
        ifs.close();
    }

    bool insert(const Value& data) {
        if (loadFactor() >= 1.0) { // Hệ số tải lớn hơn hoặc bằng 1, tái băm dữ liệu 
            reHashing();
        }
        int index = hashFunction(data.key);
        if (index < 0 || index >= m) { // index không hợp lệ
            return false;
        }
        for (auto& cur : table[index]) {
            if (cur.key == data.key) {
                cur.val = data.val;
                return true; // Cập nhật lại giá trị vì key đã tồn tại
            }
        }
        table[index].push_back(data); // Trường hợp key chưa tồn tại
        count++;
        return true;
    }

    void reHashing() {
        m = m << 1;
        n++;
        count = 0;
        vector<list<Value>>oldTable = table;
        table.clear();
        table.assign(m, list<Value>());
        for (auto& bucket : oldTable) {
            for (auto& val : bucket) {
                rawInsert(val);
            }
        }
        oldTable.clear();
        oldTable.shrink_to_fit();
    }

    string search(const string& key) {
        int index = hashFunction(key);
        if (index < 0 || table[index].empty()) { // index không hợp lệ hoặc không tại key trong bảng băm
            return "Not Found";
        }
        auto it = table[index].begin();
        while (it != table[index].end() && it->key != key) {
            it++;
        }
        if (it == table[index].end()) { // Trường hợp key không tồn tại
            return "Not Found";
        }
        else { //Trường hợp key tồn tại, trả về giá trị của key
            return it->val;
        }
    }

    bool Delete(const string& key) {
        int index = hashFunction(key);
        if (index < 0 || table[index].empty()) { // index không hợp lệ hoặc không tại key trong bảng băm
            return false;
        }
        auto it = table[index].begin();
        while (it != table[index].end() && it->key != key) {
            it++;
        }
        if (it == table[index].end()) { // Trường hợp key không tồn tại
            return false;
        }
        else { //Trường hợp key tồn tại, xóa giá trị tìm được
            table[index].erase(it);
            count--;
            return true;
        }
    }

    double loadFactor() {
        return (double)count / m;
    }

    int maxChainLength() {
        int maxLen = 0;
        for (const auto& bucket : table) {
            maxLen = max(maxLen, (int)bucket.size());
        }
        return maxLen;
    }

    double averageChainLength() {
        if (count == 0) return 0.0;
        double sum = 0;
        int cnt = 0;
        for (const auto& bucket : table) {
            if (!bucket.empty()) {
                cnt++;
                sum += bucket.size();
            }
        }
        return sum / cnt; // Độ dài chuỗi trung bình trong các bucket đã sử dụng
    }

    void clear() { // Xóa bảng băm
        m = 0;
        n = 0;
        count = 0;
        table.clear();
        table.shrink_to_fit();
    }

    int getSize() {
        return count;
    }
};


class ModuloHashing {
private:
    vector<list<Value>> table;
    size_t m = 0; // kích thước bảng băm có giá trị là 2^n
    int n = 0; // lũy thừa của cơ số 2
    size_t count = 0; // số lượng khóa đã được phân bố vào bảng băm

    bool isSpaceOnly(string& s) {
        return all_of(s.begin(), s.end(), [](char ch) {
            return isspace(static_cast<unsigned char>(ch));
            });
    }

    size_t findTableSize(int size) {
        size_t res = 1;
        n = 0;
        while (res <= size) {
            res = res << 1;
            n++;
        }
        return res;
    }

    int hashFunction(const string& key) {
        uint32_t k = 0;
        for (int i = 0; i < key.size(); ++i) { //Băm từng kí tự
            int val = static_cast<int>(key[i]);
            if (val < 0 || val > 127) return -1;
            k = (k * 31 + val); // 31 là số nguyên tố thường dùng để tránh các va chạm như "AB" và "BA"
        }
        return k & (m - 1);
    }

    bool rawInsert(const Value& data) {
        int index = hashFunction(data.key);
        if (index < 0 || index >= m) { // index không hợp lệ
            return false;
        }
        table[index].push_back(data); // Không cần kiểm tra key đã tồn tại hay chưa vì trong bảng băm cũ, các key là duy nhất
        count++;
        return true;
    }

public:
    ModuloHashing() {}

    void build(const string& path) {
        vector<Value> data;
        readFile(path, data);
        m = findTableSize((int)data.size()); // Tìm kích thước của bảng băm, đồng thời gán lại n là bậc lũy thừa cơ số 2 với 2^n = m
        table.assign(m, list<Value>());

        for (const auto& cur : data) {
            insert(cur);
        }
    }

    void readFile(const string& path, vector<Value>& vec) {
        ifstream ifs(path);
        if (!ifs.is_open()) {
            cerr << "Can't open file: " << path << endl;
            return;
        }
        string temp;
        Value cur;
        while (getline(ifs, temp)) {
            if (temp.empty() || isSpaceOnly(temp) || temp.size() < 3) continue;
            stringstream ss(temp);
            ss >> cur.key;
            cur.val = temp;
            vec.push_back(cur);
        }
        ifs.close();
    }

    bool insert(const Value& data) {
        if (loadFactor() >= 1.0) { // Hệ số tải lớn hơn hoặc bằng 1, tái băm dữ liệu 
            reHashing();
        }
        int index = hashFunction(data.key);
        if (index < 0 || index >= m) { // index không hợp lệ
            return false;
        }
        for (auto& cur : table[index]) {
            if (cur.key == data.key) {
                cur.val = data.val;
                return true; // Cập nhật lại giá trị vì key đã tồn tại
            }
        }
        table[index].push_back(data); // Trường hợp key chưa tồn tại
        count++;
        return true;
    }

    void reHashing() {
        m = m << 1; 
        n++;
        count = 0;
        vector<list<Value>>oldTable = table;
        table.clear();
        table.assign(m, list<Value>());
        for (auto& bucket : oldTable) {
            for (auto& val : bucket) {
                rawInsert(val);
            }
        }
        oldTable.clear();
        oldTable.shrink_to_fit();
    }

    string search(const string& key) {
        int index = hashFunction(key);
        if (index < 0 || table[index].empty()) { // index không hợp lệ hoặc không tại key trong bảng băm
            return "Not Found";
        }
        auto it = table[index].begin();
        while (it != table[index].end() && it->key != key) {
            it++;
        }
        if (it == table[index].end()) { // Trường hợp key không tồn tại
            return "Not Found";
        }
        else { //Trường hợp key tồn tại, trả về giá trị của key
            return it->val;
        }
    }

    bool Delete(const string& key) {
        int index = hashFunction(key);
        if (index < 0 || table[index].empty()) { // index không hợp lệ hoặc không tại key trong bảng băm
            return false;
        }
        auto it = table[index].begin();
        while (it != table[index].end() && it->key != key) {
            it++;
        }
        if (it == table[index].end()) { // Trường hợp key không tồn tại
            return false;
        }
        else { //Trường hợp key tồn tại, xóa giá trị tìm được
            table[index].erase(it);
            count--;
            return true;
        }
    }

    double loadFactor() {
        return (double)count / m;
    }

    int maxChainLength() {
        int maxLen = 0;
        for (const auto& bucket : table) {
            maxLen = max(maxLen, (int)bucket.size());
        }
        return maxLen;
    }

    double averageChainLength() {
        if (count == 0) return 0.0;
        double sum = 0;
        int cnt = 0;
        for (const auto& bucket : table) {
            if (!bucket.empty()) {
                cnt++;
                sum += bucket.size();
            }
        }
        return sum / cnt; // Độ dài chuỗi trung bình trong các bucket đã sử dụng
    }

    void clear() { // Xóa bảng băm
        m = 0;
        n = 0;
        count = 0;
        table.clear();
        table.shrink_to_fit();
    }
    
    int getSize() {
        return count;
    }
};


SIZE_T getMemoryUsageMB() { //Hàm đo bộ nhớ đã sử dụng
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / (1024 * 1024); // Đơn vị: MB
    }
    return 0;
}

SIZE_T getMemoryUsageKB() { //Hàm đo bộ nhớ đã sử dụng
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024; // Đơn vị: KB
    }
    return 0;
}


int main() {
    string path;
    std::cout << "Input the path of file: ";
    cin >> path;
    std::cout << endl;
    

    // Hashing
    FibonacciHashing FH;
    auto start = chrono::high_resolution_clock::now();
    SIZE_T before = getMemoryUsageMB();
    FH.build(path);
    SIZE_T after = getMemoryUsageMB();
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    std::cout << "FIBONACCI HASHING: " << endl;
    std::cout << "Run time: " << duration.count() << "ms\t\t" << "Memory: " << after - before << " MB" << endl;
    std::cout << "Load factor: " << FH.loadFactor() << endl;
    std::cout << "Average chain length: " << FH.averageChainLength() << endl;
    std::cout << "Maximum chain length: " << FH.maxChainLength() << endl;
    std::cout << endl;

    ModuloHashing MH;
    start = chrono::high_resolution_clock::now();
    before = getMemoryUsageMB();
    MH.build(path);
    after = getMemoryUsageMB();
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    std::cout << "MODULO HASHING: " << endl;
    std::cout << "Run time: " << duration.count() << "ms\t\t" << "Memory: " << after - before << " MB" << endl;
    std::cout << "Load factor: " << MH.loadFactor() << endl;
    std::cout << "Average chain length: " << MH.averageChainLength() << endl;
    std::cout << "Maximum chain length: " << MH.maxChainLength() << endl;
    std::cout << endl;
    std::cout << "------------------------------------------------------" << endl << endl;


    // Search
    int n;
    std::cout << "Input the numbers of keys which you want to SEARCH: ";
    cin >> n;
    vector<string> vec(n);
    std::cout << "Input " << n << " keys: " << endl;
    for (string& s : vec) {
        cin >> s;
    }
    std::cout << endl;

    std::cout << "Result of Fibonacci Hashing:" << endl;
    start = chrono::high_resolution_clock::now();
    before = getMemoryUsageKB();
    for (string& s : vec) {
        std::cout<<FH.search(s)<<endl;
    }
    after = getMemoryUsageKB();
    end = chrono::high_resolution_clock::now();
    auto ans = chrono::duration_cast<chrono::microseconds>(end - start);
    std::cout << "Run time: " << ans.count() << " microseconds\t\t" << "Memory: " << after - before << " KB" << endl << endl;

    std::cout << "Result of Modulo Hashing:" << endl;
    start = chrono::high_resolution_clock::now();
    before = getMemoryUsageKB();
    for (string& s : vec) {
        std::cout << MH.search(s) << endl;
    }
    after = getMemoryUsageKB();
    end = chrono::high_resolution_clock::now();
    ans = chrono::duration_cast<chrono::microseconds>(end - start);
    std::cout << "Run time: " << ans.count() << " microseconds\t\t" << "Memory: " << after - before << " KB" << endl << endl;
    std::cout << "------------------------------------------------------" << endl << endl;


    // Delete
    std::cout << "Input the numbers of keys which you want to DELETE: ";
    cin >> n;
    std::cout << "Input " << n << " keys: " << endl;
    for (string& s : vec) {
        cin >> s;
    }
    std::cout << endl;

    std::cout << "Result of Fibonacci Hashing:" << endl;
    start = chrono::high_resolution_clock::now();
    before = getMemoryUsageKB();
    for (string& s : vec) {
        if (FH.Delete(s)) {
            std::cout << "Delete completed key " << s << endl;
            std::cout << "Result of search key " << s << " after delete: ";
            std::cout<< FH.search(s) << endl;
        }
        else {
            std::cout << "Delete is not completed" << endl;
        }
    }
    after = getMemoryUsageKB();
    end = chrono::high_resolution_clock::now();
    ans = chrono::duration_cast<chrono::microseconds>(end - start);
    std::cout << "Run time: " << ans.count() << " microseconds\t\t" << "Memory: " << after - before << " KB" << endl << endl;

    std::cout << "Result of Modulo Hashing:" << endl;
    start = chrono::high_resolution_clock::now();
    before = getMemoryUsageKB();
    for (string& s : vec) {
        if (MH.Delete(s)) {
            std::cout << "Delete completed key " << s << endl;
            std::cout << "Result of search key " << s << " after delete: ";
            std::cout << MH.search(s) << endl;
        }
        else {
            std::cout << "Delete is not completed" << endl;
        }
    }
    after = getMemoryUsageKB();
    end = chrono::high_resolution_clock::now();
    ans = chrono::duration_cast<chrono::microseconds>(end - start);
    std::cout << "Run time: " << ans.count() << " microseconds\t\t" << "Memory: " << after - before << " KB" << endl << endl;
    std::cout << "------------------------------------------------------" << endl << endl;


    // Insert
    std::cout << "Input the numbers of keys and values which you want to INSERT: ";
    cin >> n;
    vector<Value> vec1(n);
    std::cout << "Input " << n << " keys and values: " << endl;
    cin.ignore();
    for (int i = 0; i < n; i++) {
        string s;
        getline(cin, s);
        vec1[i].val = s;
        stringstream ss(s);
        ss >> vec1[i].key;
    }
    std::cout << endl;

    std::cout << "Result of Fibonacci Hashing:" << endl;
    start = chrono::high_resolution_clock::now();
    before = getMemoryUsageKB();
    for (auto& cur : vec1) {
        if (FH.insert(cur)) {
            std::cout << "Insert key " << cur.key << " completed" << endl;
            std::cout << "Result of search key " << cur.key << " after insert: " << endl;
            std::cout << FH.search(cur.key) << endl;
        }
        else {
            std::cout << "Insert key " << cur.key << " is not completed" << endl;
        }
    }
    after = getMemoryUsageKB();
    end = chrono::high_resolution_clock::now();
    ans = chrono::duration_cast<chrono::microseconds>(end - start);
    std::cout << "Run time: " << ans.count() << " microseconds\t\t" << "Memory: " << after - before << " KB" << endl << endl;

    std::cout << "Result of Modulo Hashing:" << endl;
    start = chrono::high_resolution_clock::now();
    before = getMemoryUsageKB();
    for (auto& cur : vec1) {
        if (MH.insert(cur)) {
            std::cout << "Insert key " << cur.key << " completed" << endl;
            std::cout << "Result of search key " << cur.key << " after insert: " << endl;
            std::cout << MH.search(cur.key) << endl;
        }
        else {
            std::cout << "Insert key " << cur.key << " is not completed" << endl;
        }
    }
    after = getMemoryUsageKB();
    end = chrono::high_resolution_clock::now();
    ans = chrono::duration_cast<chrono::microseconds>(end - start);
    std::cout << "Run time: " << ans.count() << " microseconds\t\t" << "Memory: " << after - before << " KB" << endl << endl;
    std::cout << "------------------------------------------------------" << endl << endl;


    FH.clear();
    MH.clear();

    return 0;
}