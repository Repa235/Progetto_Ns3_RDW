nFlows=(2 4 6 8 10 12 14 16) #creo un vettore con il numero di flussi

rm *.txt
rm *.tr

for n in ${!nFlows[@]}; do #avvio un ciclo FOR per tutti i valori del vettore

echo "Lancio la simulazione con "${nFlows[$n]}" flussi" # scritta video 

./waf --run "tesina-repaci-pt1 --verbose=false --flows=${nFlows[$n]} --output=true" 

done

echo "Che standard stai usando ?"

read nomeStandard

mkdir $nomeStandard 

mv *.txt $nomeStandard
mv *.tr $nomeStandard

echo "Ho finito, troverai tutto nella cartella $nomeStandard"


