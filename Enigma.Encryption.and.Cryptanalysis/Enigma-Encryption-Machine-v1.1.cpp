
#include <algorithm>
#include <chrono>
#include <csignal>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

using namespace std;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string REFLECTOR_B = "YRUHQSLDPXNGOKMIEBFZCWVJAT";

struct RotorSpec {
    string wiring;
    string notch;
};

map<string, RotorSpec> ROTORS = {
    {"I",   {"EKMFLGDQVZNTOWYHXUSPAIBRCJ", "Q"}},
    {"II",  {"AJDKSIRUXBLHWTMCQGZNPYFVOE", "E"}},
    {"III", {"BDFHJLCPRTXVZNYEIWGAKMUSQO", "V"}},
    {"IV",  {"ESOVPZJAYQUIRHXLNFTGKDCMWB", "J"}},
    {"V",   {"VZBRGITYUPSDNHLXAWMJQOFECK", "Z"}}
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

    Rotor(string n, string w, string no, char r = 'A', char p = 'A')
        : name(std::move(n)), wiring(std::move(w)), notch(std::move(no)),
          ring(static_cast<int>(ALPHABET.find(r))),
          pos(static_cast<int>(ALPHABET.find(p))) {}

    void step() {
        pos = (pos + 1) % 26;
    }

    bool is_at_notch() const {
        return notch.find(ALPHABET[pos]) != string::npos;
    }

    int forward(int c) const {
        const int shift = pos - ring;
        const int idx = mod(c + shift, 26);
        const char ch = wiring[idx];
        return mod(static_cast<int>(ALPHABET.find(ch)) - shift, 26);
    }

    int backward(int c) const {
        const int shift = pos - ring;
        const int idx = mod(c + shift, 26);
        const char ch = ALPHABET[idx];
        const int out_idx = static_cast<int>(wiring.find(ch));
        return mod(out_idx - shift, 26);
    }
};

class EnigmaMachine {
public:
    vector<Rotor> rotors;
    string reflector;
    map<char, char> plugboard;

    EnigmaMachine(const vector<string>& rotor_names,
                  const string& rings,
                  const string& positions,
                  const vector<string>& plugboard_pairs) {
        for (size_t i = 0; i < rotor_names.size(); ++i) {
            const string& name = rotor_names[i];
            rotors.emplace_back(name, ROTORS[name].wiring, ROTORS[name].notch,
                                rings[i], positions[i]);
        }
        reflector = REFLECTOR_B;
        plugboard = setup_plugboard(plugboard_pairs);
    }

    static map<char, char> setup_plugboard(const vector<string>& pairs) {
        map<char, char> pb;
        for (char c : ALPHABET) {
            pb[c] = c;
        }
        for (const string& pair : pairs) {
            if (pair.length() == 2) {
                const char a = pair[0];
                const char b = pair[1];
                pb[a] = b;
                pb[b] = a;
            }
        }
        return pb;
    }

    void step_rotors() {
        const bool step_middle = rotors[1].is_at_notch();
        const bool step_right = rotors[2].is_at_notch();

        if (step_middle) {
            rotors[0].step();
            rotors[1].step();
        } else if (step_right) {
            rotors[1].step();
        }
        rotors[2].step();
    }

    char process_char(char char_in) {
        if (ALPHABET.find(char_in) == string::npos) {
            return char_in;
        }

        step_rotors();

        char c = plugboard[char_in];
        int c_idx = static_cast<int>(ALPHABET.find(c));

        for (int i = static_cast<int>(rotors.size()) - 1; i >= 0; --i) {
            c_idx = rotors[i].forward(c_idx);
        }

        c_idx = static_cast<int>(ALPHABET.find(reflector[c_idx]));

        for (size_t i = 0; i < rotors.size(); ++i) {
            c_idx = rotors[i].backward(c_idx);
        }

        const char final_char = ALPHABET[c_idx];
        return plugboard[final_char];
    }

    string process_text(const string& text) {
        string result;
        result.reserve(text.size());

        for (unsigned char raw : text) {
            const char c = static_cast<char>(toupper(raw));
            if (c >= 'A' && c <= 'Z') {
                result += process_char(c);
            }
        }
        return result;
    }
};

