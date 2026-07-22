import sys
import time
import math
import random
import textwrap
import concurrent.futures

from collections import Counter

import matplotlib
import matplotlib.pyplot as plt
import matplotlib.style as style

from matplotlib.backends.backend_pdf import PdfPages

# + --------------------------------------------------------- +
# | KRIEGSWEHRMACHT 5-ROTOR SPECIFICATIONS                    |
# + --------------------------------------------------------- +
ROTORS = {
    'I':    {'wiring': 'EKMFLGDQVZNTOWYHXUSPAIBRCJ', 'notch': 'Q'},
    'II':   {'wiring': 'AJDKSIRUXBLHWTMCQGZNPYFVOE', 'notch': 'E'},
    'III':  {'wiring': 'BDFHJLCPRTXVZNYEIWGAKMUSQO', 'notch': 'V'},
    'IV':   {'wiring': 'ESOVPZJAYQUIRHXLNFTGKDCMWB', 'notch': 'J'},
    'V':    {'wiring': 'VZBRGITYUPSDNHLXAWMJQOFECK', 'notch': 'Z'},
    'VI':   {'wiring': 'JPGVOUMFYQBENHZRDKASXLICTW', 'notch': 'ZM'},
    'VII':  {'wiring': 'NZJHGRCXMYSWBOUFAIVLPEKQDT', 'notch': 'ZM'},
    'VIII': {'wiring': 'FKQHTLXOCBJSPDZRAMEWNIUYGV', 'notch': 'ZM'}
}
REFLECTOR_B = 'YRUHQSLDPXNGOKMIEBFZCWVJAT'
ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'

GERMAN_SCORES = {
    'ER': 10, 'EN': 9, 'CH': 8, 'DE': 7, 'ND': 6, 'TE': 5, 
    'IN': 5, 'IE': 4, 'GE': 4, 'EI': 4, 'UN': 3, 'ST': 3,
    'DER': 15, 'DIE': 15, 'UND': 15, 'EIN': 12, 'SCH': 15, 'CHT': 10,
    'TEN': 12, 'GEN': 12, 'VER': 12, 'TER': 10, 'ICH': 15
}

# + --------------------------------------------------------- +
# | THE ULTIMATE WWII GERMAN LINGUISTIC DATABASE (NLP MODULE) |
# + --------------------------------------------------------- +

