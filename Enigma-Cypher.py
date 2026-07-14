import sys
import random

# Kriegsmarine M3/M4/Franken Enigma Walzenspezifikationen
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

class Rotor:
    def __init__(self, name, wiring, notch, ring='A', pos='A'):
        self.name = name
        self.wiring = wiring
        self.notch = notch
        self.ring = ALPHABET.index(ring)
        self.pos = ALPHABET.index(pos)

    def step(self):
        self.pos = (self.pos + 1) % 26

    def is_at_notch(self):
        return ALPHABET[self.pos] in self.notch

    def forward(self, c):
        shift = self.pos - self.ring
        idx = (c + shift) % 26
        char = self.wiring[idx]
        return (ALPHABET.index(char) - shift) % 26

    def backward(self, c):
        shift = self.pos - self.ring
        idx = (c + shift) % 26
        char = ALPHABET[idx]
        out_idx = self.wiring.index(char)
        return (out_idx - shift) % 26

class EnigmaMachine:
    def __init__(self, rotor_names, rings, positions, plugboard_pairs):
        self.rotors = [
            Rotor(name, ROTORS[name]['wiring'], ROTORS[name]['notch'], ring, pos)
            for name, ring, pos in zip(rotor_names, rings, positions)
        ]
        self.reflector = REFLECTOR_B
        self.plugboard = self.setup_plugboard(plugboard_pairs)

    def setup_plugboard(self, pairs):
        pb = {c: c for c in ALPHABET}
        for pair in pairs:
            if len(pair) == 2:
                a, b = pair[0], pair[1]
                pb[a], pb[b] = b, a
        return pb

    def step_rotors(self):
        r1, r2, r3, r4, r5 = self.rotors
        
        step_r4 = r5.is_at_notch()
        step_r3 = r4.is_at_notch()
        step_r2 = r3.is_at_notch()
        step_r1 = r2.is_at_notch()
        
        if step_r1:
            r1.step()
            r2.step()
        elif step_r2:
            r2.step()
            r3.step()
        elif step_r3:
            r3.step()
            r4.step()
        elif step_r4:
            r4.step()
            
        r5.step()

    def process_char(self, char):
        if char not in ALPHABET: return char
        self.step_rotors()
        
        c = self.plugboard[char]
        c_idx = ALPHABET.index(c)
        
        for rotor in reversed(self.rotors):
            c_idx = rotor.forward(c_idx)
            
        c_idx = ALPHABET.index(self.reflector[c_idx])
        
        for rotor in self.rotors:
            c_idx = rotor.backward(c_idx)
            
        final_char = ALPHABET[c_idx]
        return self.plugboard[final_char]

    def process_text(self, text):
        return ''.join(self.process_char(c) for c in text.replace(' ', '').upper())

def get_rotors():
    available_rotors = ['I', 'II', 'III', 'IV', 'V', 'VI', 'VII', 'VIII']
    selected = []
    
    print("\n[!] WALZENORDNUNG (ROTOR-KONFIGURATION)")
    print("    Setzen Sie 5 Walzen ein. (Von links nach rechts).")
    
    for i in range(1, 6):
        while True:
            r = input(f"[*] Walze {i} einsetzen (Verfügbar: {', '.join(available_rotors)}): ").upper()
            if r in available_rotors:
                selected.append(r)
                available_rotors.remove(r)
                break
            elif r in selected:
                print(f"[-] ACHTUNG! Walze '{r}' ist bereits in der Maschine verriegelt!")
            else:
                print("[-] VERBOTEN! Ungültige Walzenkennung. Überprüfen Sie Ihre Ausrüstung!")
    return selected

def get_5_letters(prompt_text, default='AAAAA'):
    while True:
        val = input(prompt_text).upper() or default
        if len(val) == 5 and val.isalpha():
            return val
        print("[-] NEIN! Befehl verweigert! Exakt 5 Buchstaben erforderlich (z.B. ABCDE).")

def generate_random_steckerbrett():
    letters = list(ALPHABET)
    random.shuffle(letters)
    # 12 pairs = 24 letters. 2 letters remain unplugged.
    pairs = [letters[i] + letters[i+1] for i in range(0, 24, 2)]
    unplugged = letters[24:]
    return pairs, unplugged

