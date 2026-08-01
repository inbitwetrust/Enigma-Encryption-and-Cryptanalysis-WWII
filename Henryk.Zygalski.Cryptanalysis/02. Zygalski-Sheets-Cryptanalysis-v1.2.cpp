
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <fstream>
#include <sstream>

using namespace std;

string ROTORS[5] = {

    "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
    "AJDKSIRUXBLHWTMCQGZNPYFVOE",
    "BDFHJLCPRTXVZNYEIWGAKMUSQO",
    "ESOVPZJAYQUIRHXLNFTGKDCMWB",
    "VZBRGITYUPSDNHLXAWMJQOFECK"
};

char NOTCHES[5] = {'Q', 'E', 'V', 'J', 'Z'};
string REFLECTOR = "YRUHQSLDPXNGOKMIEBFZCWVJAT";

struct Record {
    int type;
    string indicator;
};

struct Enigma {
    int w[3][26];
    int inv_w[3][26];
    int n[3];
    int p[3];
    int r[3];
    int ref[26];

    void step() {
        bool s1 = (p[1] == n[1]);
        bool s2 = (p[2] == n[2]);
        if (s1) {
            p[0] = (p[0] + 1) % 26;
            p[1] = (p[1] + 1) % 26;
        } else if (s2) {
            p[1] = (p[1] + 1) % 26;
        }
        p[2] = (p[2] + 1) % 26;
    }

    int enc(int c) {
        for (int i = 2; i >= 0; --i) {
            int shift = (p[i] - r[i] + 26) % 26;
            c = (w[i][(c + shift) % 26] - shift + 26) % 26;
        }
        c = ref[c];
        for (int i = 0; i < 3; ++i) {
            int shift = (p[i] - r[i] + 26) % 26;
            c = (inv_w[i][(c + shift) % 26] - shift + 26) % 26;
        }
        return c;
    }
};

Enigma create_enigma(vector<int> rots, string ring, string pos) {
    Enigma e;
    for (int i = 0; i < 3; ++i) {
        e.n[i] = NOTCHES[rots[i]] - 'A';
        e.r[i] = ring[i] - 'A';
        e.p[i] = pos[i] - 'A';
        for (int j = 0; j < 26; ++j) {
            e.w[i][j] = ROTORS[rots[i]][j] - 'A';
            e.inv_w[i][ROTORS[rots[i]][j] - 'A'] = j;
        }
    }
    for (int j = 0; j < 26; ++j) {
        e.ref[j] = REFLECTOR[j] - 'A';
    }
    return e;
}

bool has_female(Enigma e, int type) {
    int step_a = type / 10;
    int step_b = type % 10;
    Enigma ea = e;
    for(int i = 0; i < step_a; ++i) ea.step();
    Enigma eb = e;
    for(int i = 0; i < step_b; ++i) eb.step();

    for (int x = 0; x < 26; ++x) {
        if (ea.enc(x) == eb.enc(x)) return true;
    }
    return false;
}

mutex print_mtx;
atomic<int> completed_orders(0);

void worker(vector<vector<int>> orders, vector<Record> records) {
    for (auto rots : orders) {
        for (int i = 0; i < 17576; ++i) {
            string ring = "";
            ring += (char)('A' + (i / 676));
            ring += (char)('A' + ((i / 26) % 26));
            ring += (char)('A' + (i % 26));

            bool valid = true;
            for (auto rec : records) {
                Enigma e = create_enigma(rots, ring, rec.indicator);
                if (!has_female(e, rec.type)) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                lock_guard<mutex> lock(print_mtx);
                cout << "\n[!] ZYGALSKI MATCH FOUND [!] \n";
                cout << "    Rotor Order: " << rots[0]+1 << "-" << rots[1]+1 << "-" << rots[2]+1 << "\n";
                cout << "    Ringstellung: " << ring << "\n";
            }
        }
        completed_orders++;
    }
}

