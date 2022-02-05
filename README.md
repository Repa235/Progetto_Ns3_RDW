# Progetto di reti e dispositivi wireless - Simulazioni in NS3

## Descrizione della simulazione
Una rete WiFi indoor è costituita da 8 Stazioni Wireless organizzate su una griglia,avente parametri GridWidth=4, DeltaX=6, DeltaY=6, e da un AP posto al centro dellatopologia. L'AP è connesso ad un server attraverso un link p2p avente i seguentiparametri: Delay=5ms, DataRate=120Mbps. 
Si consideri un numero di flussi TCP, variabile da 1 a 8, tra le stazioni WiFi e il server.Ogni flusso TCP, avente DataRate=1Mbps, è modellabile con applicazione ON-OFF cheinizia   all’istante   t=1sec   avente   periodo   ON   costante   per   tutta   la   durata   dellasimulazione, di durata 30sec. 
In parallelo, si consideri un numero di flussi UDP,variabile da 1 a 8, tra ciascuna delle stazioni WiFi e il server. Ogni flusso UDP èmodellabile come traffico CBR avente Data Rate=1Mbps, inizia all’istante t=1sec e siconclude alla fine della simulazione.Ad ogni run della simulazione, il numero di flussi TCP e UDP deve essere equivalente.
Ad esempio, al run 1 abbiamo 1 flusso TCP e 1 flusso UDP, al run 2 abbiamo 2 flussiTCP e 2 flussi UDP, al run 3 abbiamo 3 flussi TCP e 3 flussi UDP, etc. 

I file .cc vanno inseriti nella cartella scratch i restanti file in un cartella chiamata "my-simulation" (se non è presente basta crearla)

Poi compilare il tutto con
```
./waf
```


## Prima simulazione
Prima campagna di simulazione: Nello  scenario  descritto,  si valutino le seguentimetriche medie: throughput, delay, packet losses, pacchetti ricevuti, al variare delnumero dei flussi e al variare dello standard WiFi: 11a, 11g, 11ac. Output: Un grafico con Throughput (asse Y) al variare del numero dei flussi (asse Xvaria da 2 a 16, a step di 2 unità. Ad esempio, 2, 4, 6, 8, etc.). Nel grafico dovrannoapparire tre curve,   rispettivamente per i casi 11a , per 11g , 11ac.Nelle stessecondizioni, si realizzi un grafico con delay, un grafico con pacchetti persi, un graficocon pacchetti ricevuti.
```
./my-simulation/ScriptSimulazione1.sh
```

## Seconda simulazione
Seconda campagna di simulazione:  Nello stesso ambiente di rete, si consideri unnuovo scenario applicativo in cui esiste solo un flusso TCP, tra il server e la stazionezero,   avente   DataRate=10Mbps,   modellabile   con   applicazione   ON-OFF   che   iniziaall’istante t=1sec avente periodo ON costante per tutta la durata della simulazione, didurata 30sec. Si grafici il throughput istantaneo in questa condizione, e quando ilflusso TCP coesiste con un flusso UDP-CBR avente DataRate=10Mbps, inizia  all’istantet=1sec e si conclude alla fine della simulazione. Output: Un grafico con Throughput istantaneo (asse Y) al variare del tempo in secondi(asse X). Nel grafico dovranno apparire due curve, una per il caso “solo flusso TCP”,una per il caso in cui il flusso TCP coesiste con quello UDP. 
```
./waf --run=tesina-repaci-pt2
```
