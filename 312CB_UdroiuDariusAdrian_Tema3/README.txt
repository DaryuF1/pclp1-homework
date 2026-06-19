Pentru rezolvarea problemei am avut nevoie de cateva functii ajutatoare, pe care le voi explica ce face fiecare:

- functia my_strdup: este versiunea facuta de mana a functiei strdup, care aloca memorie si copiaza continutul stringului, returnand un pointer spre noul string

-functia hex_val: transforma un caracteer hexazecimal in numeral sau intreg

Apoi, am avut nevoie de niste functii pt. gestionarea memoriei folosite(adica sa dau free la ce am folosit):

- fct. free_image: elibereaza memoria pt. o imagine
 
- fct. copy_image: creeaza o copie a imaginii; aloca mem. noua si copiaza pixelii din imaginea sursa (prin asta se intelege ca daca modific copia nu este afectata originalul) 

- fct. free_lsys: elibereaza mem. pt. un LSystem (= sterge axioma, numele fisierului, parcurge lista de reguli si sterge sirul "to", apoi sterge lista de reguli)

- fct. copy_lsys: copiaza starea unui LSystem, duplicand axioma si fiecare regula in on parte

- fct. free_glyph: sterge bitmapul unei litere

- fct. free_font: parcurge toate cele 256 caractere (posibile) asci si le elibereaza pe cele incarcate, apoi sterge structura noului font

- fct. copy_font: copiaza tot fontul; avem nevoie de asta pt. ca starea programului trebuie sa fie complet independenta pt. undo

- fct. free_program_state: apeleaza toate fct. de free mai sus pt. a sterge o stare complete

- fct copy_program_state: apeleaza toate fct. de copy de mai sus pt. a crea o noua intrare in istoricul undo

In continuare, am avut nevoie de niste fct. ajutatoare pt. a gestiona optima fisierele binare primate:

- fct. save_ppm: deschide un fisier binar si scrie antetul formatului ppm, apoi scrie matricea de pixeli in fisier

- fct. load_font_bdf: pe scurt, parseaza date; citeste fisierul bdf; cand gaseste STARTCHAR aloca un nou caracter; cand gaseste BBX citeste dimensiunile literei; cand gaseste BITMAP intra intr-o bucla, unde citeste linii hexazecimale, le converteste in biti si le salveaza in glyph->bitmap; cand citeste ENDCHAR salveaza litera in tabelul fontului la indexul dat de codul sau asci

In cele ce urmeaza voi explica functiile necesare rezolvarea problemei:

- fct. draw_pixel_safe: verifica daca coord. (x,y) sunt in interiorul imaginii; daca sunt valide coloreaxa pixelul; in plus, converteste sistemul cartezian in sistemul matriceal al imaginii

- fct. draw_line: implementeaza alg. lui Bresenhan; uneste 2 puncta cu o linie folosind doar adunari si scaderi pt. a decide care pixeli trebuie aprinsi pt. a aproxima linia dreapta

- fct. draw_text: parcurge fiecare litera din textul dat si gaseste glyph-ul corespunzator in font, apoi parcurge bitmap-ul literei si deseneaza pixeli acolo unde e 1; muta cursorul la dreapta cu dwidth pt. a face loc urmatoarei litere

* Functiile pt. L-System si Bitcheck:

- fct. derive: genereaza sirul rezultat in urma axiomei; pt. fiecare iteratie(de la 0 la n) creeaza un buffer nou; parcurge sirul vechi, iar daca un caracter are o regula adauga inlocuitorul in sirul dau, altfel il copiaza; gestioneaza realocarea memoriei daca sirul devine prea mare

- fct. run_bitcheck: detecteaza erorile, liniarizeaza imaginea si foloseste o masca de 4 biti care se muta bit cu bit peste date; daca gaseste 0010 sau 1101 calculeaza inapoi coord. pixelului afectat si afiseaza mesajul de eroare

In final, o sa explic main-ul:

In primul rand, alocam structura MasterState, apoi initializam pointerii cu NULL si indicia stack-urilor cu -1. Apoi, in while, citim comenzile:
LSYSTEM - salvam starea curenta in undo; citim fisierul de reguli
LOAD - salvam starea curenta in undo, citeste headerul si pixelii din fisierul binar ppm
TURTLE - salveaza starea curenta in undo; apeleaza derive pt. a obtine sirul de comenzi("F": calculeaza noua pozitie [sin/cos] si apeleaza draw_line; "+"/"-": modifica unghiul; "[": pune pozitia curenta [x,y,unghi] intr-o stiva locala; "]": scoate pozitia din stiva)
FONT - incarca fontul bdf folosind load_font_bdf; salveaza undo inainte de a schimba fontul curent
TYPE - extrage textul dintre ghilimele, extrage coord. si culoarea; apeleaza draw_text
SAVE - apeleaza save_ppm; nu pune nimic in undo
BITCHECK - apeleaza run_bitcheck
UNDO - daca exista ceva in stack-ul undo muta starea curenta in stack-ul redo si scoate ultima stare din undo si o face curenta
REDO - daca exista ceva in stack-ul redo muta starea curenta in undo si scoate ultima stare din redo si o face curenta

Cand while-ul se opreste, programul elibereaza toata memoria: starea curenta si tot ce a ramas in stivele de undo si redo

