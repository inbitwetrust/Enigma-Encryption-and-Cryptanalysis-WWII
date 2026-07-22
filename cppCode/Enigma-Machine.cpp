#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <cctype>
#include <set>
#include <csignal>
#include <cstdlib>
#include <sstream>

using namespace std;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string REFLECTOR_B = "YRUHQSLDPXNGOKMIEBFZCWVJAT";

struct RotorSpec {
    string wiring;
    string notch;
};

// Kriegsmarine M3/M4/Franken Enigma Walzenspezifikationen[cite: 1]

map<string, RotorSpec> ROTORS = {
    {"I",    {"EKMFLGDQVZNTOWYHXUSPAIBRCJ", "Q"}},
    {"II",   {"AJDKSIRUXBLHWTMCQGZNPYFVOE", "E"}},
    {"III",  {"BDFHJLCPRTXVZNYEIWGAKMUSQO", "V"}},
    {"IV",   {"ESOVPZJAYQUIRHXLNFTGKDCMWB", "J"}},
    {"V",    {"VZBRGITYUPSDNHLXAWMJQOFECK", "Z"}},
    {"VI",   {"JPGVOUMFYQBENHZRDKASXLICTW", "ZM"}},
    {"VII",  {"NZJHGRCXMYSWBOUFAIVLPEKQDT", "ZM"}},
    {"VIII", {"FKQHTLXOCBJSPDZRAMEWNIUYGV", "ZM"}}
};

int mod(int a, int b) {
    return (a % b + b) % b;
}

class Rotor {
public:
    string name;
    string wiring;
    string notch;
    int ring;
    int pos;

    Rotor(string n, string w, string no, char r = 'A', char p = 'A') {
        name = n;
        wiring = w;
        notch = no;
        ring = ALPHABET.find(r);
        pos = ALPHABET.find(p);
    }

    void step() {
        pos = (pos + 1) % 26;  
    }

    bool is_at_notch() {
        return notch.find(ALPHABET[pos]) != string::npos;  
    }

    int forward(int c) {
        int shift = pos - ring;  
        int idx = mod(c + shift, 26);  
        char ch = wiring[idx];  
        return mod((int)ALPHABET.find(ch) - shift, 26);  
    }

    int backward(int c) {
        int shift = pos - ring;  
        int idx = mod(c + shift, 26);  
        char ch = ALPHABET[idx];  
        int out_idx = wiring.find(ch);  
        return mod(out_idx - shift, 26);  
    }
};

class EnigmaMachine {
public:
    vector<Rotor> rotors;
    string reflector;
    map<char, char> plugboard;

    EnigmaMachine(vector<string> rotor_names, string rings, string positions, vector<string> plugboard_pairs) {
        for (size_t i = 0; i < rotor_names.size(); ++i) {
            string name = rotor_names[i];
            rotors.push_back(Rotor(name, ROTORS[name].wiring, ROTORS[name].notch, rings[i], positions[i]));
        }
        reflector = REFLECTOR_B;
        plugboard = setup_plugboard(plugboard_pairs);
    }

    map<char, char> setup_plugboard(vector<string> pairs) {
        map<char, char> pb;
        for (char c : ALPHABET) {
            pb[c] = c;
        }
        for (string pair : pairs) {
            if (pair.length() == 2) {
                char a = pair[0];
                char b = pair[1];
                pb[a] = b;
                pb[b] = a;
            }
        }
        return pb;
    }

    void step_rotors() {
        bool step_r4 = rotors[4].is_at_notch();
        bool step_r3 = rotors[3].is_at_notch();
        bool step_r2 = rotors[2].is_at_notch();
        bool step_r1 = rotors[1].is_at_notch();

        if (step_r1) {
            rotors[0].step();
            rotors[1].step();
        } else if (step_r2) {
            rotors[1].step();
            rotors[2].step();
        } else if (step_r3) {
            rotors[2].step();
            rotors[3].step();
        } else if (step_r4) {
            rotors[3].step();
        }
        rotors[4].step();
    }

    char process_char(char char_in) {
        if (ALPHABET.find(char_in) == string::npos) return char_in;
        
        step_rotors();

        char c = plugboard[char_in];
        int c_idx = ALPHABET.find(c);

        for (int i = rotors.size() - 1; i >= 0; --i) {
            c_idx = rotors[i].forward(c_idx);
        }

        c_idx = ALPHABET.find(reflector[c_idx]);

        for (size_t i = 0; i < rotors.size(); ++i) {
            c_idx = rotors[i].backward(c_idx);
        }

        char final_char = ALPHABET[c_idx];
        return plugboard[final_char];
    }

