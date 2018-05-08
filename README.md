# PC-Tema2

Student: **George Neacșu**\
Grupă: **321CD**\
Email: **[george@neacsugeorge.com](mailto:george@neacsugeorge.com)**

# Detalii implementare

Pașii de implementare precum și ordinea acestora
se pot vedea în commit-uri **[aici](https://github.com/neacsugeorge/PC-Tema2/commits/master)**.
Durata de implementare este de **17h36min** (fără README) conform [WakaTime](https://wakatime.com)
și în funcție de data la care se face corectarea se poate analiza
timpul de implementare **[aici](https://wakatime.com/@george_neacsu/projects/twxexlttnf?start=2018-05-02&end=2018-05-08)**.

Ordinea de implementare:

1. CardManager
2. Logger
3. ConnectionManager
4. CommandManager

În implementarea modului de funcționare al clientului
cât și a serverului am încercat (inconștient) să urmez conceptul
[Isomorphic JavaScript](https://en.wikipedia.org/wiki/Isomorphic_JavaScript).
Sunt un mare fan JavaScript. Astfel, după cum se poate
vedea în CommandManager, majoritatea funcțiilor au cod
care au funcționalitate atât pentru client cât și pentru
server.

Există un anumit număr de mesaje ce se trimit între
client și server, de tipul **identificator-comanda \<parametru1> ... \<parametruN>**.
Pe lângă comenzile care au trebuit implementate (login, logout etc.)
există anumite comenzi utilitare (error, message), de asemenea, există
anumite comenzi cu prefixul *confirm* care au fost folosite în cazul
comenzilor *transfer* și *unlock*.

Pentru comenzile *transfer* și *unlock* am preferat să folosesc modelul:\
`Client -> "transfer destinatie suma" -> Server`\
`Server -> "transfer destinatie suma <date suplimentare de la server>" -> Client`\
ca un fel de întrebare de confirmare din partea server-ului, astfel că
ulterior clientul confirmă comanda trimisă inițial prin adăugarea la începutul
comenzii a șirului de caractere *"confirm"*. Astfel avem avantajul
că putem trimite de la server date suplimentare (ex: în cazul *transfer* serverul
trimite numele și prenumele destinației) pe care le interpretăm(render) cum facem
în general pe Front-End :)\
`Client -> "confirmtransfer destinatie suma" -> Server`\
`Server -> <mesaj de confirmare> -> Client`

Codul nu are foarte multe comentarii, dar în schimb m-am chinuit să
îl scriu cât mai clar.

# Motivare întarziere
Eu sunt vinovat pentru întârzierea temei (m-am apucat târziu,
vineri 04 mai) și sunt 100% sigur că aș fi putut să o trimit cu
24-36h înainte dar am ajutat la realizarea subiectelor pentru
concursul național [Acadnet](http://acadnet.ro/), concurs care
este foarte important pentru mine deoarece datorită lui am intrat
la Automatică. Pot dovedi atât prin faptul că sunt în comisia de 
organizare [Comisie Acadnet](http://acadnet.ro/nationala/comisie)
cât și prin gardianul de la intrare în EG-uri care ne-a văzut pe
mine și pe colegul Valentin Mocanu când am plecat luni dimineața
în jurul orei 05-06 din facultate. Astfel dacă se poate reduce
depunctarea măcar cu 1 punct eu sunt fericit. \
Mulțumesc anticipat!

