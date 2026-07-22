#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <random>
#include <thread>
#include <future>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;

// + --------------------------------------------------------- +
// | KRIEGSWEHRMACHT 5-ROTOR SPECIFICATIONS                    |
// + --------------------------------------------------------- +

struct RotorDef { string wiring; string notch; };
map<string, RotorDef> ROTORS = {
    {"I",    {"EKMFLGDQVZNTOWYHXUSPAIBRCJ", "Q"}},
    {"II",   {"AJDKSIRUXBLHWTMCQGZNPYFVOE", "E"}},
    {"III",  {"BDFHJLCPRTXVZNYEIWGAKMUSQO", "V"}},
    {"IV",   {"ESOVPZJAYQUIRHXLNFTGKDCMWB", "J"}},
    {"V",    {"VZBRGITYUPSDNHLXAWMJQOFECK", "Z"}},
    {"VI",   {"JPGVOUMFYQBENHZRDKASXLICTW", "ZM"}},
    {"VII",  {"NZJHGRCXMYSWBOUFAIVLPEKQDT", "ZM"}},
    {"VIII", {"FKQHTLXOCBJSPDZRAMEWNIUYGV", "ZM"}}
};
const string REFLECTOR_B = "YRUHQSLDPXNGOKMIEBFZCWVJAT";
const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

unordered_map<string, int> GERMAN_SCORES = {
    {"ER", 10}, {"EN", 9}, {"CH", 8}, {"DE", 7}, {"ND", 6}, {"TE", 5},
    {"IN", 5}, {"IE", 4}, {"GE", 4}, {"EI", 4}, {"UN", 3}, {"ST", 3},
    {"DER", 15}, {"DIE", 15}, {"UND", 15}, {"EIN", 12}, {"SCH", 15}, {"CHT", 10},
    {"TEN", 12}, {"GEN", 12}, {"VER", 12}, {"TER", 10}, {"ICH", 15}
};

