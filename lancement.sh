#!/bin/sh

#Lancement du driver
echo "Lancement du driver..."
(cd driver && ./driver) &
PID_DRIVER=$!

sleep 1

#Lancememt de l'application
echo "Lancement de l'application..."
(cd app && ./app) &
PID_APP=$!

# Arret du driver et de l'application
printf "Appuyez sur une touche pour arrêter le driver et l'application..."
read tmp
kill $PID_DRIVER 2>/dev/null
kill $PID_APP 2>/dev/null 

echo "Driver et application arrêtés."