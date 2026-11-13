# c-httpd
A lightweight HTTP web server written in C

seq 1 50 | xargs -n1 -P50 curl -s -o /dev/null -w "%{http_code}\n" http://192.168.178.104:10000/index.html


sudo apt update && sudo apt install -y apache2-utils
ab -n 1000 -c 50 http://192.168.178.104:10000/index.html