vector<string> WEHRMACHT_DICT = {

    "DIE", "DER", "UND", "IN", "ZU", "DEN", "DAS", "NICHT", "VON", "SIE", "IST", "DES", 
    "SICH", "MIT", "DEM", "AUF", "FUER", "DASS", "WIE", "EIN", "SIND", "EINE", "ALS", 
    "AN", "NACH", "AUS", "ES", "UM", "NUR", "WAR", "AM", "MEHR", "ODER", "HABEN", "WIR", 
    "MAN", "DA", "SEIN", "NOCH", "BEI", "WAS", "WIRD", "VOR", "IM", "WENN", "MIR", "ZUM", 
    "ZUR", "BIS", "HABE", "IHRE", "DANN", "IHNEN", "SEINE", "ALLE", "ALLER", "WIEDER", "MEINE", 
    "SCHON", "MICH", "ALSO", "GANZ", "WURDE", "GEGEN", "DAMIT", "ZWAR", "IHR", "ETWAS", 
    "KEINE", "DIR", "VIEL", "MUSS", "OHNE", "DIESE", "MACHEN", "WERDEN", "WEIL", "DOCH", 
    "JETZT", "UNTER", "SEHR", "HIER", "KANN", "WO", "ALLES", "NUN", "JEDER", "SOLL", "SOLLEN", 
    "WEITER", "ABER", "VOM", "BEIM", "DURCH", "UEBER", "UNS", "DIESER", "DIESES", "WELCHE", 
    "WELCHER", "DABEI", "DARUM", "DAZU", "DAGEGEN", "DAHER", "DESHALB", "OBWOHL", "TROTZDEM",

    "NULL", "EINS", "ZWEI", "ZWO", "DREI", "VIER", "FUENF", "SECHS", "SIEBEN", "ACHT", "NEUN",
    "ZEHN", "ELF", "ZWOELF", "DREIZEHN", "VIERZEHN", "FUENFZEHN", "SECHSZEHN", "SIEBZEHN", "ACHTZEHN", "NEUNZEHN",
    "ZWANZIG", "DREISSIG", "VIERZIG", "FUENFZIG", "SECHZIG", "SIEBZIG", "ACHTZIG", "NEUNZIG", "HUNDERT", "TAUSEND",
    "MILLION", "ERSTER", "ZWEITER", "DRITTER", "VIERTER", "FUENFTER", "SECHSTER", "SIEBTER", "ACHTER",
    "MONTAG", "DIENSTAG", "MITTWOCH", "DONNERSTAG", "FREITAG", "SAMSTAG", "SONNTAG", 
    "HEUTE", "MORGEN", "GESTERN", "UHR", "MINUTEN", "STUNDEN", "TAGE", "WOCHEN", "MONATE", "JAHR", 
    "JANUAR", "FEBRUAR", "MAERZ", "APRIL", "MAI", "JUNI", "JULI", "AUGUST", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DEZEMBER",
    "VORMITTAG", "NACHMITTAG", "MITTERNACHT", "DAEMMERUNG", "FRUEHLING", "SOMMER", "HERBST", "WINTER",

    "ENIGMA", "CHIFFRIERMASCHINE", "WALZE", "WALZEN", "UMKEHRWALZE", "EINTRITTSWALZE", 
    "STECKER", "STECKERBRETT", "STECKERVERBINDUNG", "LAMPENFELD", "TASTATUR", 
    "GLUEHLAMPE", "BATTERIE", "ROTOR", "ROTOREN", "KONTAKT", "KONTAKTE", "KABEL", 
    "WALZENLAGE", "RINGSTELLUNG", "GRUNDSTELLUNG", "FENSTER", "SCHALTKARTE",
    "SCHLUESSEL", "TAGESSCHLUESSEL", "MONATSSCHLUESSEL", "SPRUCHSCHLUESSEL", 
    "KENNGRUPPE", "KENNGRUPPENBUCH", "KENNWORT", "KENNNUMMER", "DOPPELBUCHSTABE", 
    "DOPPELBUCHSTABENTAUSCH", "SCHLUESSELTAFEL", "KURZSIGNAL", "KURZSIGNALHEFT", 
    "WETTERKURZSCHLUESSEL", "VERSCHLUESSELT", "ENTSCHLUESSELT", "DECHIFFRIEREN", 
    "CHIFFRIEREN", "CHIFFRAT", "DECHIFFRAT", "KRYPTOGRAMM", "KLARTEXT", "GEHEIMTEXT", 
    "BLINDTEXT", "NULLEN", "BLINDGAENGER", "SIGNATUR", "AUTHENTIFIZIERUNG", "ROUTING",
    "CHIFFRE", "GEHEIM", "STRENG", "STRENGGEHEIM", "GEHEIMSACHE", "CHEFSACHE", "KOMMANDOSACHE", 
    "VERTRAULICH", "NURFUEROFFIZIERE", "NURFUERKOMMANDANTEN", "OFFEN", "UNVERSCHLUESSELT",
    "FUNK", "FUNKER", "FUNKSPRUCH", "FUNKSTILLE", "FREQUENZ", "FREQUENZBAND", 
    "WELLENLAENGE", "KURZWELLE", "LANGWELLE", "MITTELWELLE", "ULTRAKURZWELLE", 
    "SENDER", "EMPFAENGER", "FUNKGERAET", "ANTENNE", "SIGNAL", "SIGNALSTAERKE", 
    "RAUSCHEN", "INTERFERENZ", "STOERUNG", "RELAIS", "STROMVERSORGUNG",
    "MELDUNG", "FUNKSTATION", "FUNKZENTRALE", "FUNKNETZ", "FUNKSTRECKE", "FUNKOFFIZIER", 
    "HORCHDIENST", "HORCHPOSTEN", "PEILUNG", "KREUZPEILUNG", "PEILSTELLE", 
    "FUNKABWEHR", "STOERSENDER", "ABGEHOERT", "UEBERMITTELT", "EMPFANG", 
    "BESTAETIGUNG", "BESTAETIGT", "WIEDERHOLEN", "KONTROLLE", "ENTZAEHLT", 
    "FUNKDISZIPLIN", "FUNKFEUER", "TELEGRAMM", "FUNKTELEGRAMM",
    "MORSE", "MORSETASTE", "MORSECODE", "MORSEALPHABET", "TELEGRAFIE", 
    "FUNKTELEGRAFIE", "PUNKT", "STRICH", "QAM", "QRM", "QSA", "QTC", "QTR",

    "HEER", "WEHRMACHT", "OBERKOMMANDO", "GENERALSTAB", "STAB", "HEERESGRUPPE", 
    "ARMEE", "ARMEEKORPS", "KORPS", "DIVISION", "BRIGADE", "REGIMENT", 
    "BATAILLON", "ABTEILUNG", "KOMPANIE", "STAFFEL", "ZUG", "GRUPPE", 
    "BEFEHLSHABER", "KOMMANDEUR", "HAUPTQUARTIER", "FRONTABSCHNITT",
    "INFANTERIE", "ARTILLERIE", "KAVALLERIE", "PIONIER", "PIONIERE", "STURMPIONIER", 
    "GRENADIER", "PANZERGRENADIER", "GEBIRGSJAEGER", "FUESILIER", "SANITAETER", 
    "NACHRICHTENTRUPPE", "FELDGENDARMERIE", "SCHUETZE", "RICHTSCHUETZE", "KANONIER",
    "SOLDAT", "SOLDATEN", "GEFREITER", "OBERGEFREITER", "STABSGEFREITER", 
    "UNTEROFFIZIER", "UNTERFELDWEBEL", "FELDWEBEL", "OBERFELDWEBEL", "STABSFELDWEBEL", 
    "OFFIZIER", "OFFIZIERE", "LEUTNANT", "OBERLEUTNANT", "HAUPTMANN", 
    "MAJOR", "OBERSTLEUTNANT", "OBERST", "GENERALMAJOR", "GENERALLEUTNANT", 
    "GENERALDERINFANTERIE", "GENERALOBERST", "GENERAL", "FELDMARSCHALL",
    "STUETZPUNKT", "STELLUNG", "LINIE", "BRUECKENKOPF", "BUNKER", "FESTUNG", 
    "GRABEN", "SCHUETZENGRABEN", "MINENFELD", "STACHELDRAHT", "PANZERSPERRE",
    "GELANDE", "WALD", "WALDRAND", "BERG", "HUEGEL", "TAL", "FLUSS", "BRUECKE", 
    "STRASSENKREUZUNG", "BAHNDAMM", "SUMPF", "SCHLUCHT",

    "FRONT", "VORAUS", "ZURUECK", "VORMARSCH", "RUECKZUG", "KAMPF", "GEFECHT", "ANGRIFF",
    "VERTEIDIGUNG", "SIEG", "VERLUSTE", "GEFANGENE", "GEFALLEN", "VERWUNDET", "LAZARETT",
    "BLITZKRIEG", "KESSEL", "FLANKE", "DURCHBRUCH", "SPERRFEUER", "BOMBARDIERUNG", "FEUERPAUSE", 
    "WAFFENSTILLSTAND", "KAPITULATION", "GRENZE", "FRONTKEMPF", "UEBERRASCHUNG", "RUECKZUGSLINIE",
    "NACHSCHUB", "PROVIANT", "VERPFLEGUNG", "EISENBAHN", "KRAFTWAGEN", "WERKSTATT", "ERSATZTEILE", 
    "FUNKGERAET", "BATTERIE", "STROM", "BAHNHOF", "STRASSE", "TREIBSTOFFLAGER", "EISENBAHNLINIE",
    "VERNICHTET", "UMZINGELT", "ZERSCHLAGEN", "EINGESCHLOSSEN", "AUSGEBROCHEN", "VERSTAERKUNG",

    "PANZER", "PANZERDIVISION", "TIGER", "PANTHER", "KAMPFWAGEN", "STURMGESCHUETZ", "KETTENKRAD",
    "HALBKETTENFAHRZEUG", "SPAEHWAGEN", "MOTORRAD", "KRAFTFAHRZEUG", "TREIBSTOFF", "BENZIN", "DIESEL",
    "WAFFE", "WAFFEN", "MUNITION", "PATRONE", "PATRONEN", "KUGEL", "KUGELN", "HUESLEN", "HUELSEN",
    "GEWEHR", "KARABINER", "MASCHINENGEWEHR", "PISTOLE", "LUGER", "SCHMEISSER", "MASCHINENPISTOLE",
    "GRANATE", "GRANATEN", "HANDGRANATE", "PANZERFAUST", "PANZERSCHRECK", "GESCHUETZ",
    "KALIBER", "SPRENGSTOFF", "DYNAMIT", "ZUENDER", "FEUER", "SCHIESSEN", "TREFFER", "LADEHEMMUNG",
    "ARTILLERIEFEUER", "FLAKGESCHUETZ", "PAK", "PANZERABWEHRKANONE",

    "LUFTWAFFE", "FLUGZEUG", "FLUGZEUGE", "JAEGER", "BOMBER", "STUKA", "FLIEGER", "PILOT",
    "FALLSCHIRM", "FALLSCHIRMJAEGER", "LUFTLANDE", "ABSPRUNG", "GESCHWADER", "STAFFEL",
    "MESSERSCHMITT", "JUNKERS", "HEINKEL", "FOCKEWULF", "DORNIER", "LUFTANGRIFF", "BOMBEN", "ABWURF",
    "FLUGPLATZ", "FLUGHAFEN", "AUFKLAERUNG", "LUFTABWEHR", "FLAK", "ROHR", "VISIER", "TIEFFLUG",
    "LUFTWAFFENFELDFAHNEN", "JAGDGESCHWADER", "BOMBERGESCHWADER", "LUFTWAFFENSTUETZPUNKT",

    "KRIEGSMARINE", "MARINE", "UBOOT", "UBOTE", "UBOOTE", "FLOTTE", "SCHIFF", "SCHIFFE",
    "KREUZER", "ZERSTOERER", "SCHLACHTSCHIFF", "KONVOI", "GELEITZUG", "GELEITZUEGE", "GELEIT",
    "HILFSKREUZER", "ATLANTIK", "NORDSEE", "OSTSEE", "MITTELMEER", "MUNDUNG", "HAFEN", "KANAL",
    "KANALS", "KUESTE", "KURS", "WELLEN", "TAUCHEN", "PERISKOP", "SEEMEILEN", "TIEFE", "BUG",
    "HECK", "KNOTEN", "ADMIRAL", "GROSSADMIRAL", "KAPIETAEN", "KOMMANDANT", "KOMMANDANTEN", "MATROSE", 
    "KAMPFSCHWIMMER", "FROSCHMANN", "MEERESGRUND", "ATEMGERAET", "KLEINSTUBOOT", "TORPEDO",
    "TORPEDOS", "MINEN", "MINENSUCHER", "WASSERBOMBE", "GEWAESSER", "FUNKERMATROSE",
    "MARINEQUADRAT", "RUDER", "SCHLINGERN", "STEUERBORD", "BACKBORD", "ACHTERN", "KIELWASSER",
    "SCHIFFSARTILLERIE", "KAPITAENLEUTNANT", "KORVETTENKAPITAEN", "FREGATTENKAPITAEN",

    "WETTER", "WETTERLAGE", "WETTERVORHERSAGE", "VORHERSAGE", "BESSERUNG", "VERSCHLECHTERUNG", "BESTAENDIG", "UNBESTAENDIG",
    "REGEN", "NIESELREGEN", "SCHAUER", "SCHNEE", "SCHNEEFALL", "HAGEL", "GRAUPEL", "GEWITTER", "NIEDERSCHLAG",
    "WOLKEN", "WOLKENDECKE", "BEWOELKT", "BEDECKT", "HEITER", "WOLKENLOS", "NEBEL", "NEBELS", "DUNST", "SICHT", "SICHTWEITE", "KLARSICHT",
    "TEMPERATUR", "KALT", "WARM", "HITZE", "FROST", "EIS", "TAUWETTER", "GEFRIERPUNKT", "LUFTFEUCHTIGKEIT",
    "WIND", "WINDSTAERKE", "BOEEN", "STURM", "STURMS", "ORKAN", "STURMTIEF", "ORKANTIEF", "LUFTDRUCK", "HOCHDRUCK", "TIEFDRUCK", 
    "BAROMETER", "MILLIBAR", "ISOBAREN", "TIEF", "HOCH", "SEEGANG", "WELLENHOEHE", "BRANDUNG", "FLUT", "EBBE",
    "NORDEN", "SUEDEN", "OSTEN", "WESTEN", "NORDWEST", "NORDOST", "SUEDWEST", "SUEDOST",
    "MOND", "SONNE", "STERN", "NACHT", "TAG", "DAEMMERUNG",
    
    "BERLIN", "PARIS", "LONDON", "MOSKAU", "STALINGRAD", "ROM", "AFRIKA", "EUROPA", "INSEL", 
    "OZEAN", "STADT", "DORF", "MUENCHEN", "HAMBURG", "KIEL", "WILHELMSHAVEN", "BREST", "LORIENT",
    "SAINTNAZAIRE", "LAPALICE", "BORDEAUX", "GIBRALTAR", "MALTA", "KRETA", "TOBRUK", "ELALAMEIN",
    "KIEW", "MINSK", "LENINGRAD", "KRONSTADT", "SEWASTOPOL", "WARSCHAU", "OSLO", "NARVIK",
    "KANALINSELN", "RHEIN", "DONAU", "WOLGA", "DNJEPR", "ALPEN", "KAUKASUS",

    "BARBAROSSA", "SEELOEWE", "WESERUEBUNG", "GELB", "ZITADELLE", "ANTON", "ATTILA", 
    "BODENPLATTE", "MARITA", "MERKUR", "TAIFUN", "WINTERGEWITTER",

    "BEFEHL", "BEFEHLE", "BEFOHLEN", "BEFIEHLT", "SOFORT", "SOFORTIGEN", "BESTAETIGEN", "MELDEN",
    "ERREICHEN", "POSITION", "POSITIONEN", "QUADRAT", "ABSCHNITT", "ENDE", "EINSATZ", "VERSTANDEN",
    "OPERATION", "GEFAHR", "SICHERHEIT", "SPERRE", "ALARM", "ZIEL", "EINHEIT", "EINHEITEN",
    "KOORDINATEN", "GRAD", "BERICHT", "PLAN", "AUFGRUND", "EXTREMEN", "DICHTEN", "UEBERWACHEN",
    "GEWAEHRLEISTEN", "DEUTSCHER", "DEUTSCHE", "ENGLISCHE", "BRITISCHE", "AMERIKANISCHE",
    "RUSSISCHE", "SOWJETISCHE", "GELUNGEN", "GESCHEITERT", "ZERSTOERT", "VERSENKT", 
    "BESCHAEDIGT", "GEMELDET", "GEFUNDEN", "SCHNELL", "LANGSAM", "BEREIT", "WARTEN", "VORRUECKEN", 
    "AUSFUEHREN", "ABBRECHEN", "DRINGEND", "WICHTIG", "KRITISCH", "ERFOLGREICH", "UNMOEGLICH",
    "EINGETROFFEN", "ABGEMELDET", "BESTAETIGT", "AUSGEFUEHRT", "ANGEGRIFFEN", "VERTEIDIGT",

    "FEIND", "FEINDE", "FEINDLICH", "FEINDLICHE", "FEINDLICHEN", "FEINDES",
    "GEGNER", "GEGNERISCH", "GEGNERISCHE", "GEGNERISCHEN", "ALLIIERTE", "ALLIIERTEN",
    "FEINDBERUEHRUNG", "FEINDDRUCK", "FEINDLAGE", "FEINDEINWIRKUNG", 
    "FEINDNACHRICHTEN", "WIDERSTAND", "PARTISAN", "PARTISANEN", "BANDEN", "TERRORISTEN", "SABOTEURE",
    "TRUPPE", "TRUPPEN", "STREITKRAEFTE", "VERBAND", "VERBAENDE", "KONTINGENT", "KAMPFGRUPPE", "KAMPFGRUPPEN", "BESATZUNG", "BESATZUNGEN",
    "VORHUT", "NACHHUT", "STOSSTRUPP", "SPAEHTRUPP", "KAMPFKRAFT", "SPITZE"
};

