# Project ULTRA: Historical Enigma Machine and Bombe-Inspired Cryptanalysis  
### Computational Archaeology  
![License](https://img.shields.io/badge/License-MIT-blue.svg)
![Python Version](https://img.shields.io/badge/Python-3.10%2B-blueviolet)
![Security Level](https://img.shields.io/badge/Security_Level-1940s_Hardware-red.svg)
![Enigma](https://img.shields.io/badge/Status-Cracked_by_Turing-success.svg)  

> A native C++ reconstruction of a three-rotor Enigma cryptanalytic workflow, combining historically motivated crib analysis with modern parallel search, language scoring, evolutionary plugboard optimization, and automated report generation.  

> *"We can only see a short distance ahead, but we can see plenty there that needs to be done."*  
> — **Alan Turing**, The grandfather of computer science

**George W. Aravidis**  
Ulm-Söflingen, Germany · July 2026  
Contact: `csec.unbutton326@passinbox.com`

---

## Abstract

This repository contains an academic and historical cryptanalysis demonstrator for the German Enigma cipher machine. The program models a three-rotor Enigma configuration and attempts to recover plausible machine settings and plaintext from an intercepted ciphertext.

The implementation is written as a single native C++17 program and is designed primarily for **openSUSE Tumbleweed**. Its cryptanalytic pipeline combines:

- exhaustive rotor-order and starting-position enumeration;
- a Bombe-inspired electrical-constraint procedure;
- known-plaintext or **crib** alignment;
- multithreaded Ringstellung evaluation;
- dictionary-derived bigram and quadgram scoring;
- Index of Coincidence filtering;
- mutation-based evolutionary optimization of the Steckerbrett;
- Viterbi-style German word segmentation;
- optional generation of a simulated historical-style ULTRA dossier in PDF format.

The project is intended for the study of classical cryptography, computational history, heuristic optimization, and parallel search. It is **not** suitable for protecting modern information.

---

## Historical and Technical Scope

The software represents a **three-rotor Enigma model** using three rotors selected from the historical rotor set I–V and a fixed Reflector B. It implements rotor stepping, including the middle-rotor double-stepping behavior represented by the program's stepping logic.

The cryptanalytic procedure is **Bombe-inspired**. It is not a cycle-accurate simulation or engineering reproduction of the electromechanical Bombes constructed at Bletchley Park. Instead, it translates selected historical principles into a modern CPU-oriented constraint-search architecture.

---

## Supported Features

### Enigma Core

- Rotors **I, II, III, IV, and V**.
- Automatic enumeration of all **60 ordered three-rotor selections**.
- Fixed **Reflector B**.
- Three-letter Grundstellung representation.
- Three-letter Ringstellung representation.
- Forward and reverse rotor traversal.
- Rotor turnover and double-stepping logic.
- Symmetric Steckerbrett letter-pair substitution.
- Input normalization to uppercase alphabetic text.

### Phase 1: Bombe-Inspired Constraint Search

- Automatic testing of historically motivated crib hypotheses at offset `0`.
- Manual crib input when predefined hypotheses do not produce a candidate.
- Automatic scanning of every admissible crib offset for a manually supplied crib.
- Rejection of alignments that violate the Enigma property that a letter cannot encrypt to itself.
- Electrical-consistency propagation across crib and ciphertext relationships.
- Parallel enumeration of:
  - `60` rotor orders;
  - `26` right-rotor ring settings;
  - `17,576` three-letter starting positions.
- Total first-stage core search space:

```text
60 × 26 × 17,576 = 27,418,560 core states
```

- Automatic use of the logical CPU concurrency reported by the operating system.
- Precomputed rotor mappings and thread-local working buffers.
- Candidate collection with an internal upper bound to prevent unbounded result growth.

### Phase 1.5: Ringstellung Search

- Multithreaded evaluation of the remaining left and middle Ringstellung components.
- Evaluation of `26 × 26 = 676` ring combinations for each surviving Phase 1 candidate.
- Adjustment of the corresponding starting positions.
- Language-based ranking of candidate decryptions.

### Phase 2: Evolutionary Steckerbrett Search

- Mutation-based evolutionary search.
- Elitist retention of high-scoring candidates.
- Default population size: `200`.
- Default generation count: `300`.
- Default mutation probability: `0.25`.
- Preservation of plugboard relationships inferred by the Bombe-inspired constraint phase.
- Live terminal display of progress, score evolution, and the current plaintext candidate.

### German-Language Scoring and Reconstruction

- Embedded German and historically oriented vocabulary.
- Normalization of German orthography:
  - `Ä → AE`
  - `Ö → OE`
  - `Ü → UE`
  - `ß → SS`
- Dictionary-derived bigram and quadgram scoring.
- Index of Coincidence filtering for implausible candidates.
- Viterbi-style dynamic-programming segmentation of continuous plaintext.
- Maximum candidate word span of 20 characters during segmentation.

### Manual Review

After the automated stages, the program presents the current plaintext and Steckerbrett candidate. The operator may:

- accept the result by pressing **Enter**;
- accept the result by entering `PERFEKT`;
- enter a two-letter pair such as `HJ` to replace a plugboard connection and immediately recompute the plaintext.

### PDF Intelligence Dossier

The program dynamically writes and executes a temporary Python script that generates:

```text
Hut6_Ultra_Report_Classified.pdf
```

With Matplotlib alone, the program generates a basic five-page report. When NumPy, NetworkX, and Seaborn are also available, the report expands to ten pages and may include:

- recovered Enigma settings;
- intercepted ciphertext and plaintext reconstruction;
- evolutionary score history;
- expected and observed German letter frequencies;
- historical commentary;
- Steckerbrett wiring visualization;
- crib–ciphertext relationship graph;
- bigram heatmap;
- Shannon entropy comparison;
- simulated traffic-analysis profile;
- simulated tactical projection.

> **Important:** The dossier is a modern, stylized simulation. Station names, operator profiles, frequencies, tactical diagrams, and similar narrative fields are generated for educational presentation and must not be treated as authenticated historical records.

---

## Repository Layout

The principal source file is:

```text
Enigma-BOMBE-Cryptanalysis-Attack.cpp
```

At runtime, report generation may create:

```text
temp_Enigma_Generate_Report.py
Hut6_Ultra_Report_Classified.pdf
```

The temporary Python script is intentionally left in the working directory and can be inspected for reproducibility or removed after the report has been generated.

---

## System Requirements

### Primary Platform

- openSUSE Tumbleweed
- 64-bit Linux environment recommended
- Terminal with ANSI escape-sequence support

### Compiler

- GCC/G++ with C++17 support
- POSIX thread support

### Optional Report Environment

- Python 3
- Matplotlib, required for PDF generation
- NumPy, optional advanced report dependency
- NetworkX, optional advanced report dependency
- Seaborn, optional advanced report dependency

### Hardware Considerations

The first cryptanalytic phase evaluates more than 27 million core states and uses all CPU threads reported by `std::thread::hardware_concurrency()`. Runtime depends heavily on:

- processor generation;
- number of logical CPU cores;
- ciphertext length;
- crib length;
- number of admissible crib offsets;
- number of surviving candidates;
- thermal and power limits.

A modern multicore processor is strongly recommended. Sustained high CPU utilization is expected during the exhaustive and Ringstellung search phases.

---

## Installation on openSUSE Tumbleweed

### 1. Refresh the Package Metadata

```bash
sudo zypper refresh
```

### 2. Install the Native Build Tools

```bash
sudo zypper install git gcc-c++
```

These packages are sufficient to compile and execute the C++ cryptanalytic engine. Python is only required for the optional PDF report.

### 3. Install the Python Environment for Report Generation

```bash
sudo zypper install python3 python3-virtualenv
```

Clone the repository and enter its directory:

```bash
git clone https://github.com/inbitwetrust/Enigma-Encryption-and-Cryptanalysis-WWII.git
cd Enigma-Encryption-and-Cryptanalysis-WWII
```

Create the virtual environment expected by the program:

```bash
virtualenv --python=python3 myenv
```

Alternatively, when the standard-library `venv` module is available:

```bash
python3 -m venv myenv
```

Activate the environment and install the report dependencies:

```bash
source myenv/bin/activate
python -m pip install --upgrade pip
python -m pip install matplotlib numpy networkx seaborn
deactivate
```

The C++ program automatically uses `myenv/bin/python3` when that interpreter exists. Otherwise, it falls back to the system command `python3`.

### Minimal PDF Installation

Only Matplotlib is required for the basic five-page report:

```bash
source myenv/bin/activate
python -m pip install matplotlib
deactivate
```

---

## Compilation

Run the compilation command from the repository directory.

### Recommended Portable Build

```bash
g++ -O3 -std=c++17 -pthread Enigma-BOMBE-Cryptanalysis-Attack.cpp -o Enigma-BOMBE-Cryptanalysis-Attack
```

### CPU-Specific Optimized Build

The following command permits GCC to use instructions available on the local processor:

```bash
g++ -O3 -march=native -std=c++17 -pthread Enigma-BOMBE-Cryptanalysis-Attack.cpp -o Enigma-BOMBE-Cryptanalysis-Attack
```

A binary compiled with `-march=native` should generally remain on the machine, or processor family, on which it was built. Use the portable build when distributing precompiled binaries.

### Development Build with Additional Diagnostics

```bash
g++ -O2 -g -std=c++17 -pthread -Wall -Wextra -Wpedantic \
    Enigma-BOMBE-Cryptanalysis-Attack.cpp -o Enigma-BOMBE-Cryptanalysis-Attack-debug
```

---

## Execution

Start the program from the repository directory:

```bash
./Enigma-BOMBE-Cryptanalysis-Attack
```

The current directory matters because the Python report script and the resulting PDF are written there.

---

## Input Format

The program requests an intercepted Enigma ciphertext:

```text
[?] ENTER INTERCEPTED CIPHERTEXT:
>
```

Input is normalized according to the following rules:

- lowercase letters are converted to uppercase;
- spaces are removed;
- digits are removed;
- punctuation is removed;
- only alphabetic characters are retained.

For predictable behavior, supply ciphertext and cribs using the Latin letters `A`–`Z`.

Example:

```text
GCDSE AHUGW TQGRK VLFGX UCALX VYMIG MMNMF DXTGN VHVRM
```

is normalized internally to:

```text
GCDSEAHUGWTQGRKVLFGXUCALXVYMIGMMNMFDXTGNVHVRM
```

For actual cryptanalysis, enter the intercepted ciphertext rather than a mixture of ciphertext and descriptive text.

---

## Cryptanalysis Workflow

### Step 1: Enter the Ciphertext

Paste the complete intercepted ciphertext at the prompt and press **Enter**.

### Step 2: Automatic Crib Hypotheses

The program first evaluates an embedded list of historically motivated expressions at offset `0`, including weather-report, command, routing, and message-format vocabulary.

These expressions are hypotheses only. Their inclusion does not establish that a particular intercepted message historically contained them.

### Step 3: Manual Crib, When Required

When the predefined hypotheses do not produce a surviving candidate, the program requests a known or suspected plaintext fragment:

```text
[?] ENTER KNOWN PLAINTEXT (CRIB):
>
```

A useful crib should:

- be shorter than or equal to the ciphertext;
- use only letters `A`–`Z`;
- represent a plausible fragment of the original German plaintext;
- preferably be long enough to produce meaningful constraint propagation;
- avoid guesswork presented as certainty.

For a manually supplied crib, the program automatically examines every offset that satisfies the no-self-encryption rule.

### Step 4: Bombe-Inspired Core-State Search

The program scans all supported rotor orders, right-rotor ring values, and starting positions. A terminal progress bar displays the number of processed states.

### Step 5: Ringstellung Ranking

Surviving candidates are expanded across the remaining ring settings and ranked through the German-language scoring engine.

### Step 6: Steckerbrett Evolution

The mutation-based evolutionary phase attempts to improve the plugboard configuration and plaintext score across 300 generations.

### Step 7: Manual Review

Review the recovered plaintext and plugboard candidate. Press **Enter**, type `PERFEKT`, or supply a replacement letter pair.

### Step 8: Plaintext Segmentation and Report

The final candidate is segmented into probable German words. When Python and Matplotlib are available, the program creates the PDF dossier automatically.

---

## Output

A successful run displays:

- the best-scoring rotor order;
- recovered or representative Grundstellung;
- recovered or representative Ringstellung;
- crib offset;
- inferred Steckerbrett pairs;
- raw decrypted text;
- Viterbi-segmented plaintext candidate;
- language score and progress telemetry.

The optional report is saved as:

```text
Hut6_Ultra_Report_Classified.pdf
```

---

## Verifying the Installation

### Verify the Compiler

```bash
g++ --version
```

### Verify Python

```bash
python3 --version
```

### Verify the Virtual Environment

```bash
myenv/bin/python3 --version
```

### Verify the Basic Report Dependency

```bash
myenv/bin/python3 -c "import matplotlib; print(matplotlib.__version__)"
```

### Verify All Advanced Report Dependencies

```bash
myenv/bin/python3 -c "import matplotlib, numpy, networkx, seaborn; print('Report dependencies available')"
```

---

## Troubleshooting

### `g++: command not found`

Install the C++ compiler:

```bash
sudo zypper install gcc-c++
```
## Troubleshooting

### `g++: command not found`

Install the C++ compiler:

```bash
sudo zypper install gcc-c++
```

### `Permission denied` When Starting the Program

Ensure that the binary is executable:

```bash
chmod +x Enigma-BOMBE-Cryptanalysis-Attack
./Enigma-BOMBE-Cryptanalysis-Attack
```
### PDF Report Generation Fails

Confirm that the virtual environment and Matplotlib are available:

```bash
myenv/bin/python3 -c "import matplotlib"
```

Reinstall the reporting dependencies when necessary:

```bash
source myenv/bin/activate
python -m pip install --upgrade matplotlib numpy networkx seaborn
deactivate
```
### Only Five Report Pages Are Generated

This is expected when one or more advanced dependencies are unavailable. The program enables the ten-page advanced report only when all three modules can be imported:

```text
numpy
networkx
seaborn
```

### Terminal Output Appears Corrupted

The live progress interface uses ANSI colors and cursor-positioning sequences. Run the program in a modern terminal emulator such as Konsole, GNOME Terminal, or another terminal with ANSI support. Avoid redirecting the interactive progress display to a plain text file.

### CPU Usage Reaches 100%

This is expected. The program deliberately distributes the exhaustive search across the available logical CPU threads.

### The Search Takes a Long Time

The search cost grows with the number of admissible crib offsets and surviving candidates. A more plausible and sufficiently informative crib can reduce ambiguity, although it cannot guarantee success.  

### No Candidate Configuration Is Found

Possible causes include:

- an incorrect crib;
- an incorrect crib alignment assumption;
- ciphertext produced by a different Enigma model;
- unsupported rotors or reflector;
- transcription errors;
- a plugboard or machine configuration outside the implemented model;
- insufficient linguistic evidence in a short message.

Try a different historically and linguistically plausible crib. Do not interpret the absence of a candidate as proof that the message was not produced by Enigma.

---

## Methodological Limitations

The implementation should be interpreted as an educational cryptanalysis system rather than a universal Enigma solver.

Current modeling and search assumptions include:

- exactly three moving rotors;
- rotor selection limited to I–V;
- fixed Reflector B;
- no support for the four-rotor naval M4 configuration;
- no thin Beta or Gamma rotor;
- no Reflector C or thin naval reflector;
- language scoring oriented toward German and military/historical vocabulary;
- heuristic ranking that may prefer plausible-looking text over the true plaintext;
- stochastic evolutionary search whose exact result may vary between runs;
- predefined cribs initially tested only at offset `0`;
- a stylized rather than electromechanically exact Bombe implementation;
- potential equivalent representations of left-rotor ring and position settings in the implemented three-rotor model.

Successful output remains a **candidate reconstruction** and should be validated independently.

---

## Reproducibility Notes

The exhaustive search stages are deterministic for a fixed input and program version. The Steckerbrett evolutionary stage seeds its pseudo-random generator from the system clock, so its path and final candidate may differ between executions.

For rigorous experiments, researchers may wish to modify the source so that the random seed can be supplied explicitly and recorded with the result.

---

## Academic & Legal Disclaimer  

Written by George W. Aravidis  
Email Contact: csec.unbutton326@passinbox.com  

This repository is explicitly designated for academic research, historical reconstruction, and satisfying an intense nerd curiosity.  
Do not use this to encrypt modern sensitive communications; a standard smartphone can crack this cipher faster than you can brew a cup of Earl Grey tea.  

## Legal & Ethical Disclaimer  

### A Tribute to Bletchley Park: The Dawn of Modern Computing  

This project is, above all, a profound historical homage to the World War II Enigma machine and the unparalleled genius of the cryptanalysts at Bletchley Park. We stand on the shoulders of giants—Alan Turing, Gordon Welchman, Joan Clarke, Hugh Alexander, and the thousands of brilliant minds in Hut 8 and beyond. Their monumental intellectual triumph in breaking the Enigma cipher did not merely shorten the deadliest conflict in human history; it laid the very foundational stones of theoretical computer science and artificial intelligence. This codebase is a humble digital monument to their legacy—dedicated to the minds that cracked the uncrackable.  

### Educational and Historical Research Only  

This software is provided strictly "as is" and is intended exclusively for academic, educational, and historical research purposes. The project serves solely to reconstruct, visualize, and document the cryptomechanical methods utilized during the Second World War. It is a tool for learning, not a tool for operational secrecy.  

### Absolute Limitation of Liability  

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  

By accessing, downloading, reproducing, or executing this repository's contents, the user explicitly acknowledges that they do so at their own absolute risk. The author assumes zero liability for any unauthorized, unlawful, or unintended use. You agree to fully indemnify and hold the author harmless from any legal consequences, damages, or claims arising from the modification, redistribution, or application of this software in non-compliant environments.  

### Stringent Compliance with Global Export Controls & Cryptography Laws  

The global legislative framework governing cryptography forms a highly complex, strictly enforced legal matrix. Users are expressly notified that the downloading, use, distribution, modification, or mere possession of cryptographic software—irrespective of its historical context, lack of modern security, or computationally obsolete nature—is heavily regulated and may constitute a severe legal violation in certain jurisdictions.  

Non-Exhaustive Regulatory Examples:  

1. Absolute Cryptography Bans: In specific nations, the unauthorized use, development, or possession of any cryptographic algorithm—regardless of its strength or historical origin—is strictly prohibited by law. Using this software in such regions may result in immediate criminal prosecution.  

2. International Treaties (The Wassenaar Arrangement): Even historical cryptographic implementations may fall under definitions of "Dual-Use Goods," potentially restricting their cross-border transfer.  

3. United States Export Controls (EAR & ITAR): Historically, cryptographic software was classified as a defense article (munition) under the International Traffic in Arms Regulations (ITAR). Today, although this repository contains open-source and computationally obsolete code, its global distribution is governed by the Export Administration Regulations (EAR) administered by the US Bureau of Industry and Security (BIS). Because platforms like GitHub are subject to US jurisdiction, the sharing and exporting of this code are tightly bound by these federal frameworks.  

4. European Union (Dual-Use Regulations): EU frameworks governing the export, transit, and brokering of dual-use technologies apply strict controls on cryptographic software leaving or moving within the EU zone.  

It is the absolute, sole, and non-delegable responsibility of the end-user to conduct comprehensive due diligence prior to engagement. By cloning, forking, downloading, executing, or in any way interacting with this repository, you explicitly and unconditionally assume all legal, technical, and operational risks.

**1. Waiver of Litigation & Indemnification:**  
You hereby agree to an unconditional waiver of liability, irrevocably forfeiting any right to litigate, initiate legal proceedings, or bring any claims whatsoever against the author(s) or contributors under any global jurisdiction. Furthermore, you expressly agree to fully indemnify, defend, and hold harmless the author(s) from any liabilities, claims, fines, or legal fees arising directly or indirectly from your use, misuse, or distribution of this codebase.

**2. No Defense of Ignorance & Strict Prohibition:**  
Ignorance of export controls, regional cryptography restrictions, or total cryptographic bans does not constitute a valid legal defense. **If you do not explicitly agree to these terms, you are legally and strictly prohibited from accessing or interacting with this software.**  

### Security Awareness: Do Not Use for Real Encryption  
This implementation reconstructs historical cryptographic primitives that have been mathematically and computationally decimated. While breaking this cipher in the 1940s required a warehouse full of electromechanical *Bombe* machines and the combined genius of Bletchley Park, a modern smartwatch could brute-force the entire keyspace before your coffee gets cold.  

**DO NOT, UNDER ANY CIRCUMSTANCES,** utilize this framework to encrypt contemporary sensitive data, passwords, Personally Identifiable Information (PII), or any form of personal or production communications. By modern standards, these ciphers provide **absolutely zero cryptographic security** against contemporary attacks. The implementation is intended exclusively for education, historical reconstruction, algorithmic study, and lawful academic research.  

Deploying this historical code for real-world data protection constitutes catastrophic technical negligence. The author explicitly disclaims any and all liability for data breaches, leaks, or data loss resulting from the misuse of this educational framework in a production environment.  


