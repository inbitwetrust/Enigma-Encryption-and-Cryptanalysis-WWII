
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <ctime>

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
 |  Intercepted Messages Generator v2.0                      |
 |  Zygalski Protocol Edition (Indicator + Cipher)           |
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

    vector<int> daily_rotors = {1, 0, 3}; 
    string daily_ring = "FSU";
    
    int num_messages;
    cout << "[?] How many messages should we intercept? (e.g., 200 recommended): ";
    if (!(cin >> num_messages)) return 1;

    mt19937 rng(time(0));
    uniform_int_distribution<int> dist(0, 25);
    
    ofstream outfile("intercepts.txt");
    int females_count = 0;

    cout << "[*] Setting up Enigma with Secret Daily Key (Rotors: II-I-IV, Ring: FSU)...\n";
    cout << "[*] Simulating careless German radio traffic...\n\n";

    for (int i = 0; i < num_messages; ++i) {
        string indicator = "";
        for(int j=0; j<3; ++j) indicator += (char)('A' + dist(rng));
        
        string msg_key = "";
        for(int j=0; j<3; ++j) msg_key += (char)('A' + dist(rng));
        
        string plaintext_header = msg_key + msg_key;
        string encrypted_header = "";
        
        Enigma e = create_enigma(daily_rotors, daily_ring, indicator);
        for(char c : plaintext_header) {
            e.step();
            encrypted_header += (char)('A' + e.enc(c - 'A'));
        }
        
        bool is_female = false;
        if (encrypted_header[0] == encrypted_header[3]) is_female = true;
        if (encrypted_header[1] == encrypted_header[4]) is_female = true;
        if (encrypted_header[2] == encrypted_header[5]) is_female = true;
        
        if (is_female) females_count++;
        
        string dummy_payload = "";
        for(int j=0; j<15; ++j) dummy_payload += (char)('A' + dist(rng));
        
        outfile << indicator << " " << encrypted_header << " " << dummy_payload << "\n";
    }
    
    outfile.close();
    cout << "[+] Generation complete! Saved to 'intercepts.txt'.\n";
    cout << "[+] Generated " << num_messages << " total intercepts.\n";
    cout << "[+] Found " << females_count << " 'females' (Double encipherment collisions).\n";
    cout << "[*] The file is ready to be fed into the patched Zygalski Simulator (v1.2).\n";
    
    return 0;
}