    string process_text(string text) {
        string result = "";
        for (char c : text) {
            if (c != ' ') {
                result += process_char(toupper(c));
            }
        }
        return result;
    }
};

string to_upper(string s) {
    string res = s;
    transform(res.begin(), res.end(), res.begin(), ::toupper);
    return res;
}

vector<string> get_rotors() {
    vector<string> available_rotors = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII"};
    vector<string> selected;

    cout << "\n[!] WALZENORDNUNG (ROTOR-KONFIGURATION)\n";  
    cout << "    Setzen Sie 5 Walzen ein. (Von links nach rechts).\n";  

    for (int i = 1; i <= 5; ++i) {
        while (true) {
            string r;
            cout << "[*] Walze " << i << " einsetzen (Verfügbar: ";  
            for (size_t j = 0; j < available_rotors.size(); ++j) {
                cout << available_rotors[j] << (j < available_rotors.size() - 1 ? ", " : "");  
            }
            cout << "): ";  
            
            getline(cin, r);
            r = to_upper(r);

            if (find(available_rotors.begin(), available_rotors.end(), r) != available_rotors.end()) {
                selected.push_back(r);  
                available_rotors.erase(remove(available_rotors.begin(), available_rotors.end(), r), available_rotors.end());  
                break;
            } else if (find(selected.begin(), selected.end(), r) != selected.end()) {
                cout << "[-] ACHTUNG! Walze '" << r << "' ist bereits in der Maschine verriegelt!\n";  
            } else {
                cout << "[-] VERBOTEN! Ungültige Walzenkennung. Überprüfen Sie Ihre Ausrüstung!\n";  
            }
        }
    }
    return selected;
}

string get_5_letters(string prompt_text, string default_val = "AAAAA") {
    while (true) {
        string val;
        cout << prompt_text;
        getline(cin, val);
        val = to_upper(val);
        if (val.empty()) val = default_val;  

        bool is_alpha = true;
        for (char c : val) {
            if (!isalpha(c)) is_alpha = false;  
        }

        if (val.length() == 5 && is_alpha) {
            return val;  
        }
        cout << "[-] NEIN! Befehl verweigert! Exakt 5 Buchstaben erforderlich (z.B. ABCDE).\n";  
    }
}

void generate_random_steckerbrett(vector<string>& pairs, vector<char>& unplugged) {
    string letters = ALPHABET;  
    
    random_device rd;
    mt19937 g(rd());
    shuffle(letters.begin(), letters.end(), g);  

    for (int i = 0; i < 24; i += 2) {
        string p = "";
        p += letters[i];  
        p += letters[i+1];  
        pairs.push_back(p);  
    }
    for (int i = 24; i < 26; ++i) {
        unplugged.push_back(letters[i]);  
    }
}

vector<string> steckerbrett_routine() {
    cout << "\n[!] STECKERBRETT VERKABELUNG\n";  
    cout << "    [BLETCHLEY PARK INTERCEPT]: 'Sir! The German operator is trying to leave ONE letter unplugged!'\n";  
    cout << "    [ALAN TURING]: '26 is an even number, you bloody fool! They have to leave TWO alone! Let them play...'\n";  
    cout << "    [END OF INTERCEPT]\n\n";  

    while (true) {
        vector<string> pairs;
        vector<char> unplugged;
        generate_random_steckerbrett(pairs, unplugged);  

        string pairs_str = "";
        for (const string& p : pairs) pairs_str += p + " ";  
        
        string unplugged_str = "";
        unplugged_str += unplugged[0];  
        unplugged_str += ", ";  
        unplugged_str += unplugged[1];  

        cout << "[*] VORGESCHLAGENE VERKABELUNG (Zufall): " << pairs_str << "\n";  
        cout << "[*] UNVERBUNDEN (Freibleibend): " << unplugged_str << "\n";  

        string choice;
        cout << "\n[?] Akzeptieren? (JA / ENTER für neu / MV für 'Meine Verkabelung'): ";  
        getline(cin, choice);
        choice = to_upper(choice);

        if (choice == "JA" || choice == "YES") {
            cout << "[+] STECKERBRETT KONFIGURIERT UND VERRIEGELT!\n";  
            return pairs;  
        } else if (choice == "MV") {
            while (true) {
                cout << "\n[!] MANUELLE EINGABE (Meine Verkabelung)\n";  
                cout << "[*] Geben Sie Ihre Paare ein (z.B. AB CD EF, oder ENTER für leer): ";  
                string manual_input_str;
                getline(cin, manual_input_str);
                manual_input_str = to_upper(manual_input_str);

                if (manual_input_str.empty()) {
                    cout << "[+] KEINE VERKABELUNG. STECKERBRETT LEER UND VERRIEGELT!\n";  
                    return {};  
                }

                vector<string> manual_input;
                stringstream ss(manual_input_str);
                string word;
                while (ss >> word) {
                    manual_input.push_back(word);  
                }

                bool valid = true;
                set<char> seen;
                for (const string& pair : manual_input) {
                    if (pair.length() != 2 || !isalpha(pair[0]) || !isalpha(pair[1])) {
                        cout << "[-] HALT! Ungültige Verbindung '" << pair << "'. Exakt 2 Buchstaben erforderlich!\n";  
                        valid = false;
                        break;
                    }
                    if (pair[0] == pair[1]) {
                        cout << "[-] VERBOTEN! Ein Buchstabe kann nicht mit sich selbst verbunden werden ('" << pair << "').\n";  
                        valid = false;
                        break;
                    }
                    if (seen.count(pair[0]) || seen.count(pair[1])) {
                        cout << "[-] ACHTUNG! Kurzschluss in '" << pair << "'. Jeder Buchstabe darf nur einmal verwendet werden!\n";  
                        valid = false;
                        break;
                    }
                    seen.insert(pair[0]);  
                    seen.insert(pair[1]);  
                }

                if (valid) {
                    cout << "[+] MANUELLES STECKERBRETT KONFIGURIERT UND VERRIEGELT!\n";  
                    return manual_input;  
                }
            }
        }
    }
}