unordered_set<string> VOCAB_SET(WEHRMACHT_DICT.begin(), WEHRMACHT_DICT.end());

class FastEnigmaCore {
public:
    vector<string> r;
    vector<string> n;
    vector<int> rings;
    vector<int> p_init;
    vector<int> p;
    string ref;

    FastEnigmaCore(const vector<string>& rotor_names, const string& rings_str, const string& pos_str) {
        for (const auto& name : rotor_names) {
            r.push_back(ROTORS[name].wiring);
            n.push_back(ROTORS[name].notch);
        }
        for (char c : rings_str) rings.push_back(ALPHABET.find(c));
        for (char c : pos_str) p_init.push_back(ALPHABET.find(c));
        p = p_init;
        ref = REFLECTOR_B;
    }

    void step() {
        bool step_r1 = n[1].find(ALPHABET[p[1]]) != string::npos;
        bool step_r2 = n[2].find(ALPHABET[p[2]]) != string::npos;
        bool step_r3 = n[3].find(ALPHABET[p[3]]) != string::npos;
        bool step_r4 = n[4].find(ALPHABET[p[4]]) != string::npos;

        if (step_r1) {
            p[0] = (p[0] + 1) % 26;
            p[1] = (p[1] + 1) % 26;
        } else if (step_r2) {
            p[1] = (p[1] + 1) % 26;
            p[2] = (p[2] + 1) % 26;
        } else if (step_r3) {
            p[2] = (p[2] + 1) % 26;
            p[3] = (p[3] + 1) % 26;
        } else if (step_r4) {
            p[3] = (p[3] + 1) % 26;
        }
        p[4] = (p[4] + 1) % 26;
    }
};

vector<unordered_map<char, char>> get_core_perms(const vector<string>& rotors, const string& rings, const string& pos, int length) {
    FastEnigmaCore enigma(rotors, rings, pos);
    vector<unordered_map<char, char>> perms;
    for (int k = 0; k < length; ++k) {
        enigma.step();
        unordered_map<char, char> perm;
        for (char c : ALPHABET) {
            int c_idx = ALPHABET.find(c);
            for (int i = 4; i >= 0; --i) {
                int shift = enigma.p[i] - enigma.rings[i];
                int access = (c_idx + shift) % 26;
                if (access < 0) access += 26;
                c_idx = ((int)ALPHABET.find(enigma.r[i][access]) - shift) % 26;
                if (c_idx < 0) c_idx += 26;
            }
            c_idx = ALPHABET.find(enigma.ref[c_idx]);
            for (int i = 0; i < 5; ++i) {
                int shift = enigma.p[i] - enigma.rings[i];
                int access = (c_idx + shift) % 26;
                if (access < 0) access += 26;
                c_idx = ((int)enigma.r[i].find(ALPHABET[access]) - shift) % 26;
                if (c_idx < 0) c_idx += 26;
            }
            perm[c] = ALPHABET[c_idx];
        }
        perms.push_back(perm);
    }
    return perms;
}

double calculate_ioc(const string& text) {
    if (text.length() < 2) return 0.0;
    unordered_map<char, int> counts;
    for (char c : text) counts[c]++;
    double n = text.length();
    double ioc = 0;
    for (auto const& [key, val] : counts) {
        ioc += val * (val - 1);
    }
    return ioc / (n * (n - 1));
}

vector<int> find_valid_crib_offsets(const string& ciphertext, const string& crib) {
    vector<int> offsets;
    for (size_t i = 0; i <= ciphertext.length() - crib.length(); ++i) {
        bool valid = true;
        for (size_t j = 0; j < crib.length(); ++j) {
            if (ciphertext[i + j] == crib[j]) {
                valid = false;
                break;
            }
        }
        if (valid) offsets.push_back(i);
    }
    return offsets;
}