WEHRMACHT_DICT = [

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
    "ZEHN", "ELF", "ZWOELF", "ZWANZIG", "DREISSIG", "VIERZIG", "FUENFZIG", "HUNDERT", "TAUSEND",
    "ERSTER", "ZWEITER", "DRITTER", "MONTAG", "DIENSTAG", "MITTWOCH", "DONNERSTAG", "FREITAG", 
    "SAMSTAG", "SONNTAG", "HEUTE", "MORGEN", "GESTERN", "UHR", "MINUTEN", "STUNDEN", "TAGE", 
    "WOCHEN", "MONATE", "JAHR", "JANUAR", "FEBRUAR", "MAERZ", "APRIL", "MAI", "JUNI", "JULI", 
    "AUGUST", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DEZEMBER",

    "FUNK", "FUNKER", "FUNKSPRUCH", "FUNKSTILLE", "ENIGMA", "SCHLUESSEL", "GEHEIM", "STRENG",
    "STRENGGEHEIM", "MORSE", "FREQUENZ", "EMPFAENGER", "SENDER", "VERSCHLUESSELT", "KLARTEXT",
    "MELDUNG", "FUNKSTATION", "WELLENLAENGE", "FUNKOFFIZIER", "EMPFANG", "BESTAETIGUNG",
    "WIEDERHOLEN", "KONTROLLE", "UEBERMITTELT", "SIGNAL", "ENTZAEHLT", "CHIFFRE",

    "HEER", "WEHRMACHT", "INFANTERIE", "ARTILLERIE", "KAVALLERIE", "PIONIER", "PIONIERE",
    "SOLDAT", "SOLDATEN", "SCHUETZE", "GRENADIER", "OFFIZIER", "OFFIZIERE", "UNTEROFFIZIER",
    "FELDWEBEL", "LEUTNANT", "HAUPTMANN", "MAJOR", "OBERST", "GENERAL", "FELDMARSCHALL",
    "ARMEE", "KORPS", "DIVISION", "REGIMENT", "BATAILLON", "KOMPANIE", "ZUG", "GRUPPE",
    "FRONT", "VORAUS", "ZURUECK", "VORMARSCH", "RUECKZUG", "KAMPF", "GEFECHT", "ANGRIFF",
    "VERTEIDIGUNG", "SIEG", "VERLUSTE", "GEFANGENE", "GEFALLEN", "VERWUNDET", "LAZARETT",
    "HAUPTQUARTIER", "STUETZPUNKT", "STELLUNG", "LINIE", "BRUECKE", "WALD", "BERG", "FLUSS",

    "PANZER", "PANZERDIVISION", "TIGER", "PANTHER", "KAMPFWAGEN", "STURMGESCHUETZ", "KETTENKRAD",
    "HALBKETTENFAHRZEUG", "SPAEHWAGEN", "MOTORRAD", "KRAFTFAHRZEUG", "TREIBSTOFF", "BENZIN",

    "LUFTWAFFE", "FLUGZEUG", "FLUGZEUGE", "JAEGER", "BOMBER", "STUKA", "FLIEGER", "PILOT",
    "FALLSCHIRM", "FALLSCHIRMJAEGER", "LUFTLANDE", "ABSPRUNG", "GESCHWADER", "STAFFEL",
    "MESSERSCHMITT", "JUNKERS", "HEINKEL", "FOCKEWULF", "LUFTANGRIFF", "BOMBEN", "ABWURF",
    "FLUGPLATZ", "FLUGHAFEN", "AUFKLAERUNG", "LUFTABWEHR", "FLAK", "ROHR", "VISIER",

    "KRIEGSMARINE", "MARINE", "UBOOT", "UBOTE", "UBOOTE", "FLOTTE", "SCHIFF", "SCHIFFE",
    "KREUZER", "ZERSTOERER", "SCHLACHTSCHIFF", "KONVOI", "GELEITZUG", "GELEITZUEGE", "GELEIT",
    "HILFSKREUZER", "ATLANTIK", "NORDSEE", "OSTSEE", "MITTELMEER", "MUNDUNG", "HAFEN", "KANAL",
    "KANALS", "KUESTE", "KURS", "WELLEN", "TAUCHEN", "PERISKOP", "SEEMEILEN", "TIEFE", "BUG",
    "HECK", "KNOTEN", "ADMIRAL", "KAPIETAEN", "KOMMANDANT", "KOMMANDANTEN", "MATROSE", 
    "KAMPFSCHWIMMER", "FROSCHMANN", "MEERESGRUND", "ATEMGERAET", "KLEINSTUBOOT", "TORPEDO",
    "TORPEDOS", "MINEN", "MINENSUCHER", "WASSERBOMBE", "GEWAESSER",

    "WAFFE", "WAFFEN", "MUNITION", "PATRONE", "PATRONEN", "KUGEL", "KUGELN", "HUESLEN",
    "GEWEHR", "KARABINER", "MASCHINENGEWEHR", "PISTOLE", "LUGER", "SCHMEISSER", "MASCHINENPISTOLE",
    "GRANATE", "GRANATEN", "HANDGRANATE", "PANZERFAUST", "PANZERSCHRECK", "GESCHUETZ",
    "KALIBER", "SPRENGSTOFF", "DYNAMIT", "ZUENDER", "FEUER", "SCHIESSEN", "TREFFER", "LADEHEMMUNG",

    "WETTER", "WETTERVORHERSAGE", "VORHERSAGE", "STURM", "STURMS", "REGEN", "SCHNEE", "GEWITTER",
    "LUFTDRUCK", "TEMPERATUR", "BEWOELKT", "NEBEL", "NEBELS", "SICHT", "SICHTWEITE", "NORDEN",
    "SUEDEN", "OSTEN", "WESTEN", "WIND", "STURMTIEF", "HOCHDRUCK", "TIEFDRUCK", "WOLKEN", "KLAR",

    "BEFEHL", "BEFEHLE", "BEFOHLEN", "BEFIEHLT", "SOFORT", "SOFORTIGEN", "BESTAETIGEN", "MELDEN",
    "ERREICHEN", "POSITION", "POSITIONEN", "QUADRAT", "ABSCHNITT", "ENDE", "EINSATZ", "VERSTANDEN",
    "OPERATION", "GEFAHR", "SICHERHEIT", "SPERRE", "ALARM", "ZIEL", "EINHEIT", "EINHEITEN",
    "KOORDINATEN", "GRAD", "BERICHT", "PLAN", "AUFGRUND", "EXTREMEN", "DICHTEN", "UEBERWACHEN",
    "GEWAEHRLEISTEN", "DEUTSCHER", "DEUTSCHE", "ENGLISCHE", "BRITISCHE", "AMERIKANISCHE",
    "RUSSISCHE", "SOWJETISCHE", "GELUNGEN", "GESCHEITERT", "ZERSTOERT", "VERSENKT", 
    "BESCHAEDIGT", "GEMELDET", "GEFUNDEN",

    "FEIND", "FEINDE", "FEINDLICH", "FEINDLICHE", "FEINDLICHEN", "FEINDES",
    "GEGNER", "TRUPPEN", "TRUPPE",

]
VOCAB_SET = set(WEHRMACHT_DICT)