namespace UI {
int WIDTH = 98;

const string RESET      = "\033[0m";
const string BOLD       = "\033[1m";
const string DIM        = "\033[2m";
const string NEON_GREEN = "\033[38;2;105;255;70m";
const string MATRIX     = "\033[38;2;40;185;60m";
const string RADIOACTIVE= "\033[38;2;225;255;35m";
const string AMBER      = "\033[38;2;255;190;40m";
const string RED        = "\033[38;2;255;70;70m";
const string DARK_BG    = "\033[48;2;2;8;3m";

void enable_ansi() {
    int terminal_columns = 100;

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (out != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(out, &mode)) {
            SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }

        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (GetConsoleScreenBufferInfo(out, &info)) {
            terminal_columns = info.srWindow.Right - info.srWindow.Left + 1;
        }
    }
    SetConsoleTitleW(L"ENIGMA // Academic Spaced-Panel Edition");
#else
    winsize window_size{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == 0 && window_size.ws_col > 0) {
        terminal_columns = window_size.ws_col;
    }
#endif

    WIDTH = clamp(terminal_columns - 2, 78, 132);
}

void clear_screen() {
    cout << "\033[2J\033[H";
}

void sleep_ms(int milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

string repeat(char c, int count) {
    return string(max(0, count), c);
}

string crop(const string& text, size_t max_len) {
    if (text.size() <= max_len) {
        return text;
    }
    if (max_len <= 3) {
        return text.substr(0, max_len);
    }
    return text.substr(0, max_len - 3) + "...";
}

void frame_top() {
    cout << MATRIX << "+" << repeat('-', WIDTH) << "+" << RESET << '\n';
}

void frame_bottom() {
    cout << MATRIX << "+" << repeat('-', WIDTH) << "+" << RESET << '\n';
}

void frame_separator() {
    cout << MATRIX << "+" << repeat('-', WIDTH) << "+" << RESET << '\n';
}

void frame_plain(const string& text = "") {
    const string clipped = crop(text, WIDTH - 2);
    cout << MATRIX << "| " << RESET
         << NEON_GREEN << clipped << RESET
         << repeat(' ', WIDTH - 2 - static_cast<int>(clipped.size()))
         << MATRIX << " |" << RESET << '\n';
}

void frame_field(const string& label, const string& value) {
    const string clipped_value = crop(value, WIDTH - static_cast<int>(label.size()) - 4);
    const int used = static_cast<int>(label.size() + 2 + clipped_value.size());

    cout << MATRIX << "| " << RESET
         << RADIOACTIVE << BOLD << label << RESET
         << MATRIX << ": " << RESET
         << NEON_GREEN << clipped_value << RESET
         << repeat(' ', WIDTH - 2 - used)
         << MATRIX << " |" << RESET << '\n';
}


string pad_right(const string& text, int width) {
    if (width <= 0) {
        return "";
    }
    if (static_cast<int>(text.size()) >= width) {
        return text.substr(0, static_cast<size_t>(width));
    }
    return text + repeat(' ', width - static_cast<int>(text.size()));
}

void frame_two_columns(const string& left_label,
                       const string& left_value,
                       const string& right_label,
                       const string& right_value,
                       bool heading = false) {
    const int inner_width = WIDTH - 2;
    const int divider_width = 3;  // visible separator: " | "
    const int available = inner_width - divider_width;

    int left_width = available / 2;
    int right_width = available - left_width;
    if (right_width < 42) {
        right_width = 42;
        left_width = available - right_width;
    }

    constexpr int LEFT_LABEL_WIDTH = 14;
    constexpr int RIGHT_LABEL_WIDTH = 9;

    cout << MATRIX << "| " << RESET;

    if (heading) {
        const string left_heading = crop(left_label, static_cast<size_t>(left_width));
        cout << RADIOACTIVE << BOLD << left_heading << RESET
             << repeat(' ', left_width - static_cast<int>(left_heading.size()));
    } else if (!left_label.empty()) {
        const string label = pad_right(left_label, LEFT_LABEL_WIDTH);
        const int value_width = max(0, left_width - LEFT_LABEL_WIDTH - 2);
        const string value = crop(left_value, static_cast<size_t>(value_width));
        const int visible = LEFT_LABEL_WIDTH + 2 + static_cast<int>(value.size());

        cout << RADIOACTIVE << BOLD << label << RESET
             << MATRIX << ": " << RESET
             << NEON_GREEN << value << RESET
             << repeat(' ', left_width - visible);
    } else {
        cout << repeat(' ', left_width);
    }

    cout << MATRIX << " | " << RESET;

    if (heading) {
        const string right_heading = crop(right_label, static_cast<size_t>(right_width));
        cout << RADIOACTIVE << BOLD << right_heading << RESET
             << repeat(' ', right_width - static_cast<int>(right_heading.size()));
    } else if (!right_label.empty()) {
        const string label = pad_right(right_label, RIGHT_LABEL_WIDTH);
        const int value_width = max(0, right_width - RIGHT_LABEL_WIDTH - 3);
        const string value = crop(right_value, static_cast<size_t>(value_width));
        const int visible = RIGHT_LABEL_WIDTH + 3 + static_cast<int>(value.size());

        cout << RADIOACTIVE << BOLD << label << RESET
             << MATRIX << " - " << RESET
             << NEON_GREEN << value << RESET
             << repeat(' ', right_width - visible);
    } else {
        cout << repeat(' ', right_width);
    }

    cout << MATRIX << " |" << RESET << '\n';
}

void frame_rotor_configuration(const vector<string>& selected_rotors,
                               const string& rings,
                               const string& positions) {
    frame_two_columns("AKTUELLE KONFIGURATION", "",
                      "ROTOR-REFERENZ (VERDRAHTUNG)", "", true);

    frame_two_columns("", "", "", "");

    string selected;
    for (size_t i = 0; i < selected_rotors.size(); ++i) {
        selected += selected_rotors[i];
        if (i + 1 < selected_rotors.size()) {
            selected += " | ";
        }
    }

    frame_two_columns("WALZENORDNUNG", selected,
                      "Rotor I", ROTORS.at("I").wiring);
    frame_two_columns("RINGSTELLUNG", rings,
                      "Rotor II", ROTORS.at("II").wiring);
    frame_two_columns("GRUNDSTELLUNG", positions,
                      "Rotor III", ROTORS.at("III").wiring);
    frame_two_columns("REFLEKTOR", "B",
                      "Rotor IV", ROTORS.at("IV").wiring);
    frame_two_columns("", "",
                      "Rotor V", ROTORS.at("V").wiring);
}

void section(const string& title) {
    cout << '\n' << MATRIX << "[" << RESET
         << RADIOACTIVE << BOLD << title << RESET
         << MATRIX << "]" << RESET << '\n';
}

string join(const vector<string>& values, const string& separator = " ") {
    string result;
    for (size_t i = 0; i < values.size(); ++i) {
        result += values[i];
        if (i + 1 < values.size()) {
            result += separator;
        }
    }
    return result;
}

string format_groups(const string& text, size_t group_size = 5) {
    string result;
    for (size_t i = 0; i < text.size(); ++i) {
        result += text[i];
        if ((i + 1) % group_size == 0 && i + 1 < text.size()) {
            result += ' ';
        }
    }
    return result;
}


enum class ClipboardBackend {
    None,
    WindowsApi,
    Pbcopy,
    WlCopy,
    Xclip,
    Xsel,
    Osc52
};

struct ClipboardResult {
    bool success;
    ClipboardBackend backend;
};

string clipboard_backend_name(ClipboardBackend backend) {
    switch (backend) {
        case ClipboardBackend::WindowsApi: return "WINDOWS API";
        case ClipboardBackend::Pbcopy:     return "PBCOPY";
        case ClipboardBackend::WlCopy:     return "WL-COPY";
        case ClipboardBackend::Xclip:      return "XCLIP";
        case ClipboardBackend::Xsel:       return "XSEL";
        case ClipboardBackend::Osc52:      return "OSC 52 TERMINAL";
        default:                           return "NICHT VERFUEGBAR";
    }
}

#ifndef _WIN32
bool command_exists(const string& command) {
    const string check = "command -v " + command + " >/dev/null 2>&1";
    return system(check.c_str()) == 0;
}

string base64_encode(const string& input) {
    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    string output;
    output.reserve(((input.size() + 2) / 3) * 4);

    unsigned int accumulator = 0;
    int bits = -6;
    for (unsigned char byte : input) {
        accumulator = (accumulator << 8) | byte;
        bits += 8;
        while (bits >= 0) {
            output.push_back(table[(accumulator >> bits) & 0x3F]);
            bits -= 6;
        }
    }

    if (bits > -6) {
        output.push_back(table[((accumulator << 8) >> (bits + 8)) & 0x3F]);
    }
    while (output.size() % 4 != 0) {
        output.push_back('=');
    }
    return output;
}

bool copy_via_osc52(const string& text) {
    if (!isatty(STDOUT_FILENO)) {
        return false;
    }

    const string encoded = base64_encode(text);
    const char* tmux = getenv("TMUX");
    const char* screen = getenv("STY");

    if (tmux != nullptr && *tmux != '\0') {
        cout << "\033Ptmux;\033\033]52;c;" << encoded << "\a\033\\";
    } else if (screen != nullptr && *screen != '\0') {
        cout << "\033P\033]52;c;" << encoded << "\a\033\\";
    } else {
        cout << "\033]52;c;" << encoded << "\a";
    }

    cout.flush();
    return cout.good();
}

ClipboardResult copy_with_pipe(const string& text,
                               const string& command,
                               ClipboardBackend backend) {
    FILE* pipe = popen(command.c_str(), "w");
    if (pipe == nullptr) {
        return {false, ClipboardBackend::None};
    }

    const size_t written = fwrite(text.data(), 1, text.size(), pipe);
    const int status = pclose(pipe);
    if (written == text.size() && status == 0) {
        return {true, backend};
    }
    return {false, ClipboardBackend::None};
}
#endif

ClipboardResult copy_to_clipboard(const string& text) {
#ifdef _WIN32
    const int required = MultiByteToWideChar(
        CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
    if (required <= 0) {
        return {false, ClipboardBackend::None};
    }

    vector<wchar_t> wide_text(static_cast<size_t>(required));
    if (MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1,
                            wide_text.data(), required) <= 0) {
        return {false, ClipboardBackend::None};
    }

    if (!OpenClipboard(nullptr)) {
        return {false, ClipboardBackend::None};
    }

    if (!EmptyClipboard()) {
        CloseClipboard();
        return {false, ClipboardBackend::None};
    }

    const size_t bytes = wide_text.size() * sizeof(wchar_t);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (memory == nullptr) {
        CloseClipboard();
        return {false, ClipboardBackend::None};
    }

    void* destination = GlobalLock(memory);
    if (destination == nullptr) {
        GlobalFree(memory);
        CloseClipboard();
        return {false, ClipboardBackend::None};
    }

    memcpy(destination, wide_text.data(), bytes);
    GlobalUnlock(memory);

    if (SetClipboardData(CF_UNICODETEXT, memory) == nullptr) {
        GlobalFree(memory);
        CloseClipboard();
        return {false, ClipboardBackend::None};
    }

    CloseClipboard();
    return {true, ClipboardBackend::WindowsApi};
#else
#if defined(__APPLE__)
    if (command_exists("pbcopy")) {
        ClipboardResult result =
            copy_with_pipe(text, "pbcopy", ClipboardBackend::Pbcopy);
        if (result.success) {
            return result;
        }
    }
#else
    if (command_exists("wl-copy")) {
        ClipboardResult result =
            copy_with_pipe(text, "wl-copy", ClipboardBackend::WlCopy);
        if (result.success) {
            return result;
        }
    }

    if (command_exists("xclip")) {
        ClipboardResult result = copy_with_pipe(
            text, "xclip -selection clipboard", ClipboardBackend::Xclip);
        if (result.success) {
            return result;
        }
    }

    if (command_exists("xsel")) {
        ClipboardResult result = copy_with_pipe(
            text, "xsel --clipboard --input", ClipboardBackend::Xsel);
        if (result.success) {
            return result;
        }
    }
#endif

    if (copy_via_osc52(text)) {
        return {true, ClipboardBackend::Osc52};
    }

    return {false, ClipboardBackend::None};
#endif
}


size_t count_enigma_letters(const string& text) {
    return static_cast<size_t>(count_if(text.begin(), text.end(), [](unsigned char c) {
        const char upper = static_cast<char>(toupper(c));
        return upper >= 'A' && upper <= 'Z';
    }));
}

string normalize_for_display(const string& text) {
    string output;
    output.reserve(text.size());
    bool previous_was_space = true;

    for (unsigned char raw : text) {
        const char c = static_cast<char>(toupper(raw));
        if (c >= 'A' && c <= 'Z') {
            output += c;
            previous_was_space = false;
        } else if (!previous_was_space && !output.empty()) {
            output += ' ';
            previous_was_space = true;
        }
    }

    if (!output.empty() && output.back() == ' ') {
        output.pop_back();
    }
    return output;
}

vector<string> wrap_tokens(const string& text, size_t line_width) {
    vector<string> lines;
    if (line_width == 0) {
        return lines;
    }

    istringstream parser(text);
    string token;
    string line;

    while (parser >> token) {
        while (token.size() > line_width) {
            if (!line.empty()) {
                lines.push_back(line);
                line.clear();
            }
            lines.push_back(token.substr(0, line_width));
            token.erase(0, line_width);
        }

        if (token.empty()) {
            continue;
        }

        if (line.empty()) {
            line = token;
        } else if (line.size() + 1 + token.size() <= line_width) {
            line += ' ';
            line += token;
        } else {
            lines.push_back(line);
            line = token;
        }
    }

    if (!line.empty()) {
        lines.push_back(line);
    }
    if (lines.empty()) {
        lines.push_back("");
    }
    return lines;
}

void frame_document(const string& label,
                    const string& text,
                    size_t source_characters,
                    size_t enigma_letters) {
    const string status = label + "  //  ZEICHEN: " + to_string(source_characters)
                        + "  //  ENIGMA-BUCHSTABEN: " + to_string(enigma_letters);
    frame_plain(status);
    frame_plain();

    const size_t number_width = 4;
    const size_t text_width = static_cast<size_t>(WIDTH) - number_width - 5;
    const vector<string> lines = wrap_tokens(text.empty() ? "<LEER>" : text, text_width);

    for (size_t i = 0; i < lines.size(); ++i) {
        ostringstream number;
        number << setw(static_cast<int>(number_width)) << setfill('0') << (i + 1);
        const string& line = lines[i];

        cout << MATRIX << "| " << RESET
             << MATRIX << number.str() << " | " << RESET
             << NEON_GREEN << line << RESET
             << repeat(' ', static_cast<int>(text_width - line.size()))
             << MATRIX << " |" << RESET << '\n';
    }
}

void boot_animation() {
    clear_screen();
    cout << DARK_BG;
    cout << MATRIX << "[SYSTEM] " << RESET << NEON_GREEN
         << "Terminalinitialisierung.............OK" << RESET << '\n';
    sleep_ms(100);
    cout << MATRIX << "[SYSTEM] " << RESET << NEON_GREEN
         << "Rotorsteuerung.....................OK" << RESET << '\n';
    sleep_ms(100);
    cout << MATRIX << "[SYSTEM] " << RESET << NEON_GREEN
         << "Umkehrwalze B......................OK" << RESET << '\n';
    sleep_ms(100);
    cout << MATRIX << "[SYSTEM] " << RESET << RADIOACTIVE << BOLD
         << "Enigma-Simulation..................BEREIT" << RESET << '\n';
    sleep_ms(220);
}

void title() {
    cout << DARK_BG;
    cout << RADIOACTIVE << BOLD << R"(
  ███████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗
  ██╔════╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗
  █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║
  ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║
  ███████╗██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║
  ╚══════╝╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝
)" << RESET;
    cout << MATRIX
         << "    >> WWII Enigma Machine Emulator | Bletchley Park Edition\n"
         << "    >> Created by George W. Aravidis | Ulm Söflingen | July 2026\n"
         << "    >> Email address # csec.unbutton326@passinbox.com\n\n"
         << RESET;
}


