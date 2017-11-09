1) co zostało zrobione:
   a) w /usr/include/minix/callnr.h dodano identyfikator nowego wywołania systemowego GETPROCR i zwiększono N_CALLS w tym samym pliku o jeden
   b) w /usr/src/mm/proto.h dodano deklaracje funkcji do_getprocnr.
   c) w /usr/src/mm/main.c dodano definicje do_getprocnr.
   d) w /usr/src/mm/table.c dodano adres (nazwe) do_getprocnr, zaś w /usr/src/mm/table.c dodano adres pusty no_sys.
   e) napisano funkcje program testujący w /root 