class FastEnigmaCore:
    def __init__(self, rotor_names, rings_str, pos_str):
        self.r = [ROTORS[n]['wiring'] for n in rotor_names]
        self.n = [ROTORS[n]['notch'] for n in rotor_names]
        self.rings = [ALPHABET.index(c) for c in rings_str]
        self.p_init = [ALPHABET.index(c) for c in pos_str]
        self.p = self.p_init.copy()
        self.ref = REFLECTOR_B

    def step(self):
        step_r1 = ALPHABET[self.p[1]] in self.n[1]
        step_r2 = ALPHABET[self.p[2]] in self.n[2]
        step_r3 = ALPHABET[self.p[3]] in self.n[3]
        step_r4 = ALPHABET[self.p[4]] in self.n[4]
        
        if step_r1:
            self.p[0] = (self.p[0] + 1) % 26
            self.p[1] = (self.p[1] + 1) % 26
        elif step_r2:
            self.p[1] = (self.p[1] + 1) % 26
            self.p[2] = (self.p[2] + 1) % 26
        elif step_r3:
            self.p[2] = (self.p[2] + 1) % 26
            self.p[3] = (self.p[3] + 1) % 26
        elif step_r4:
            self.p[3] = (self.p[3] + 1) % 26
        self.p[4] = (self.p[4] + 1) % 26

def get_core_perms(rotors, rings, pos, length):
    enigma = FastEnigmaCore(rotors, rings, pos)
    perms = []
    for _ in range(length):
        enigma.step()
        perm = {}
        for c in ALPHABET:
            c_idx = ALPHABET.index(c)
            for i in range(4, -1, -1):
                shift = enigma.p[i] - enigma.rings[i]
                c_idx = (ALPHABET.index(enigma.r[i][(c_idx + shift) % 26]) - shift) % 26
            c_idx = ALPHABET.index(enigma.ref[c_idx])
            for i in range(5):
                shift = enigma.p[i] - enigma.rings[i]
                c_idx = (enigma.r[i].index(ALPHABET[(c_idx + shift) % 26]) - shift) % 26
            perm[c] = ALPHABET[c_idx]
        perms.append(perm)
    return perms

def calculate_ioc(text):
    if len(text) < 2: return 0.0
    counts = Counter(text)
    n = len(text)
    ioc = sum(c * (c - 1) for c in counts.values()) / (n * (n - 1))
    return ioc

def find_valid_crib_offsets(ciphertext, crib):
    offsets = []
    for i in range(len(ciphertext) - len(crib) + 1):
        valid = True
        for j in range(len(crib)):
            if ciphertext[i+j] == crib[j]:
                valid = False
                break
        if valid:
            offsets.append(i)
    return offsets

def solve_graph_helper(S, remaining_eqs, crib, ciphertext, perms):
    if not remaining_eqs: return S
    best_eq, best_score = None, -1
    
    for eq in remaining_eqs:
        p, c = crib[eq], ciphertext[eq]
        score = (1 if p in S else 0) + (1 if c in S else 0)
        if score > best_score:
            best_score, best_eq = score, eq
            if score == 2: break
            
    p, c, idx = crib[best_eq], ciphertext[best_eq], best_eq
    next_remaining = [e for e in remaining_eqs if e != best_eq]
    
    if best_score == 2:
        if perms[idx][S[p]] == S[c]: return solve_graph_helper(S, next_remaining, crib, ciphertext, perms)
        return None
    elif best_score == 1:
        known, unknown = (p, c) if p in S else (c, p)
        target = perms[idx][S[known]] if p in S else perms[idx][S[c]]
        if not p in S:
            target = perms[idx][S[c]]
            
        if target in S and S[target] != unknown: return None
        S_new = S.copy()
        S_new[unknown] = target
        S_new[target] = unknown
        return solve_graph_helper(S_new, next_remaining, crib, ciphertext, perms)
    else:
        for guess_Sp in ALPHABET:
            if guess_Sp in S: continue
            target_Sc = perms[idx][guess_Sp]
            if target_Sc in S: continue
            if p == c and guess_Sp != target_Sc: continue
            if (guess_Sp == c and target_Sc != p) or (target_Sc == p and guess_Sp != c): continue
            
            S_new = S.copy()
            S_new[p], S_new[guess_Sp] = guess_Sp, p
            S_new[c], S_new[target_Sc] = target_Sc, c
            res = solve_graph_helper(S_new, next_remaining, crib, ciphertext, perms)
            if res is not None: return res
        return None

