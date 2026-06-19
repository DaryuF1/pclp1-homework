Window join:
Problema in sine e foarte usoara, rezumandu-se la a afisa cmmmc-ul si cmmdc-ul perechilor de numere de pe pozitiile i si j (j<i) cu proprietatea ca diferenta timestamp-ului sa fie  mai mica sau egala ca variabila window (la mine in cod variabila window este notata cu win). Totusi, cea mai mare problema la aceasta problema a fost sa gasesc tipul potrivit de date pt. variabilele citite, astfel incat sa nu-mi iau wrong answer ca-mi puscau testele.(am folosit pana la urma unsigned long long, definit in cod ca ull pt. simplificarea codului)
In cele ce urmeaza voi prezenta codul problemei.
Am definit o structura, numita pair, pt. a simplifica scrisul si a retine mult mai simplu ce ma intereseaza. Apoi, am facut functiile de cmmmc si cmmdc necesare in rezolvare problemei. Am citit datele de intrare pana cand a aparut perechea 0 0, unde s-a oprit citirea si am retinut in 2 vectori (t si v) datele citite. In cele din urma, verificat pt. fiecare pereche i si j proprietatea explicate mai sus si am retinut in structura de date definita la inceput cmmmc-ul, cmmdc-ul si timestamp-ul pt. fiecare variabila, caci problema inpune conditia sa afisam datele sortate dupa timestamp. In final, am facut sortarea si am afisat raspunsul, adica cmmmc-ul si cmmdc-ul.

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Akari:
Prima data intelesesem ceva mult mai complex din enunt si scrisesem un cod de 300 de linii la care am fost foarte surprins cand am luat 0 puncta pe teste:)Dupa inca jumatate de ora de lectura pe textul problemei m-am prins ce vrea de la mine si anume: avand un grid cu niste celule egale cu "L", "#" sau "-" trebuie completata matricea dupa regulile jocului akari, prima cerinta, iar la a doua cerinta trebuie sa verific daca o configuratie de matrice poate respecta regulile jocului akari.
In cele ce urmeaza voi prezenta codul problemei.
In timpul citirii matricei m-am folosit de 2 tablouri auxiliare pt. a retine pozitiile celulelor negre cu numere (vectorii x si y, care retin pozitia liniei, respective pozitia coloanei) si o matrice v, unde am notat cu -1 faptul ca nu am voie sa completez matricea pe pozitia (i,j) daca valoarea matricei adiacente (v) este -1.

Cerinta 1:
Am facut rezolvarea primei cerinte in functia solve1, pe care am construit-o in coada programului pt. a nu aglomera inceputul. In rezolvarea acestei probleme am avut nevoie de 2 functii ajutatoare care sa fluidizez programul.

Functia fill primeste ca parametru pozitia unde se afla un bec (reprezentat de caracterul 'L') si completeaza orizontal si vertical cu 'x' matricea, reprezentand faptul ca pozitia (i,j) e luminata de becu . In plus, programul respecta regula de a umple pe verticala sau orizontala cu 'x' cat timp nu intalnesc o celula neagra.

Functia fill_ak primeste ca parametru pozitiile unde se afla numere si numara cate becuri vecine are pozitia (i,j). In cazul in care nr. de becuri este egal cu numarul de pe celula neagra fac celulele adiacente egale cu 'x' in cazul in care nu este vreun bec acolo.

In final, pt. finalizarea acestei cerinte aplic subprogramul fill pt. fiecare celula care este notata cu 'L', apoi aplic subprogramul fill_ak pt. celulele negre, unde se afla un numar, urmand ca apoi sa afisez matricea completata.

Cerinta 2:
Am facut rezolvarea celei de-a doua cerinte in functia solve2, pe care am construit-o in coada programului pt. a nu aglomera inceputul. In rezolvarea acestei probleme am avut nevoie de 2 functii ajutatoare care sa fluidizez programul.

Functia ver1 primeste ca parametrii pozitia unde se afla un bec si returneaza 1 daca nu mai exista niciun bec pe aceasi linie sau coloana care se lumineaza reciproc cu becul de pe pozitia (i,j).

Functia ver2 primeste ca parametru pozitia celulelor negre cu numere si verifica daca suma celulelor adiacente cu bec si suma celulelor adiacente neiluminate este mai mare sau egal cu numarul de pe celula neagra. In caz afirmativ, functia returneaza 1.

In final, pt. finalizarea acestei cerinte verific daca cele 2 conditii sunt respectate si, in caz afirmati, afisez "ichi", altfel "zero".

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Helicopters:
Problema asta e mult mai usoara fata de precedenta, enuntul problemei rezumandu-se in a verifica daca un avion aterizeaza bine sau nu.(a ateriza bine inseamna ca triunghiul determinat de avion sa contina mai multe numere de 1 decat de 0).
Rezolvarea problemei este inpusa de a Numara celulele de 0 din triunghiurile determinate de avioane. Pt. a face asta avem 4 cazuri posibile, determinate de varful triunghiului, reprezentat de intersectia catetelor(1 deasupra, -1 dedesubt).Astfel, daca variabila kkk/2(repr nr de cellule din triunghi) este mai mica dececat kk(nr. de celule egale cu 0) inseamna ca avionul poate ateriza bine si imi retin indicele acestuia intr-un vector, v. Daca kk este egal cu 0 inseamna ca triunghiul are doar valori pozitive si contorizez nr. de astfel de triunghiuri prin variabila cnt. In plus, retin numarul de ordine al avioanelor care sunt pozitionate necorespunzator in vectorul w. In final, afisez ce imi cere problema, afisare bazata pe vectorii si variabilele optinute mai sus. 