bool solve_graph_helper(unordered_map<char, char>& S, vector<int> remaining_eqs, const string& crib, const string& ciphertext, const vector<unordered_map<char, char>>& perms) {
    if (remaining_eqs.empty()) return true;
    
    int best_eq = -1;
    int best_score = -1;
    
    for (int eq : remaining_eqs) {
        char p = crib[eq];
        char c = ciphertext[eq];
        int score = (S.count(p) ? 1 : 0) + (S.count(c) ? 1 : 0);
        if (score > best_score) {
            best_score = score;
            best_eq = eq;
            if (score == 2) break;
        }
    }
    
    char p = crib[best_eq];
    char c = ciphertext[best_eq];
    int idx = best_eq;
    
    vector<int> next_remaining;
    for (int eq : remaining_eqs) if (eq != best_eq) next_remaining.push_back(eq);
    
    if (best_score == 2) {
        if (perms[idx].at(S[p]) == S[c]) {
            return solve_graph_helper(S, next_remaining, crib, ciphertext, perms);
        }
        return false;
    } else if (best_score == 1) {
        char known = S.count(p) ? p : c;
        char unknown = S.count(p) ? c : p;
        char target = S.count(p) ? perms[idx].at(S[known]) : perms[idx].at(S[c]);
        if (!S.count(p)) target = perms[idx].at(S[c]);
        
        if (S.count(target) && S[target] != unknown) return false;
        
        unordered_map<char, char> S_new = S;
        S_new[unknown] = target;
        S_new[target] = unknown;
        
        if (solve_graph_helper(S_new, next_remaining, crib, ciphertext, perms)) {
            S = S_new;
            return true;
        }
        return false;
    } else {
        for (char guess_Sp : ALPHABET) {
            if (S.count(guess_Sp)) continue;
            char target_Sc = perms[idx].at(guess_Sp);
            if (S.count(target_Sc)) continue;
            if (p == c && guess_Sp != target_Sc) continue;
            if ((guess_Sp == c && target_Sc != p) || (target_Sc == p && guess_Sp != c)) continue;
            
            unordered_map<char, char> S_new = S;
            S_new[p] = guess_Sp; S_new[guess_Sp] = p;
            S_new[c] = target_Sc; S_new[target_Sc] = c;
            
            if (solve_graph_helper(S_new, next_remaining, crib, ciphertext, perms)) {
                S = S_new;
                return true;
            }
        }
        return false;
    }
}

pair<int, unordered_map<char, char>> mp_solve_turing_menu(const string& crib, const string& ct_slice, const vector<unordered_map<char, char>>& perms_slice, int offset) {
    vector<int> equations;
    for (size_t i = 0; i < crib.length(); ++i) equations.push_back(i);
    unordered_map<char, char> S;
    if (solve_graph_helper(S, equations, crib, ct_slice, perms_slice)) {
        return {offset, S};
    }
    return {offset, {}};
}

string apply_decryption(const string& ciphertext, unordered_map<char, char>& S_dict, const vector<string>& rotors, const string& rings, const string& pos) {
    FastEnigmaCore enigma(rotors, rings, pos);
    string out = "";
    for (char char_in : ciphertext) {
        enigma.step();
        char mapped = S_dict.count(char_in) ? S_dict[char_in] : char_in;
        int c_idx = ALPHABET.find(mapped);
        
        for (int i = 4; i >= 0; --i) {
            int shift = enigma.p[i] - enigma.rings[i];
            int access = (c_idx + shift) % 26;
            if (access < 0) access += 26;
            c_idx = ((int)ALPHABET.find(enigma.r[i][access]) - shift) % 26;
            if (c_idx < 0) c_idx += 26;
        }
        c_idx = ALPHABET.find(enigma.ref[c_idx]);
        for (int i = 0; i < 5; ++i) {
            int shift = enigma.p[i] - enigma.rings[i];
            int access = (c_idx + shift) % 26;
            if (access < 0) access += 26;
            c_idx = ((int)enigma.r[i].find(ALPHABET[access]) - shift) % 26;
            if (c_idx < 0) c_idx += 26;
        }
        char out_char = ALPHABET[c_idx];
        out += S_dict.count(out_char) ? S_dict[out_char] : out_char;
    }
    return out;
}

int fuzzy_language_score(const string& text) {
    int score = 0;
    int text_len = text.length();
    
    for (const string& word : WEHRMACHT_DICT) {
        int wl = word.length();
        if (wl < 5) {
            size_t pos = 0;
            while ((pos = text.find(word, pos)) != string::npos) {
                score += wl * 1000;
                pos += word.length();
            }
            continue;
        }
        
        for (int i = 0; i <= text_len - wl; ++i) {
            string window = text.substr(i, wl);
            if (window == word) {
                score += wl * 1000;
            } else {
                int mismatches = 0;
                for (int j = 0; j < wl; ++j) {
                    if (word[j] != window[j]) mismatches++;
                    if (mismatches > 1) break;
                }
                if (mismatches == 1) score += wl * 500;
            }
        }
    }
    
    for (int i = 0; i < text_len - 1; ++i) {
        string bi = text.substr(i, 2);
        if (GERMAN_SCORES.count(bi)) score += GERMAN_SCORES[bi];
    }
    for (int i = 0; i < text_len - 2; ++i) {
        string tri = text.substr(i, 3);
        if (GERMAN_SCORES.count(tri)) score += GERMAN_SCORES[tri];
    }
    return score;
}

int fast_pass_annealing(const string& ciphertext, unordered_map<char, char> locked_S, const vector<string>& rotors, const string& rings, const string& pos) {
    unordered_map<char, char> current_S = locked_S;
    string current_text = apply_decryption(ciphertext, current_S, rotors, rings, pos);
    int best_score = fuzzy_language_score(current_text);
    
    double temperature = 1000.0;
    double cooling_rate = 0.85;
    int iterations_per_temp = 5;
    
    vector<char> unplugged;
    for (char c : ALPHABET) {
        if (!current_S.count(c) || current_S[c] == c) unplugged.push_back(c);
    }
    if (unplugged.size() < 2) return best_score;
    
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> dist(0.0, 1.0);
    
    while (temperature > 1.0) {
        for (int i = 0; i < iterations_per_temp; ++i) {
            int idx_a = rng() % unplugged.size();
            int idx_b = rng() % unplugged.size();
            while (idx_a == idx_b) idx_b = rng() % unplugged.size();
            char a = unplugged[idx_a], b = unplugged[idx_b];
            
            unordered_map<char, char> neighbor_S = current_S;
            
            bool a_free = !neighbor_S.count(a) || neighbor_S[a] == a;
            bool b_free = !neighbor_S.count(b) || neighbor_S[b] == b;
            
            if (a_free && b_free) {
                neighbor_S[a] = b; neighbor_S[b] = a;
            } else {
                char target_a = neighbor_S.count(a) ? neighbor_S[a] : a;
                char target_b = neighbor_S.count(b) ? neighbor_S[b] : b;
                neighbor_S.erase(target_a); neighbor_S.erase(a);
                neighbor_S.erase(target_b); neighbor_S.erase(b);
                neighbor_S[a] = b; neighbor_S[b] = a;
            }
            
            string neighbor_text = apply_decryption(ciphertext, neighbor_S, rotors, rings, pos);
            int neighbor_score = fuzzy_language_score(neighbor_text);
            
            int delta = neighbor_score - best_score;
            if (delta > 0 || exp(delta / temperature) > dist(rng)) {
                current_S = neighbor_S;
                if (neighbor_score > best_score) best_score = neighbor_score;
            }
        }
        temperature *= cooling_rate;
    }
    return best_score;
}