def mp_solve_turing_menu(args):
    crib, ct_slice, perms_slice, offset = args
    equations = list(range(len(crib)))
    res = solve_graph_helper({}, equations, crib, ct_slice, perms_slice)
    return offset, res

def apply_decryption(ciphertext, S_dict, rotors, rings, pos):
    enigma = FastEnigmaCore(rotors, rings, pos)
    out = []
    for char in ciphertext:
        enigma.step()
        c_idx = ALPHABET.index(S_dict.get(char, char))
        for i in range(4, -1, -1):
            shift = enigma.p[i] - enigma.rings[i]
            c_idx = (ALPHABET.index(enigma.r[i][(c_idx + shift) % 26]) - shift) % 26
        c_idx = ALPHABET.index(enigma.ref[c_idx])
        for i in range(5):
            shift = enigma.p[i] - enigma.rings[i]
            c_idx = (enigma.r[i].index(ALPHABET[(c_idx + shift) % 26]) - shift) % 26
        out.append(S_dict.get(ALPHABET[c_idx], ALPHABET[c_idx]))
    return ''.join(out)

def fuzzy_language_score(text):
    score = 0
    text_len = len(text)
    
    for word in WEHRMACHT_DICT:
        wl = len(word)
        if wl < 5:
            score += text.count(word) * wl * 1000
            continue
            
        for i in range(text_len - wl + 1):
            window = text[i:i+wl]
            if window == word:
                score += wl * 1000
            else:
                mismatches = 0
                for a, b in zip(word, window):
                    if a != b:
                        mismatches += 1
                        if mismatches > 1: break
                if mismatches == 1:
                    score += wl * 500
                    
    score += sum(GERMAN_SCORES.get(text[i:i+2], 0) for i in range(len(text)-1))
    score += sum(GERMAN_SCORES.get(text[i:i+3], 0) for i in range(len(text)-2))
    return score