void dashboard(const vector<string>& rotor_names,
               const string& rings,
               const string& positions,
               const vector<string>& plugboard,
               const string& plaintext,
               const string& ciphertext,
               const string& notice) {
    clear_screen();
    title();

    frame_top();
    frame_field("SITZUNG", "AKTIV  //  KNOTEN ENIGMA-01  //  VERBINDUNG GESICHERT");
    frame_separator();
    frame_rotor_configuration(rotor_names, rings, positions);
    frame_separator();
    frame_field("STECKERBRETT", plugboard.empty() ? "LEER" : join(plugboard));
    frame_separator();

    if (plaintext.empty()) {
        frame_document("KLARTEXT EINGABE", "<WARTEN AUF EINGABE>", 0, 0);
        frame_separator();
        frame_document("GEHEIMTEXT AUSGABE", "<KEINE AUSGABE>", 0, 0);
    } else {
        const string display_plaintext = normalize_for_display(plaintext);
        const size_t letter_count = count_enigma_letters(plaintext);
        frame_document("KLARTEXT EINGABE", display_plaintext, plaintext.size(), letter_count);
        frame_separator();
        frame_document("GEHEIMTEXT AUSGABE", ciphertext, ciphertext.size(),
                       count_enigma_letters(ciphertext));
    }

    frame_separator();
    frame_field("ZWISCHENABLAGE", "/COPY gruppiert  //  /KOMPAKT ohne Leerzeichen");
    if (!notice.empty()) {
        frame_field("MELDUNG", notice);
    }
    frame_field("SYSTEM", "VOLLTEXTMODUS  //  SYSTEM-ZWISCHENABLAGE  //  /EXIT BEENDET");
    frame_bottom();
}
}

