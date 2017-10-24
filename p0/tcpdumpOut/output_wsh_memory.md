# Propósito
Este documento ha sido redactado para facilitar la interpretación del documento (generado con la herramienta tcpdump) output_wsh de este mismo repositorio. 
## Explicación
La **topología** de la red en este caso consistió en un servidor de hora que se ejecutó en una de nuestras placas ci40 al cual accedimos con un cliente desde la otra. 

La captura de paquetes se produjo así mismo en uno de nuestros dipositivos.

Los detalles del 

| Ip Cliente    |Ip Servidor    | Puerto Cliente    |Puerto Servidor    | 
| :------------- |:-------------| :------------- |:-------------| 
| 176.16.0.115      | 176.16.0.1 | 57347     | 7777| 

Por ese motivo, análogamente a como se explica en el documento que tcpdumpHistoria se recomienda emplear el siguiente filtro en wireshark al abir el archivo. 

    udp.port == 57347 || udp.port== 7777
    
Para una correcta visualización de los datos de aplicación se recomienda así mismo en la opción "protocol preferences", habilitar la opción show data as text.  

El paquete número 4361, corresponde a la petición de hora por parte del cliente enviando el caracter 't', el 4362 la respuesta del servidor a esa petición.

Los paquetes 4687 y 4688 son la solicitud y respuesta de la fecha respectivamente.

Los siguiente paquetes corresponden a más peticiones de fecha y de hora.

Los paquetes 6754 y 6755 corresponden al cierre de la conexión.


|Num|Data |Text |Src|
|:--  |:--  |:--  |:--  |
|4361|74|t|Cliente|
|4362|486f75723a0930383a34373a33330a|Hour:\t08:47:33\n|Servidor|
|4687|64|d|Cliente|
|4688|596561723a32302d3330094d6f6e74683a3031094461793a...|Year:20-30\tMonth:01\tDay:31\n|Servidor|
|6754|71|q|Cliente|
|6755|476f6f6462796520636c69656e740a|Goodbye client\n|Servidor|