def steckerbrett_routine():
    print("\n[!] STECKERBRETT VERKABELUNG")
    print("    [BLETCHLEY PARK INTERCEPT]: 'Sir! The German operator is trying to leave ONE letter unplugged!'")
    print("    [ALAN TURING]: '26 is an even number, you bloody fool! They have to leave TWO alone! Let them play...'")
    print("    [END OF INTERCEPT]\n")
    
    while True:
        pairs, unplugged = generate_random_steckerbrett()
        pairs_str = " ".join(pairs)
        unplugged_str = ", ".join(unplugged)
        
        print(f"[*] VORGESCHLAGENE VERKABELUNG (Zufall): {pairs_str}")
        print(f"[*] UNVERBUNDEN (Freibleibend): {unplugged_str}")
        
        choice = input("\n[?] Akzeptieren? (JA / ENTER für neu / MV für 'Meine Verkabelung'): ").upper()
        
        if choice == 'JA' or choice == 'YES':
            print("[+] STECKERBRETT KONFIGURIERT UND VERRIEGELT!")
            return pairs
        elif choice == 'MV':
            while True:
                print("\n[!] MANUELLE EINGABE (Meine Verkabelung)")
                manual_input = input("[*] Geben Sie Ihre Paare ein (z.B. AB CD EF, oder ENTER für leer): ").upper().split()
                if not manual_input:
                    print("[+] KEINE VERKABELUNG. STECKERBRETT LEER UND VERRIEGELT!")
                    return []
                
                valid = True
                seen = set()
                for pair in manual_input:
                    if len(pair) != 2 or not pair.isalpha():
                        print(f"[-] HALT! Ungültige Verbindung '{pair}'. Exakt 2 Buchstaben erforderlich!")
                        valid = False
                        break
                    if pair[0] == pair[1]:
                        print(f"[-] VERBOTEN! Ein Buchstabe kann nicht mit sich selbst verbunden werden ('{pair}').")
                        valid = False
                        break
                    if pair[0] in seen or pair[1] in seen:
                        print(f"[-] ACHTUNG! Kurzschluss in '{pair}'. Jeder Buchstabe darf nur einmal verwendet werden!")
                        valid = False
                        break
                    seen.add(pair[0])
                    seen.add(pair[1])
                
                if valid:
                    print("[+] MANUELLES STECKERBRETT KONFIGURIERT UND VERRIEGELT!")
                    return manual_input

def run_emulator():
    print(r"""
     ██████╗███╗   ██╗██╗ ██████╗ ███╗   ███╗ █████╗  
     ██╔═══╝████╗  ██║██║██╔════╝ ████╗ ████║██╔══██╗ 
     █████╗  ██╔██╗ ██║██║██║  ███╗██╔████╔██║███████║
     ██╔══╝  ██║╚██╗██║██║██║   ██║██║╚██╔╝██║██╔══██║
     ██████╗ ██║ ╚████║██║╚██████╔╝██║ ╚═╝ ██║██║  ██║
      ╚═════╝ ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝
    >> Enigma Machine Ultimate v8.0 | Bletchley Park Edition
    >> (c) George W. Aravidis
    >> Email address # csec.unbutton326@passinbox.com
    """)
    print("[!] GEHEIME KOMMUNIKATIONSVERBINDUNG WIRD HERGESTELLT...")
    print("[!] STROMVERSORGUNG DER WALZEN AKTIVIERT...\n")
    
    r_input = get_rotors()
    
    print("\n[!] RINGSTELLUNG")
    ring_input = get_5_letters("[*] Ringstellung eingeben (Standard AAAAA): ")
    
    print("\n[!] GRUNDSTELLUNG")
    pos_input = get_5_letters("[*] Grundstellung eingeben (Standard AAAAA): ")
    
    pb_input = steckerbrett_routine()
    
    enigma = EnigmaMachine(r_input, ring_input, pos_input, pb_input)
    
    print("\n=======================================================")
    print(f"[+] WALZEN: {r_input}")
    print(f"[+] RINGSTELLUNG: {ring_input} | GRUNDSTELLUNG: {pos_input}")
    print(f"[+] STECKERBRETT: {' '.join(pb_input) if pb_input else 'LEER'}")
    print("[+] MASCHINE BEREIT. WARTEN AUF KLARTEXT.")
    print("[+] (Schreiben Sie 'EXIT' um das Programm zu beenden)")
    print("=======================================================")
    
    while True:
        msg = input("\n[KLARTEXT EINGABE] > ").upper()
        if msg == 'EXIT': 
            print("\n[-] VERBINDUNG GETRENNT. MASCHINE WIRD ZERSTÖRT...")
            print("    [BLETCHLEY PARK INTERCEPT]: 'Bloody hell! The Jerry operator logged off. Time for tea, lads.'")
            break
        
        cipher = enigma.process_text(msg)
        formatted = ' '.join(cipher[i:i+5] for i in range(0, len(cipher), 5))
        print(f"[GEHEIMTEXT AUSGABE] > {formatted}")

if __name__ == "__main__":
    try:
        run_emulator()
    except KeyboardInterrupt:
        print("\n\n[-] NOTABSCHALTUNG! CODEBÜCHER VERBRENNEN...")
        sys.exit(0)