int main() {
    cout << R"(
 + --------------------------------------------------------- +
 |  ██████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗          |
 |  ██╔═══╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗         |
 |  █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║        |
 |  ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║        |
 |  ██████╗ ██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║        |
 |  ╚═════╝ ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝        |
 |                                                           |
 |  Zygalski Sheets Cryptanalysis Simulator v1.2             |
 |  Patched Edition: Dynamic Indicator Parsing               |
 |                                                           |
 |  Academic Cryptanalysis Demonstrator v16                  |
 |                                                           |
 |  Created by George W. Aravidis                            |
 |  Cybersicherheitsforschung                                |
 |                                                           |
 |  Ulm Söflingen | Deutschland                              |
 |  Email address # csec.unbutton326@passinbox.com           |
 + --------------------------------------------------------- +
)" << "\n\n";

    cout << "[*] System Initialized. Awaiting intercepted messages file.\n";
    
    string filename;
    cout << "[?] Enter the filename containing the intercepts (e.g., intercepts.txt): ";
    cin >> filename;

    ifstream infile(filename);
    if (!infile.is_open()) {
        cout << "[-] ERROR: Could not open the specified file. Are you sure the path is correct?\n";
        return 1;
    }

    vector<Record> records;
    string line;
    int total_scanned = 0;

    cout << "[*] Parsing file and filtering for 'female' characteristics...\n";

    while (getline(infile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string indicator, word;
        
        ss >> indicator >> word;

        if (indicator.length() == 3 && word.length() >= 6) {
            total_scanned++;
            
            if (word[0] == word[3]) {
                records.push_back({14, indicator});
                cout << "    [+] Found Type 14 female with Indicator " << indicator << ": " << word << "\n";
            }
            if (word[1] == word[4]) {
                records.push_back({25, indicator});
                cout << "    [+] Found Type 25 female with Indicator " << indicator << ": " << word << "\n";
            }
            if (word[2] == word[5]) {
                records.push_back({36, indicator});
                cout << "    [+] Found Type 36 female with Indicator " << indicator << ": " << word << "\n";
            }
        }
    }

    cout << "\n[*] File parsing complete.\n";
    cout << "    Total messages scanned: " << total_scanned << "\n";
    cout << "    Total 'females' extracted: " << records.size() << "\n\n";

    if (records.empty()) {
        cout << "[-] No females found in the intercepted traffic. The cryptanalysis cannot proceed.\n";
        cout << "    Wait for more messages from the intercept stations.\n";
        return 0;
    }

    cout << "[*] Outstanding! Stacking the digital Zygalski sheets based on extracted data...\n";
    cout << "[*] Shining the analog lamps... searching 1,054,560 combinations.\n\n";

    vector<vector<int>> all_rots;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (i == j) continue;
            for (int k = 0; k < 5; ++k) {
                if (i == k || j == k) continue;
                all_rots.push_back({i, j, k});
            }
        }
    }

    unsigned int cores = thread::hardware_concurrency();
    if (cores == 0) cores = 4;

    vector<thread> threads;
    int chunk = 60 / cores;
    if (chunk == 0) chunk = 1;

    for (unsigned int i = 0; i < cores; ++i) {
        int start = i * chunk;
        int end = (i == cores - 1) ? 60 : start + chunk;
        if (start >= 60) break;
        vector<vector<int>> slice(all_rots.begin() + start, all_rots.begin() + end);
        threads.push_back(thread(worker, slice, records));
    }

    while (completed_orders < 60) {
        cout << "\r    [~] Scanning Rotor Orders: " << completed_orders << " / 60 completed..." << flush;
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    cout << "\r    [~] Scanning Rotor Orders: 60 / 60 completed...          \n";
    cout << "\n[*] Search finished!\n";
    cout << "[*] Excellent work today, cryptanalyst! Goodbye.\n";

    return 0;
}