pair<unordered_map<char, char>, vector<int>> simulated_annealing_steckerbrett(const string& ciphertext, unordered_map<char, char> locked_S, const vector<string>& rotors, const string& rings, const string& pos) {
    cout << "\n    [!] PHASE 2: Initiating Thermodynamic AI (Simulated Annealing)..." << endl;
    
    const string GREEN = "\033[92m";
    const string CYAN = "\033[96m";
    const string RED = "\033[91m";
    const string YELLOW = "\033[93m";
    const string RESET = "\033[0m";

    unordered_map<char, char> current_S = locked_S;
    string current_text = apply_decryption(ciphertext, current_S, rotors, rings, pos);
    int current_score = fuzzy_language_score(current_text);
    
    unordered_map<char, char> best_S = current_S;
    int best_score = current_score;
    vector<int> score_history = {current_score};
    
    double START_TEMP = 10000.0;
    double temperature = START_TEMP;
    double cooling_rate = 0.92;
    int iterations_per_temp = 15;
    
    vector<char> unplugged;
    for (char c : ALPHABET) {
        if (!current_S.count(c) || current_S[c] == c) unplugged.push_back(c);
    }
    if (unplugged.size() < 2) return {best_S, score_history};
    
    int chunk_size = 75;
    int chunks_count = ceil((double)ciphertext.length() / chunk_size);
    int total_lines_to_clear = chunks_count + 2;
    
    cout << "    [*] Matrix Initialized. Scanning configurations...\n";
    cout << "    " << YELLOW << "[!] WARNING: Matrix decryption slows down as it stabilizes." << RESET << "\n";
    cout << "    " << YELLOW << "[!] PLEASE WAIT. DO NOT INTERRUPT UNTIL T REACHES 0000.0K!" << RESET << "\n\n";
    
    for (int i=0; i<total_lines_to_clear; ++i) cout << "\n";
    
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> dist(0.0, 1.0);
    
    while (temperature > 0.1) {
        for (int i = 0; i < iterations_per_temp; ++i) {
            int idx_a = rng() % unplugged.size();
            int idx_b = rng() % unplugged.size();
            while (idx_a == idx_b) idx_b = rng() % unplugged.size();
            char a = unplugged[idx_a], b = unplugged[idx_b];
            
            unordered_map<char, char> neighbor_S = current_S;
            bool a_free = !neighbor_S.count(a) || neighbor_S[a] == a;
            bool b_free = !neighbor_S.count(b) || neighbor_S[b] == b;
            
            if (a_free && b_free) {
                neighbor_S[a] = b; neighbor_S[b] = a;
            } else {
                char target_a = neighbor_S.count(a) ? neighbor_S[a] : a;
                char target_b = neighbor_S.count(b) ? neighbor_S[b] : b;
                neighbor_S.erase(target_a); neighbor_S.erase(a);
                neighbor_S.erase(target_b); neighbor_S.erase(b);
                neighbor_S[a] = b; neighbor_S[b] = a;
            }
            
            string neighbor_text = apply_decryption(ciphertext, neighbor_S, rotors, rings, pos);
            int neighbor_score = fuzzy_language_score(neighbor_text);
            
            int delta = neighbor_score - current_score;
            if (delta > 0 || exp(delta / temperature) > dist(rng)) {
                current_S = neighbor_S;
                current_score = neighbor_score;
                current_text = neighbor_text;
                
                if (current_score > best_score) {
                    best_score = current_score;
                    best_S = current_S;
                    score_history.push_back(best_score);
                }
            }
        }
        
        double progress = 1.0 - (temperature / START_TEMP);
        progress = max(0.0, min(1.0, progress));
        
        int bar_len = 15;
        int filled = progress * bar_len;
        string bar = string(filled, '#') + string(bar_len - filled, '-');
        
        int reveal_count = progress * ciphertext.length();
        
        printf("\033[%dA", total_lines_to_clear);
        printf("\r    [AI] [%s] %3d%% | T:%06.1fK | \033[93mDON'T INTERRUPT UNTIL T=0!\033[0m | BEST: %d\033[K\n\n", bar.c_str(), (int)(progress*100), temperature, best_score);
        
        for (size_t i = 0; i < ciphertext.length(); i += chunk_size) {
            string colored_chunk = "";
            for (size_t j = i; j < min(i + chunk_size, ciphertext.length()); ++j) {
                if (j < reveal_count) {
                    string color = ((j / 5) % 2 == 0) ? GREEN : CYAN;
                    colored_chunk += color + current_text[j] + RESET;
                } else {
                    colored_chunk += RED + ciphertext[j] + RESET;
                }
            }
            cout << "    " << colored_chunk << "\033[K\n";
        }
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(25));
        
        temperature *= cooling_rate;
    }

    string best_text = apply_decryption(ciphertext, best_S, rotors, rings, pos);
    
    printf("\033[%dA", total_lines_to_clear);
    printf("\r    [AI] [%s] 100%% | T:0000.0K | FINAL BEST SCORE: %d\033[K\n\n", string(15, '#').c_str(), best_score);
    
    for (size_t i = 0; i < best_text.length(); i += chunk_size) {
        string colored_chunk = "";
        for (size_t j = i; j < min(i + chunk_size, best_text.length()); ++j) {
            string color = ((j / 5) % 2 == 0) ? GREEN : CYAN;
            colored_chunk += color + best_text[j] + RESET;
        }
        cout << "    " << colored_chunk << "\033[K\n";
    }
    cout.flush();

    cout << "\n    [!] Thermodynamic matrix stabilized. Final optimum score: " << best_score << endl;
    return {best_S, score_history};
}

string nlp_word_segmentation(string text) {
    text.erase(remove_if(text.begin(), text.end(), ::isspace), text.end());
    int n = text.length();
    
    vector<double> dp(n + 1, 0.0);
    vector<int> back(n + 1, 0);
    
    for (int i = 1; i <= n; ++i) {
        double best_score = -1e9;
        int best_k = 0;
        
        for (int k = max(0, i - 20); k < i; ++k) {
            string word = text.substr(k, i - k);
            double score = 0;
            if (VOCAB_SET.count(word)) {
                score = dp[k] + pow(word.length(), 3) * 10;
            } else {
                score = dp[k] - (word.length() + 2);
            }
            
            if (score > best_score) {
                best_score = score;
                best_k = k;
            }
        }
        dp[i] = best_score;
        back[i] = best_k;
    }
    
    vector<string> words;
    int curr = n;
    while (curr > 0) {
        int prev = back[curr];
        words.push_back(text.substr(prev, curr - prev));
        curr = prev;
    }
    
    reverse(words.begin(), words.end());
    string res = "";
    for (size_t i = 0; i < words.size(); ++i) {
        res += words[i];
        if (i != words.size() - 1) res += " ";
    }
    return res;
}

