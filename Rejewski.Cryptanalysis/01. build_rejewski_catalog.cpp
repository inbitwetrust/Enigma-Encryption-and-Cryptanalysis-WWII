
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <unordered_map>

using namespace std;

const std::string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

struct RotorConfig {
    std::string forward;
    std::string backward;
    char notch;
};

std::unordered_map<std::string, RotorConfig> ROTORS = {
    {"I",   {"EKMFLGDQVZNTOWYHXUSPAIBRCJ", "UWYGADFPVZBECKMTHXSLRINQOJ", 'Q'}},
    {"II",  {"AJDKSIRUXBLHWTMCQGZNPYFVOE", "AJPCZWRLFBDKOTYUQGENHXMIVS", 'E'}},
    {"III", {"BDFHJLCPRTXVZNYEIWGAKMUSQO", "TAGBPCSDQEUFVNZHYIXJWLRKOM", 'V'}},
    {"IV",  {"ESOVPZJAYQUIRHXLNFTGKDCMWB", "HZWVARTNLGUPXQCEJMBSKDYOIF", 'J'}},
    {"V",   {"VZBRGITYUPSDNHLXAWMJQOFECK", "QCYLXWENFTZOSMVJUDKGIARPHB", 'Z'}}
};

class Rotor {
public:
    std::string name;
    RotorConfig config;
    int offset;
    char window;
    Rotor* next_rotor;
    Rotor* prev_rotor;

    Rotor(std::string rotor_num, char window_letter, Rotor* next = nullptr) {
        name = rotor_num;
        config = ROTORS[rotor_num];
        window = window_letter;
        offset = ALPHABET.find(window);
        next_rotor = next;
        prev_rotor = nullptr;
    }

    void step() {
        if (next_rotor != nullptr && window == config.notch) {
            next_rotor->step();
        }
        offset = (offset + 1) % 26;
        window = ALPHABET[offset];
    }

    int encode_letter(int index, bool forward = true) {
        std::string wiring = forward ? config.forward : config.backward;
        char output_letter = wiring[(index + offset) % 26];
        int output_index = (ALPHABET.find(output_letter) - offset + 26) % 26;

        if (next_rotor && forward) {
            return next_rotor->encode_letter(output_index, forward);
        } else if (prev_rotor && !forward) {
            return prev_rotor->encode_letter(output_index, forward);
        }
        return output_index;
    }
};

class Enigma {
public:
    Rotor* l_rotor;
    Rotor* m_rotor;
    Rotor* r_rotor;
    std::string reflector = "YRUHQSLDPXNGOKMIEBFZCWVJAT"; 

    Enigma(std::string key, std::vector<std::string> order = {"I", "II", "III"}) {
        r_rotor = new Rotor(order[2], key[2]);
        m_rotor = new Rotor(order[1], key[1], r_rotor);
        l_rotor = new Rotor(order[0], key[0], m_rotor);
        
        m_rotor->prev_rotor = l_rotor;
        r_rotor->prev_rotor = m_rotor;
    }

    ~Enigma() {
        delete l_rotor;
        delete m_rotor;
        delete r_rotor;
    }

    void step_rotors() {
        l_rotor->step(); 
    }

    char encode_decode_letter(char letter) {
        step_rotors();
        
        int input_idx = ALPHABET.find(letter);
        
        int left_pass = l_rotor->encode_letter(input_idx, true);
        
        char refl_char = reflector[left_pass];
        int refl_idx = ALPHABET.find(refl_char);
        
        int final_idx = r_rotor->encode_letter(refl_idx, false);
        
        return ALPHABET[final_idx];
    }

    std::string encipher(std::string message) {
        std::string cipher = "";
        for (char c : message) {
            if (c >= 'A' && c <= 'Z') {
                cipher += encode_decode_letter(c);
            }
        }
        return cipher;
    }
};

vector<string> generate_all_keys() {
    vector<string> keys;
    keys.reserve(17576); 
    for (char a = 'A'; a <= 'Z'; ++a) {
        for (char b = 'A'; b <= 'Z'; ++b) {
            for (char c = 'A'; c <= 'Z'; ++c) {
                keys.push_back(string(1, a) + b + c);
            }
        }
    }
    return keys;
}

