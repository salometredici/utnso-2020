#!/bin/bash

cd .. && cd sindicato && make clean && make 
gnome-terminal -e ./sindicato
cd .. && cd comanda && make clean && make
gnome-terminal -e ./comanda
cd .. && cd restaurante && make clean && make
gnome-terminal -e ./restaurante
cd .. && cd app && make clean && make
gnome-terminal -e ./app
cd .. && cd cliente && make clean && make
gnome-terminal -e ./cliente