void generate_pdf_report(const vector<int>& score_history, const string& final_text, const string& human_text, const string& ciphertext, const vector<string>& rotors, const string& rings, const string& pos, const unordered_map<char, char>& steckerbrett, int offset) {
    cout << "\n[*] GENERATING 'ULTRA' CLASSIFIED 4-PAGE DOSSIER VIA PYTHON MATPLOTLIB BRIDGE...\n";
    
    string stecker_str = "";
    unordered_set<char> seen;
    for (const auto& pair : steckerbrett) {
        if (pair.first != pair.second && !seen.count(pair.first)) {
            stecker_str += string(1, pair.first) + string(1, pair.second) + " ";
            seen.insert(pair.first); seen.insert(pair.second);
        }
    }
    if (stecker_str.empty()) stecker_str = "NONE";

    string history_str = "[";
    for(size_t i=0; i<score_history.size(); i++) {
        history_str += to_string(score_history[i]);
        if(i != score_history.size()-1) history_str += ", ";
    }
    history_str += "]";

    string rotors_str = "['" + rotors[0] + "', '" + rotors[1] + "', '" + rotors[2] + "', '" + rotors[3] + "', '" + rotors[4] + "']";

    ofstream py_file("generate_report.py");
    py_file << R"PYTHON(
import matplotlib.pyplot as plt
import matplotlib.style as style
from matplotlib.backends.backend_pdf import PdfPages
import math
import textwrap
from collections import Counter
import time

def calculate_ioc(text):
    if len(text) < 2: return 0.0
    counts = Counter(text)
    n = len(text)
    ioc = sum(c * (c - 1) for c in counts.values()) / (n * (n - 1))
    return ioc

def run():
    score_history = )PYTHON" << history_str << R"PYTHON(
    final_text = ")PYTHON" << final_text << R"PYTHON("
    human_text = ")PYTHON" << human_text << R"PYTHON("
    ciphertext = ")PYTHON" << ciphertext << R"PYTHON("
    rotors = )PYTHON" << rotors_str << R"PYTHON(
    rings = ")PYTHON" << rings << R"PYTHON("
    pos = ")PYTHON" << pos << R"PYTHON("
    stecker_str = ")PYTHON" << stecker_str << R"PYTHON("
    offset = )PYTHON" << offset << R"PYTHON(

    # Parse stecker_str back to dict
    steckerbrett = {}
    if stecker_str != "NONE":
        for p in stecker_str.split():
            steckerbrett[p[0]] = p[1]
            steckerbrett[p[1]] = p[0]

    german_freq = {
        'E': 17.40, 'N': 9.78, 'I': 7.55, 'S': 7.27, 'R': 7.00, 'A': 6.51, 'T': 6.15,
        'D': 5.08, 'H': 4.28, 'U': 4.35, 'L': 3.44, 'C': 3.06, 'G': 3.01, 'M': 2.53,
        'O': 2.51, 'B': 1.89, 'W': 1.89, 'F': 1.66, 'K': 1.21, 'Z': 1.13, 'P': 0.79,
        'V': 0.67, 'J': 0.27, 'Y': 0.04, 'X': 0.03, 'Q': 0.02
    }
    
    ioc = calculate_ioc(final_text)
    ioc_status = "CONFIRMED GERMAN" if ioc > 0.065 else "GARBLED/NOISY"
    
    text_counts = Counter(final_text)
    total_chars = len(final_text) if final_text else 1
    actual_freq = {char: (count / total_chars) * 100 for char, count in text_counts.items()}
    
    labels = list("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
    expected_y = [german_freq.get(c, 0) for c in labels]
    actual_y = [actual_freq.get(c, 0) for c in labels]

    style.use('dark_background')
    pdf_filename = "Hut6_ULTRA_Report_v18_CLASSIFIED.pdf"
    footer_text = "(c) by George W. Aravidis | Ulm Söflingen | Email: csec.unbutton326@passinbox.com | WWII Historical Reference"
    
    def add_watermark(fig):
        fig.text(0.5, 0.5, 'T O P   S E C R E T   -   U L T R A',
                 fontsize=65, color='#FF0000', alpha=0.08, 
                 ha='center', va='center', rotation=45, fontweight='bold')
                 
    with PdfPages(pdf_filename) as pdf:
        # PAGE 1
        fig1 = plt.figure(figsize=(11, 14))
        fig1.patch.set_facecolor('#050505')
        fig1.suptitle('T O P   S E C R E T   -   U L T R A\nHUT 6 DECRYPTION INTELLIGENCE DOSSIER', fontsize=22, fontweight='bold', color='#FF3333', y=0.96)
        add_watermark(fig1)

        settings = (
            f"[ TARGET MACHINE CONFIGURATION ]\n"
            f"Walzenlage (Rotors)   | : {' '.join(rotors)}\n"
            f"Ringstellung (Rings)  | : {rings}\n"
            f"Grundstellung (Pos)   | : {pos}\n"
            f"Steckerbrett (Plugs)  | : {stecker_str}"
        )
        intercept_metadata = (
            f"[ INTERCEPT METADATA ]\n"
            f"Station               | : Y-Station Beaumanor (Leicestershire, UK)\n"
            f"Frequency             | : 4155 kHz (Kriegsmarine Shortwave Band)\n"
            f"Signal Quality        | : Grade 3 (Intermittent QRM / Atmospheric Static)\n"
            f"Operator ID           | : 74-B (Section V Special Intercept Team)"
        )
        metrics = (
            f"[ CRYPTANALYSIS METRICS ]\n"
            f"Crib Dragging Offset  : {offset}\n"
            f"Index of Coincidence  : {ioc:.4f} ({ioc_status})\n"
            f"Final AI Fuzzy Score  : {score_history[-1]}"
        )
        
        # Format strings
        wrap_c = '\n'.join(textwrap.wrap(ciphertext, width=105))
        
        # The Nerd Flex: 5-Letter Grouping
        grouped_final = ' '.join(final_text[i:i+5] for i in range(0, len(final_text), 5))
        wrap_f = '\n'.join(textwrap.wrap(grouped_final, width=105))
        
        wrap_h = '\n'.join(textwrap.wrap(human_text, width=105))
        
        security_warning = (
            f"!!! WARNING: TOP SECRET ULTRA !!!\n"
            f"This document contains signals intelligence derived from high-grade Axis\n"
            f"cryptography. Under the Official Secrets Act, any unauthorized disclosure\n"
            f"is punishable by treason charges. Destroy by fire immediately after analysis."
        )

        content_p1 = f"{settings}\n\n{intercept_metadata}\n\n{metrics}\n\n[ RAW INTERCEPTED CIPHERTEXT ]\n{wrap_c}\n\n[ DECRYPTED RAW (5-LETTER GROUPS) ]\n{wrap_f}\n\n[ NLP RECONSTRUCTED PLAINTEXT ]\n{wrap_h}\n\n{security_warning}"
        
        # Adjusted Y-axis (0.92) and Font Size (8.5) to fit the new massive block
        fig1.text(0.06, 0.92, content_p1, fontsize=8.5, color='#00FF00', family='monospace', va='top')
        fig1.text(0.95, 0.02, footer_text, ha='right', fontsize=9, color='#008800', family='monospace')
        pdf.savefig(fig1, facecolor=fig1.get_facecolor(), edgecolor='none', dpi=300)
        plt.close(fig1)

        # PAGE 2
        fig2 = plt.figure(figsize=(11, 14))
        fig2.patch.set_facecolor('#050505')
        fig2.suptitle('T O P   S E C R E T   -   U L T R A\nHUT 6 CRYPTANALYSIS TELEMETRY', fontsize=22, fontweight='bold', color='#FF3333', y=0.96)
        add_watermark(fig2)
        plt.subplots_adjust(top=0.88, bottom=0.1, hspace=0.35)
        ax1 = fig2.add_subplot(2, 1, 1)
        ax1.plot(range(len(score_history)), score_history, marker='o', color='#00FF00', linewidth=2)
        ax1.set_title('Phase 2.5: Thermodynamic AI Evolution (Simulated Annealing)', color='white', fontweight='bold')
        ax1.set_xlabel('Improvements Discovered over Time')
        ax1.set_ylabel('Fuzzy Linguistic Score')
        ax1.grid(True, alpha=0.3, color='#00FF00', linestyle='--')
        ax1.set_facecolor('#0a0a0a')
        ax2 = fig2.add_subplot(2, 1, 2)
        x = range(len(labels))
        width = 0.4
        ax2.bar([i - width/2 for i in x], expected_y, width=width, label='Expected (German)', color='#333333')
        ax2.bar([i + width/2 for i in x], actual_y, width=width, label='Actual (Decrypted)', color='#00FF00', alpha=0.8)
        ax2.set_title('Letter Frequency: Expected vs Actual', color='white', fontweight='bold')
        ax2.set_xticks(x)
        ax2.set_xticklabels(labels, color='#00FF00')
        ax2.set_ylabel('Frequency (%)')
        ax2.legend()
        ax2.set_facecolor('#0a0a0a')
        fig2.text(0.95, 0.02, footer_text, ha='right', fontsize=9, color='#008800', family='monospace')
        pdf.savefig(fig2, facecolor=fig2.get_facecolor(), edgecolor='none', dpi=300)
        plt.close(fig2)

        # PAGE 3
        fig3 = plt.figure(figsize=(11, 14))
        fig3.patch.set_facecolor('#050505')
        fig3.suptitle('T O P   S E C R E T   -   U L T R A\nHUT 6 DECLASSIFIED HISTORICAL ARCHIVE', fontsize=22, fontweight='bold', color='#FF3333', y=0.96)
        add_watermark(fig3)
        
        tribute = (
            "BLETCHLEY PARK MEMORIAL ARCHIVE\n"
            "This decryption engine is dedicated to Alan Turing, Gordon Welchman, Joan Clarke,\n"
            "and the thousands of brilliant minds at Bletchley Park. Their relentless pursuit\n"
            "of cracking the Enigma cipher shortened WWII and laid the bedrock for modern\n"
            "computer science. They broke the unbreakable."
        )
        
        routing_history = (
            "[ DECLASSIFIED ROUTING PROTOCOL ]\n"
            "This raw decrypt was generated by the cryptanalysts of Hut 6 (Army/Luftwaffe) and\n"
            "Hut 8 (Naval). Upon successful decryption, specialized runners would immediately rush\n"
            "this document out of the codebreaking huts and securely deliver it to the intelligence\n"
            "analysts in Hut 3 or 4. There, the German plaintext was analyzed and prioritized.\n"
            "The final intelligence product, stamped 'ULTRA', was transmitted via secure teleprinter\n"
            "to MI6 headquarters in London and frequently directly to Winston Churchill's desk,\n"
            "altering the course of the war without the Axis powers ever suspecting a thing."
        )

        turing_bombe = (
            "[ THE TURING-WELCHMAN BOMBE ]\n"
            "The decryption was facilitated by the logic of the electro-mechanical 'Bombe',\n"
            "designed by Alan Turing and enhanced by Gordon Welchman's 'diagonal board'.\n"
            "Operating out of outstations like Eastcote and Stanmore, hundreds of Wrens (Women's\n"
            "Royal Naval Service) operated these noisy, oil-leaking machines 24/7 to deduce\n"
            "the daily Enigma wheel settings by exploiting German operational flaws."
        )
        
        current_date_str = time.strftime('%d %B %Y').upper()
        declassification = (
            "+-----------------------------------------------------------------------------------------+\n"
            f"Declassified by UK National Archives - {current_date_str}\n"
            "Archival Reconstruction for Educational purposes. Based on declassified GCHQ/ULTRA data.\n"
            "Historical Project 'Enigma Cracker' - APPROVED FOR ACADEMIC RELEASE\n"
            "+-----------------------------------------------------------------------------------------+"
        )

        content_p3 = f"{tribute}\n\n\n{routing_history}\n\n\n{turing_bombe}\n\n\n\n\n\n\n\n\n\n{declassification}"
        
        fig3.text(0.06, 0.88, content_p3, fontsize=11, color='#00FF00', family='monospace', va='top')
        fig3.text(0.95, 0.02, footer_text, ha='right', fontsize=9, color='#008800', family='monospace')
        pdf.savefig(fig3, facecolor=fig3.get_facecolor(), edgecolor='none', dpi=300)
        plt.close(fig3)

        # PAGE 4
        fig4 = plt.figure(figsize=(11, 14))
        fig4.patch.set_facecolor('#050505')
        fig4.suptitle('T O P   S E C R E T   -   U L T R A\nTACTICAL WIRING BLUEPRINT (STECKERBRETT)', fontsize=22, fontweight='bold', color='#FF3333', y=0.96)
        add_watermark(fig4)
        ax4 = fig4.add_subplot(1, 1, 1)
        ax4.set_facecolor('#050505')
        ax4.axis('off')
        
        radius = 10
        coords = {}
        for i, char in enumerate(labels):
            angle = math.radians(90 - (360 / 26) * i)
            coords[char] = (radius * math.cos(angle), radius * math.sin(angle))
            
        drawn_connections = set()
        for k, v in steckerbrett.items():
            if k != v and (k, v) not in drawn_connections and (v, k) not in drawn_connections:
                x_vals = [coords[k][0], coords[v][0]]
                y_vals = [coords[k][1], coords[v][1]]
                ax4.plot(x_vals, y_vals, color='#00FFFF', linewidth=2.5, alpha=0.8, zorder=1)
                drawn_connections.add((k, v))
                drawn_connections.add((v, k))
                
        for char, (x, y) in coords.items():
            is_plugged = steckerbrett.get(char, char) != char
            node_color = '#FF0000' if is_plugged else '#333333'
            ax4.plot(x, y, 'o', color=node_color, markersize=24, zorder=2)
            ax4.text(x, y, char, color='white', fontsize=12, ha='center', va='center', fontweight='bold', zorder=3)
            
        fig4.text(0.5, 0.88, f"PLUGBOARD MAPPING ({len(drawn_connections)//2} ACTIVE CONNECTIONS)", color='#00FF00', fontsize=14, ha='center', va='top', family='monospace', fontweight='bold')
        fig4.text(0.95, 0.02, footer_text, ha='right', fontsize=9, color='#008800', family='monospace')
        pdf.savefig(fig4, facecolor=fig4.get_facecolor(), edgecolor='none', dpi=300)
        plt.close(fig4)

if __name__ == "__main__":
    run()
)PYTHON";
    py_file.close();
    
    int ret = system("python3 generate_report.py");
    if (ret == 0) {
        cout << "[+] SUCCESS: Classified 4-Page Report typed up and sealed as 'Hut6_ULTRA_Report_v18_CLASSIFIED.pdf'\n";
    } else {
        cout << "[-] Python plotting failed. Check dependencies.\n";
    }
}


