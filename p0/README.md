# Redes, Protocolos e Interfaces
## Práctica 0

 Sergio Semedi Barranco

 Lucas Segarra

### Explicación

Para la realización de esta práctica hemos decidido montar todo el entorno de redes sobre una
maquina física en vez de tener todo preparado en una MV.

La maquina tiene que tener un Linux instalado y el servicio dhcpd, en este mismo directorio se
encuentra el fichero ./prepare_ci40 que es un script que te prepara el entorno necesario para
que tu maquina actue como servidor para el ci40.


#### TOPOLOGÍA DE RED

Los ficheros de configuración OpenWRT necesarios para levantar la red se encuentran en net/config.

* ZONA LAN

Esta zona dispone de conexión a internet y esta formada por 3 interfaces lógicas wpan, wan y
lan. Es una zona NO RESTRICTIVA en la que aceptaremos y enturaemos paquetes que acabarán
saliendo por eth0 gracias a wan.

** wan

Disponemos de una interfaz lógica que actua como cliente dhcp (que es la que se conectará
a la máquina física), esta interfáz lógica esta directamente enganchada a eth0 lo que nos
permitirá la salida de paquetes.

** lan

Esta interfaz lógica se encarga de direccionar el punto de acceso wireless(wlan0), es un servidor
dhcpd que manejará la red 172.16.0.0/12 para dar leases a cualquier cliente que quiera
conectarse mediante WIFI. Gracias a que se encuentra en la zona LAN los paquetes pueden
hacer forwarding hacia wan para salir a internet por eth0.


##### TOPO0

Basta con conectar un portatil a la wifi PtoXX.

##### TOPO1

En el fichero topo1/config brindamos los ficheros necesarios para que una segunda ci40
actue como cliente de la red



#### Ci40 console output

root@myCi40:~# ip a

    1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1
        link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
        inet 127.0.0.1/8 scope host lo
           valid_lft forever preferred_lft forever
        inet6 ::1/128 scope host 
           valid_lft forever preferred_lft forever
    2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 00:19:f5:89:12:36 brd ff:ff:ff:ff:ff:ff
        inet 10.9.9.9/24 brd 10.9.9.255 scope global eth0
           valid_lft forever preferred_lft forever
        inet6 fe80::219:f5ff:fe89:1236/64 scope link 
           valid_lft forever preferred_lft forever
    3: wpan0: <BROADCAST,NOARP> mtu 123 qdisc fq_codel state DOWN group default qlen 300
        link/ieee802.15.4 02:19:f5:ff:fe:89:12:38 brd ff:ff:ff:ff:ff:ff:ff:ff
    5: tayga-nat64: <POINTOPOINT,MULTICAST,NOARP,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 500
        link/none 
    7: wlan0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq state UP group default qlen 1000
        link/ether 00:19:f5:89:12:33 brd ff:ff:ff:ff:ff:ff
        inet 172.16.0.1/12 brd 172.31.255.255 scope global wlan0
           valid_lft forever preferred_lft forever
        inet6 fe80::219:f5ff:fe89:1233/64 scope link 
           valid_lft forever preferred_lft forever

root@myCi40:~# ip route

    default via 10.9.9.1 dev eth0  proto static  src 10.9.9.9
    10.9.9.0/24 dev eth0  proto kernel  scope link  src 10.9.9.9
    10.9.9.1 dev eth0  proto static  scope link  src 10.9.9.9
    172.16.0.0/12 dev wlan0  proto kernel  scope link  src 172.16.0.1
    192.168.255.0/24 dev tayga-nat64  proto static  scope link
