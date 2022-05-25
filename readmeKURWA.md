Opowiadanie autorstwa Pana Łopaty:


kolejność działań wykonywach przez obie strony:

Klient:
-łączy się z serwerem
-dołącza do stołu (jedynego)
-po dołączeniu tworzy wątek nasłuchujący
-pętla oczekiwania na start (wykorzystuje semafor)
-pętla gry. Tutaj tylko wyświetla, wysyłanie by się odbywało za pomocą funkcji SendInput()

Serwer:
-tworzy wątek stołu już na początku (+rezerwacja pamięci na wszystko)
-inicjalizuje system socketów (serwerowe tego wymagają)
-nasłuchuje na połączenie
-oczekuje pierwszej wiadomosci od klienta (tutaj JOIN)
-dodaje go do stołu i tworzy jego wątek nasłuchujący

Table:
-sprawdza czy wszyscy dołączyli
-Wysyła init pozycje i plansze
-po chwili wysyła sygnał startu
-wysyła różnice do graczy

Disclaimer do Table'a, wszystkie wysyłanie muszą być wywołane osobno dla każdego z graczy!


z własnej woli by wam to już dać zabijam kotki pamięciowe (rip)


to co jest wasze do modyfikacji głównie TO:
client.cpp
i table.cpp 
!!!!!!!