string get_clean_text(string prompt) {
    while (true) {
        cout << prompt;
        string val;
        getline(cin, val);
        string clean = "";
        for (char c : val) {
            if (isalpha(c)) clean += toupper(c);
        }
        if (clean.empty()) {
            cout << "[-] VERBOTEN! Input cannot be empty or invalid.\n";
            continue;
        }
        return clean;
    }
}

string get_5_letters(string prompt) {
    while (true) {
        string val = get_clean_text(prompt);
        if (val.length() == 5) return val;
        cout << "[-] NEIN! Must be exactly 5 letters (e.g. ABCDE).\n";
    }
}

vector<string> get_rotors() {
    vector<string> available = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII"};
    vector<string> selected;
    cout << "\n+ " << string(57, '-') << " +\n";
    cout << "| WALZENORDNUNG (ROTOR CONFIGURATION)                     |\n";
    cout << "+ " << string(57, '-') << " +\n";
    for (int i = 1; i <= 5; ++i) {
        while (true) {
            cout << "[*] Rotor " << i << " (Available: ";
            for(size_t j=0; j<available.size(); j++) cout << available[j] << (j==available.size()-1 ? "" : ", ");
            cout << "): ";
            string r;
            getline(cin, r);
            for (auto & c: r) c = toupper(c);
            auto it = find(available.begin(), available.end(), r);
            if (it != available.end()) {
                selected.push_back(r);
                available.erase(it);
                break;
            } else {
                cout << "[-] ERROR: Invalid rotor '" << r << "' or already used. Try again.\n";
            }
        }
    }
    return selected;
}

