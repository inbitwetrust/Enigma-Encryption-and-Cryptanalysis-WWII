
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;

string get_chain_lengths(char perm[26]) {
    bool visited[26] = {false};
    vector<int> chain_lengths;

    for (int i = 0; i < 26; ++i) {
        if (perm[i] == 0) continue; 
        
        if (!visited[i]) {
            int current = i;
            int length = 0;
            while (!visited[current] && perm[current] != 0) {
                visited[current] = true;
                current = perm[current] - 'A';
                length++;
            }
            chain_lengths.push_back(length);
        }
    }
    
    sort(chain_lengths.begin(), chain_lengths.end());
    string res = "";
    for (int len : chain_lengths) res += to_string(len);
    return res;
}

void print_banner() {
    cout << R"(
+ --------------------------------------------------------- +
|  ██████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗          |
|  ██╔═══╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗         |
|  █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║        |
|  ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║        |
|  ██████╗ ██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║        |
|  ╚═════╝ ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝        |
|                                                           |
|  World War II Enigma — Cryptanalysis Exploitation Tool    |
|  Rejewski Chain Footprint Extractor & Matcher             |
|  High-Speed Target Index Database Linear Search           |
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
}

int main() {
    print_banner();

    char AD[26] = {0}, BE[26] = {0}, CF[26] = {0};
    
    ifstream infile("InterceptedMessages.txt");
    if (!infile.is_open()) {
        cerr << "[!] Oh no! I couldn't find 'InterceptedMessages.txt'. Please ensure the file is in the same folder!" << endl;
        return 1;
    }

    string cipher_word;
    int msg_count = 0;
    
    cout << "[*] Welcome! Loading your intercepted messages and extracting permutations..." << endl;
    
    while (infile >> cipher_word) {
        if (cipher_word.length() >= 6) {
            if (AD[cipher_word[0] - 'A'] == 0) AD[cipher_word[0] - 'A'] = cipher_word[3];
            if (BE[cipher_word[1] - 'A'] == 0) BE[cipher_word[1] - 'A'] = cipher_word[4];
            if (CF[cipher_word[2] - 'A'] == 0) CF[cipher_word[2] - 'A'] = cipher_word[5];
            msg_count++;
        }
        infile >> cipher_word; 
    }
    infile.close();

    string target_index = "AD:" + get_chain_lengths(AD) + 
                          " BE:" + get_chain_lengths(BE) + 
                          " CF:" + get_chain_lengths(CF);
                          
    cout << "[+] Fantastic job! Successfully analyzed " << msg_count << " messages." << endl;
    cout << "[+] Your target Mathematical Fingerprint is:\n--> " << target_index << endl;

    cout << "\n[*] Initiating high-speed catalog scan. Please hold tight..." << endl;
    
    auto start_time = chrono::high_resolution_clock::now();
    
    ifstream catalog("Rejewski-Chains-Catalog.txt");
    if (!catalog.is_open()) {
        cerr << "[!] Oops! I couldn't locate the 'Rejewski-Chains-Catalog.txt' database. Did you run the compiler first?" << endl;
        return 1;
    }

    string line;
    vector<string> discovered_matches;
    
    int total_lines = 1054560; 
    int current_line = 0;
    int update_interval = 25000;

    while (getline(catalog, line)) {
        current_line++;
        
        if (line.find(target_index) != string::npos) {
            discovered_matches.push_back(line);
        }

        if (current_line % update_interval == 0 || current_line == total_lines) {
            float progress = (float)current_line / total_lines;
            if (progress > 1.0f) progress = 1.0f; 
            
            int bar_width = 50;
            int pos = bar_width * progress;
            
            cout << "[";
            for (int i = 0; i < bar_width; ++i) {
                if (i < pos) cout << "#";
                else if (i == pos) cout << ">";
                else cout << " ";
            }
            cout << "] " << setw(3) << int(progress * 100.0) << " %\r";
            cout.flush();
        }
    }
    catalog.close();
    cout << endl << endl;

    if (!discovered_matches.empty()) {
        for (const auto& match : discovered_matches) {
            cout << "[=>] Match gracefully extracted: " << match << endl;
        }
    } else {
        cout << "[-] Hmm, no matches found this time. Let's double-check the intercepts and try again!" << endl;
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;
    
    cout << "\n[*] Database scan completed smoothly in " << fixed << setprecision(4) << elapsed.count() << " seconds." << endl;
    cout << "[*] Ready for your next decryption task! Have a wonderful day!" << endl;

    return 0;
}