string to_upper(string text) {
    transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(toupper(c));
    });
    return text;
}

vector<string> get_rotors() {
    vector<string> available_rotors = {"I", "II", "III", "IV", "V"};
    vector<string> selected;

    UI::section("WALZENORDNUNG // ROTOR-KONFIGURATION");
    cout << UI::MATRIX << "Setzen Sie drei Walzen von links nach rechts ein.\n" << UI::RESET;

    for (int i = 1; i <= 3; ++i) {
        while (true) {
            string rotor;
            cout << UI::RADIOACTIVE << "[*] Walze " << i << " einsetzen " << UI::RESET
                 << UI::MATRIX << "(" << UI::join(available_rotors, ", ") << "): " << UI::RESET;
            getline(cin, rotor);
            rotor = to_upper(rotor);

            if (find(available_rotors.begin(), available_rotors.end(), rotor) != available_rotors.end()) {
                selected.push_back(rotor);
                available_rotors.erase(
                    remove(available_rotors.begin(), available_rotors.end(), rotor),
                    available_rotors.end());
                cout << UI::NEON_GREEN << "[+] WALZE " << rotor << " VERRIEGELT.\n" << UI::RESET;
                break;
            }

            if (find(selected.begin(), selected.end(), rotor) != selected.end()) {
                cout << UI::RED << "[-] Walze '" << rotor << "' ist bereits eingesetzt.\n" << UI::RESET;
            } else {
                cout << UI::RED << "[-] Ungueltige Walzenkennung. Erlaubt: "
                     << UI::join(available_rotors, ", ") << "\n" << UI::RESET;
            }
        }
    }
    return selected;
}

