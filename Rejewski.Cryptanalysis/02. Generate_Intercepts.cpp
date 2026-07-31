
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <iomanip>
#include <algorithm>
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

void print_banner() {
    cout << R"(+ --------------------------------------------------------- +
|  ██████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗          |
|  ██╔═══╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗         |
|  █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║        |
|  ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║        |
|  ██████╗ ██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║        |
|  ╚═════╝ ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝        |
|                                                           |
|  World War II Enigma — Radio Intercept Simulator          |
|  High-Entropy Ciphertext Payload Generator                |
|  Mersenne Twister RNG • Double-Header Protocol            |
|  Automated Y-Station Traffic Emulation                    |
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

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, 25);

    string day_key;
    cout << "[?] Enter Secret Day Key (Grundstellung, 3 letters, e.g., ZEK): ";
    cin >> day_key;
    
    for (auto & c: day_key) c = toupper(c);
    
    if (day_key.length() != 3) {
        cout << "[!] FATAL ERROR: Day key must be exactly 3 characters." << endl;
        return 1;
    }

    string r1, r2, r3;
    cout << "[?] Enter Rotor Configuration (e.g., I III V): ";
    cin >> r1 >> r2 >> r3;
    
    for (auto & c: r1) c = toupper(c);
    for (auto & c: r2) c = toupper(c);
    for (auto & c: r3) c = toupper(c);
    
    vector<string> rotors = {r1, r2, r3};

    int num_messages;
    cout << "[?] Target number of intercepted messages to generate (Min 300 recommended): ";
    cin >> num_messages;

    cout << "\n[+] INITIALIZING MERSENNE TWISTER RNG..." << endl;
    cout << "[+] GENERATING " << num_messages << " UNIQUE CIPHERTEXTS..." << endl << endl;

    ofstream outfile("InterceptedMessages.txt");
    if (!outfile.is_open()) {
        cerr << "[!] CRITICAL ERROR: Unable to initialize output stream." << endl;
        return 1;
    }
    
    int update_interval = max(1, num_messages / 100);

    for (int i = 0; i < num_messages; ++i) {
        string m_key = "";
        for (int j = 0; j < 3; ++j) {
            m_key += (char)('A' + distrib(gen));
        }
        
        Enigma enigma(day_key, rotors);
        
        string double_key_cipher = enigma.encipher(m_key + m_key);
        
        string payload = "";
        for (int j = 0; j < 15; ++j) {
            payload += (char)('A' + distrib(gen));
        }
        
        outfile << double_key_cipher << " " << payload << "\n";

        if ((i + 1) % update_interval == 0 || (i + 1) == num_messages) {
            float progress = (float)(i + 1) / num_messages;
            int bar_width = 50;
            int pos = bar_width * progress;
            
            cout << "[";
            for (int k = 0; k < bar_width; ++k) {
                if (k < pos) cout << "#";
                else if (k == pos) cout << ">";
                else cout << " ";
            }
            cout << "] " << setw(3) << int(progress * 100.0) << " %\r";
            cout.flush();
        }
    }
    
    cout << endl << endl;
    outfile.close();
    
    cout << string(61, '-') << endl;
    cout << " RADIO INTERCEPTION PROTOCOL COMPLETE" << endl;
    cout << string(61, '-') << endl;
    cout << "[+] 'InterceptedMessages.txt' successfully populated." << endl;
    cout << "[!] High-entropy data ready for cryptanalysis." << endl;
    
    return 0;
}
