G90
G92 X0 Y0 Z0
M106 S255

;Corta os cantos da placa
G1 F500 X62.5 Y0
G1 F500 X62.5 Y-105.0
G1 F500 X0 Y-105.0
G1 F500 X0 Y0
;
;G1 F50 Z-0.1
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;G1 F50 Z-0.2
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;G1 F50 Z-0.3
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;G1 F50 Z-0.4
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;G1 F50 Z-0.5
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;G1 F50 Z-0.6
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;G1 F50 Z-0.7
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;G1 F50 Z-0.8
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;G1 F50 Z-0.9
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;G1 F50 Z-1.0
;G1 F100 X62.5 Y0
;G1 F100 X62.5 Y-105.0
;G1 F100 X0 Y-105.0
;G1 F100 X0 Y0
;
;Finaliza o trabalho
G1 F100 Z2.0
M106 S0
M1
