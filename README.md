# Progetto di reti e dispositivi wireless - Simulazioni in NS3

## Descrizione dello scenario di simulazione
Una rete WiFi indoor è costituita da 8 Stazioni Wireless organizzate su una griglia,avente parametri GridWidth=4, DeltaX=6, DeltaY=6, e da un AP posto al centro della topologia. L'AP è connesso ad un server attraverso un link p2p avente i seguenti parametri: Delay=5ms, DataRate=120Mbps. 
Si consideri un numero di flussi TCP, variabile da 1 a 8, tra le stazioni WiFi e il server.Ogni flusso TCP, avente DataRate=1Mbps, è modellabile con applicazione ON-OFF che inizia all’istante   t=1sec   avente   periodo   ON   costante   per   tutta   la   durata   della simulazione, di durata 30sec. 
In parallelo, si consideri un numero di flussi UDP,variabile da 1 a 8, tra ciascuna delle stazioni WiFi e il server. Ogni flusso UDP è modellabile come traffico CBR avente Data Rate=1Mbps, inizia all’istante t=1sec e siconclude alla fine della simulazione.Ad ogni run della simulazione, il numero di flussi TCP e UDP deve essere equivalente.
Ad esempio, al run 1 abbiamo 1 flusso TCP e 1 flusso UDP, al run 2 abbiamo 2 flussiTCP e 2 flussi UDP, al run 3 abbiamo 3 flussi TCP e 3 flussi UDP, etc. 

I file .cc vanno inseriti nella cartella scratch i restanti file in un cartella chiamata "my-simulation" (se non è presente basta crearla)

Poi compilare il tutto con:
```
./waf
```


## Prima simulazione
Nello  scenario  descritto,  si valutino le seguenti metriche medie: throughput, delay, packet losses, pacchetti ricevuti, al variare del numero dei flussi e al variare dello standard WiFi: 11a, 11g, 11ac. Output: Un grafico con Throughput (asse Y) al variare del numero dei flussi (asse Xvaria da 2 a 16, a step di 2 unità. Ad esempio, 2, 4, 6, 8, etc.). Nel grafico dovranno apparire tre curve,   rispettivamente per i casi 11a , per 11g , 11ac.Nelle stesse condizioni, si realizzi un grafico con delay, un grafico con pacchetti persi, un grafico con pacchetti ricevuti.
```
./my-simulation/ScriptSimulazione1.sh
```
I plt dei grafici di questa simulazione sono (my_): thr, delay, rx, lost


## Seconda simulazione
Nello stesso ambiente di rete, si consideri un nuovo scenario applicativo in cui esiste solo un flusso TCP, tra il server e la stazionezero,   avente   DataRate=10Mbps,   modellabile   con   applicazione   ON-OFF   che   inizia all’istante t=1sec avente periodo ON costante per tutta la durata della simulazione, didurata 30sec. Si grafici il throughput istantaneo in questa condizione, e quando il flusso TCP coesiste con un flusso UDP-CBR avente DataRate=10Mbps, inizia  all’istante t=1sec e si conclude alla fine della simulazione. Output: Un grafico con Throughput istantaneo (asse Y) al variare del tempo in secondi(asse X). Nel grafico dovranno apparire due curve, una per il caso “solo flusso TCP”,una per il caso in cui il flusso TCP coesiste con quello UDP. 
```
./waf --run=tesina-repaci-pt2
```
In questo script sono stati fatti due run, uno con la parte UDP attiva e l'altro commentando la componente TCP nell'applicazione di rete. I due file in uscita possono essere chiamati come si preferisce, io ho deciso di chiamarli: instant_ThroughputTCPUDP.txt e instant_ThroughputTCP_OnlyTCP.txt
Chiaramente il file di out è: my_InstantThrT.plt