vector<vector<string>> generate_rotor_permutations() {
    vector<string> all_rotors = {"I", "II", "III", "IV", "V"};
    vector<vector<string>> perms;
    
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            for (int k = 0; k < 5; ++k) {
                if (i != j && i != k && j != k) {
                    perms.push_back({all_rotors[i], all_rotors[j], all_rotors[k]});
                }
            }
        }
    }
    return perms;
}

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
    for (int len : chain_lengths) {
        res += to_string(len);
    }
    return res;
}

void print_banner() {
    cout << R"(+ --------------------------------------------------------- +
|  ██████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗          |
|  ██╔═══╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗         |
|  █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║        |
|  ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║        |
|  ██████╗ ██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║        |
|  ╚═════╝ ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝        |
|                                                           |
|  World War II Enigma — Rejewski Chains Catalog Compiler   |
|  Native C++ High-Performance Permutation Engine           |
|  Exhaustive Rotor State Space • 60-Rotor Permutations     |
|  1,054,560 Cryptographic Cycle Footprints                 |
|                                                           |
|  Academic Cryptanalysis Demonstrator v16                  |
|                                                           |
|  Created by George W. Aravidis                            |
|  Cybersicherheitsforschung                                |
|                                                           |
|  Ulm Söflingen | Deutschland                              |
|  Email address # csec.unbutton326@passinbox.com           |
+ --------------------------------------------------------- +)" << "\n\n";
}

int main() {
    print_banner();
    
    vector<string> keys = generate_all_keys();
    vector<vector<string>> rotor_perms = generate_rotor_permutations();

    int total_iterations = keys.size() * rotor_perms.size();
    int current_iteration = 0;

    cout << "[+] Identified " << keys.size() << " base keys and " << rotor_perms.size() << " rotor permutations." << endl;
    cout << "[!] Commencing CPU stress test for " << total_iterations << " unique states..." << endl << endl;

    auto start_time = chrono::high_resolution_clock::now();

    ofstream outfile("Rejewski-Chains-Catalog.txt");
    if (!outfile.is_open()) {
        cerr << "[!] CRITICAL ERROR: Unable to initialize output stream for 'chains_catalog.txt'." << endl;
        return 1;
    }

    for (const auto& order : rotor_perms) {
        for (const auto& day_key : keys) {
            
            char AD[26] = {0}, BE[26] = {0}, CF[26] = {0};
            
            for(int i = 0; i < 26; ++i) {
                char current_char = 'A' + i;
                
                string m_key_AD = string(1, current_char) + "AA";
                Enigma enigma_AD(day_key, order);
                string cipher_AD = enigma_AD.encipher(m_key_AD + m_key_AD);
                AD[cipher_AD[0] - 'A'] = cipher_AD[3];
                
                string m_key_BE = "A" + string(1, current_char) + "A";
                Enigma enigma_BE(day_key, order);
                string cipher_BE = enigma_BE.encipher(m_key_BE + m_key_BE);
                BE[cipher_BE[1] - 'A'] = cipher_BE[4];
                
                string m_key_CF = "AA" + string(1, current_char);
                Enigma enigma_CF(day_key, order);
                string cipher_CF = enigma_CF.encipher(m_key_CF + m_key_CF);
                CF[cipher_CF[2] - 'A'] = cipher_CF[5];
            }
            
            string chain_index = "AD:" + get_chain_lengths(AD) + 
                                 " BE:" + get_chain_lengths(BE) + 
                                 " CF:" + get_chain_lengths(CF);
            
            outfile << chain_index << " | KEY:" << day_key << " ROTORS:" 
                    << order[0] << "," << order[1] << "," << order[2] << "\n";
                    
            current_iteration++;
            
            if (current_iteration % 5000 == 0 || current_iteration == total_iterations) {
                float progress = (float)current_iteration / total_iterations;
                int bar_width = 50;
                int pos = bar_width * progress;
                
                cout << "[";
                for (int i = 0; i < bar_width; ++i) {
                    if (i < pos) cout << "=";
                    else if (i == pos) cout << ">";
                    else cout << " ";
                }
                cout << "] " << setw(3) << int(progress * 100.0) << " %\r";
                cout.flush(); 
            }
        }
    }
    
    cout << endl; 
    outfile.close();

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;

    cout << "\n" << string(61, '-') << endl;
    cout << " CATALOG COMPILATION COMPLETE" << endl;
    cout << string(61, '-') << endl;
    cout << "[!] Elapsed execution time: " << fixed << setprecision(4) << elapsed.count() << " seconds." << endl;
    cout << "[+] Database 'Rejewski-Chains-Catalog.txt' successfully deployed." << endl;
    
    return 0;
}