pair<string, unordered_map<char, char>> interactive_override(const string& ciphertext, unordered_map<char, char> current_S, const vector<string>& rotors, const string& rings, const string& pos) {
    cout << "\n+ " << string(57, '-') << " +\n";
    cout << "| PHASE 3: HUT 6 OVERRIDE TERMINAL                        |\n";
    cout << "+ " << string(57, '-') << " +\n";
    
    while (true) {
        string final_text = apply_decryption(ciphertext, current_S, rotors, rings, pos);
        string formatted = "";
        for (size_t i = 0; i < final_text.length(); i += 5) {
            formatted += final_text.substr(i, 5) + " ";
        }
        
        string plugs = "";
        unordered_set<char> seen;
        for (const auto& pair : current_S) {
            if (pair.first != pair.second && !seen.count(pair.first)) {
                plugs += string(1, pair.first) + string(1, pair.second) + " ";
                seen.insert(pair.first); seen.insert(pair.second);
            }
        }
        
        cout << "\n[>] CURRENT PLUGBOARD: " << (plugs.empty() ? "NONE" : plugs) << "\n";
        cout << "[>] CURRENT DECRYPT:\n    " << formatted << "\n\n";
        
        cout << "[?] Are there typos? Enter 2 letters to swap (e.g. HJ), or type 'PERFEKT' to finish:\n> ";
        string action;
        getline(cin, action);
        for (auto & c: action) c = toupper(c);
        
        if (action == "PERFEKT") return {final_text, current_S};
        else if (action.length() == 2) {
            char a = action[0], b = action[1];
            if (a == b) {
                cout << "[-] Fool! You cannot plug a letter into itself.\n";
                continue;
            }
            if (current_S.count(a) && current_S[a] != a) current_S.erase(current_S[a]);
            if (current_S.count(b) && current_S[b] != b) current_S.erase(current_S[b]);
            current_S.erase(a); current_S.erase(b);
            current_S[a] = b; current_S[b] = a;
            cout << "[+] Wire " << a << "-" << b << " manually patched.\n";
        } else {
            cout << "[-] Invalid input. Enter exactly 2 letters, or 'PERFEKT'.\n";
        }
    }
}

int main() {
    cout << R"(
 + --------------------------------------------------------- +
 |                                                           |
 |  ██████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗          |
 |  ██╔═══╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗         |
 |  █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║        |
 |  ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║        |
 |  ██████╗ ██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║        |
 |  ╚═════╝ ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝        |
 |                                                           |
 |  [ THE ULTIMATE BLETCHLEY PARK ENGINE v18.0]              |
 |  [ NLP META-HEURISTIC PRUNING ENABLED ]                   |
 |                                                           |
 |  (c) George W. Aravidis | Ulm Söflingen                   |
 |  Email address # csec.unbutton326@passinbox.com           |
 + --------------------------------------------------------- +
    )" << "\n";
    cout << "[!] ERROR HANDLING: MAXIMUM. NO TOLERANCE FOR MISTAKES.\n\n";
    
    vector<string> rotors = get_rotors();
    string rings = get_5_letters("\n[*] ENTER CAPTURED RINGSTELLUNG (e.g. FDWAB): ");
    string pos = get_5_letters("[*] ENTER CAPTURED GRUNDSTELLUNG (e.g. FLPDF): ");
    
    string ciphertext = get_clean_text("\n[?] ENTER INTERCEPTED CIPHERTEXT:\n> ");
    string crib;
    while (true) {
        crib = get_clean_text("\n[?] ENTER KNOWN PLAINTEXT (CRIB):\n> ");
        if (crib.length() > ciphertext.length()) cout << "[-] VERBOTEN! Crib is longer than the Ciphertext. Try again.\n";
        else break;
    }
    
    cout << "\n+ " << string(57, '-') << " +\n";
    cout << "| PHASE 1: INITIATING TRUE TURING BOMBE ENGINE            |\n";
    cout << "+ " << string(57, '-') << " +\n";
    auto start_time = chrono::high_resolution_clock::now();
    
    vector<int> offsets = find_valid_crib_offsets(ciphertext, crib);
    if (offsets.empty()) {
        cout << "[-] FATAL: Crib crashes at every possible offset! (Check your crib for errors)\n";
        return 1;
    }
    
    cout << "    [+] Crib Dragging: Found " << offsets.size() << " possible valid offsets.\n";
    cout << "    [*] Spawning C++ std::async worker threads for distributed Bombe attack...\n\n";
    
    vector<unordered_map<char, char>> perms = get_core_perms(rotors, rings, pos, ciphertext.length());
    
    vector<future<pair<int, unordered_map<char, char>>>> futures;
    for (int offset : offsets) {
        string ct_slice = ciphertext.substr(offset, crib.length());
        vector<unordered_map<char, char>> perms_slice(perms.begin() + offset, perms.begin() + offset + crib.length());
        futures.push_back(async(launch::async, mp_solve_turing_menu, crib, ct_slice, perms_slice, offset));
    }
    
    vector<pair<int, unordered_map<char, char>>> valid_bombe_results;
    int completed_tasks = 0;
    int total_tasks = futures.size();
    
    for (auto& fut : futures) {
        auto result = fut.get();
        completed_tasks++;
        double percent = (double)completed_tasks / total_tasks;
        int bar_len = 15;
        int filled = percent * bar_len;
        string bar = string(filled, '#') + string(bar_len - filled, '-');
        
        string crib_preview = crib.substr(0, 5);
        string ct_preview = ciphertext.substr(result.first, 5);
        
        printf("\r    [BOMBE] [%s] %3d%% | OFS: %03d | MATCH: %s->%-5s", bar.c_str(), (int)(percent*100), result.first, crib_preview.c_str(), ct_preview.c_str());
        cout.flush();
        
        if (!result.second.empty()) valid_bombe_results.push_back(result);
    }
    cout << "\n";
    
    if (valid_bombe_results.empty()) {
        cout << "\n[-] CRITICAL FAILURE IN BOMBE MATRIX!\n";
        cout << "[-] Contradiction detected. The Crib does not match this Ciphertext with these settings.\n";
        return 1;
    }
    
    int best_offset;
    unordered_map<char, char> bombe_S;
    
    if (valid_bombe_results.size() > 1) {
        cout << "\n+ " << string(57, '-') << " +\n";
        cout << "| PHASE 1.5: META-HEURISTIC MULTI-OFFSET PRUNING          |\n";
        cout << "+ " << string(57, '-') << " +\n";
        cout << "    [*] Found " << valid_bombe_results.size() << " conflicting valid offsets. Initiating fast-pass evaluation...\n";
        
        int best_heur_score = -1;
        for (size_t i = 0; i < valid_bombe_results.size(); ++i) {
            printf("\r    [PRUNING] Assessing offset %03d out of %zu...\033[K", valid_bombe_results[i].first, valid_bombe_results.size());
            cout.flush();
            int score = fast_pass_annealing(ciphertext, valid_bombe_results[i].second, rotors, rings, pos);
            if (score > best_heur_score) {
                best_heur_score = score;
                best_offset = valid_bombe_results[i].first;
                bombe_S = valid_bombe_results[i].second;
            }
        }
        cout << "\n    [+] Pruning complete. Offset " << best_offset << " selected as True Peak (Heuristic Score: " << best_heur_score << ").\n";
    } else {
        best_offset = valid_bombe_results[0].first;
        bombe_S = valid_bombe_results[0].second;
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> diff = end_time - start_time;
        cout << "    [+] Bombe deduced initial Steckerbrett at unique offset " << best_offset << " in " << diff.count() << " seconds.\n";
    }
    
    auto sa_result = simulated_annealing_steckerbrett(ciphertext, bombe_S, rotors, rings, pos);
    auto override_result = interactive_override(ciphertext, sa_result.first, rotors, rings, pos);
    
    cout << "\n+ " << string(57, '-') << " +\n";
    cout << "| PHASE 4: NLP LINGUISTIC RECONSTRUCTION                  |\n";
    cout << "+ " << string(57, '-') << " +\n";
    cout << "    [*] Parsing Viterbi string graph...\n";
    
    string human_readable_text = nlp_word_segmentation(override_result.first);
    
    cout << "\n[>] RECONSTRUCTED TRANSMISSION:\n";
    
    for (size_t i = 0; i < human_readable_text.length(); i += 80) {
        cout << "    " << human_readable_text.substr(i, 80) << "\n";
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    
    cout << "\n[+] MESSAGE SECURED. PRINTING FOR CHURCHILL...\n";
    
    if (!override_result.first.empty() && !sa_result.second.empty()) {
        generate_pdf_report(sa_result.second, override_result.first, human_readable_text, ciphertext, rotors, rings, pos, override_result.second, best_offset);
    }
    
    return 0;
}