// Global flag to handle KeyboardInterrupt logic[cite: 1]
void signal_handler(int signum) {
    cout << "\n\n[-] NOTABSCHALTUNG! CODEBÜCHER VERBRENNEN...\n";  
    exit(0);  
}

void run_emulator() {
    cout << R"(
     ██████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗  
     ██╔═══╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗ 
     █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║
     ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║
     ██████╗ ██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║
      ╚═════╝ ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝
    >> Enigma Machine Ultimate v8.0 | Bletchley Park Edition
    >> (c) George W. Aravidis | Ulm Söflingen
    >> Email address # csec.unbutton326@passinbox.com
    )" << "\n";  
    
    cout << "[!] GEHEIME KOMMUNIKATIONSVERBINDUNG WIRD HERGESTELLT...\n";  
    cout << "[!] STROMVERSORGUNG DER WALZEN AKTIVIERT...\n\n";  

    vector<string> r_input = get_rotors();  

    cout << "\n[!] RINGSTELLUNG\n";  
    string ring_input = get_5_letters("[*] Ringstellung eingeben (Standard AAAAA): ");  

    cout << "\n[!] GRUNDSTELLUNG\n";  
    string pos_input = get_5_letters("[*] Grundstellung eingeben (Standard AAAAA): ");  

    vector<string> pb_input = steckerbrett_routine();  

    EnigmaMachine enigma(r_input, ring_input, pos_input, pb_input);  

    cout << "\n=======================================================\n";  
    cout << "[+] WALZEN: ";  
    for (size_t i = 0; i < r_input.size(); ++i) {
        cout << r_input[i] << (i < r_input.size() - 1 ? ", " : "");  
    }
    cout << "\n[+] RINGSTELLUNG: " << ring_input << " | GRUNDSTELLUNG: " << pos_input << "\n";  
    
    cout << "[+] STECKERBRETT: ";
    if (pb_input.empty()) {
        cout << "LEER\n";
    } else {
        for (const string& p : pb_input) cout << p << " ";
        cout << "\n";
    }
    cout << "[+] MASCHINE BEREIT. WARTEN AUF KLARTEXT.\n";
    cout << "[+] (Schreiben Sie 'EXIT' um das Programm zu beenden)\n";
    cout << "=======================================================\n";

    while (true) {
        string msg;
        cout << "\n[KLARTEXT EINGABE] > ";
        getline(cin, msg);
        msg = to_upper(msg);

        if (msg == "EXIT") {
            cout << "\n[-] VERBINDUNG GETRENNT. MASCHINE WIRD ZERSTÖRT...\n";
            cout << "    [BLETCHLEY PARK INTERCEPT]: 'Bloody hell! The Jerry operator logged off. Time for tea, lads.'\n";
            break;
        }

        string cipher = enigma.process_text(msg);
        string formatted = "";
        for (size_t i = 0; i < cipher.length(); ++i) {
            formatted += cipher[i];
            if ((i + 1) % 5 == 0 && i != cipher.length() - 1) {
                formatted += " ";
            }
        }
        cout << "[GEHEIMTEXT AUSGABE] > " << formatted << "\n";
    }
}

int main() {

    signal(SIGINT, signal_handler);
    
    run_emulator();
    return 0;
}
