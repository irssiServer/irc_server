FROM ubuntu:latest

RUN apt-get update
RUN apt-get upgrade -y
RUN apt install inspircd -y && sed -i 's/<connect/<connect password="123"\n/g' /etc/inspircd/inspircd.conf
RUN apt install irssi -y
RUN apt install tcpflow -y
RUN mkdir /var/run/inspircd/ && chmod 777 /var/run/inspircd/

CMD ["inspircd", "--runasroot", "--nofork", "--debug"]

