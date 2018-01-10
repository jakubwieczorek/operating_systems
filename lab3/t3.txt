
Ćwiczenie 3
Synchronizacja procesów z wykorzystaniem semaforów

1. Cel ćwiczenia

Należy napisać program w języku C w środowisku systemu Linux realizujący
rozwiązanie problemu zadanego przez prowadzącego ćwiczenie. Problem jest
związany z organizacją N buforów komunikacyjnych, pomiędzy którymi są
narzucone dodatkowe więzy.

2. Bufor komunikacyjny

Bufor komunikacyjny jest strukturą danych mieszczącą maksymalnie M elementów
jednakowego typu. Elementy z bufora mogą być wyjmowane w kolejności
umieszczania (FIFO) lub odwrotnej (LIFO, stos). 

3. Zadanie do zrealizowania

Należy zrealizować typ "bufor komunikacyjny". W czasie implementacji należy
zapewnić synchronizację:

- nie dopuścić do czytania z pustego bufora,

- nie dopuścić do zapisu do pełnego bufora,

- zadbać o "nie przeszkadzanie sobie" procesów zapisujących do bufora i
  procesów czytających z bufora.
  
Dodatkowe więzy mogą dotyczyć liczby elementów aktualnie przechowywanych w
buforach, kolejności zapisu do i odczytu z poszczególnych buforów, liczby
elementów zapisywanych/odczytywanych jednocześnie, operacji na wielu buforach
jednocześnie, itd.

4. Przygotowanie do zajęć

- proszę zapoznać się z definicją semafora oraz przykładem zastosowania w
  problemie producent-konsument ze slajdów wykładu. W szczególności są Państwo
  zobligowani znać podaną na slajdach dokładną definicję semafora. Proszę nie
  pomylić definicji z realizacją.
  
  http://www.ia.pw.edu.pl/~tkruk/edu/soi.b/wyklad/w04.pdf
  (Semafory: definicja, Semafory: producent-konsument),

- http://pl.wikipedia.org/wiki/Semafor_(informatyka),

- $ man semop/semget/ipcs/ipcrm,

- "5.2 Processes Semaphores" w rozdziale "Advanced Linux Programming'
  dostępnym on-line:
  http://www.advancedlinuxprogramming.com/alp-folder/alp-ch05-ipc.pdf

- wskazanym byłoby też wprowadzająco choćby przejrzenie rozdziału dotyczącego 
  procesów:

  http://www.advancedlinuxprogramming.com/alp-folder/alp-ch03-processes.pdf

  $ man 2 fork
  $ info libc "Process Creation Example"