def simulated_annealing_steckerbrett(ciphertext, locked_S, rotors, rings, pos):
    print("\n    [!] PHASE 2: Initiating Thermodynamic AI (Simulated Annealing)...")
    
    GREEN = '\033[92m'
    CYAN = '\033[96m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    RESET = '\033[0m'

    current_S = locked_S.copy()
    current_text = apply_decryption(ciphertext, current_S, rotors, rings, pos)
    current_score = fuzzy_language_score(current_text)
    
    best_S = current_S.copy()
    best_score = current_score
    score_history = [current_score]
    
    START_TEMP = 10000.0
    temperature = START_TEMP
    cooling_rate = 0.92
    iterations_per_temp = 15
    
    unplugged = [c for c in ALPHABET if current_S.get(c, c) == c]
    if len(unplugged) < 2:
        return best_S, score_history
        
    print(f"    [*] Matrix Initialized. Scanning configurations...\n")
    
    chunk_size = 75
    chunks_count = math.ceil(len(ciphertext) / chunk_size)
    total_lines_to_clear = chunks_count + 2
    
    print("\n" * total_lines_to_clear)

    print(f"    [*] Matrix Initialized. Scanning configurations...")
    print(f"    {YELLOW}[!] WARNING: Matrix decryption slows down as it stabilizes.{RESET}")
    print(f"    {YELLOW}[!] PLEASE WAIT. DO NOT INTERRUPT UNTIL T REACHES 0000.0K!{RESET}\n")
    
    chunk_size = 75

    
    while temperature > 0.1:
        for _ in range(iterations_per_temp):
            a, b = random.sample(unplugged, 2)
            neighbor_S = current_S.copy()
            
            if neighbor_S.get(a, a) == a and neighbor_S.get(b, b) == b:
                neighbor_S[a], neighbor_S[b] = b, a
            else:
                target_a = neighbor_S.get(a, a)
                target_b = neighbor_S.get(b, b)
                
                neighbor_S.pop(target_a, None)
                neighbor_S.pop(a, None)
                neighbor_S.pop(target_b, None)
                neighbor_S.pop(b, None)
                
                neighbor_S[a] = b
                neighbor_S[b] = a
            
            neighbor_text = apply_decryption(ciphertext, neighbor_S, rotors, rings, pos)
            neighbor_score = fuzzy_language_score(neighbor_text)
            
            delta = neighbor_score - current_score
            if delta > 0 or math.exp(delta / temperature) > random.random():
                current_S = neighbor_S
                current_score = neighbor_score
                current_text = neighbor_text
                
                if current_score > best_score:
                    best_score = current_score
                    best_S = current_S.copy()
                    score_history.append(best_score)
                    
        progress = 1.0 - (temperature / START_TEMP)
        progress = max(0.0, min(1.0, progress))
        
        bar_len = 15
        filled = int(progress * bar_len)
        bar = '█' * filled + '░' * (bar_len - filled)
        
        reveal_count = int(progress * len(ciphertext))
        
        sys.stdout.write(f"\033[{total_lines_to_clear}A")
        sys.stdout.write(f"\r    [AI] [{bar}] {int(progress*100):>3}% | T:{temperature:06.1f}K | {YELLOW}DON'T INTERRUPT UNTIL T=0!{RESET} | BEST: {best_score}\033[K\n\n")
        
        for i in range(0, len(ciphertext), chunk_size):
            colored_chunk = ""
            for j in range(i, min(i + chunk_size, len(ciphertext))):
                if j < reveal_count:
                    color = GREEN if (j // 5) % 2 == 0 else CYAN
                    colored_chunk += f"{color}{current_text[j]}{RESET}"
                else:
                    colored_chunk += f"{RED}{ciphertext[j]}{RESET}"
            
            sys.stdout.write(f"    {colored_chunk}\033[K\n")
            
        sys.stdout.flush()
        time.sleep(0.025)
        
        temperature *= cooling_rate

    best_text = apply_decryption(ciphertext, best_S, rotors, rings, pos)
    
    sys.stdout.write(f"\033[{total_lines_to_clear}A")
    sys.stdout.write(f"\r    [AI] [{'█' * 15}] 100% | T:0000.0K | FINAL BEST SCORE: {best_score}\033[K\n\n")
    
    for i in range(0, len(best_text), chunk_size):
        colored_chunk = ""
        for j in range(i, min(i + chunk_size, len(best_text))):
            color = GREEN if (j // 5) % 2 == 0 else CYAN
            colored_chunk += f"{color}{best_text[j]}{RESET}"
        sys.stdout.write(f"    {colored_chunk}\033[K\n")
        
    sys.stdout.flush()

    print(f"\n    [!] Thermodynamic matrix stabilized. Final optimum score: {best_score}")
    return best_S, score_history


# + --------------------------------------------------------- +
# | NLP VITERBI WORD SEGMENTER (PHASE 4)                      |
# + --------------------------------------------------------- +

def nlp_word_segmentation(text):
    text = text.replace(" ", "")
    n = len(text)
    
    dp = [0.0] * (n + 1)
    back = [0] * (n + 1)
    
    for i in range(1, n + 1):
        best_score = float('-inf')
        best_k = 0
        
        for k in range(max(0, i - 20), i):
            word = text[k:i]
            
            if word in VOCAB_SET:
                score = dp[k] + (len(word) ** 3) * 10
            else:
                score = dp[k] - (len(word) + 2)
                
            if score > best_score:
                best_score = score
                best_k = k
                
        dp[i] = best_score
        back[i] = best_k
        
    words = []
    curr = n
    while curr > 0:
        prev = back[curr]
        words.append(text[prev:curr])
        curr = prev
        
    return " ".join(words[::-1])

# + --------------------------------------------------------- +
# | THE MI6 CLASSIFIED REPORT GENERATOR (3-PAGE DOSSIER)      |
# + --------------------------------------------------------- +

def generate_pdf_report(score_history, final_text, human_text, ciphertext, rotors, rings, pos, steckerbrett, offset):
    print("\n[*] GENERATING 'ULTRA' CLASSIFIED 4-PAGE DOSSIER...")
    
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
    pdf_filename = "Hut6_ULTRA_Report_v16_CLASSIFIED.pdf"
    
    footer_text = "(c) by George W. Aravidis | Email : csec.unbutton326@passinbox.com | Historical Reference"
    
    def add_watermark(fig):
        fig.text(0.5, 0.5, 'T O P   S E C R E T   -   U L T R A',
                 fontsize=65, color='#FF0000', alpha=0.08, 
                 ha='center', va='center', rotation=45, fontweight='bold')
                 
    with PdfPages(pdf_filename) as pdf:
        
        fig1 = plt.figure(figsize=(11, 14))
        fig1.patch.set_facecolor('#050505')
        fig1.suptitle('T O P   S E C R E T   -   U L T R A\nHUT 6 DECRYPTION INTELLIGENCE DOSSIER',
                     fontsize=22, fontweight='bold', color='#FF3333', y=0.96)
        
        add_watermark(fig1)

        plugs = []
        seen = set()
        for k, v in steckerbrett.items():
            if k != v and k not in seen:
                plugs.append(f"{k}{v}")
                seen.add(k)
                seen.add(v)
        stecker_str = ' '.join(plugs) if plugs else 'NONE'

        settings = (
            f"[ TARGET MACHINE CONFIGURATION ]\n"
            f"Walzenlage (Rotors)   : {' - '.join(rotors)}\n"
            f"Ringstellung (Rings)  : {rings}\n"
            f"Grundstellung (Pos)   : {pos}\n"
            f"Steckerbrett (Plugs)  : {stecker_str}"
        )
        
        intercept_metadata = (
            f"[ INTERCEPT METADATA ]\n"
            f"Station       : Y-Station Beaumanor (Leicestershire, UK)\n"
            f"Frequency     : 4155 kHz (Kriegsmarine Shortwave Band)\n"
            f"Signal Quality: Grade 3 (Intermittent QRM / Atmospheric Static)\n"
            f"Operator ID   : 74-B (Section V Special Intercept Team)"
        )

        metrics = (
            f"[ CRYPTANALYSIS METRICS ]\n"
            f"Crib Dragging Offset  : {offset}\n"
            f"Index of Coincidence  : {ioc:.4f} ({ioc_status})\n"
            f"Final AI Fuzzy Score  : {score_history[-1]}"
        )

        wrap_c = '\n'.join(textwrap.wrap(ciphertext, width=95))
        wrap_h = '\n'.join(textwrap.wrap(human_text, width=95))

        security_warning = (
            f"!!! WARNING: TOP SECRET ULTRA !!!\n"
            f"This document contains signals intelligence derived from high-grade Axis\n"
            f"cryptography. Under the Official Secrets Act, any unauthorized disclosure\n"
            f"is punishable by treason charges. Destroy by fire immediately after analysis."
        )

        content_p1 = f"{settings}\n\n{intercept_metadata}\n\n{metrics}\n\n[ RAW INTERCEPTED CIPHERTEXT ]\n{wrap_c}\n\n[ NLP RECONSTRUCTED PLAINTEXT ]\n{wrap_h}\n\n{security_warning}"
        fig1.text(0.06, 0.90, content_p1, fontsize=10, color='#00FF00', family='monospace', va='top')
        fig1.text(0.95, 0.02, footer_text, ha='right', fontsize=9, color='#008800', family='monospace')
        
        pdf.savefig(fig1, facecolor=fig1.get_facecolor(), edgecolor='none', dpi=300)
        plt.close(fig1)

        fig2 = plt.figure(figsize=(11, 14))
        fig2.patch.set_facecolor('#050505')
        fig2.suptitle('T O P   S E C R E T   -   U L T R A\nHUT 6 CRYPTANALYSIS TELEMETRY',
                     fontsize=22, fontweight='bold', color='#FF3333', y=0.96)
        
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
        ax2.legend(facecolor='#0a0a0a', edgecolor='#00FF00', labelcolor='linecolor')
        ax2.set_facecolor('#0a0a0a')

        fig2.text(0.95, 0.02, footer_text, ha='right', fontsize=9, color='#008800', family='monospace')
        pdf.savefig(fig2, facecolor=fig2.get_facecolor(), edgecolor='none', dpi=300)
        plt.close(fig2)
        
        fig3 = plt.figure(figsize=(11, 14))
        fig3.patch.set_facecolor('#050505')
        fig3.suptitle('T O P   S E C R E T   -   U L T R A\nHUT 6 DECLASSIFIED HISTORICAL ARCHIVE',
                     fontsize=22, fontweight='bold', color='#FF3333', y=0.96)
                     
        add_watermark(fig3)

        tribute = (
            "+ --------------------------------------------------------------------------------- +\n"
            "| BLETCHLEY PARK MEMORIAL ARCHIVE                                                   |\n"
            "| This decryption engine is dedicated to Alan Turing, Gordon Welchman, Joan Clarke, |\n"
            "| and the thousands of brilliant minds at Bletchley Park. Their relentless pursuit  |\n"
            "| of cracking the Enigma cipher shortened WWII and laid the bedrock for modern      |\n"
            "| computer science. They broke the unbreakable.                                     |\n"
            "+ --------------------------------------------------------------------------------- +"
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
        
        fig4 = plt.figure(figsize=(11, 14))
        fig4.patch.set_facecolor('#050505')
        fig4.suptitle('T O P   S E C R E T   -   U L T R A\nTACTICAL WIRING BLUEPRINT (STECKERBRETT)',fontsize=22, fontweight='bold', color='#FF3333', y=0.96)
        
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
                
        for char, (x, y) in coords.items():
            is_plugged = steckerbrett.get(char, char) != char
            node_color = '#FF0000' if is_plugged else '#333333'
            
            ax4.plot(x, y, 'o', color=node_color, markersize=24, zorder=2)
            ax4.text(x, y, char, color='white', fontsize=12, ha='center', va='center', fontweight='bold', zorder=3)
            
        plt.subplots_adjust(top=0.82, bottom=0.05)
        
        fig4.text(0.5, 0.88, f"PLUGBOARD MAPPING ({len(drawn_connections)} ACTIVE CONNECTIONS)",
        color='#00FF00', fontsize=14, ha='center', va='top', family='monospace', fontweight='bold')

        
        fig4.text(0.95, 0.02, footer_text, ha='right', fontsize=9, color='#008800', family='monospace')
        pdf.savefig(fig4, facecolor=fig4.get_facecolor(), edgecolor='none', dpi=300)
        plt.close(fig4)

    print(f"\n[+] SUCCESS: Classified 4-Page Report typed up and sealed as '{pdf_filename}'")


# + --------------------------------------------------------- +
# | TERMINAL I/O                                              |
# + --------------------------------------------------------- +

def get_clean_text(prompt, allow_empty=False):
    while True:
        val = input(prompt).upper().replace(" ", "").strip()
        if not val and allow_empty: return ""
        if not val:
            print("[-] VERBOTEN! Input cannot be empty.")
            continue
        if not val.isalpha():
            print("[-] ACHTUNG! Only letters are allowed. No numbers, punctuation, or symbols.")
            continue
        return val

def get_5_letters(prompt):
    while True:
        val = get_clean_text(prompt)
        if len(val) == 5: return val
        print("[-] NEIN! Must be exactly 5 letters (e.g. ABCDE).")

def get_rotors():
    available = ['I', 'II', 'III', 'IV', 'V', 'VI', 'VII', 'VIII']
    selected = []
    print("\n+ " + "-"*57 + " +")
    print("| WALZENORDNUNG (ROTOR CONFIGURATION)                     |")
    print("+ " + "-"*57 + " +")
    for i in range(1, 6):
        while True:
            r = input(f"[*] Rotor {i} (Available: {', '.join(available)}): ").upper().strip()
            if r in available:
                selected.append(r)
                available.remove(r)
                break
            else:
                print(f"[-] ERROR: Invalid rotor '{r}' or already used. Try again.")
    return selected

def interactive_override(ciphertext, current_S, rotors, rings, pos):
    print("\n+ " + "-"*57 + " +")
    print("| PHASE 3: HUT 6 OVERRIDE TERMINAL                        |")
    print("+ " + "-"*57 + " +")
    
    while True:
        final_text = apply_decryption(ciphertext, current_S, rotors, rings, pos)
        formatted = ' '.join(final_text[i:i+5] for i in range(0, len(final_text), 5))
        
        plugs = []
        seen = set()
        for k, v in current_S.items():
            if k != v and k not in seen:
                plugs.append(f"{k}{v}")
                seen.add(k)
                seen.add(v)
                
        print(f"\n[>] CURRENT PLUGBOARD: {' '.join(plugs) if plugs else 'NONE'}")
        print(f"[>] CURRENT DECRYPT:\n    {formatted}\n")
        
        action = get_clean_text("[?] Are there typos? Enter 2 letters to swap (e.g. HJ), or type 'PERFEKT' to finish:\n> ")
        
        if action == "PERFEKT":
            return final_text, current_S
            
        elif len(action) == 2:
            a, b = action[0], action[1]
            if a == b:
                print("[-] Fool! You cannot plug a letter into itself.")
                continue
            
            for char in (a, b):
                if char in current_S and current_S[char] != char:
                    old_target = current_S[char]
                    del current_S[old_target]
                    del current_S[char]
            
            current_S[a], current_S[b] = b, a
            print(f"[+] Wire {a}-{b} manually patched.")
        else:
            print("[-] Invalid input. Enter exactly 2 letters, or 'PERFEKT'.")

# + --------------------------------------------------------- +
# | MAIN ENGINE                                               |
# + --------------------------------------------------------- +

def run_ultimate_cracker():
    print(r"""
 + --------------------------------------------------------- +
 |                                                           |
 |  ██████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗          |
 |  ██╔═══╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗         |
 |  █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║        |
 |  ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║        |
 |  ██████╗ ██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║        |
 |  ╚═════╝ ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝        |
 |                                                           |
 |  [ THE ULTIMATE BLETCHLEY PARK ENGINE v16.0]              |
 |  [ NLP LINGUISTIC RECONSTRUCTION ENABLED   ]              |
 |                                                           |
 |  (c) George W. Aravidis                                   |
 |  Email address # csec.unbutton326@passinbox.com           |
 + --------------------------------------------------------- +
    """)
    print("[!] ERROR HANDLING: MAXIMUM. NO TOLERANCE FOR MISTAKES.\n")
    
    rotors = get_rotors()
    rings = get_5_letters("\n[*] ENTER CAPTURED RINGSTELLUNG (e.g. FDWAB): ")
    pos = get_5_letters("[*] ENTER CAPTURED GRUNDSTELLUNG (e.g. FLPDF): ")
    
    ciphertext = get_clean_text("\n[?] ENTER INTERCEPTED CIPHERTEXT:\n> ")
    
    while True:
        crib = get_clean_text("\n[?] ENTER KNOWN PLAINTEXT (CRIB):\n> ")
        if len(crib) > len(ciphertext):
            print("[-] VERBOTEN! Crib is longer than the Ciphertext. Try again.")
        else:
            break
            
    print("\n+ " + "-"*57 + " +")
    print("| PHASE 1: INITIATING TRUE TURING BOMBE ENGINE            |")
    print("+ " + "-"*57 + " +")
    start_time = time.time()
    
    offsets = find_valid_crib_offsets(ciphertext, crib)
    if not offsets:
        print("[-] FATAL: Crib crashes at every possible offset! (Check your crib for errors)")
        sys.exit(1)
        
    print(f"    [+] Crib Dragging: Found {len(offsets)} possible valid offsets.")
    print(f"    [*] Spawning worker threads for distributed Bombe attack...\n")
    
    perms = get_core_perms(rotors, rings, pos, len(ciphertext))
    tasks = []
    for offset in offsets:
        tasks.append((crib, ciphertext[offset:offset+len(crib)], perms[offset:offset+len(crib)], offset))
        
    valid_bombe_results = []
    
    total_tasks = len(tasks)
    completed_tasks = 0
    with concurrent.futures.ProcessPoolExecutor() as executor:
        futures = {executor.submit(mp_solve_turing_menu, task): task for task in tasks}
        for future in concurrent.futures.as_completed(futures):
            completed_tasks += 1
            task_info = futures[future]
            current_offset = task_info[3]
            
            percent = completed_tasks / total_tasks
            bar_len = 15
            filled = int(percent * bar_len)
            bar = '█' * filled + '░' * (bar_len - filled)
            
            crib_preview = crib[:5]
            ct_preview = ciphertext[current_offset:current_offset+5]
            
            output_str = f"\r    [BOMBE] [{bar}] {int(percent*100):>3}% | OFS: {current_offset:03d} | MATCH: {crib_preview}->{ct_preview:<5}"
            sys.stdout.write(output_str.ljust(95))
            sys.stdout.flush()
            
            result = future.result()
            if result[1] is not None:
                valid_bombe_results.append(result)
                
    print() 
                
    if not valid_bombe_results:
        print("\n[-] CRITICAL FAILURE IN BOMBE MATRIX!")
        print("[-] Contradiction detected. The Crib does not match this Ciphertext with these settings.")
        sys.exit(1)
        
    best_offset, bombe_S = valid_bombe_results[0]
    print(f"    [+] Bombe deduced initial Steckerbrett at offset {best_offset} in {time.time() - start_time:.4f} seconds.")
    
    completed_S, score_history = simulated_annealing_steckerbrett(ciphertext, bombe_S, rotors, rings, pos)
    
    final_decrypted_text, final_plugboard = interactive_override(ciphertext, completed_S, rotors, rings, pos)
    
    # ---------------------------------------------------------
    # PHASE 4: LINGUISTIC RECONSTRUCTION
    # ---------------------------------------------------------
    print("\n+ " + "-"*57 + " +")
    print("| PHASE 4: NLP LINGUISTIC RECONSTRUCTION                  |")
    print("+ " + "-"*57 + " +")
    print("    [*] Parsing Viterbi string graph...")
    
    human_readable_text = nlp_word_segmentation(final_decrypted_text)
    
    print("\n[>] RECONSTRUCTED TRANSMISSION:")
    
    formatted_output = textwrap.fill(human_readable_text, width=80)
    for line in formatted_output.split('\n'):
        print(f"    {line}")
        time.sleep(0.05)
        
    print("\n[+] MESSAGE SECURED. PRINTING FOR CHURCHILL...")
    
    if final_decrypted_text and score_history:
        generate_pdf_report(score_history, final_decrypted_text, human_readable_text, ciphertext, rotors, rings, pos, final_plugboard, best_offset)

if __name__ == "__main__":
    try:
        run_ultimate_cracker()
    except KeyboardInterrupt:
        print("\n\n[-] EMERGENCY PROTOCOL. WIPING TAPES...")
        sys.exit(0)