string get_3_letters(const string& prompt_text, const string& default_value = "AAA") {
    while (true) {
        string value;
        cout << UI::RADIOACTIVE << prompt_text << UI::RESET;
        getline(cin, value);
        value = to_upper(value);

        if (value.empty()) {
            value = default_value;
        }

        const bool valid = value.length() == 3 && all_of(value.begin(), value.end(), [](unsigned char c) {
            return c >= 'A' && c <= 'Z';
        });

        if (valid) {
            return value;
        }

        cout << UI::RED
             << "[-] Exakt drei Buchstaben A-Z erforderlich, zum Beispiel ABC.\n"
             << UI::RESET;
    }
}

void generate_random_steckerbrett(vector<string>& pairs, vector<char>& unplugged) {
    string letters = ALPHABET;

    random_device rd;
    mt19937 generator(rd());
    shuffle(letters.begin(), letters.end(), generator);

    for (int i = 0; i < 24; i += 2) {
        string pair;
        pair += letters[i];
        pair += letters[i + 1];
        pairs.push_back(pair);
    }

    for (int i = 24; i < 26; ++i) {
        unplugged.push_back(letters[i]);
    }
}

vector<string> steckerbrett_routine() {
    UI::section("STECKERBRETT // NEON PATCH PANEL");
    cout << UI::MATRIX
         << "ENTER erzeugt eine neue Zufallsverkabelung, JA akzeptiert sie, MV erlaubt manuelle Eingabe.\n"
         << UI::RESET;

    while (true) {
        vector<string> pairs;
        vector<char> unplugged;
        generate_random_steckerbrett(pairs, unplugged);

        cout << '\n' << UI::RADIOACTIVE << "[*] VORGESCHLAGENE VERKABELUNG: " << UI::RESET
             << UI::NEON_GREEN << UI::join(pairs) << UI::RESET << '\n';
        cout << UI::RADIOACTIVE << "[*] UNVERBUNDEN: " << UI::RESET
             << UI::NEON_GREEN << unplugged[0] << ", " << unplugged[1] << UI::RESET << '\n';

        string choice;
        cout << UI::RADIOACTIVE
             << "[?] Akzeptieren? (JA / ENTER fuer neu / MV fuer manuell): "
             << UI::RESET;
        getline(cin, choice);
        choice = to_upper(choice);

        if (choice == "JA" || choice == "YES") {
            cout << UI::NEON_GREEN << "[+] STECKERBRETT KONFIGURIERT UND VERRIEGELT.\n" << UI::RESET;
            return pairs;
        }

        if (choice == "MV") {
            while (true) {
                cout << '\n' << UI::RADIOACTIVE
                     << "[*] Paare eingeben, z.B. AB CD EF; ENTER bedeutet leer: "
                     << UI::RESET;

                string manual_text;
                getline(cin, manual_text);
                manual_text = to_upper(manual_text);

                if (manual_text.empty()) {
                    cout << UI::NEON_GREEN << "[+] STECKERBRETT LEER UND VERRIEGELT.\n" << UI::RESET;
                    return {};
                }

                vector<string> manual_pairs;
                stringstream parser(manual_text);
                string word;
                while (parser >> word) {
                    manual_pairs.push_back(word);
                }

                bool valid = true;
                set<char> used;

                for (const string& pair : manual_pairs) {
                    if (pair.length() != 2 ||
                        pair[0] < 'A' || pair[0] > 'Z' ||
                        pair[1] < 'A' || pair[1] > 'Z') {
                        cout << UI::RED << "[-] Ungueltige Verbindung '" << pair
                             << "'. Exakt zwei Buchstaben A-Z erforderlich.\n" << UI::RESET;
                        valid = false;
                        break;
                    }

                    if (pair[0] == pair[1]) {
                        cout << UI::RED << "[-] Ein Buchstabe kann nicht mit sich selbst verbunden werden: "
                             << pair << "\n" << UI::RESET;
                        valid = false;
                        break;
                    }

                    if (used.count(pair[0]) || used.count(pair[1])) {
                        cout << UI::RED << "[-] Jeder Buchstabe darf nur einmal verwendet werden: "
                             << pair << "\n" << UI::RESET;
                        valid = false;
                        break;
                    }

                    used.insert(pair[0]);
                    used.insert(pair[1]);
                }

                if (valid) {
                    cout << UI::NEON_GREEN
                         << "[+] MANUELLES STECKERBRETT KONFIGURIERT UND VERRIEGELT.\n"
                         << UI::RESET;
                    return manual_pairs;
                }
            }
        }
    }
}

