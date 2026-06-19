UDROIU Darius-Adrian 312CB

Rezolvarea problemei battleships

taskul 1:


Prima cerinta a problemei cere sa simulez jocul battleships pt. doi jucatori, ii vom nota a si b. Pt. rezolvarea cerintei am folosit 2 functii read_and_parsing si solve pe care le voi prezenta in cele ce urmeaza.

Functia read_and_parsing: Aceasta functie citeste pozitiile, orientarea si tipul fiecarei nave ale jucatorilor a si b (prima data citesc pt. a, apoi pt. b si tot asa) si creeaza table asociata fiecarui jucator. In plus, daca coordonatele nu sunt valide afisez mesajul aferent acestui caz, fara a schimba ordinea jucatorului. Pt. a rezolva problema am nevoie de niste vectori a (respectiv b), pa (respectiv pb), ships_a (respectiv ships_b) care semnifica tabla de joc a jucatorului a (b) dupa ce si-a pus barcile pe ea, tabla in care imi retin doar centrul fiecarei nave, respectiv informatii pt. fiecare nava citita.
   
Functia solve: simulez jocul celor 2 jucatori si afisez pt. fiecare tip de atac(bun sau rau) mesajul aferent. Pe scurt, programul citeste coordonatele unde ataca jucatorul a (respectiv b) si verifica daca a lovit o barca, daca a lovit centrul barcii, daca nu a lovit nimic, sau daca a lovit o coordonata pe care a mai atacat-o anterior. In cazul in care este un atac valid sterg punctele atacate din matricea a(respectiv b). In plus, imi calculez si numarul de lovituri pt. jucatorul a (b) si numarul de lovituri bune pt. jucatorul a(b), precum si numarul de nave plasate. Retin aceste valori in niste vectori, reprezentand nr. de lovituri a(b)/ nr. de lovituri bune pt. a(b)/ nr. de nave => la meciul cu indicele k

taskul 2:


A 2-a cerinta a problemei cere sa calculez niste statistici bazate pe datele citite la taskul 1 si sa afisez pt. fiecare cod pe care il primesc statistica corespunzatoare. Pt. rezolvarea cerintei am folosit 2 functii calc_stats si parse_code pe care le voi prezenta in cele ce urmeaza.

Functia parse_code: primeste codul citit din input si il desparte in cele 5 semnificatii din enunt.

Functia calc_stats: calculeaza cele 4 statistici din enunt cu ajutorul unor formule matematice deduse usor si cu niste algoritmi tip greedy, caci pt. a ajunge la raspunsul favorabil pt. unele cerinte din cele 4 e optim sa sortam vectorii necesari in rezolvarea cerintei. In final, afisez in functie de codul citit acuratetea totala sau acuratetea pe un meci si una dintre cele 4 statistici calculate anterior

taskul 3:
