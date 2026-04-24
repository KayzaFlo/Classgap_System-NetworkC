#!/bin/bash
# test_netcat.sh
# Problème 2 : tester le serveur UDP avec netcat.
# Usage : ./test_netcat.sh
#
# Prérequis : le serveur doit tourner dans un autre terminal
#   gcc -o exmp1_server exmp1_server.c && ./exmp1_server
#
# -u  : mode UDP (par défaut netcat utilise TCP)
# -w1 : timeout de 1 seconde pour recevoir la réponse

echo "Envoi de 'Hello Server!' au serveur UDP 127.0.0.1:8080"
echo "Hello Server!" | nc -u -w1 127.0.0.1 8080

echo ""
echo "Envoi d'un second message..."
echo "Deuxième message UDP" | nc -u -w1 127.0.0.1 8080