void signal_handler(int) {
    cout << UI::RESET << "\n\n" << UI::RED
         << "[-] NOTABSCHALTUNG // TERMINAL SESSION BEENDET.\n"
         << UI::RESET;
    std::_Exit(0);
}

void run_emulator() {
    UI::enable_ansi();
    UI::boot_animation();
    UI::clear_screen();
    UI::title();

    cout << UI::NEON_GREEN
         << "[+] GEHEIME KOMMUNIKATIONSVERBINDUNG HERGESTELLT.\n"
         << "[+] STROMVERSORGUNG DER WALZEN AKTIV.\n"
         << UI::RESET;

    const vector<string> rotor_input = get_rotors();

    UI::section("RINGSTELLUNG");
    const string ring_input = get_3_letters("[*] Ringstellung eingeben (Standard AAA): ");

    UI::section("GRUNDSTELLUNG");
    const string position_input = get_3_letters("[*] Grundstellung eingeben (Standard AAA): ");

    const vector<string> plugboard_input = steckerbrett_routine();

    string last_plaintext;
    string last_ciphertext;
    string last_compact_ciphertext;
    string notice = "BEREIT // KLARTEXT EINGEBEN ODER EINFUEGEN";

    while (true) {
        UI::dashboard(rotor_input, ring_input, position_input, plugboard_input,
                      last_plaintext, last_ciphertext, notice);

        cout << '\n' << UI::RADIOACTIVE << UI::BOLD
             << "[KLARTEXT ODER BEFEHL]" << UI::RESET << '\n'
             << UI::MATRIX << UI::DIM
             << "Text einfuegen oder /COPY, /KOMPAKT bzw. /EXIT eingeben."
             << UI::RESET << '\n'
             << UI::RADIOACTIVE << UI::BOLD << "> " << UI::RESET;

        string message;
        getline(cin, message);
        const string command = to_upper(message);

        if (command == "EXIT" || command == "/EXIT") {
            cout << '\n' << UI::AMBER
                 << "[-] VERBINDUNG GETRENNT. ENIGMA CORE WIRD HERUNTERGEFAHREN...\n"
                 << UI::RESET;
            UI::sleep_ms(250);
            cout << UI::MATRIX << "[SESSION CLOSED]\n" << UI::RESET;
            break;
        }

        const bool copy_command =
            command == "/COPY" || command == "COPY" ||
            command == "/KOPIEREN" || command == "KOPIEREN";
        const bool compact_command =
            command == "/KOMPAKT" || command == "KOMPAKT" ||
            command == "/COMPACT" || command == "COMPACT" ||
            command == "/COMPAKT" || command == "COMPAKT";

        if (copy_command || compact_command) {
            if (last_ciphertext.empty()) {
                notice = "KEIN GEHEIMTEXT VORHANDEN // ZUERST KLARTEXT EINGEBEN";
                continue;
            }

            const string& clipboard_text = compact_command
                ? last_compact_ciphertext
                : last_ciphertext;

            const UI::ClipboardResult result =
                UI::copy_to_clipboard(clipboard_text);

            if (result.success) {
                const string backend =
                    UI::clipboard_backend_name(result.backend);
                notice = compact_command
                    ? "ZWISCHENABLAGE OK // KOMPAKTER GEHEIMTEXT // " + backend
                    : "ZWISCHENABLAGE OK // GRUPPIERTER GEHEIMTEXT // " + backend;
            } else {
                notice = "ZWISCHENABLAGE NICHT VERFUEGBAR // TERMINAL ODER SYSTEM BLOCKIERT DEN ZUGRIFF";
            }
            continue;
        }

        if (message.empty()) {
            notice = "LEERE EINGABE // LETZTES ERGEBNIS BLEIBT ERHALTEN";
            continue;
        }

        EnigmaMachine enigma(rotor_input, ring_input, position_input, plugboard_input);

        last_plaintext = to_upper(message);
        last_compact_ciphertext = enigma.process_text(last_plaintext);
        last_ciphertext = UI::format_groups(last_compact_ciphertext);

        notice = "VERSCHLUESSELUNG ABGESCHLOSSEN // /COPY ODER /KOMPAKT FUER ZWISCHENABLAGE";
    }
}

int main() {
    signal(SIGINT, signal_handler);
    run_emulator();
    return 0;
}
